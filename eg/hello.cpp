// eg/hello.cpp
//
#include "cs.h"

using std::cout;

int main( int argc, const char * argv[] )
{
    cout << "Hello, world!\n";

    cout << "Full path to this executable: "        << val::exe_path()       << "\n";
    cout << "Arguments passed to this executable: " << val( argc-1, argv+1 ) << "\n";
    return 0;
}
