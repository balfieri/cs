#!/usr/bin/perl
#
# C++ Scripting Interpreter - compiles your .cs file using g++ and runs it
#
# If your C++ file is called foo.cpp, you can run it as a script using:
#
#     cs foo
# 
# Or you can just compile it yourself and run it.  Either way works.
#
use strict;
use warnings;

use FindBin;
my $bin = $FindBin::RealBin;  # full path to this script

my $cpp_name   = shift @ARGV;
$cpp_name !~ /\.cpp$/ and $cpp_name .= ".cpp";
-f $cpp_name or die "ERROR: could not find file $cpp_name\n";
my $exe_name = $cpp_name;
$exe_name =~ s/\.cpp$//;

my $CFLAGS = "-std=c++17 -O3 -Werror -Wextra -Wstrict-aliasing -pedantic -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-include-dirs  -Woverloaded-virtual -Wredundant-decls -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -g " . (defined $ENV{CS_CFLAGS} ? $ENV{CS_CFLAGS} : "");

my $cmd = "g++ ${CFLAGS} -I \"${bin}\" -o $exe_name $cpp_name";
system( $cmd ) == 0 or die "ERROR: compile of $cpp_name failed\n";
$cmd = "./${exe_name} " . join( " ", @ARGV );
system( $cmd ) == 0 or die "ERROR: command failed: $cmd\n";
