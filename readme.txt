使用之程式語言：< C++ >
使用之編譯器：< Qt creator >
檔案壓縮方式: <.zip>
6. 各檔案說明：
	 M10407418-pa4/Source/main.cpp : 主程式
	 M10407418-pa4/Source/parser.cpp : parser file
	 M10407418-pa4/Source/parser.h : parser headfile
	 M10407418-pa4/Source/routingdb.cpp : routingdb file
	 M10407418-pa4/Source/routingdb.h : routingdb headfile
	 M10407418-pa4/Source/tree.cpp : tree file
	 M10407418-pa4/Source/tree.h : tree headfile
	 M10407418-pa4/Source/lib : source code (library)
	 M10407418-pa4/Source/routing : executable binary
	 M10407418-pa4/Source/eval.pl : verification file
	 M10407418-pa4/Source/Makefile : Makefile
	 M10407418-pa4/report.doc ： 程式報告

7. 編譯方式說明：        	
   主程式：M10407418/Source/
	 請在主程式的目錄下，鍵入make指令，即可完成編譯	   
	 1) Type "make" to compile.
	   
	 2) The "gr" executable binary would be generated.
	   
	 3) If you want to re-compile, type "make clean" followed by "make".
	
   驗證程式：
	syntax :
	./eval.pl [input_file] [output_file]
	
	example : 
	./eval.pl adaptec1.capo70.2d.35.50.90.gr adaptec1.capo70.2d.35.50.90.out

           . . . . . .  
8. 執行、使用方式說明：
   主程式：
   編譯完成後，在檔案目錄下會產生一個 output_file.out 

   驗證程式：
	syntax :
	./eval.pl [input_file] [output_file]
	
	example : 
	./eval.pl adaptec1.capo70.2d.35.50.90.gr adaptec1.capo70.2d.35.50.90.out
           . . . . . .         
9. 執行結果說明：
   主程式：
	 File Names(In, Out)		Tot OF	Max OF	WL
	 example.gr example.out		     2       2  14
	
	File Names : input file and output file
	Tot OF : total overflow
	Max OF : maximum overflow in one line
	WL : total wirelength



       
