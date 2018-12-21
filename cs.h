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
    ~val();

    operator bool( void ) const;
    operator int64_t( void ) const;
    operator double( void ) const;
    operator std::string( void ) const;

    operator = ( const bool x );
    operator = ( const int64_t x );
    operator = ( const double x );
    operator = ( std::string x );
    operator = ( const val& other );

    val& push( const val& x );
    val  shift( void );

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
constexpr val undef;
inline    val list( void )                                      { return val::list(); }
inline    val list( int64_t cnt, const char * args[] )          { return val::list(); }
inline    val map( void )                                       { return val::map();  }

// Val x = func({ })
// Val x = open(“file”, “w”)
// Val x = y.matches( “regexp” )
// Val x = ptr(obj)
// Val x = thread( f, arg )
// Val x = threads( n, f, arg )
// Val x = pipepair(“app”)
// Val x = output(“app”)
// Val x = system(“app”)
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
