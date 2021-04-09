# Solitaire
Solitaire game made using C and Termbox

First, run the makefile by using the command "make".

To run part 1:
./check inputfilename.txt

To run part 2:
./advance inputfilename.txt
with optional commands
-m number: Checks for number amount of moves
-o filename: Outputs results in filename
-x: Changes style of output to exchange format

To run part 3: 
./winnable inputfilename.txt
-m number: Limits the number of moves checked to number
-v: Outputs diagnostic information

To run part 4:
./game inputfilename.txt
with optional commands
-f filename: runs game from filename
-s: runs game with randomly-generated seed
-1: Gets the next 1 card from the waste when prompted
-3: Gets the next 3 cards from the waste when prompted
-l number: Limits the amount of waste resets to number
