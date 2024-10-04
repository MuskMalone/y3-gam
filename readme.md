BUILD INFO
---------------------
To build the solution, there are some pre-requisites that you need to have:
1) Git
2) Python
3) CMake

After getting the pre-requisites, navigate to the "Build-Scripts" folder,
then run "Setup-Windows.bat"

After running the batch file, open the main solution generated in y3-gam root.
You can now build either project in the chosen configuration.

NAMING CONVENTIONS
---------------------

File name: Camel
E.g. SuperLongSourceFile.cpp SuperLongHeaderFile.h 

Filters: First letter caps
E.g. Enemies Players

Functions: Pascal case
E.g. DrawQuadMesh ThisIsASuperLongFunction

Local Variables: camel 
E.g. thisIsASuperLongVariable 

Global Variables: with prefix g
E.g. gGlobal

Static Members: with prefix s
E.g. sStatic

Data Members: with prefix m
E.g. mMember

Define/enum : ALLCAPS + Underscore
E.g. THIS_IS_A_SUPER_LONG

Namespace: Pascal Case 
E.g. HelloWorld 

Class/Struct: Pascal Case
E.g. ThisIsClass