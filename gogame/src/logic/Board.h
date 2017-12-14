#pragma once
#include <vector>
#include <map>
#include "util.h"

namespace logic
{
	class Board
	{
		//To index the grid, I use a linear index i or a position(x, y)
		//depending on how it's conveniant for me in terms of readability
		
		// Array of chains on the board
		std::vector<ChainID> _chainBoard;
		// Array of stones on the board
		std::vector<Stone> _stoneBoard;
		// Map of the number of liberties by chains
		std::map<ChainID, unsigned int> _nbLibertiesOfChains;
		// Array of last removed stones, Useful for the redistribution of liberties after one  stone is captured
		// Could (?) be useful for the score
		std::vector<Position> _lastRemovedStones;
		// Index of the next independant chain of stones to be added on the board. 
		// I decided to not reuse indices because it's not worth it. On a 9x9 grid, two Go players may play over a hundred of play.
		// All of that plus the superko rule...The risk of this variable overflowing is minimal
		ChainID _nextChainID;
		// Dimensions of the board
		int _sizeX;
		int _sizeY;

	public:
		Board(int sizeX, int sizeY);
		
		void reset();
		
		bool isPositionInsideBoard(Position pos) const;
		bool isPositionInsideBoard(int i) const;
		bool noStoneAtPosition(Position pos) const;

		// A lot of get/set functions. 
		const std::vector<Stone>& getStoneBoard() const;
		ChainID getNextChainId() const;
		int getDimensionX() const;
		int getDimensionY() const;
		Stone getStoneAt(Position pos) const;
		Stone getStoneAt(int i) const;
		void setStoneAt(int i, Stone stone);
		void setStoneAt(Position pos, Stone stone);
		ChainID getChainAt(Position pos) const;
		ChainID getChainAt(int i) const;
		void setChainAt(Position pos, ChainID chain);
		void setChainAt(int i, ChainID chain);
		unsigned int getNbLibertiesOfChain(ChainID chain) const;
		unsigned int getNbLibertiesOfChainAtPosition(Position position) const;
		void setNbLibertiesOfChain(ChainID chain, unsigned int newValue);
		const std::vector<Position>& getLastRemovedStones();

		void incrementNextChainId();
		void decrementNbLibertiesOfChain(ChainID chain);
		void incrementNbLibertiesOfChain(ChainID chain);
		bool isNextToChain(Position pos, ChainID chain);
		void removeChain(ChainID chain);
		int computeNbLibertiesInCommonBetweenChains(ChainID chain1, ChainID chain2);
		void fusionChainsFromPositions(Position pos, Position neighbourPos);
	};
}