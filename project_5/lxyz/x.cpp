#include <iostream>

void x()
{
#ifdef DEBUG
    std::cout << "*DEBUG: x.cpp catch the define!" << std::endl;
#endif
    std::cout << "This is x.cpp" << std::endl;
}

