# Global-Routing-Implementation-

# Compiler
Qt creator

# Language
C++

# File description

main.cpp : main file <br>
parser.cpp : parser file<br>
parser.h : parser headfile<br>
routingdb.cpp : routingdb file<br>
Source/routingdb.h : routingdb headfile<br>
Source/tree.cpp : tree file<br>
Source/tree.h : tree headfile<br>
Source/lib : source code (library)<br>
Source/routing : executable binary<br>
Source/eval.pl : verification file<br>
Source/Makefile : Makefile<br>
report.doc ： report document<br>

# Compile
## main file
1) Type "make" to compile. <br>
2) The "gr" executable binary would be generated.<br>
3) If you want to re-compile, type "make clean" followed by "make"<br>

## validation file

syntax : <br>
	./eval.pl input_file output_file<br>
	
example : <br>
./eval.pl adaptec1.capo70.2d.35.50.90.gr adaptec1.capo70.2d.35.50.90.out<br>

# Execution

## main file
After compiling, there is a output_file.out will be generated under the current document

## valiadation file 

syntax : <br>
	./eval.pl input_file output_file<br>
	
example : <br>
./eval.pl adaptec1.capo70.2d.35.50.90.gr adaptec1.capo70.2d.35.50.90.out <br>

# Execution result

File Names(In, Out)		Tot OF	Max OF	WL<br>
example.gr example.out		 2       2  14<br>

File Names : input file and output file<br>
Tot OF : total overflow<br>
Max OF : maximum overflow in one line<br>
WL : total wirelength<br>




