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
// The main theme here is to use dynamic typing in the form of the "val" class.
// Everything is a val.  There are some built-in val types.  There's also 
// a CustomVal type that can be used to add new val types.
// 
#ifndef __cs_h
#define __cs_h

#include <string>
#include <cmath>
#include <initializer_list>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <regex>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libproc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
// Proxy used by the [] operator to distinguish get() vs. set()
//
class val;

class ValProxy
{
public:
    ValProxy( val * v, const val * key ) : v(v), key(key) {}
    operator const val&( void ) const;                                
    void operator = ( const val& other );                      

private:
    val *       v;
    const val * key;
};

// dynamically-typed value
//
class CustomVal;

//---------------------------------------------------------------------
// This is the main dynamic value type
//---------------------------------------------------------------------
class val
{
public:
    // constructors
    val( void );
    val( bool x );
    val( int64_t x );
    val( int x );
    val( float x );
    val( double x );
    val( const char * x );
    val( std::string x );
    val( const val& x );
    val( CustomVal * x );
    val( int64_t cnt, const char * args[] );                    // for turning main() argc and argv into a list
    val( std::initializer_list<val> vals );                     // val{ a, b, c } returns a list containing a, b, c
   
    static val list( void );                                    // same as val{} which is the preferred way
   
    static val map( void );
    static val map( val& key_val_list );                        // flattened list of: key0, val0, key1, val1, ...

    static val file( const val& name, const val& options );     // creates or opens named file or pipe; option characters: rwcp
    static val file( const val& options );                      // creates temporary file or pipe

    static val func( val (*f)( const val& args ) );
    static val func( const val& code );

    static val thread(  val (*f)( const val& args ), const val& args );
    static val threads( const val& thr_cnt, val (*f)( const val& thr_index, const val& args ), const val& args );

    ~val();

    // introspection
    bool        defined( void ) const;
    std::string kind( void ) const;

    // conversions from val to common types
    operator bool( void ) const;
    operator int64_t( void ) const;
    operator double( void ) const;
    operator std::string( void ) const;
    operator CustomVal&( void ) const;

    val  operator -  ( void ) const;
    val  operator ~  ( void ) const;
    bool operator !  ( void ) const;

    #define _decl_op2( ret, op ) \
    ret  operator op ( const val& x ) const;                                                    \
    ret  operator op ( const int64_t x ) const			{ return *this op val( x ); }	\
    ret  operator op ( const int x ) const			{ return *this op val( x ); }	\
    ret  operator op ( const double x ) const			{ return *this op val( x ); }	\
    ret  operator op ( const float x ) const			{ return *this op val( x ); }	\
    ret  operator op ( const char * x ) const			{ return *this op val( x ); }	\
    ret  operator op ( std::string x ) const		        { return *this op val( x ); }	\

    #define _decl_aop2( op ) \
    val& operator op ( const val& x );                                                          \
    val& operator op ( const int64_t x )      			{ return *this op val( x ); }	\
    val& operator op ( const int x )      			{ return *this op val( x ); }	\
    val& operator op ( const double x )      			{ return *this op val( x ); }	\
    val& operator op ( const float x )      			{ return *this op val( x ); }	\
    val& operator op ( const char * x )      			{ return *this op val( x ); }	\
    val& operator op ( std::string x )      		        { return *this op val( x ); }	\
    val& operator op ( CustomVal * x )      		        { return *this op val( x ); }	\

    // meaning depends on the underlying type:
    _decl_op2( val,  +  )
    _decl_op2( val,  -  )
    _decl_op2( val,  *  )
    _decl_op2( val,  /  )
    _decl_op2( val,  %  )
    _decl_op2( val,  << )
    _decl_op2( val,  >> )
    _decl_op2( val,  &  )
    _decl_op2( bool, && )
    _decl_op2( val,  |  )
    _decl_op2( bool, || )
    _decl_op2( val,  ^  )
    _decl_op2( bool, <  )
    _decl_op2( bool, <= )
    _decl_op2( bool, >  )
    _decl_op2( bool, >= )
    _decl_op2( bool, != )
    _decl_op2( bool, == )

    _decl_aop2( =  )
    _decl_aop2( += )
    _decl_aop2( -= )
    _decl_aop2( *= )
    _decl_aop2( /= )
    _decl_aop2( %= )
    _decl_aop2( <<= )                                                                        // if LHS is a LIST, then acts like push()
    _decl_aop2( >>= )                                                                        // if LHS is a LIST, then acts like shift()
    _decl_aop2( &= )
    _decl_aop2( |= )
    _decl_aop2( ^= )

    // string-only 
    //
    char       at( const val& i ) const;                                                     // return character at index i 

