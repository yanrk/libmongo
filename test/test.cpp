#include <cstdio>
#include "libmongo.h"

int main()
{
    MongoOperator mongo_operator;
    if (!mongo_operator.init("mongodb://localhost:27017/", "db_test", "tb_test"))
    {
        printf("error: {%s}\n", mongo_operator.what());
        return (1);
    }

    if (0 == mongo_operator.count_all())
    {
        if (!mongo_operator.create_index("node_id"))
        {
            printf("error: {%s}\n", mongo_operator.what());
            return (2);
        }
    }

    if (mongo_operator.select_all())
    {
        std::string element;
        while (mongo_operator.read(element))
        {
            printf("%s\n", element.c_str());
        }

        if (!mongo_operator.remove_all())
        {
            printf("error: {%s}\n", mongo_operator.what());
            return (3);
        }
    }

    if (!mongo_operator.insert("{\"node_id\": 111, \"node_name\": \"A111\", \"node_properties\": [\"p1\", \"p2\", \"p3\"], \"reboot_time\": \"2023-03-06\", \"need_reboot\": true}"))
    {
        printf("error: {%s}\n", mongo_operator.what());
        return (4);
    }

    if (!mongo_operator.insert("{\"node_id\": 222, \"node_name\": \"A222\", \"node_properties\": [], \"reboot_time\": \"2023-03-06\", \"need_reboot\": true}"))
    {
        printf("error: {%s}\n", mongo_operator.what());
        return (5);
    }

    if (!mongo_operator.insert("{\"node_id\": 333, \"node_name\": \"A333\", \"node_properties\": [\"ppp\"], \"reboot_time\": \"2023-03-06\", \"need_reboot\": true}"))
    {
        printf("error: {%s}\n", mongo_operator.what());
        return (6);
    }

    if (!mongo_operator.update("{\"node_id\": 222}", "{\"node_name\": \"C222\", \"node_properties\": [\"222\"], \"requirements\": [\"r1\", \"r2\"], \"reboot_time\": \"2021-01-06\", \"need_reboot\": false}"))
    {
        printf("error: {%s}\n", mongo_operator.what());
        return (7);
    }

    return (0);
}
