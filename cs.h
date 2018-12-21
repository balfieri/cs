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
// cs.h - C++ Scripting
//
#ifndef __cs_h
#define __cs_h

#include <cmath>
#include <iostream>
#include <iomanip>

// dynamically-typed value
//
class val
{
public:
    enum class KIND
    {
        UNDEF,
        BOOL,
        INT,
        FLT,
        STR,
        LIST,
        MAP,
        FUNC,
        FILE,
        THREAD,
    };

    // constructors
    val( void );
    val( bool x );
    val( int64_t x );
    val( double x );
    val( const char * s );
    val( std::string s );
    val( const val& x );
    static val list( void );
    static val list( int64_t cnt, const char * args[] );
    static val map( void );
    ~val();

    operator bool( void ) const;
    operator int64_t( void ) const;
    operator double( void ) const;
    operator std::string( void ) const;

    val& operator = ( const bool x );
    val& operator = ( const int64_t x );
    val& operator = ( const double x );
    val& operator = ( std::string x );
    val& operator = ( const val& other );

    val& push( const val& x );
    val  shift( void );
    val  split( const val delim = "" );
    val  join( const val delim = "" );

private:
    KIND                kind;
    union
    {
        bool            b;
        int64_t         i;
        double          f;
        std::string     s;
        val *           l;
    } u;
};

// Misc Functions
//
const  val undef;
inline val list( void )                                      { return val::list(); }
inline val list( int64_t cnt, const char * args[] )          { return val::list( cnt, args ); }
inline val map( void )                                       { return val::map();  }

// makekind(SomeObject)
// val x = new SomeObject()
// SomeObject * y = x;
// val m = module( "name" )
// val x = kind( k )
// val x = func( f )
// val x = func( "code" )
// val x = file(“file”, “w”)
// val x = file( "w" )
// val x = y.matches( “regexp” )
// val x = ptr(obj)
// val x = thread( f, arg )
// x.join()
// val x = threads( n, f, arg )
// val x = process()            // fork
// val x = process( x, ... )    // fork+exec of file name or file
// val y = run( x )             // 
// val x = pipepair(“app”)
// val x = output(“app”)
// val x = system(“app”)
// x >> y
// x << y
// x[‘dfhf’] = rr
// foreach(e, x) ...

//-----------------------------------------------------
//-----------------------------------------------------
// 
// IMPLEMENTATION  IMPLEMENTATION  IMPLEMENTATION
//
//-----------------------------------------------------
//-----------------------------------------------------

#endif // __cs_h
