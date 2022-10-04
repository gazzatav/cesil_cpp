# cesil_cpp
Parse and execute programs written in CESIL.

<p>This code is written with C++ 20 features but will probably compile after removing the <code>use enum Mnemonic;</code> statement and adding the enum class name in the switch cases etc.</p>
<p>Create a build folder and point cmake to the topmost CMakeLists.txt file e.g <code>cmake path/to/CMakeLists.txt</code>.</p>
<p>You can type in a program using CESIL on the command line after running cesil or supply the path to a program file.</p>
<p>If you write the program a file called prog_text will be saved where cesil runs.  This file can be edited and run again.</p>
<p><b>Beware!</b> Don't let the program overwrite your carefully crafted program, it will if it can!</p>
