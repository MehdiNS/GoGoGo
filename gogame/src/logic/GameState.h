#pragma once
#include "Board.h"

namespace logic
{
	class  GameState
	{
		bool _isGameOver;
		Board _board;
		Player _currentPlayer;
		unsigned int _scoreWhite;
		unsigned int _scoreBlack;
		unsigned int _nbConsecutivePass;
		std::string _message;

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
		ChainID getStoneChainID(Position pos);
		int getDirectLiberties(Position pos, ChainID chain);

		void decreaseLiberty(ChainID chain);
		void decreaseLibertiesOfAdjacentChains(Position pos);
		void increaseLibertiesOfAdjacentChains(Position pos);
	};
}