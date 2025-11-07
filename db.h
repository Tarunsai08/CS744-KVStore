#ifndef DB_H
#define DB_H

#include <string>

int db_put(int key, const std::string &value);
std::string db_get(int key);
bool db_del(int key);

#endif