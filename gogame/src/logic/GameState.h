#pragma once
#include "Board.h"

namespace logic
{
	struct GameState
	{
		bool _isGameOver;
		Board _board;
		Player _playerCurrentlyPlaying;
		int _scoreWhite;
		int _scoreBlack;

		GameState(int x, int y) : 
			_isGameOver{ false }, 
			_board{ x, y }, 
			_playerCurrentlyPlaying{ Player::BLACK }, 
			_scoreWhite{ 0 }, 
			_scoreBlack{ 0 } 
		{}
	};
}