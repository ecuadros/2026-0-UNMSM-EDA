#include <assert.h>
#include <fstream>
#include <sstream>
#include "containers/queue.h"
#include "containers/lists.h"

using namespace std;

using QueueInt = CQueue< QueueTraits<T1> >;
using QueueString = CQueue< QueueTraits<string> >;

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

static void TestStringQueuePushPop(ostream &log) {
    QueueString q;
    q.push("first", 1);
    q.push("second", 2);
    q.push("third", 3);

    string s1 = q.pop();
    string s2 = q.pop();
    string s3 = q.pop();

    assert(s1 == "first");
    assert(s2 == "second");
    assert(s3 == "third");

    log << "TestStringQueuePushPop: OK" << endl;
}

static void TestStringQueueStreamRoundTrip(ostream &log) {
    QueueString q;
    q.push("alpha", 10);
    q.push("beta", 20);
    q.push("gamma", 30);

    ostringstream os;
    os << q;

    istringstream is(os.str());
    QueueString q2;
    is >> q2;

    string s1 = q2.pop();
    string s2 = q2.pop();
    string s3 = q2.pop();

    assert(s1 == "alpha");
    assert(s2 == "beta");
    assert(s3 == "gamma");

    log << "TestStringQueueStreamRoundTrip: OK" << endl;
}

void DemoQueue() {
    ofstream logFile("queue_test.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    TestStringQueuePushPop(logFile);
    TestStringQueueStreamRoundTrip(logFile);
    logFile << "DemoQueue: Done" << endl;
}
