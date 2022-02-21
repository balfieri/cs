<p>
Note: work in progress.

<p>
Have you ever wanted to use dynamic data structures in C++ that you find
in interpreted languages like Python? Well, you've come to the right place.
For lack of a better name, I call this C++ Script (CS).

<p>
So you can now use C++ as a scripting language, which means it might be
the only language you may need to use. More importantly, you can mix and match
dynamic and statically-typed C++ in the same program. 

<p>
Another useful feature is that this class can read and write JSON.
So you can store data in an industry-standard unstructured format
and read it into C++ easily and have it show up like other dynamic data.

<p>
This is all open-source with no restrictions.  See LICENSE.md for details (MIT license).
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
After building the CS program (which itself is a CS program), doit.build then runs a couple sanity tests using CS.</p>
</p>

<h1>Advanced</h1>

TBD

<p>
Bob Alfieri<br>
Chapel Hill, NC
</p>
