#include <assert.h>
#include <fstream>
#include <sstream>
#include "containers/lists.h"

using namespace std;

using QueueInt = CQueue< QueueTraits<T1> >;

static void TestPushPop(ostream &log) {
    QueueInt q;
    q.push(10, 1);
    q.push(20, 2);
    q.push(30, 3);

    T1 v1 = q.pop();
    T1 v2 = q.pop();
    T1 v3 = q.pop();

    assert(v1 == 10);
    assert(v2 == 20);
    assert(v3 == 30);

    log << "TestPushPop: OK" << endl;
}

static void TestPopEmptyThrows(ostream &log) {
    QueueInt q;
    bool threw = false;
    try {
        (void)q.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    log << "TestPopEmptyThrows: OK" << endl;
}

static void TestStreamRoundTrip(ostream &log) {
    QueueInt q;
    q.push(5, 50);
    q.push(7, 70);
    q.push(9, 90);

    ostringstream os;
    os << q;

    istringstream is(os.str());
    QueueInt q2;
    is >> q2;

    T1 v1 = q2.pop();
    T1 v2 = q2.pop();
    T1 v3 = q2.pop();

    assert(v1 == 5);
    assert(v2 == 7);
    assert(v3 == 9);

    log << "TestStreamRoundTrip: OK" << endl;
}

void DemoQueue() {
    ofstream logFile("queue_test.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    logFile << "DemoQueue: Done" << endl;
}
