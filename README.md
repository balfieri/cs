<p>
Have you ever wanted to use dynamic data structures in C++ just as you
do inside interpreted languages like Python? Well, you've come to the right place.
For lack of a better name, I call this C++ Script (CS). It might be the
only language you need to use because you can mix and match
dynamic and statically-typed C++ in the same program. 

<p>
Along the same lines, this class can read and write JSON, an industry-standarded
unstructured ASCII format. So you can read JSON into C++ and have it show
up like other dynamic data.

<p>
cs.cpp is a (very optional) tiny program that implements a CS interpreter. You can build it using doit.build (which is a traditional script).
Once the cs executable is built, you can "interpret" any C++ program by saying "cs my_prog". It will look for my_prog.cpp and compile and
run it on the fly. In fact, cs can compile and run itself - "cs cs". I have tested doit.build and cs.cpp on macOS and Linux, but they should
work on Cygwin with little or no modification.

<p>
This is all open-source with no restrictions. See LICENSE.md for details (MIT license).
</p>

<p>
As mentioned, the class "val" denotes a dynamically-typed value.  Vals can take on 
various built-in types, including:</p>
<ul>
<li>int64_t
<li>double 
<li>bool
<li>string - including support for regular expressions and paths
<li>list
<li>map
<li>file
<li>function
<li>thread
<li>process
</ul>

<p>
Advanced users may add other built-in val types to val using the CustomVal class.
</p>

<p>
The best way to learn CS is by example, so let's get started.
</p>

<h1>Basic</h1>

<h2>Build CS</h2>

<pre>
doit.build
</pre>

<p>
After building the optional CS program (which itself is a CS program), doit.build then runs a couple sanity tests using CS.</p>
</p>

<h2>TBD</h2>

I need to add a lot more examples.

<h1>Advanced</h1>

TBD

<p>
Bob Alfieri<br>
Chapel Hill, NC
</p>
