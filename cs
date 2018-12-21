#!/usr/bin/perl
#
# Copyright (c) 2014-2019 Robert A. Alfieri
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# C++ Scripting Interpreter - compiles your .cs file using g++ and runs it
#
my $cs_name   = shift @ARGV;
my @lines     = `cat $cs_name`;
$lines[0] =~ /^#\!/ and shift @lines;
my $cpp_name  = $cs_name . ".cpp";
open( CPP, ">$cpp_name" ) or die "ERROR: could not open $cpp_name for writing\n";
print CPP join( "", @lines );
close( CPP );
my $exe_name  = $cs_name . ".exe";
my $CFLAGS = "-std=c++17 -O3 -Werror -Wextra -Wstrict-aliasing -pedantic -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-include-dirs  -Woverloaded-virtual -Wredundant-decls -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -g";
system( "g++ ${CFLAGS} -I../ -o $exe_name $cpp_name" ) == 0 or die "ERROR: compile of $cs_name failed\n";
system( "./${exe_name}" );
