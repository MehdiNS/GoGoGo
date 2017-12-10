#pragma once
#include "BoardLogic.h"

struct GameState
{
	bool _isGameOver;
	Board _board;
	Player _playerCurrentlyPlaying;
	int _scoreWhite;
	int _scoreBlack;
};