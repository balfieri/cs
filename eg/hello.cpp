#include "cs.h"

int main( int argc, const char * argv[] )
{
    val args = list( argc, argv );
    std::cout << "hello\n";
    std::cout << "Command line: " << args.join() << "\n";
    return 0;
}
