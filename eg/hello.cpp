// eg/hello.cpp
//
#include "cs.h"

using std::cout;

int main( int argc, const char * argv[] )
{
    val exe  = val::exe_path();
    val args = val::list( argc-1, argv+1 );
    cout << "Hello, world!\n";
    cout << "Full path to this executable: "        << exe << "\n";
    cout << "Arguments passed to this executable: " << args << "\n";
    return 0;
}
