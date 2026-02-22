#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector> // Required for std::vector in TestCopyMove

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

static void TestQueueCopyMove(ostream &log) {
    // Test Copy Constructor
    QueueInt original;
    original.push(1, 10);
    original.push(2, 20);
    original.push(3, 30);

    QueueInt copied(original);
    assert(copied.pop() == 1);
    assert(copied.pop() == 2);
    assert(copied.pop() == 3);
    bool threw = false;
    try {
        (void)copied.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    
    // Ensure original is untouched
    assert(original.pop() == 1);
    assert(original.pop() == 2);
    assert(original.pop() == 3);
    try {
        (void)original.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    
    log << "TestQueueCopyMove: Copy Constructor OK" << endl;

    // Test Copy Assignment Operator
    QueueInt assign_src;
    assign_src.push(10, 100);
    assign_src.push(20, 200);

    QueueInt assign_dest;
    assign_dest.push(5, 50);
    assign_dest = assign_src;

    assert(assign_dest.pop() == 10);
    assert(assign_dest.pop() == 20);
    try {
        (void)assign_dest.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);

    // Ensure source is untouched
    assert(assign_src.pop() == 10);
    assert(assign_src.pop() == 20);
    try {
        (void)assign_src.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);

    // Test Self Assignment
    assign_src.push(1,1);
    assign_src = assign_src;
    assert(assign_src.pop() == 1);
    log << "TestQueueCopyMove: Copy Assignment Operator OK" << endl;

    // Test Move Constructor
    QueueInt move_src;
    move_src.push(100, 1);
    move_src.push(200, 2);
    move_src.push(300, 3);

    QueueInt moved_dest(std::move(move_src));
    assert(moved_dest.pop() == 100);
    assert(moved_dest.pop() == 200);
    assert(moved_dest.pop() == 300);
    try {
        (void)moved_dest.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);

    // Ensure source is empty
    try {
        (void)move_src.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    log << "TestQueueCopyMove: Move Constructor OK" << endl;

    // Test Move Assignment Operator
    QueueInt move_assign_src;
    move_assign_src.push(1000, 1);
    move_assign_src.push(2000, 2);

    QueueInt move_assign_dest;
    move_assign_dest.push(50, 5);
    move_assign_dest = std::move(move_assign_src);

    assert(move_assign_dest.pop() == 1000);
    assert(move_assign_dest.pop() == 2000);
    try {
        (void)move_assign_dest.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);

    // Ensure source is empty
    try {
        (void)move_assign_src.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    log << "TestQueueCopyMove: Move Assignment Operator OK" << endl;
}


void DemoQueue() {
    ofstream logFile("queue_tests.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    TestStringQueuePushPop(logFile);
    TestStringQueueStreamRoundTrip(logFile);
    TestQueueCopyMove(logFile); // Added this line
    logFile << "DemoQueue: Done" << endl;
}
