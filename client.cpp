#include "httplib.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    httplib::Client cli("http://127.0.0.1:8080");

    while (true) {
        cout << "\n1) Set\n2) Get\n3) Delete\n4) Exit\nChoice: ";
        int ch; cin >> ch;
        if (ch == 4) break;

        int key;
        string value;
        cin.ignore();
        if (ch == 1) {
            cout << "Enter key (int): ";
            cin >> key;
            cin.ignore();
            cout << "Enter value (string): ";
            getline(cin, value);

            auto res = cli.Post(("/kv?key=" + to_string(key)).c_str(), value, "text/plain");
            if (res) cout << res->body;
            else cout << "Server not reachable\n";
        }

        else if (ch == 2) {
            cout << "Enter key: ";
            cin >> key;

            auto res = cli.Get(("/kv?key=" + to_string(key)).c_str());
            if (res) cout << "Response: " << res->body << endl;
            else cout << "Server not reachable\n";
        }

        else if (ch == 3) {
            cout << "Enter key: ";
            cin >> key;

            auto res = cli.Delete(("/kv?key=" + to_string(key)).c_str());
            if (res) cout << res->body;
            else cout << "Server not reachable\n";
        }

        else {
            cout << "Invalid choice\n";
        }
    }

    return 0;
}