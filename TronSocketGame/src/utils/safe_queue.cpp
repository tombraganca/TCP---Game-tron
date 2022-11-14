#include <thread>
#include <mutex>
#include <list>
#include <string>

using namespace std;

class safe_queue {
    mutex m;
    list<string> str_queue;
public:
    safe_queue() {};
    void add(const string &s) {
        const lock_guard<mutex> lock(m);
        str_queue.push_back(s);
    }

    bool pop( string &s ) {
        const lock_guard<mutex> lock(m);
        if (!str_queue.empty()) {
            s = str_queue.front();
            str_queue.pop_front();
            return true;
        }
        return false;
    }
};