    // regular expressions - any val argument is first converted to a STR using std::string(), so be careful how you use this
    //
    // regex option characters: 
    //     i  == case insensitive
    //     j  == Javascript == ECMAScript        (default)
    //     p  == basic POSIX
    //     P  == extended POSIX
    //     a  == awk
    //     g  == grep
    //     G  == egrep
    //
    // regex fmt special escape sequences:
    //     $n == n-th backreference (i.e., a copy of the n-th matched group specified with parentheses in the regex pattern).
    //           n must be an integer value designating a valid backreference, greater than 0, and of two digits at most.
    //     $& == a copy of the entire match
    //     $` == the prefix (i.e., the part of the target sequence that precedes the match)
    //     $´ == the suffix (i.e., the part of the target sequence that follows the match)
    //     $$ == a single $ character
    //
    std::regex regex( const val& options="" ) const;                                         // returns compiled std::regex this regex string and options string
    val        match( const val& re, const val& options="" ) const;                          // returns LIST with entire match and submatches; else returns UNDEF
    val        match( const std::regex& regex ) const;                                       // same but uses precompiled std::regex
    val        replace( const val& regex, const val& fmt, const val& options="" ) const;     // returns substituted string using std regex fmt; else returns unmodified string
    val        replace( const std::regex& regex, const val& fmt ) const;                     // same but uses precompiled std::regex
    val        replace_all( const val& regex, const val& fmt, const val& options="", uint64_t max=1000000000 ) const; // same as replace(), but replaces all occurances up to max count
    val        replace_all( const std::regex& regex, const val& fmt, uint64_t max=1000000000 ) const;                 // same but uses precompiled std::regex

    // list or map or string
    uint64_t   size( void ) const;                              // number of entries in list or map, or number of characters in STR

    // list or map 
    bool       exists( const val& key ) const;                  // returns true if key has a legal value in list/map
    const val& get( const val& key ) const;                     // read list/map using key 
    void       set( const val& key, const val& v );             // write list/map using key with v
    ValProxy   operator [] ( const val& key );                  // could be read or write depending on context (which ValProxy will help disambiguate)
    const val& operator [] ( const val& key ) const;            // read-only

    // list-only
    val&       push( const val& x );                            // push x to tail
    val        shift( void );                                   // pop head
    val        split( const val delim = " " ) const;            // split using delimiter
    val        join( const val delim = " " ) const;             // join  using delimiter

    // map-only 

    // function-only 
    val  operator () ( ... );                                   // call function with variable list of arguments

    // thread-only 
    val  join( void );                                          // join thread or threads; returns status or list of statuses

    // processes
    val  run( val options="" ) const;                           // run this path (must be a STR)
                                                                // options: ""                  - run sync;  return int status of run; process uses same stdin, stdout, stderr 
                                                                //          "so+se"             - run async; return one string holding stdout+stderr output
                                                                //          "lo+le"             - run async; return one list() holding separate lines for stdout+stderr output
                                                                //          "so,se"             - run async; return list of 2 strings holding stdout vs. stderr outputs 
                                                                //          "lo,le"             - run async; return list of 2 lists holding stdout vs. stderr lines
                                                                //          "o+e"               - run async; return one read-only file() for combined stdout+stderr
                                                                //          "o,e"               - run async; return list of 2 read-only file() for separate stdout and stderr
                                                                //          "i,o,e"             - run async; return list of 3 file() for separate stdin, stdout and stderr

    // paths
    val         path_dir( void ) const;                         // parent directory of path
    val         path_no_dir( void ) const;                      // path without the parent directory
    val         path_no_ext( void ) const;                      // path without the file extension
    int         path_stat( struct stat& stat ) const;           // do stat() system call on path
    bool        path_exists( void ) const;                      // returns true if path exists and caller can stat it
    bool        path_is_file( void ) const;                     // returns true if path is a plain file
    bool        path_is_link( void ) const;                     // returns true if path is a symbolic link
    bool        path_is_fifo( void ) const;                     // returns true if path is a pipe or FIFO
    bool        path_is_socket( void ) const;                   // returns true if path is a socket
    bool        path_is_dir( void ) const;                      // returns true if path is a directory
    time_t      path_time_modified( void ) const;               // returns time last modified (seconds since 1970)
    time_t      path_time_accessed( void ) const;               // returns time last accessed (seconds since 1970)
    static val  exe_path( void );                               // full path of current executable

    // list/map iterator
    class iterator: public std::iterator< std::input_iterator_tag,   // iterator_category
                                          val,                       // value_type
                                          int64_t,                   // difference_type
                                          const int64_t *,           // pointer
                                          const val&                 // reference
                                        >
    {
    public:
        inline explicit  iterator( val * _v, int64_t _pos )             { v = _v; pos = _pos;                                } 
        inline iterator& operator ++ ( void )                           { pos++; return *this;                               }
        inline iterator  operator ++ ( int )                            { iterator retval = *this; ++(*this); return retval; }
        inline bool      operator == ( const iterator& other ) const    { return pos == other.pos;                           }
        inline bool      operator != ( const iterator& other ) const    { return pos != other.pos;                           }
        inline reference operator *  ( void ) const                     { return (*v)[pos];                                  }
    private:
        val *   v;
        int64_t pos;
    };

