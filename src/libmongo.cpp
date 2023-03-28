/********************************************************
 * Description : mongo db operator class
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * History     :
 * Copyright(C): 2023
 ********************************************************/

#include <memory>
#include "libmongo.h"
#include "mongoc.h"
#include "bson.h"

class MongoOperatorImpl
{
public:
    MongoOperatorImpl();
    ~MongoOperatorImpl();

public:
    bool init(const std::string & mongo_uri, const std::string & database, const std::string & table);
    void exit();

public:
    int64_t count(const std::string & select_message);
    int64_t count_all();
    bool create_index(const std::string & key, bool asc = true, bool unique = true);
    bool select(const std::string & select_message);
    bool select_all();
    bool read(std::string & select_element);
    bool insert(const std::string & insert_message);
    bool update(const std::string & select_message, const std::string & update_message);
    bool remove(const std::string & select_message);
    bool remove_all();

public:
    const std::string & get_mongo_uri() const;
    const std::string & get_database() const;
    const std::string & get_table() const;
    const char * what();

private:
    std::string                                                                 m_mongo_uri;
    std::string                                                                 m_database;
    std::string                                                                 m_table;
    std::string                                                                 m_error;
    std::unique_ptr<mongoc_client_t, void (*) (mongoc_client_t *)>              m_client;
    std::unique_ptr<mongoc_collection_t, void (*) (mongoc_collection_t *)>      m_collection;
    std::unique_ptr<mongoc_cursor_t, void (*) (mongoc_cursor_t *)>              m_cursor;
};

MongoOperatorImpl::MongoOperatorImpl()
    : m_mongo_uri()
    , m_database()
    , m_table()
    , m_error()
    , m_client(nullptr, nullptr)
    , m_collection(nullptr, nullptr)
    , m_cursor(nullptr, nullptr)
{

}

MongoOperatorImpl::~MongoOperatorImpl()
{
    exit();
}

bool MongoOperatorImpl::init(const std::string & mongo_uri, const std::string & database, const std::string & table)
{
    exit();

    do
    {
        m_mongo_uri = mongo_uri;
        m_database = database;
        m_table = table;

        mongoc_init();

        m_client = std::unique_ptr<mongoc_client_t, void (*) (mongoc_client_t *)>(mongoc_client_new(mongo_uri.c_str()), mongoc_client_destroy);
        if (!m_client)
        {
            m_error = "create mongo client failed";
            break;
        }

        m_collection = std::unique_ptr<mongoc_collection_t, void (*) (mongoc_collection_t *)>(mongoc_client_get_collection(m_client.get(), database.c_str(), table.c_str()), mongoc_collection_destroy);
        if (!m_collection)
        {
            m_error = "create mongo collection failed";
            break;
        }

        return (true);
    } while (false);

    exit();

    return (false);
}

void MongoOperatorImpl::exit()
{
    m_cursor.release();
    m_collection.release();
    m_client.release();

    mongoc_cleanup();
}

int64_t MongoOperatorImpl::count(const std::string & select_message)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (-1);
    }

    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> select_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(select_message.c_str()), select_message.size(), &error), bson_destroy);
    if (!select_bson)
    {
        m_error = error.message;
        return (-1);
    }

    int64_t result = mongoc_collection_count_documents(m_collection.get(), select_bson.get(), nullptr, nullptr, nullptr, &error);
    if (result < 0)
    {
        m_error = error.message;
        return (-1);
    }

    m_error.clear();
    return (result);
}

int64_t MongoOperatorImpl::count_all()
{
    return (count("{}"));
}

bool MongoOperatorImpl::create_index(const std::string & key, bool asc, bool unique)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    const std::string keys_json("{\"" + key + "\":" + (asc ? "1" : "-1") + "}");
    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> keys_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(keys_json.c_str()), keys_json.size(), &error), bson_destroy);
    if (!keys_bson)
    {
        m_error = error.message;
        return (false);
    }

    mongoc_index_opt_t index_opt;
    mongoc_index_opt_init(&index_opt);
    index_opt.unique = unique;

    if (!mongoc_collection_create_index_with_opts(m_collection.get(), keys_bson.get(), &index_opt, nullptr, nullptr, &error))
    {
        m_error = error.message;
        return (false);
    }

    m_error.clear();
    return (true);
}

bool MongoOperatorImpl::select(const std::string & select_message)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> select_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(select_message.c_str()), select_message.size(), &error), bson_destroy);
    if (!select_bson)
    {
        m_error = error.message;
        return (false);
    }

    m_cursor = std::unique_ptr<mongoc_cursor_t, void (*) (mongoc_cursor_t *)>(mongoc_collection_find_with_opts(m_collection.get(), select_bson.get(), nullptr, nullptr), mongoc_cursor_destroy);
    if (!m_cursor)
    {
        m_error = "collection find failed";
        return (false);
    }

    m_error.clear();
    return (true);
}

bool MongoOperatorImpl::select_all()
{
    return (select("{}"));
}

bool MongoOperatorImpl::read(std::string & select_element)
{
    if (!m_cursor)
    {
        m_error = "the cursor is invalid";
        return (false);
    }

    const bson_t * select_bson = nullptr;
    if (mongoc_cursor_next(m_cursor.get(), &select_bson))
    {
        select_element = bson_as_json(select_bson, nullptr);
        return (true);
    }
    else
    {
        select_element.clear();
        m_cursor.release();
        return (false);
    }
}

