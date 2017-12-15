# README #

## Features

- Possibility to pass a move, start a new game, place stones on the board...
- Moves are precomputed : you don't have to click on a position to see if a move is legal, you have a visual feedback before that. Of course, it's more expensive but more convenient when you don't know the game of Go (like me)
- Stone elimination implemented by constructing/fusionning/destructing chains of stones 'locally' (meaning no floodfill algo used)
- The positional superko rule is implemented by storing a hash of the board at each round. By precomputing/simulating a stone and its effects on a dummy grid, he can't see if the move is valid and if it is, use our dummy grid as the real grid for the next move
- Area socring used : Stones of color X + Empty positions surrounded by stones of color X only (computed with floodfill this time)

## How do I get set up?

On windows:
```
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
```
Launch and build gogame.sln
