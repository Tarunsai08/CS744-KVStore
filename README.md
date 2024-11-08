# KV Server

This server implements a high-performance Key-Value (KV) store using C++. It integrates a RESTful interface, a thread-safe LRU Cache, and a concurrent PostgreSQL Connection Pool for persistent storage.

## Features Added 

* **HTTP API:** Standard endpoints for PUT, GET, and DELETE operations.
* **Caching:** Uses a thread-safe **LRU Cache** for fast in-memory lookups (Capacity: 5000).
* **Persistence:** Data is permanently stored in a **PostgreSQL** database.
* **Concurrency:** Employs a **PostgreSQL Connection Pool** (size 20) and `httplib`'s thread pool for scalable handling of concurrent requests.

## Setup and Compilation

1.  **Database:**
    * Ensure PostgreSQL is running.
    * Create the `kvstore` database.
    * Create the necessary table:
        ```sql
        CREATE TABLE kv_store (
            key INT PRIMARY KEY,
            value varchar(200)
        );
        ```

2.  **Files:** The project is modularized into: `main.cpp`, `db.cpp`, `db.h`, and `cache.h`.

3.  **Compilation Command:** Compile all implementation files and link the required libraries.

    ```bash
    g++ main.cpp db.cpp -o kv_server -lpq -lpthread
    ```

---

## Usage

1.  **Start Server:**

    ```bash
    ./kv_server
    ```

2.  **API Endpoints:**

    | Method | Endpoint | Query Parameter | Body | Action |
    | :--- | :--- | :--- | :--- | :--- |
    | POST | `/kv` | `?key=<key>` | `<value>` | Create/Update. |
    | GET | `/kv` | `?key=<key>` | | Retrieve. |
    | DELETE | `/kv` | `?key=<key>` | | Delete. |

3.  **Example (using curl):**

    * **PUT:**
        ```bash
        curl -X POST "http://localhost:8080/kv?id=1" -d "test_value"
        ```
    * **GET:**
        ```bash
        curl "http://localhost:8080/kv?id=1"
        ```