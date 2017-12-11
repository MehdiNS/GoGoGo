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
		std::string _message;

		GameState(int x, int y) :
			_isGameOver{ false },
			_board{ x, y },
			_currentPlayer{ Player::BLACK },
			_scoreWhite{ 0 },
			_scoreBlack{ 0 },
			_nbConsecutivePass{ 0 },
			_message{ "Click on position to add a stone, [Espace] to pass, [N] to start a new game" }
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
				_isGameOver = true;

			if (!_isGameOver)
				changePlayer();
		}

		bool canPutStoneAtPosition(int x, int y)
		{
			// If game is not over
			// and if the position is on the board
			// and if there is no stone at the position
			// and if there is at least a liberty at that position
			// then the position is valid

			if (_isGameOver)
			{
				_message = "Game over";
				return false;
			}

			if (!_board.isPositionInsideBoard(x, y))
			{
				_message = "Can't add the stone : Outside of board";
				return false;
			}

			if (!_board.noStoneAtPosition(x, y))
			{
				_message = "Can't add the stone : Already one at position";
				return false;
			}

			if (!_board.directLibertiesAtPosition(x, y, _currentPlayer))
			{
				_message = "Can't add the stone : No liberty at position";
				return false;
			}

			_message = "Click on position to add a stone, [Space] to pass, [N] to start a new game";
			return true;
		}

		bool putStoneAtPosition(int x, int y)
		{
			if (canPutStoneAtPosition(x, y))
			{
				_board.putStoneAtPosition(x, y, playerToStone(_currentPlayer));
				_nbConsecutivePass = 0;
				changePlayer();
				return true;
			}

			return false;
		}

		std::string& getMessage()
		{
			return _message;
		}
	};
}