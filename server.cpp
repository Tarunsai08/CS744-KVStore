#include "httplib.h"
#include "cache.h" 
#include "db.h" 

#include <iostream>
#include <string>

using namespace std;
using namespace httplib;

#define CACHE_SIZE 1000
#define POOL_SIZE 32

int main() {
    LRUCache cache(CACHE_SIZE);
    Server server;

    server.Post("/kv", [&](const Request &req, Response &res) {
        if (!req.has_param("key")) { res.status = 400; res.set_content("Missing key", "text/plain"); return; }

        int key = stoi(req.get_param_value("key"));
        string value = req.body;

        cache.put(key, value);
        db_put(key, value); 

        res.set_content("OK", "text/plain");
    });

    server.Get("/kv", [&](const Request &req, Response &res) {
        if (!req.has_param("key")) { res.status = 400; res.set_content("Missing Key", "text/plain"); return; }
        int key = stoi(req.get_param_value("key"));

        string value = cache.get(key);
        if (value == "") {
            value = db_get(key); 
            if (value == "") { res.status = 404; res.set_content("Key not found", "text/plain"); return; }
            cache.put(key, value);
        }

        res.set_content(value, "text/plain");
    });

    server.Delete("/kv", [&](const Request &req, Response &res) {
        if (!req.has_param("key")) { res.status = 400; res.set_content("Missing key", "text/plain"); return; }
        int key = stoi(req.get_param_value("key"));

        bool db_deleted = db_del(key);

        if (db_deleted) {
            res.set_content("Deleted", "text/plain");
            bool cache_deleted = cache.del(key);
        } else {
            res.status = 404;
            res.set_content("Key not found", "text/plain");
        }
    });

    server.new_task_queue = [] { return new httplib::ThreadPool(POOL_SIZE); };
    server.set_keep_alive_max_count(100);

    cout << "KV Server running at http://localhost:8080" << endl;
    server.listen("0.0.0.0", 8080);

    return 0;
}