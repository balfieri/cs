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

#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

// dynamically-typed value
//
class Custom;

class val
{
public:
    // constructors
    val( void );
    val( bool x );
    val( int64_t x );
    val( double x );
    val( const char * x );
    val( std::string x );
    val( const val& x );
    val( Custom * x );
    static val list( void );
    static val list( int64_t cnt, const char * args[] );
    static val map( void );
    ~val();

    // introspection
    bool        defined( void ) const;
    std::string kind( void ) const;

    // conversions from val to common types
    operator bool( void ) const;
    operator int64_t( void ) const;
    operator double( void ) const;
    operator std::string( void ) const;
    operator Custom *( void ) const;

    // these overwrite any previous contents (including kind)
    val& operator = ( const bool x );
    val& operator = ( const int64_t x );
    val& operator = ( const double x );
    val& operator = ( std::string x );
    val& operator = ( Custom * x );
    val& operator = ( const val& other );

    // cool operators
    val& operator [] ( const int64_t i );               // index into list or map
    val& operator [] ( const std::string i );           // index into list or map
    val& operator [] ( const val& i );                  // index into list or map
    val  operator () ( ... );                           // function call

    // list-only operations
    val& push( const val& x );
    val  shift( void );
    val  split( const val delim = "" ) const;
    val  join( const val delim = "" ) const;

private:
    enum class kind
    {
        KIND,
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
        std::map<val,val>       m;
    };

    union
    {
        bool                    b;
        int64_t                 i;
        double                  f;
        String *                s;
        List *                  l;
        Map *                   m;
        Custom *                c;
    } u;

    void free( void );
};

// Globals
//
static const val undef;
std::istream& cin  = std::cin;
std::ostream& cout = std::cout;
std::ostream& cerr = std::cerr;

// Misc Functions
//
static inline val list( void )                                      { return val::list(); }
static inline val list( int64_t cnt, const char * args[] )          { return val::list( cnt, args ); }
static inline val map( void )                                       { return val::map();  }

static inline void die( std::string msg )       
{ 
    std::cout << "ERROR: " << msg << "\n"; 
    exit( 1 ); 
}

#define csassert( expr, msg ) if ( !(expr) ) die( msg )

static inline std::ostream& operator << ( std::ostream &out, const val& x )
{
    out << std::string(x); 
    return out;
}

// Custom Val
//
class Custom
{
public:
    Custom( void )                              { ref_cnt = 1; }
    virtual ~Custom()                           { csassert( ref_cnt == 0, "trying to destroy a Custom val when ref_cnt is not 0" ); }

    virtual std::string kind( void ) const      { return "Custom"; }
    virtual operator bool( void ) const         { die( "no conversion available from Custom to bool" );        return false; }
    virtual operator int64_t( void ) const      { die( "no conversion available from Custom to int64_t" );     return 0;     }
    virtual operator double( void ) const       { die( "no conversion available from Custom to double" );      return 0.0;   }
    virtual operator std::string( void ) const  { die( "no conversion available from Custom to std::string" ); return "";    }

private:
    uint64_t ref_cnt;

    friend class val;

    inline void inc_ref_cnt( void )            
    { 
        csassert( ref_cnt != 0, "shouldn't be incrementing a zero ref_cnt for a Custom val" ); 
        ref_cnt++;
    }

    inline uint64_t dec_ref_cnt( void )            
    { 
        csassert( ref_cnt != 0, "trying to decfrement a zero ref_cnt for a Custom val" );
        ref_cnt--;
        return ref_cnt;
    }
};

// val x = func( f )
// val x = func( "code" )
// val x = file(“file”, “w”)
// val x = file( "w" )
// val x = y.matches( “regexp” )
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
// class X;
//  class Proxy {
//      X*  object;
//      Key key;
//  public:
//      Proxy(X* object, Key key): object(object), key(key) {}
//      operator V() const { return object->read(key); }
//      void operator=(V const& v) { object->write(key, v); }
//  };
//  class X {
//      // ...
//  public:
//      V    read(key) const;
//      void write(key, V const& v);
//      Proxy operator[](Key key)       { return Proxy(this, key); }
//      V     operator[](Key key) const { return this->read(key); }
//      // ...
//  };

