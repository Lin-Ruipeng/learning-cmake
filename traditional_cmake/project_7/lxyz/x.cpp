#include <iostream>
#include "child.h"

void x()
{
#ifdef DEBUG
    std::cout << "*DEBUG: x.cpp catch the define!" << std::endl;
#endif
    std::cout << "This is x.cpp" << std::endl;
    
    std::cout << "The x will call child!" << std::endl;
    child();
    std::cout << "---" << std::endl;

}

