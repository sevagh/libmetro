#include "libjungle.h"
#include <iostream>
#include <stdio.h>

void jungle::eventloop()
{
	std::cout << "press ctrl-c to exit" << std::endl;
	getchar();
}
