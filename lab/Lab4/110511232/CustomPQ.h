#include"DEBUG.h"
#include<iostream>

using namespace std;

template<typename T>
class CustomPQ
{
public:
    CustomPQ();
    ~CustomPQ();
    void push(const T& val);
    void pop();
    T top();
    bool empty();
    bool find(const T& val);

private:
    
};