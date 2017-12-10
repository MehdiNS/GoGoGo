You are tasked with the implementation of a go game for 2 human players 
(no AI is required). 
See wikipedia https://en.wikipedia.org/wiki/Go_(game)#Basic_rules for a 
description of the game rules.

Players in turn use the mouse to click on the board where they wish to 
place a stone. 
Code is already provided to display the board and its stones in a window
 and handle user input to place a stone on the board. You can modify the
 existing code in any way you deem necessary. The code has been tested to build
 on windows (visual studio 2017) using cmake using the following commands:
 
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
Launch and build gogame.sln


You have to implement the following features:

Management of the 2 players and update of the game state after each
player turn
Respect and implement the 2 fundamental rules of go:
- Rule 1 (the rule of liberty) states that every stone remaining on the board
	must have at least one open "point" (an intersection, called a "liberty") 
	directly orthogonally adjacent (up, down, left, or right), or must be part 
	of a connected group that has at least one such open point ("liberty") next
	to it. Stones or groups of stones which lose their last liberty are removed 
	from the board.
- Rule 2 (the positional superko rule) states that the stones on the board must 
	never repeat a previous position of stones. Moves which would do so are forbidden,
	and thus only moves elsewhere on the board are permitted that turn.
	
The game ends when both players pass. Add user input for a player to pass its 
turn (by pressing a specific key for instance). When the game has ended compute 
the score of each player and print which one has won. 

The evaluation will take into account the quality of the code and the performance
of the application. Feel free to send us an email if you need clarification 
regarding the coding test instructions.

