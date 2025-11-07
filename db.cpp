#include "db.h" 
#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <libpq-fe.h>
#include <queue>
#include <condition_variable>
#include <memory>
#include <list>

using namespace std;

#define POOL_SIZE 20

class PGPool {
private:
    vector<PGconn*> pool;
    mutex mtx;
    condition_variable cv;

public:
    PGPool(int n) {
        pool.reserve(n);
        for (int i = 0; i < n; i++) {
            PGconn *c = PQconnectdb("host=localhost port=5432 dbname=kvstore user=postgres");
            if (PQstatus(c) != CONNECTION_OK) {
                cerr << "DB Connection failed: " << PQerrorMessage(c) << endl;
                exit(1);
            }
            prepare_statements(c);
            pool.push_back(c);
        }
    }

    static void prepare_statements(PGconn *c) {
        PQprepare(c, "put_stmt",
            "INSERT INTO kv_store (key, value) VALUES ($1, $2) "
            "ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value;",
            2, NULL);

        PQprepare(c, "get_stmt",
            "SELECT value FROM kv_store WHERE key = $1;",
            1, NULL);

        PQprepare(c, "del_stmt",
            "DELETE FROM kv_store WHERE key = $1;",
            1, NULL);
    }

    PGconn* acquire() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]{ return !pool.empty(); });
        PGconn *c = pool.back(); pool.pop_back();
        return c;
    }

    void release(PGconn* c) {
        unique_lock<mutex> lock(mtx);
        pool.push_back(c);
        lock.unlock();
        cv.notify_one();
    }

    ~PGPool() {
        for (auto &c : pool) PQfinish(c);
    }
};

PGPool dbpool(POOL_SIZE);

int db_put(int key, const string &value) {
    PGconn* c = dbpool.acquire();

    const char* params[2];
    int lengths[2];
    int formats[2] = {0, 1}; 

    string skey = to_string(key);
    params[0] = skey.c_str();
    params[1] = value.data();
    lengths[1] = value.size();

    PGresult *res = PQexecPrepared(c, "put_stmt", 2, params, lengths, formats, 0);
    bool ok = PQresultStatus(res) == PGRES_COMMAND_OK;

    PQclear(res);
    dbpool.release(c);
    return ok;
}

string db_get(int key) {
    PGconn *c = dbpool.acquire();

    string skey = to_string(key);
    const char* params[1] = { skey.c_str() };
    PGresult *res = PQexecPrepared(c, "get_stmt", 1, params, NULL, NULL, 1);

    string value = "";
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        int len = PQgetlength(res, 0, 0);
        value.assign(PQgetvalue(res, 0, 0), len);
    }

    PQclear(res);
    dbpool.release(c);
    return value;
}

bool db_del(int key) {
    PGconn *c = dbpool.acquire();

    cout << "hi";
    string skey = to_string(key);
    const char* params[1] = { skey.c_str() };
    PGresult *res = PQexecPrepared(c, "del_stmt", 1, params, NULL, NULL, 0);

    bool ok = (atoi(PQcmdTuples(res)) > 0);
    PQclear(res);
    dbpool.release(c);
    return ok;
}