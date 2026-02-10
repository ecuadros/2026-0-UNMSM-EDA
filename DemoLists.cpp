#include <iostream>
#include <sstream>
#include "containers/array.h"
#include "containers/doublelinkedlist_circular.h"
#include "foreach.h"

template <typename T>
void Print(const T& x) {
    std::cout << x << ",";
}

bool Mult7(const int& x) {
    return x % 7 == 0;
}

void DemoLists() {
    using TraitsInt = Trait1<int>;

    DoubleLinkedListCircular<TraitsInt> c;

    c.push_back(10);
    c.push_back(14);
    c.push_back(21);
    c.push_back(8);

    std::cout << c << "\n";

    std::cout << "Foreach circular: ";
    ::Foreach(c, &Print<int>);
    std::cout << "\n";

    auto it = ::FirstThat(c, &Mult7);
    if (it != c.end())
        std::cout << "FirstThat mult7: " << *it << "\n";

    std::stringstream ss("5 3 6 9 12 15");
    ss >> c;

    std::cout << c << "\n";
}
