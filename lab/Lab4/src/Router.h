#ifndef ROUTER_H
#define ROUTER_H
#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
using namespace std;

class Router
{
public:
    Router();
    ~Router();
    void parseGMP(ifstream& gmp_file);
    void parseGCL(ifstream& gcl_file);
    void parseCST(ifstream& cst_file);

private:
    Chip chip1, chip2;
    vector<vector<double>> grid_costM1, grid_costM2;
    vector<Net> nets;

    double Alpha, Beta, Gamma, Delta;
    double ViaCost;
    int GMP_LLX, GMP_LLY, GMP_WIDTH, GMP_HEIGHT;
    int GRID_W, GRID_H;
    
};

#endif