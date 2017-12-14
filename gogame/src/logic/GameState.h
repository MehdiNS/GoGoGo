#pragma once
#include "Board.h"
#include <random>
#include <set>

namespace logic
{
	class GameState
	{
		// We keep two boards in memory, the real one and the fake one we use to see if the superko rule is applied
		Board _board;
		Board _simulatedBoard;

		// Set of the hash of all the boards configuration happening during a game. A typical Go game has a thousand of rounds
		// So the set will grow that much. In the typical superko case  (1 for 1, see below), this is not necessary, but I have
		// seen weirder configuration of superko when take a snapshot of the board at each round is the (only?) way to deal with it
		std::set<unsigned long long int>  _oldBoardsHash;

		// The current player placing a stone
		Player _currentPlayer;

		// The score, compute at the end of the game
		unsigned int _scoreWhite;
		unsigned int _scoreBlack;

		// Counter used to decide whether or not both players passed
		unsigned int _nbConsecutivePass;

		// Message displayed in UI
		std::string _message;

		// Whether the game is over or not
		bool _isGameOver;

		// Positional Superko necessary elements
		unsigned long long int ZobristTable[81][2];
		std::random_device rd;
		std::mt19937 prng{ rd() };

	public:
		GameState(int xDim, int yDim);

		void reset();
		void pass();
		void changePlayer();
		void computeFinalScore();

		int getBoardDimensionX() const;
		int getBoardDimensionY() const;
		const Board& getBoard() const;
		Player getCurrentPlayer() const;
		const std::string& getMessage() const;
		unsigned int getScoreBlack() const;
		unsigned int getScoreWhite() const;

		bool precomputeStonePlacement(Position pos);
		bool putStoneAtPosition(Position pos);
		bool couldCaptureStone(Position pos);
		bool canBeLinkedToChainWithLiberties(Position pos);
		ChainID getStoneChainID(Position pos);
		int getDirectLiberties(Position pos, ChainID chain);

		void decreaseLiberty(ChainID chain);
		void decreaseLibertiesOfAdjacentChains(Position pos);
		void increaseLibertiesOfAdjacentChains(Position pos);

		unsigned long long int randomInt();
		void initTable();
		unsigned long long int computeHash(const std::vector<Stone>& stoneBoard);
	};
}