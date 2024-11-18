#include "DEBUG.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include "Inst.h"
#include <iomanip>
using namespace std;

ostream &operator<<(ostream &os, const Inst &inst)
{
    os << inst.getName() << " " << (long)inst.getX() << " " << (long)inst.getY() << " " << (long)inst.getWidth() << " " << (long)inst.getHeight() << " " << ((inst.getFixed()) ? "FIX" : "NOTFIX");
    return os;
}