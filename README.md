<p>
Note: work in progress.

<p>
With the goal of having C++ be the only language one needs to learn, this 
repo supplies a single header file (cs.h) that embeds a 
dynamically-typed scripting language inside C++.  
For lack of a better name, it's called C++ Script (CS).
CS can be used for one-off scripts as well as more complicated programs
that contain a mixture of dynamically-typed and statically-typed code.
</p>

<p>
This is all open-source with no restrictions.  See LICENSE.md for details (MIT license).
</p>

<p>
The class "val" denotes a dynamically-typed value.  Vals can take on 
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