    iterator begin( void )                                              { return iterator( this, 0 );                        }
    iterator end( void )                                                { return iterator( this, size()-1 );                 }
    #define foreach( it, x ) for( auto it = x.begin(); it != x.end(); it++ )






//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//
//  ___                 _                           _        _   _
// |_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __
//  | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \
//  | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
// |___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
//               |_|
//
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------

private:
    enum class kind
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
        PROCESS,
        CUSTOM,
    };

    static std::string kind_to_str( const enum kind& k );

    enum kind                   k;

    struct String
    {
        uint64_t                ref_cnt;
        std::string             s;
    };

    struct List
    {
        uint64_t                ref_cnt;
        std::vector<val>        l;
    };

    struct Map
    {
        uint64_t                ref_cnt;
        std::map<std::string,val> m;
    };

    union
    {
        bool                    b;
        int64_t                 i;
        double                  f;
        String *                s;
        List *                  l;
        Map *                   m;
        CustomVal *             c;
    } u;

    void free( void );
};

//---------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------
static const val undef;

//---------------------------------------------------------------------
// Stream I/O
//---------------------------------------------------------------------
static inline   std::ostream& operator << ( std::ostream &out, const val& x )
{
    out << std::string(x); 
    return out;
}

//---------------------------------------------------------------------
// Assertions
//---------------------------------------------------------------------
static inline void die( std::string msg )       
{ 
    std::cout << "ERROR: " << msg << "\n"; 
    exit( 1 ); 
}

#define csassert( expr, msg ) if ( !(expr) ) die( msg )

//---------------------------------------------------------------------
// CustomVal Val - allows you to extend val
//---------------------------------------------------------------------
class CustomVal
{
public:
    CustomVal( void )                                           { ref_cnt = 1; }
    virtual ~CustomVal()                                        { csassert( ref_cnt == 0, "trying to destroy a CustomVal val when ref_cnt is not 0" ); }

    virtual std::string kind( void ) const                      { return "CustomVal"; }
    virtual operator bool( void ) const                         { die( "no override available for CustomVal operator bool" );            return false; }
    virtual operator int64_t( void ) const                      { die( "no override available for CustomVal operator int64_t" );         return 0;     }
    virtual operator double( void ) const                       { die( "no override available for CustomVal operator double" );          return 0.0;   }
    virtual operator std::string( void ) const                  { die( "no override available for CustomVal operator std::string" );     return "";    }

    virtual CustomVal  operator -  ( void ) const               { die( "no override available for CustomVal operator -" );               return *this; } 
    virtual CustomVal  operator ~  ( void ) const               { die( "no override available for CustomVal operator ~" );               return *this; } 
    virtual bool       operator !  ( void ) const               { die( "no override available for CustomVal operator !" );               return false; }
    virtual CustomVal  operator +  ( const val& x ) const       { die( "no override available for CustomVal operator +" );      (void)x; return *this; } 
    virtual CustomVal  operator -  ( const val& x ) const       { die( "no override available for CustomVal operator -" );      (void)x; return *this; } 
    virtual CustomVal  operator *  ( const val& x ) const       { die( "no override available for CustomVal operator *" );      (void)x; return *this; } 
    virtual CustomVal  operator /  ( const val& x ) const       { die( "no override available for CustomVal operator /" );      (void)x; return *this; } 
    virtual CustomVal  operator %  ( const val& x ) const       { die( "no override available for CustomVal operator %" );      (void)x; return *this; } 
    virtual CustomVal  operator &  ( const val& x ) const       { die( "no override available for CustomVal operator &" );      (void)x; return *this; } 
    virtual bool       operator && ( const val& x ) const       { die( "no override available for CustomVal operator &&" );     (void)x; return *this; } 
    virtual CustomVal  operator |  ( const val& x ) const       { die( "no override available for CustomVal operator |" );      (void)x; return *this; } 
    virtual bool       operator || ( const val& x ) const       { die( "no override available for CustomVal operator ||" );     (void)x; return *this; } 
    virtual CustomVal  operator ^  ( const val& x ) const       { die( "no override available for CustomVal operator ^" );      (void)x; return *this; } 
    virtual CustomVal  operator << ( const val& x ) const       { die( "no override available for CustomVal operator <<" );     (void)x; return *this; }
    virtual CustomVal  operator >> ( const val& x ) const       { die( "no override available for CustomVal operator >>" );     (void)x; return *this; }
    virtual bool       operator <  ( const val& x ) const       { die( "no override available for CustomVal operator <" );      (void)x; return false; }
    virtual bool       operator <= ( const val& x ) const       { die( "no override available for CustomVal operator <=" );     (void)x; return false; }
    virtual bool       operator >  ( const val& x ) const       { die( "no override available for CustomVal operator >" );      (void)x; return false; }
    virtual bool       operator >= ( const val& x ) const       { die( "no override available for CustomVal operator >=" );     (void)x; return false; }
    virtual bool       operator != ( const val& x ) const       { die( "no override available for CustomVal operator !=" );     (void)x; return false; }
    virtual bool       operator == ( const val& x ) const       { die( "no override available for CustomVal operator ==" );     (void)x; return false; }

