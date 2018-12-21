#include "cs.h"

int main( int argc, const char * argv[] )
{
    val args = val::list( argc, argv );
    std::cout << "Hello! Command line args were: " << args.join() << "\n";
    return 0;
}
