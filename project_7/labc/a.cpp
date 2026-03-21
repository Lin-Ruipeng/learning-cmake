#include <iostream>

void a()
{
#ifdef DEBUG
    std::cout << "*DEBUG: a.cpp catch the define!" << std::endl;
#endif
    std::cout << "This is a.cpp" << std::endl;
}

