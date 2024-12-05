#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
#include"Router.h"
using namespace std;

int main(int argc, char* argv[]) 
{
    ifstream gmp_file(argv[1]);
    ifstream gcl_file(argv[2]);
    ifstream cst_file(argv[3]);
    ofstream out_file(argv[4]);

    Router router;

    // test parsing
    router.parseGMP(gmp_file);
    router.parseGCL(gcl_file);
    router.parseCST(cst_file);

    gmp_file.close();
    gcl_file.close();
    cst_file.close();
    out_file.close();

    return 0;
}