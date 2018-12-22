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
my $cpp_name   = shift @ARGV;
$cpp_name !~ /\.cpp$/ and $cpp_name .= ".cpp";
-f $cpp_name or die "ERROR: could not find file $cpp_name\n";
$exe_name = $cpp_name;
$exe_name =~ s/\.cpp$//;

my $CFLAGS = "-std=c++17 -O3 -Werror -Wextra -Wstrict-aliasing -pedantic -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-include-dirs  -Woverloaded-virtual -Wredundant-decls -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -g " . (defined $ENV{CS_CFLAGS} ? $ENV{CS_CFLAGS} : "");

system( "g++ ${CFLAGS} -I../ -o $exe_name $cpp_name" ) == 0 or die "ERROR: compile of $cpp_name failed\n";
my $cmd = "./${exe_name} " . join( " ", @ARGV );
system( $cmd ) == 0 or die "ERROR: command failed: $cmd\n";
