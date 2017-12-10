#pragma once
#include "Board.h"

namespace logic
{
	struct GameState
	{
		bool _isGameOver;
		Board _board;
		Player _currentPlayer;
		unsigned int _scoreWhite;
		unsigned int _scoreBlack;
		unsigned int _nbConsecutivePass;

		GameState(int x, int y) :
			_isGameOver{ false },
			_board{ x, y },
			_currentPlayer{ Player::BLACK },
			_scoreWhite{ 0 },
			_scoreBlack{ 0 },
			_nbConsecutivePass{ 0 }
		{}

		void reset()
		{
			_isGameOver = false;
			_board.reset();
			_currentPlayer = Player::BLACK;
			_scoreWhite = 0;
			_scoreBlack = 0;
			_nbConsecutivePass = 0;
		}

		void changePlayer()
		{
			_currentPlayer = (_currentPlayer == Player::WHITE) ? Player::BLACK : Player::WHITE;
		}

		Player getCurrentPlayer() const
		{
			return _currentPlayer;
		}

		void pass()
		{
			_nbConsecutivePass++;
			if (!_isGameOver && _nbConsecutivePass == 2)
			{
				_isGameOver = true;
			}

			if (!_isGameOver)
				changePlayer();
		}

		bool canPutStoneAtPosition(int x, int y)
		{
			return (!_isGameOver && _board.isValidPosition(x, y) && _board.noStoneAtPosition(x, y));
		}

		void putStoneAtPosition(int x, int y)
		{
			if (canPutStoneAtPosition(x, y))
			{
				_board.putStoneAtPosition(x, y, _currentPlayer);
				_nbConsecutivePass = 0;
				changePlayer();
			}
		}
	};
}