    virtual CustomVal& operator =  ( const val& x )             { die( "no override available for CustomVal operator =" );      (void)x; return *this; } 
    virtual CustomVal& operator += ( const val& x )             { die( "no override available for CustomVal operator +=" );     (void)x; return *this; } 
    virtual CustomVal& operator -= ( const val& x )             { die( "no override available for CustomVal operator -=" );     (void)x; return *this; } 
    virtual CustomVal& operator *= ( const val& x )             { die( "no override available for CustomVal operator *=" );     (void)x; return *this; } 
    virtual CustomVal& operator /= ( const val& x )             { die( "no override available for CustomVal operator /=" );     (void)x; return *this; } 
    virtual CustomVal& operator %= ( const val& x )             { die( "no override available for CustomVal operator %=" );     (void)x; return *this; } 
    virtual CustomVal& operator &= ( const val& x )             { die( "no override available for CustomVal operator &=" );     (void)x; return *this; } 
    virtual CustomVal& operator |= ( const val& x )             { die( "no override available for CustomVal operator |=" );     (void)x; return *this; } 
    virtual CustomVal& operator ^= ( const val& x )             { die( "no override available for CustomVal operator ^=" );     (void)x; return *this; } 

    virtual uint64_t   size( void ) const                       { die( "no override available for CustomVal size()" );                   return 0; }
    virtual bool       exists( const val& k ) const             { die( "no override available for CustomVal exists()" );        (void)k; return false; }
    virtual const val& get( const val& k ) const                { die( "no override available for CustomVal get()" );           (void)k; return undef; }
    virtual void       set( const val& k, const val& x )        { die( "no override available for CustomVal set()" );           (void)k; (void)x;      }

private:
    uint64_t ref_cnt;

    friend class val;

    inline void inc_ref_cnt( void )            
    { 
        csassert( ref_cnt != 0, "shouldn't be incrementing a zero ref_cnt for a CustomVal val" ); 
        ref_cnt++;
    }

    inline uint64_t dec_ref_cnt( void )            
    { 
        csassert( ref_cnt != 0, "trying to decfrement a zero ref_cnt for a CustomVal val" );
        ref_cnt--;
        return ref_cnt;
    }
};

std::string val::kind_to_str( const enum kind& k )
{
    #define kcase( _k ) case kind::_k: return #_k;
    switch( k )
    {
        kcase( UNDEF )
        kcase( BOOL )
        kcase( INT )
        kcase( FLT )
        kcase( STR )
        kcase( LIST )
        kcase( MAP )
        kcase( FUNC )
        kcase( FILE )
        kcase( THREAD )
        kcase( PROCESS )
        kcase( CUSTOM )
        default: return "<unknown kind>";
    }
}

inline val::val( void )
{
    k = kind::UNDEF;
}

inline val::val( bool x )
{
    k = kind::BOOL;
    u.b = x;
}

inline val::val( int64_t x )
{
    k = kind::INT;
    u.i = x;
}

inline val::val( int x )
{
    k = kind::INT;
    u.i = x;
}

inline val::val( double x )
{
    k = kind::FLT;
    u.f = x;
}

inline val::val( float x )
{
    k = kind::FLT;
    u.f = x;
}

inline val::val( const char * x )
{
    k = kind::STR;
    u.s = new String;
    u.s->ref_cnt = 1;
    u.s->s = std::string( x );
}

inline val::val( std::string x )
{
    k = kind::STR;
    u.s = new String;
    u.s->ref_cnt = 1;
    u.s->s = x;
}

inline val::val( CustomVal * x )
{
    x->inc_ref_cnt();
    k = kind::CUSTOM;
    u.c = x;
}

inline val::val( int64_t cnt, const char * args[] )
{
    *this = list();
    for( int64_t i = 0; i < cnt; i++ ) push( args[i] ); 
}

inline val::val( std::initializer_list<val> vals )
{
    *this = list();
    for( auto it = vals.begin(); it != vals.end(); it++ ) push( *it );
}

inline val::val( const val& x )
{
    k = kind::UNDEF;
    *this = x;
}

inline val val::list( void )
{
    val l;
    l.k = kind::LIST;
    l.u.l = new List;
    l.u.l->ref_cnt = 1;
    return l;
}

inline val val::map( void )
{
    val m;
    m.k = kind::MAP;
    m.u.m = new Map;
    m.u.m->ref_cnt = 1;
    return m;
}

inline void val::free( void )
{
    switch( k )
    {
        case kind::STR:
            csassert( u.s->ref_cnt > 0, "bad STR ref count" );
            if ( --u.s->ref_cnt == 0 ) delete u.s;
            u.s = nullptr;
            break;

        case kind::LIST:
            csassert( u.l->ref_cnt > 0, "bad LIST ref count" );
            if ( --u.l->ref_cnt == 0 ) delete u.l;
            u.l = nullptr;
            break;

        case kind::MAP:
            csassert( u.l->ref_cnt > 0, "bad MAP ref count" );
            if ( --u.m->ref_cnt == 0 ) delete u.m;
            u.m = nullptr;
            break;

        case kind::CUSTOM:
            if ( u.c->dec_ref_cnt() == 0 ) delete u.c;
            u.c = nullptr;
            break;

        default:
            break;
    }
    k = kind::UNDEF;
}

