#pragma once

#include <string>
#include <map>

struct Entry
{
    int id;
    std::string name;
};

class Table
{
private:
    std::map<int, Entry> table_;

public:
    bool insert(int key, const std::string &data)
    {
        if (table_.count(key))
        {
            return false;
        }

        table_[key] = {key, data};
        return true;
    }
    void truncate() { table_.clear(); }

    Table intersection(const Table& other);
    Table symmetric_difference(const Table& other);

    std::string print();
};
