#include "cs.h"

int main( int argc, const char * argv[] )
{
    val args = val::list( argc, argv );
    std::cout << "Command line args: " << args << "\n";
    return 0;
}