inline val::~val()
{
    free();
}

inline bool val::defined( void ) const
{
    return k != kind::UNDEF;
}

inline std::string val::kind( void ) const
{
    return (k == kind::CUSTOM) ? u.c->kind() : kind_to_str( k );
}

inline val::operator bool( void ) const
{
    switch( k )
    {
        case kind::BOOL:                return u.b;
        case kind::INT:                 return u.i != 0;
        case kind::STR:                 return u.s->s == "true" || u.s->s == "1";
        case kind::LIST:                return size() != 0;
        case kind::MAP:                 return size() != 0;
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to bool" ); return false;
    }
}

inline val::operator int64_t( void ) const
{
    switch( k )
    {
        case kind::BOOL:                return int64_t(u.b);
        case kind::INT:                 return u.i;
        case kind::FLT:                 return int64_t(u.f);
        case kind::STR:                 return std::atoi(u.s->s.c_str());
        case kind::LIST:                return size();
        case kind::MAP:                 return size();
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to int64_t" ); return 0;
    }
}

inline val::operator double( void ) const
{
    switch( k )
    {
        case kind::INT:                 return double(u.i);
        case kind::FLT:                 return u.f;
        case kind::STR:                 return std::atof(u.s->s.c_str());
        case kind::LIST:                return double(size());
        case kind::MAP:                 return double(size());
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to double" ); return 0.0;
    }
}

inline val::operator std::string( void ) const
{
    switch( k )
    {
        case kind::BOOL:                return u.b ? "true" : "false";
        case kind::INT:                 return std::to_string(u.i);
        case kind::FLT:                 return std::to_string(u.f);
        case kind::STR:                 return u.s->s;
        case kind::LIST:                return join( " " );
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to std::string" ); return "";
    }
}

inline val::operator CustomVal&( void ) const
{
    csassert( k == kind::CUSTOM, "can't convert " + kind_to_str(k) + " to CustomVal *" );
    return *u.c;
}

inline val val::operator - ( void ) const
{
    switch( k )
    {
        case kind::BOOL:                
        case kind::INT:                 return -int64_t( *this );
        case kind::CUSTOM:              return new CustomVal( -*u.c );
        default:                        return -double( *this );
    }
}

inline bool val::operator ! ( void ) const
{
    return !bool( *this );
}

inline val val::operator ~ ( void ) const
{
    switch( k )
    {
        case kind::CUSTOM:              return new CustomVal( ~*u.c );
        default:                        return ~int64_t( *this );
    }
}

