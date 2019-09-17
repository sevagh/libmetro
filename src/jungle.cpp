#include <stdio.h>
#include <iostream>
#include "libjungle.h"

void jungle::eventloop() {
  std::cout << "press ctrl-c to exit" << std::endl;
  getchar();
}
