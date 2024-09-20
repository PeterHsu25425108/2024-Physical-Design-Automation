#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    ifstream in_file(argv[1]);
    ofstream out_file(argv[2]);

    // print the first line of in_file to out_file
    string line;
    getline(in_file, line);
    out_file << line << endl;

    in_file.close();
    out_file.close();

    return 0;
}