//-----------------------------------------------------
//-----------------------------------------------------
// 
// IMPLEMENTATION  IMPLEMENTATION  IMPLEMENTATION
//
//-----------------------------------------------------
//-----------------------------------------------------

std::string val::kind_to_str( const enum kind& k )
{
    #define kcase( _k ) case kind::_k: return #_k;
    switch( k )
    {
        kcase( KIND )
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

inline val::val( double x )
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

inline val::val( Custom * x )
{
    x->inc_ref_cnt();
    k = kind::CUSTOM;
    u.c = x;
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

inline val val::list( int64_t cnt, const char * args[] )
{
    val l = list();
    for( int64_t i = 0; i < cnt; i++ )
    {
        l.push( args[i] ); 
    }
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

bool val::defined( void ) const
{
    return k != kind::UNDEF;
}

std::string val::kind( void ) const
{
    return (k == kind::CUSTOM) ? u.c->kind() : kind_to_str( k );
}

val::operator bool( void ) const
{
    switch( k )
    {
        case kind::BOOL:                return u.b;
        case kind::INT:                 return u.i != 0;
        case kind::STR:                 return u.s->s == "true" || u.s->s == "1";
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to bool" ); return false;
    }
}

val::operator int64_t( void ) const
{
    switch( k )
    {
        case kind::BOOL:                return int64_t(u.b);
        case kind::INT:                 return u.i;
        case kind::FLT:                 return int64_t(u.f);
        case kind::STR:                 return std::atoi(u.s->s.c_str());
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to int64_t" ); return 0;
    }
}

val::operator double( void ) const
{
    switch( k )
    {
        case kind::INT:                 return double(u.i);
        case kind::FLT:                 return u.f;
        case kind::STR:                 return std::atof(u.s->s.c_str());
        case kind::CUSTOM:              return *u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to double" ); return 0.0;
    }
}

val::operator std::string( void ) const
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

val::operator Custom *( void ) const
{
    switch( k )
    {
        case kind::CUSTOM:              return u.c;
        default:                        die( "can't convert " + kind_to_str(k) + " to Custom *" ); return nullptr;
    }
}

val& val::operator = ( const bool x )
{
    free();
    k = kind::BOOL;
    u.b = x;
    return *this;
}

val& val::operator = ( const int64_t x )
{
    free();
    k = kind::INT;
    u.i = x;
    return *this;
}

val& val::operator = ( const double x )
{
    free();
    k = kind::FLT;
    u.f = x;
    return *this;
}

val& val::operator = ( std::string x )
{
    free();
    k = kind::STR;
    u.s = new String{ 1, std::string( x ) };
    return *this;
}

val& val::operator = ( Custom * x )
{
    free();
    x->inc_ref_cnt();
    k = kind::CUSTOM;
    u.c = x;
    return *this;
}

val& val::operator = ( const val& other )
{
    free();
    k = other.k;
    u = other.u;
    switch( k ) 
    {
        case kind::STR:         u.s->ref_cnt++; break;
        case kind::LIST:        u.l->ref_cnt++; break;
        case kind::MAP:         u.m->ref_cnt++; break;
        default:                                break;
    }
    return *this;
}

val& val::push( const val& x )
{
    csassert( k == kind::LIST, "can only push a LIST" );
    u.l->l.push_back( x );
    return *this;
}

val  val::shift( void )
{
    csassert( k == kind::LIST, "can only shift a LIST" );
    csassert( !u.l->l.empty(), "trying to shift an empty LIST" );
    auto it = u.l->l.begin();
    val v = *it;
    u.l->l.erase( it );
    return v;
}

val  val::join( const val delim ) const
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

#endif // __cs_h