inline val val::operator + ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c + x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c + *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i + x.u.i;
            case kind::FLT:             return u.f + x.u.f;
            case kind::STR:             return u.s->s + x.u.s->s;
            case kind::LIST:            { val v = *this; v.push( x ); return v; }
            default:                    die( kind_to_str( k ) + " + " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) + double( x );
        } else if ( k == kind::STR ) {
            return std::string( *this ) + std::string( x );
        } else if ( k == kind::LIST ) {
            val v = *this;
            v.push( x );
            return v;
        } else {
            die( kind_to_str( k ) + " + " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator - ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c - x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c - *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i - x.u.i;
            case kind::FLT:             return u.f - x.u.f;
            default:                    die( kind_to_str( k ) + " - " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) - double( x );
        } else {
            die( kind_to_str( k ) + " - " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator * ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c * x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c * *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i * x.u.i;
            case kind::FLT:             return u.f * x.u.f;
            default:                    die( kind_to_str( k ) + " * " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) * double( x );
        } else {
            die( kind_to_str( k ) + " * " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator / ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c / x );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i / x.u.i;
            case kind::FLT:             return u.f / x.u.f;
            default:                    die( kind_to_str( k ) + " / " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) / double( x );
        } else {
            die( kind_to_str( k ) + " / " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator % ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c % x );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i % x.u.i;
            case kind::FLT:             return std::remainder( u.f, x.u.f );
            default:                    die( kind_to_str( k ) + " % " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return std::remainder( double( *this ), double( x ) );
        } else {
            die( kind_to_str( k ) + " % " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator & ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c & x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c & *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b & x.u.i;
            case kind::INT:             return u.i & x.u.i;
            default:                    die( kind_to_str( k ) + " & " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT  && x.k == kind::BOOL) ||
             (k == kind::BOOL && x.k == kind::INT) ) {
            return int64_t( *this ) & int64_t( x );
        } else {
            die( kind_to_str( k ) + " & " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator && ( const val& x ) const
{
    return bool( *this ) && bool( x );
}

inline val val::operator | ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c | x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c | *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b | x.u.i;
            case kind::INT:             return u.i | x.u.i;
            default:                    die( kind_to_str( k ) + " | " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT  || x.k == kind::BOOL) ||
             (k == kind::BOOL || x.k == kind::INT) ) {
            return int64_t( *this ) | int64_t( x );
        } else {
            die( kind_to_str( k ) + " | " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator || ( const val& x ) const
{
    return bool( *this ) || bool( x );
}

inline val val::operator ^ ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c ^ x );
    } else if ( x.k == kind::CUSTOM ) {
        return new CustomVal( *x.u.c ^ *this );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b ^ x.u.i;
            case kind::INT:             return u.i ^ x.u.i;
            default:                    die( kind_to_str( k ) + " | " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT  || x.k == kind::BOOL) ||
             (k == kind::BOOL || x.k == kind::INT) ) {
            return int64_t( *this ) ^ int64_t( x );
        } else {
            die( kind_to_str( k ) + " ^ " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator << ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c << x );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i << x.u.i;
            case kind::FLT:             return u.f * std::pow( 2.0, x.u.f );
            case kind::STR:             return u.s->s + x.u.s->s;
            case kind::LIST:            { val v = *this; v.push( x ); return v; }
            default:                    die( kind_to_str( k ) + " << " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) * std::pow( 2.0, double( x ) );
        } else if ( k == kind::STR ) {
            return std::string( *this ) + std::string( x );
        } else if ( k == kind::LIST ) {
            val v = *this;
            v.push( x );
            return v;
        } else {
            die( kind_to_str( k ) + " << " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val val::operator >> ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return new CustomVal( *u.c >> x );
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::INT:             return u.i >> x.u.i;
            case kind::FLT:             return u.f / std::pow( 2.0, x.u.f );
            default:                    die( kind_to_str( k ) + " >> " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) / std::pow( 2.0, double( x ) );
        } else {
            die( kind_to_str( k ) + " >> " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator < ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c < x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c >= *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b < x.u.b;
            case kind::INT:             return u.i < x.u.i;
            case kind::FLT:             return u.f < x.u.f;
            default:                    die( kind_to_str( k ) + " < " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) < double( x );
        } else {
            die( kind_to_str( k ) + " < " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator <= ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c <= x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c > *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b <= x.u.b;
            case kind::INT:             return u.i <= x.u.i;
            case kind::FLT:             return u.f <= x.u.f;
            default:                    die( kind_to_str( k ) + " <= " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) <= double( x );
        } else {
            die( kind_to_str( k ) + " <= " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator > ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c > x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c <= *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b > x.u.b;
            case kind::INT:             return u.i > x.u.i;
            case kind::FLT:             return u.f > x.u.f;
            default:                    die( kind_to_str( k ) + " > " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) > double( x );
        } else {
            die( kind_to_str( k ) + " > " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator >= ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c >= x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c < *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b >= x.u.b;
            case kind::INT:             return u.i >= x.u.i;
            case kind::FLT:             return u.f >= x.u.f;
            default:                    die( kind_to_str( k ) + " >= " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) >= double( x );
        } else {
            die( kind_to_str( k ) + " >= " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator != ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c != x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c != *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b != x.u.b;
            case kind::INT:             return u.i != x.u.i;
            case kind::FLT:             return u.f != x.u.f;
            case kind::STR:             return u.s->s != x.u.s->s;
            default:                    die( kind_to_str( k ) + " != " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) != double( x );
        } else {
            die( kind_to_str( k ) + " != " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline bool val::operator == ( const val& x ) const
{
    if ( k == kind::CUSTOM ) {
        return *u.c == x;
    } else if ( x.k == kind::CUSTOM ) {
        return *x.u.c == *this;
    } else if ( k == x.k ) {
        switch( k )
        {
            case kind::BOOL:            return u.b == x.u.b;
            case kind::INT:             return u.i == x.u.i;
            case kind::FLT:             return u.f == x.u.f;
            case kind::STR:             return u.s->s == x.u.s->s;
            default:                    die( kind_to_str( k ) + " == " + kind_to_str( x.k ) + " is not supported" ); return val();
        }
    } else {
        if ( (k == kind::INT && x.k == kind::FLT) ||
             (k == kind::FLT && x.k == kind::INT) ) {
            return double( *this ) == double( x );
        } else {
            die( kind_to_str( k ) + " == " + kind_to_str( x.k ) + " is not supported" );
            return val();
        }
    }
}

inline val& val::operator = ( const val& x )
{
    free();
    k = x.k;
    u = x.u;
    switch( k ) 
    {
        case kind::STR:         u.s->ref_cnt++; break;
        case kind::LIST:        u.l->ref_cnt++; break;
        case kind::MAP:         u.m->ref_cnt++; break;
        case kind::CUSTOM:      *u.c = x;       break;
        default:                                break;
    }
    return *this;
}

inline val& val::operator += ( const val& x )
{
    switch( k ) 
    {
        case kind::INT:         u.i    += int64_t( x );     break;
        case kind::FLT:         u.f    += double( x );      break;
        case kind::STR:         u.s->s += std::string( x ); break;
        case kind::LIST:        push( x );                  break;
        case kind::CUSTOM:      *u.c += x;                  break;
        default:                die( "+= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator -= ( const val& x )
{
    switch( k ) 
    {
        case kind::INT:         u.i  -= int64_t( x );       break;
        case kind::FLT:         u.f  -= double( x );        break;
        case kind::CUSTOM:      *u.c -= x;                  break;
        default:                die( "-= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator *= ( const val& x )
{
    switch( k ) 
    {
        case kind::INT:         u.i  *= int64_t( x );       break;
        case kind::FLT:         u.f  *= double( x );        break;
        case kind::CUSTOM:      *u.c *= x;                  break;
        default:                die( "*= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator /= ( const val& x )
{
    switch( k ) 
    {
        case kind::INT:         u.i  /= int64_t( x );       break;
        case kind::FLT:         u.f  /= double( x );        break;
        case kind::CUSTOM:      *u.c /= x;                  break;
        default:                die( "/= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator %= ( const val& x )
{
    switch( k ) 
    {
        case kind::INT:         u.i  %= int64_t( x );       break;
        case kind::FLT:         u.f   = std::remainder( u.f, double( x ) ); break;
        case kind::CUSTOM:      *u.c %= x;                  break;
        default:                die( "%= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator &= ( const val& x )
{
    switch( k ) 
    {
        case kind::BOOL:        u.b  &= bool( x );          break;
        case kind::INT:         u.i  &= int64_t( x );       break;
        case kind::CUSTOM:      *u.c &= x;                  break;
        default:                die( "&= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator |= ( const val& x )
{
    switch( k ) 
    {
        case kind::BOOL:        u.b  |= bool( x );          break;
        case kind::INT:         u.i  |= int64_t( x );       break;
        case kind::CUSTOM:      *u.c |= x;                  break;
        default:                die( "|= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::operator ^= ( const val& x )
{
    switch( k ) 
    {
        case kind::BOOL:        u.b  ^= bool( x );          break;
        case kind::INT:         u.i  ^= int64_t( x );       break;
        case kind::CUSTOM:      *u.c ^= x;                  break;
        default:                die( "^= not defined for " + kind_to_str( k ) ); break;
    }
    return *this;
}

inline val& val::push( const val& x )
{
    csassert( k == kind::LIST, "can only push a LIST" );
    u.l->l.push_back( x );
    return *this;
}

inline val  val::shift( void )
{
    csassert( k == kind::LIST, "can only shift a LIST" );
    csassert( !u.l->l.empty(), "trying to shift an empty LIST" );
    auto it = u.l->l.begin();
    val v = *it;
    u.l->l.erase( it );
    return v;
}

inline val  val::join( const val delim ) const
{
    csassert( k == kind::LIST, "can only join a LIST" );
    std::string s = "";
    for( auto it = u.l->l.begin(); it != u.l->l.end(); it++ )
    {
        if ( it != u.l->l.begin() ) s += std::string( delim );
        s += std::string( *it );
    }
    return val( s );
}

inline char val::at( const val& i ) const
{
    csassert( k == kind::STR, "at() allowed only on STR" );    
    return u.s->s.at( int64_t( i ) );
}

inline std::regex val::regex( const val& options ) const
{
    // validate options
    std::string o_s = options;
    std::regex::flag_type flags;
    bool got_grammar = false;
    for( size_t i = 0; i < o_s.length(); i++ )
    {
        char ch = o_s.at( i );
        switch( ch )
        {
            case 'i': flags |= std::regex_constants::icase;                                    break;
            case 'j': flags |= std::regex_constants::ECMAScript;       got_grammar = true;     break;
            case 'p': flags |= std::regex_constants::basic;            got_grammar = true;     break;
            case 'P': flags |= std::regex_constants::extended;         got_grammar = true;     break;
            case 'a': flags |= std::regex_constants::awk;              got_grammar = true;     break;
            case 'g': flags |= std::regex_constants::grep;             got_grammar = true;     break;
            case 'G': flags |= std::regex_constants::egrep;            got_grammar = true;     break;
            default: die( "unknown regex option character: " + std::to_string(ch) );           break;
        }
    }
    if ( !got_grammar ) flags |= std::regex_constants::ECMAScript;

    return std::regex( std::string(*this), flags );
}

inline val val::match( const std::regex& regex ) const
{
    // convert to strings 
    std::string s   = *this;
    std::smatch sm;
    if ( !std::regex_match( s, sm, regex ) ) return val();  // return UNDEF

    // return matches as a list
    val matches = list();
    for( size_t i = 0; i < sm.size(); i++ ) matches += sm[i];
    return matches;
}

inline val val::match( const val& re, const val& options ) const
{
    return match( re.regex( options ) );
}

inline val val::replace( const std::regex& regex, const val& fmt ) const
{
    std::string s   = *this;
    std::string f_s = fmt;
    return std::regex_replace( s, regex, f_s );
}

inline val val::replace( const val& re, const val& fmt, const val& options ) const
{
    return replace( re.regex( options ), fmt );
}

inline val val::replace_all( const std::regex& regex, const val& fmt, uint64_t max ) const
{
    std::string s   = *this;
    std::string f_s = fmt;
    for( size_t i = 0; i < max && std::regex_search( s, regex ); i++ )
    {
        s = std::regex_replace( s, regex, f_s );
    }
    return s;
}

inline val val::replace_all( const val& re, const val& fmt, const val& options, uint64_t max ) const
{
    return replace_all( re.regex( options ), fmt, max );
}

inline uint64_t val::size( void ) const
{
    switch( k ) 
    {
        case kind::STR:        
        {
            return u.s->s.length();
        }

        case kind::LIST:        
        {
            return u.l->l.size();
        }

        case kind::MAP:        
        {
            return u.m->m.size();
        }

        case kind::CUSTOM:      
        {
            return u.c->size();
        }

        default:
        {
            die( "can't call size() on a " + kind_to_str(k) + " val" );  
            return false;
        }
    }
}

inline bool val::exists( const val& key ) const
{
    switch( k ) 
    {
        case kind::LIST:        
        {
            int64_t index = key;
            return index >= 0 && index < int64_t(u.l->l.size());
        }

        case kind::MAP:        
        {
            auto it = u.m->m.find( std::string( key ) );
            return it != u.m->m.end();
        }

        case kind::CUSTOM:      
        {
            return u.c->exists( key );
        }

        default:
        {
            die( "can't call exists() on a " + kind_to_str(k) + " val" );  
            return false;
        }
    }
}

inline       ValProxy::operator const val&( void ) const        { return v->get( *key );   }
inline void  ValProxy::operator = ( const val& other )          { v->set( *key, other );   }

inline ValProxy   val::operator [] ( const val& key )           { return ValProxy( this, &key ); }
inline const val& val::operator [] ( const val& key ) const     { return get( key ); }

inline const val& val::get( const val& key ) const
{
    switch( k ) 
    {
        case kind::LIST:        
        {
            int64_t index = key;
            csassert( index >= 0 && index < int64_t(u.l->l.size()), "LIST index is out of range" );
            return u.l->l[index];
        }

        case kind::MAP:        
        {
            auto it = u.m->m.find( std::string( key ) );
            csassert( it != u.m->m.end(), "MAP key " + std::string(key) + " does not exist" );
            return it->second;
        }

        case kind::CUSTOM:      
        {
            return u.c->get( key );
        }

        default:
        {
            die( "can't call get() on a " + kind_to_str(k) + " val" );  
            return undef;
        }
    }
}

inline void val::set( const val& key, const val& v )
{
    switch( k ) 
    {
        case kind::LIST:        
        {
            u.l->l[int64_t(key)] = v;
            break;
        }

        case kind::MAP:        
        {
            u.m->m[std::string(key)] = v;
            break;
        }

        case kind::CUSTOM:      
        {
            u.c->set( key, v );
            break;
        }

        default:
        {
            die( "can't call set() on a " + kind_to_str(k) + " val" );  
            break;
        }
    }
}

val val::run( val options ) const
{
    csassert( options == "", "run() supports no options yet" );
    std::string cmd = *this;
    return std::system( cmd.c_str() );
}

inline val val::path_dir( void ) const
{
    csassert( k == kind::STR, "path_dir() must be called on a STR val" );
    size_t pos = u.s->s.find_last_of( "/\\" );
    return val( u.s->s.substr( 0, pos ) );
}

inline val val::path_no_dir( void ) const
{
    csassert( k == kind::STR, "path_no_dir() must be called on a STR val" );
    size_t pos = u.s->s.find_last_of( "/\\" );
    return val( u.s->s.substr( pos+1 ) );
}

inline val val::path_no_ext( void ) const
{
    csassert( k == kind::STR, "path_no_dir() must be called on a STR val" );
    size_t pos = u.s->s.find_last_of( "/\\." );
    char c = u.s->s.at( pos );
    return (c == '.') ? val( u.s->s.substr( 0, pos-1 ) ) : *this;
}

int val::path_stat( struct stat& ss ) const
{
    csassert( k == kind::STR, "path_stat() must be called on a STR val" );
    return stat( std::string( *this ).c_str(), &ss );
}

bool val::path_exists( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0;
}

bool val::path_is_file( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0 && S_ISREG( ss.st_mode );
}

bool val::path_is_link( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0 && S_ISLNK( ss.st_mode );
}

bool val::path_is_fifo( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0 && S_ISFIFO( ss.st_mode );
}

bool val::path_is_socket( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0 && S_ISSOCK( ss.st_mode );
}

bool val::path_is_dir( void ) const
{
    struct stat ss;
    return path_stat( ss ) == 0 && S_ISDIR( ss.st_mode );
}

time_t val::path_time_modified( void ) const
{
    struct stat ss;
    int r = path_stat( ss );
    csassert( r == 0, "can't ss " + std::string( *this ) );
    return ss.st_mtime;
}

time_t val::path_time_accessed( void ) const
{
    struct stat ss;
    int r = path_stat( ss );
    csassert( r == 0, "can't ss " + std::string( *this ) );
    return ss.st_atime;
}

inline val val::exe_path( void )
{
    pid_t pid = getpid();
    char path[PROC_PIDPATHINFO_MAXSIZE];
    int ret = proc_pidpath( pid, path, sizeof(path) );
    csassert( ret > 0, "proc_pidpath() had an error" );
    return val( path );
}

#endif // __cs_h
