/********************************************************
 * Description : mongo db operator class
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * History     :
 * Copyright(C): 2023
 ********************************************************/

#ifndef MONGO_OPERATOR_H
#define MONGO_OPERATOR_H


#ifdef _MSC_VER
    #define LIBMONGO_CDECL           __cdecl
    #define LIBMONGO_STDCALL         __stdcall
    #ifdef EXPORT_LIBMONGO_DLL
        #define LIBMONGO_API         __declspec(dllexport)
    #else
        #ifdef USE_LIBMONGO_DLL
            #define LIBMONGO_API     __declspec(dllimport)
        #else
            #define LIBMONGO_API
        #endif // USE_LIBMONGO_DLL
    #endif // EXPORT_LIBMONGO_DLL
#else
    #define LIBMONGO_CDECL
    #define LIBMONGO_STDCALL
    #define LIBMONGO_API
#endif // _MSC_VER

#include <cstdint>
#include <string>

class MongoOperatorImpl;

class LIBMONGO_API MongoOperator
{
public:
    MongoOperator();
    ~MongoOperator();

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
    MongoOperatorImpl     * m_mongo_operator_impl;
};


#endif // MONGO_OPERATOR_H
