#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>

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

void DemoStack() {
    ofstream logFile("stack_test.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    TestStringStackPushPop(logFile);
    TestStringStackStreamRoundTrip(logFile);
    logFile << "DemoStack: Done" << endl;
}
