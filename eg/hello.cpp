#include "cs.h"

int main( int argc, const char * argv[] )
{
    val args = val::list( argc, argv );
    std::cout << "Hello, world!\nCommand line: " << args.join() << "\n";
    return 0;
}
