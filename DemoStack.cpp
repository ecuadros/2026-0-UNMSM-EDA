#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>

#include "containers/lists.h"
#include "containers/stack.h"

using namespace std;

using StackInt = CStack< StackTraits<int> >;

static void TestPushPop(ostream &log) {
    StackInt st;
    st.push(10, 1);
    st.push(20, 2);
    st.push(30, 3);

    int v1 = st.pop();
    int v2 = st.pop();
    int v3 = st.pop();

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

    int v1 = st2.pop();
    int v2 = st2.pop();
    int v3 = st2.pop();

    assert(v1 == 9);
    assert(v2 == 7);
    assert(v3 == 5);

    log << "TestStreamRoundTrip: OK" << endl;
}

void DemoStack() {
    ofstream logFile("stack_test.log");
    TestPushPop(logFile);
    TestPopEmptyThrows(logFile);
    TestStreamRoundTrip(logFile);
    logFile << "DemoStack: Done" << endl;
}