bool MongoOperatorImpl::insert(const std::string & insert_message)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> insert_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(insert_message.c_str()), insert_message.size(), &error), bson_destroy);
    if (!insert_bson)
    {
        m_error = error.message;
        return (false);
    }

    if (!mongoc_collection_insert_one(m_collection.get(), insert_bson.get(), nullptr, nullptr, &error))
    {
        m_error = error.message;
        return (false);
    }

    m_error.clear();
    return (true);
}

bool MongoOperatorImpl::update(const std::string & select_message, const std::string & update_message)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> select_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(select_message.c_str()), select_message.size(), &error), bson_destroy);
    if (!select_bson)
    {
        m_error = error.message;
        return (false);
    }

    const std::string update_setting("{\"$set\":" + update_message + "}");
    std::unique_ptr<bson_t, void (*) (bson_t *)> update_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(update_setting.c_str()), update_setting.size(), &error), bson_destroy);
    if (!update_bson)
    {
        m_error = error.message;
        return (false);
    }

    const std::string option_setting("{\"upsert\":true}");
    std::unique_ptr<bson_t, void (*) (bson_t *)> option_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(option_setting.c_str()), option_setting.size(), &error), bson_destroy);
    if (!option_bson)
    {
        m_error = error.message;
        return (false);
    }

    if (!mongoc_collection_update_one(m_collection.get(), select_bson.get(), update_bson.get(), option_bson.get(), nullptr, &error))
    {
        m_error = error.message;
        return (false);
    }

    m_error.clear();
    return (true);
}

bool MongoOperatorImpl::remove(const std::string & select_message)
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> remove_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(select_message.c_str()), select_message.size(), &error), bson_destroy);
    if (!remove_bson)
    {
        m_error = error.message;
        return (false);
    }

    if (!mongoc_collection_delete_one(m_collection.get(), remove_bson.get(), nullptr, nullptr, &error))
    {
        m_error = error.message;
        return (false);
    }

    m_error.clear();
    return (true);
}

bool MongoOperatorImpl::remove_all()
{
    if (!m_collection)
    {
        m_error = "the operator is invalid";
        return (false);
    }

    const std::string select_message("{}");
    bson_error_t error = { 0x0 };
    std::unique_ptr<bson_t, void (*) (bson_t *)> remove_bson(bson_new_from_json(reinterpret_cast<const uint8_t *>(select_message.c_str()), select_message.size(), &error), bson_destroy);
    if (!remove_bson)
    {
        m_error = error.message;
        return (false);
    }

    if (!mongoc_collection_delete_many(m_collection.get(), remove_bson.get(), nullptr, nullptr, &error))
    {
        m_error = error.message;
        return (false);
    }

    m_error.clear();
    return (true);
}

const std::string & MongoOperatorImpl::get_mongo_uri() const
{
    return (m_mongo_uri);
}

const std::string & MongoOperatorImpl::get_database() const
{
    return (m_database);
}

const std::string & MongoOperatorImpl::get_table() const
{
    return (m_table);
}

const char * MongoOperatorImpl::what()
{
    return (m_error.c_str());
}

MongoOperator::MongoOperator()
    : m_mongo_operator_impl(nullptr)
{

}

MongoOperator::~MongoOperator()
{
    exit();
}

bool MongoOperator::init(const std::string & mongo_uri, const std::string & database, const std::string & table)
{
    exit();

    m_mongo_operator_impl = new MongoOperatorImpl;
    if (nullptr != m_mongo_operator_impl && m_mongo_operator_impl->init(mongo_uri, database, table))
    {
        return (true);
    }

    exit();

    return (false);
}

void MongoOperator::exit()
{
    if (nullptr != m_mongo_operator_impl)
    {
        m_mongo_operator_impl->exit();
        delete m_mongo_operator_impl;
        m_mongo_operator_impl = nullptr;
    }
}

int64_t MongoOperator::count(const std::string & select_message)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->count(select_message) : -1);
}

int64_t MongoOperator::count_all()
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->count_all() : -1);
}

bool MongoOperator::create_index(const std::string & key, bool asc, bool unique)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->create_index(key, asc, unique) : false);
}

bool MongoOperator::select(const std::string & select_message)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->select(select_message) : false);
}

bool MongoOperator::select_all()
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->select_all() : false);
}

bool MongoOperator::read(std::string & select_element)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->read(select_element) : false);
}

bool MongoOperator::insert(const std::string & insert_message)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->insert(insert_message) : false);
}

bool MongoOperator::update(const std::string & select_message, const std::string & update_message)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->update(select_message, update_message) : false);
}

bool MongoOperator::remove(const std::string & select_message)
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->remove(select_message) : false);
}

bool MongoOperator::remove_all()
{
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->remove_all() : false);
}

const std::string & MongoOperator::get_mongo_uri() const
{
    static const std::string s_dummy;
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->get_mongo_uri() : s_dummy);
}

const std::string & MongoOperator::get_database() const
{
    static const std::string s_dummy;
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->get_database() : s_dummy);
}

const std::string & MongoOperator::get_table() const
{
    static const std::string s_dummy;
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->get_table() : s_dummy);
}

const char * MongoOperator::what()
{
    static const char * s_dummy = "the operator is invalid";
    return (nullptr != m_mongo_operator_impl ? m_mongo_operator_impl->what() : s_dummy);
}
