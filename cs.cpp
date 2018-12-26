// Copyright (c) 2014-2019 Robert A. Alfieri
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// cs.cpp - C++ Scripting Interpreter
//
// There's really not much to this.  You build this once using doit.build
// in this directory.  Then put this directory on your PATH.
// If you put a line like this at the top of a .cpp script and make the file
// executable, you can simply run the .cpp file as you would any script:
//
// #!cs
//
#include "cs.h"                 // this interpreter is also a C++ script

using std::cout;

int main( int argc, const char * argv[] )
{
    csassert( argc > 1, "current usage: cs <basename>" );
    val cs_dir   = val::exe_path_dir();
    val exe_name = argv[1];
    val cpp_name = exe_name + ".cpp";
    csassert( cpp_name.path_exists(), val("cpp file ") + cpp_name + " does not exist" );
    val CFLAGS   = val( " -std=c++17 -O3 -Werror -Wextra -Wstrict-aliasing -pedantic" ) +
                   " -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization" +
                   " -Wformat=2 -Winit-self -Wmissing-include-dirs  -Woverloaded-virtual -Wredundant-decls -Wsign-promo" +
                   " -Wstrict-overflow=5 -Wswitch-default -Wundef -g" +
                   " -I \"" + cs_dir + "\"";
    val cmd = val("g++ ") + CFLAGS + " -o " + exe_name + " " + cpp_name;
    if ( cmd.run() != val(0) ) die( "build failed" );
    cmd = val("./") + exe_name;
    if ( cmd.run() != val(0) ) die( "run failed" );
    return 0;
}
