#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector> // Required for std::vector in TestCopyMove
#include "containers/lists.h"
#include "containers/stack.h"

using namespace std;

using StackInt = CStack< StackTraits<T1> >;
using StackString = CStack< StackTraits<string> >;

static void TestPushPop(ostream &log) {
    StackInt st;
    st.push(10, 1);
    st.push(20, 2);
    st.push(30, 3);

    T1 v1 = st.pop();
    T1 v2 = st.pop();
    T1 v3 = st.pop();

    assert(v1 == 30);
    assert(v2 == 20);
    assert(v3 == 10);

    log << "TestPushPop: OK" << endl;
}

static void TestPopEmptyThrows(ostream &log) {
    StackInt st;
    bool threw = false;
    try {
        (void)st.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    log << "TestPopEmptyThrows: OK" << endl;
}

static void TestStreamRoundTrip(ostream &log) {
    StackInt st;
    st.push(5, 50);
    st.push(7, 70);
    st.push(9, 90);

    ostringstream os;
    os << st;

    istringstream is(os.str());
    StackInt st2;
    is >> st2;

    T1 v1 = st2.pop();
    T1 v2 = st2.pop();
    T1 v3 = st2.pop();

    assert(v1 == 9);
    assert(v2 == 7);
    assert(v3 == 5);

    log << "TestStreamRoundTrip: OK" << endl;
}

static void TestStringStackPushPop(ostream &log) {
    StackString st;
    st.push("hello", 1);
    st.push("world", 2);
    st.push("gemini", 3);

    string s1 = st.pop();
    string s2 = st.pop();
    string s3 = st.pop();

    assert(s1 == "gemini");
    assert(s2 == "world");
    assert(s3 == "hello");

    log << "TestStringStackPushPop: OK" << endl;
}

static void TestStringStackStreamRoundTrip(ostream &log) {
    StackString st;
    st.push("apple", 10);
    st.push("banana", 20);
    st.push("cherry", 30);

    ostringstream os;
    os << st;

    istringstream is(os.str());
    StackString st2;
    is >> st2;

    string s1 = st2.pop();
    string s2 = st2.pop();
    string s3 = st2.pop();

    assert(s1 == "cherry");
    assert(s2 == "banana");
    assert(s3 == "apple");

    log << "TestStringStackStreamRoundTrip: OK" << endl;
}

static void TestStackCopyMove(ostream &log) {
    // Test Copy Constructor
    StackInt original;
    original.push(1, 10);
    original.push(2, 20);
    original.push(3, 30);

    StackInt copied(original);
    log << copied << endl;
    assert(copied.pop() == 3);
    assert(copied.pop() == 2);
    assert(copied.pop() == 1);
    bool threw = false;
    try {
        (void)copied.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    
    // Ensure original is untouched
    assert(original.pop() == 3);
    assert(original.pop() == 2);
    assert(original.pop() == 1);
    try {
        (void)original.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);
    
    log << "TestStackCopyMove: Copy Constructor OK" << endl;

    // Test Copy Assignment Operator
    StackInt assign_src;
    assign_src.push(10, 100);
    assign_src.push(20, 200);

    StackInt assign_dest;
    assign_dest.push(5, 50);
    assign_dest = assign_src;

    assert(assign_dest.pop() == 20);
    assert(assign_dest.pop() == 10);
    try {
        (void)assign_dest.pop();
    } catch (const exception &) {
        threw = true;
    }
    assert(threw);

    // Ensure source is untouched
    assert(assign_src.pop() == 20);
    assert(assign_src.pop() == 10);
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
    log << "TestStackCopyMove: Copy Assignment Operator OK" << endl;

    // Test Move Constructor
    StackInt move_src;
    move_src.push(100, 1);
    move_src.push(200, 2);
    move_src.push(300, 3);

    StackInt moved_dest(std::move(move_src));
    assert(moved_dest.pop() == 300);
    assert(moved_dest.pop() == 200);
    assert(moved_dest.pop() == 100);
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
    log << "TestStackCopyMove: Move Constructor OK" << endl;

    // Test Move Assignment Operator
    StackInt move_assign_src;
    move_assign_src.push(1000, 1);
    move_assign_src.push(2000, 2);

    StackInt move_assign_dest;
    move_assign_dest.push(50, 5);
    move_assign_dest = std::move(move_assign_src);

    assert(move_assign_dest.pop() == 2000);
    assert(move_assign_dest.pop() == 1000);
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
    log << "TestStackCopyMove: Move Assignment Operator OK" << endl;
}


void DemoStack() {
    ofstream logFile("stack_tests.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    TestStringStackPushPop(logFile);
    TestStringStackStreamRoundTrip(logFile);
    TestStackCopyMove(logFile); // Added this line
    logFile << "DemoStack: Done" << endl;
}
