#pragma once
#include "Board.h"
#include <random>
#include <set>

namespace logic
{
	class  GameState
	{
		Board _board;
		Board _simulatedBoard;
		std::set<unsigned long long int>  _oldBoardsHash;
		Player _currentPlayer;
		unsigned int _scoreWhite;
		unsigned int _scoreBlack;
		unsigned int _nbConsecutivePass;
		std::string _message;
		bool _isGameOver;

		// Positional Superko
		unsigned long long int ZobristTable[81][2];
		std::random_device rd;
		std::mt19937 prng{ rd() };

	public:
		GameState(int xDim, int yDim);

		void reset();
		void pass();
		void changePlayer();
		
		int getBoardDimensionX() const;
		int getBoardDimensionY() const;
		const Board& getBoard() const;
		Player getCurrentPlayer() const;
		const std::string& getMessage() const;

		bool canPutStoneAtPosition(Position pos);
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