#pragma once
#include "GameState.h"
#include "Board.h"
#include <iostream>
#include <algorithm>

namespace logic
{

	GameState::GameState(int xDim, int yDim) :
		_isGameOver{ false },
		_board{ xDim, yDim },
		_simulatedBoard{ xDim, yDim },
		_currentPlayer{ Player::BLACK },
		_scoreWhite{ 0 },
		_scoreBlack{ 0 },
		_nbConsecutivePass{ 0 },
		_message{ "Click on position to add a stone, [Space] to pass, [N] to start a new game" }
	{
		initTable();
	}

	void GameState::reset()
	{
		_isGameOver = false;
		_board.reset();
		_simulatedBoard.reset();
		_currentPlayer = Player::BLACK;
		_scoreWhite = 0;
		_scoreBlack = 0;
		_nbConsecutivePass = 0;
		initTable();
		_oldBoardsHash.clear();
	}

	int GameState::getBoardDimensionX() const
	{
		return _board.getDimensionX();
	}

	int GameState::getBoardDimensionY() const
	{
		return _board.getDimensionY();
	}

	const Board & GameState::getBoard() const
	{
		return _board;
	}

	void GameState::changePlayer()
	{
		_currentPlayer = (_currentPlayer == Player::WHITE) ? Player::BLACK : Player::WHITE;
	}

	void floodfill(Position pos, const std::vector<Stone>& stoneBoard, std::vector<int>& visit, bool& seenWhite, bool& seenBlack, int& nbMarked, int value)
	{
		auto indexPos = (pos.x + pos.y * 9);
		bool isInBoard = pos.x >= 0 && pos.y >= 0 && pos.x < 9 && pos.y < 9;

		if (isInBoard)
		{
			// Position is empty and non marked
			if (stoneBoard[indexPos] == Stone::NONE && visit[indexPos] == 0)
			{
				// We mark the position we visited, and increase the counter of empty position in the region
				visit[indexPos] = value;
				nbMarked++;
				// Let's see if there are positions not already visited around the neighbours
				floodfill(getNorthPosition(pos), stoneBoard, visit, seenWhite, seenBlack, nbMarked, value);
				floodfill(getSouthPosition(pos), stoneBoard, visit, seenWhite, seenBlack, nbMarked, value);
				floodfill(getWestPosition(pos), stoneBoard, visit, seenWhite, seenBlack, nbMarked, value);
				floodfill(getEastPosition(pos), stoneBoard, visit, seenWhite, seenBlack, nbMarked, value);
			}
			else if (stoneBoard[indexPos] == Stone::BLACK) 
				seenBlack = true;
			else if (stoneBoard[indexPos] == Stone::WHITE) 
				seenWhite = true;
		}
	}

	// We use area scoring to determine the score. I ignored the komi.
	// This function is terrible in terms of complexity for what it does, but given the score is computed only at the 
	// end of the game, and also because I don't see a 
	void GameState::computeFinalScore()
	{
		const int sizeBoard = _board.getDimensionX() * _board.getDimensionY();

		// Compute the number of stones left on the board
		for (auto& stone : _board.getStoneBoard())
		{
			if (stone == Stone::BLACK)
				_scoreBlack++;
			else if (stone == Stone::WHITE)
				_scoreWhite++;
		}

		// Prepare the floodfill with some way to remember the positions already visited, those we don't want to visit
		std::vector<int> positionsToVisit(sizeBoard, -1); // -1 -> Stone, 0 -> Non-visited position,  1+ -> Visited positioon
		for (int i = 0; i < sizeBoard; ++i)
		{
			if (_board.noStoneAtPosition(i))
				positionsToVisit[i] = 0;
		}

		// Iterative floodfill. We iterate over the board, and each time we encounter an empty position not already visited
		// we floodfill the area, and see the color of the stones we're reaching
		for (int i = 0; i < sizeBoard; ++i)
		{
			if (_board.noStoneAtPosition(i) && positionsToVisit[i] == 0)
			{
				bool seenWhite = false;
				bool seenBlack = false;
				int nbMarked = 0;
				int x = i % 9;
				int y = i / 9;
				floodfill({x, y}, _board.getStoneBoard(), positionsToVisit, seenWhite, seenBlack, nbMarked, i+1);

				// If during the floodfill we only encountered stone of one color, we had the number of the group to 
				// their respective counter
				if (seenWhite == true && seenBlack == false)
					_scoreWhite += nbMarked;
				if (seenBlack == true && seenWhite == false)
					_scoreBlack += nbMarked;
			}
		}
	}

	Player GameState::getCurrentPlayer() const
	{
		return _currentPlayer;
	}

	void GameState::pass()
	{
		_nbConsecutivePass++;

		if (!_isGameOver && _nbConsecutivePass == 2)
		{
			computeFinalScore();
			_isGameOver = true;
		}

		if (!_isGameOver)
			changePlayer();
	}

	bool GameState::precomputeStonePlacement(Position pos)
	{
		// We work on a copy of the board, to leave the original the way it is before making sure we can add a stone at position
		_simulatedBoard = _board;

		// If the game is already over, early exit
		if (_isGameOver)
		{
			if (_scoreBlack > _scoreWhite)
				_message = "Game over : Player Black won ! [N] for a new game";
			else if (_scoreBlack < _scoreWhite)
				_message = "Game over : Player White won ! [N] for a new game";
			else
				_message = "Game over : Draw ! [N] for a new game";
			return false;
		}

		// If the position is outside the board, early exit
		if (!_simulatedBoard.isPositionInsideBoard(pos))
		{
			_message = "Can't add the stone : Outside of board";
			return false;
		}

		// If there's already a stone at the position, early exit
		if (!_simulatedBoard.noStoneAtPosition(pos))
		{
			_message = "Can't add the stone : Already one at position";
			return false;
		}

		// At this point, let's check if the stone would still have at least one liberty after this turn

		// We compute the ID of the chain. If it's a new one, we pick our 'nextChainId' var, otherwise, we pick the lowest
		// index chain adjacent to the position 
		ChainID chain = getStoneChainID(pos);

		// We compute the number of direct liberties for the stone
		int nbDirectLiberties = getDirectLiberties(pos, chain);
		int nbTotalLiberties = nbDirectLiberties;

		// If the new stone is part of a not-newly created chain, we need to decrease his number of liberties by 1
		if (chain != _simulatedBoard.getNextChainId())
			nbTotalLiberties += _simulatedBoard.getNbLibertiesOfChain(chain) - 1;

		// If there's zero liberty at the position, let's check if placing a stone there could capture other stone(s)
		// and creating at least one new liberty, or if he can grab a liberty (at least 2 actually, 1 to connect with it, 
		// and 1 still a liberty) from an adjacent stone 
		if (nbTotalLiberties == 0 && (!couldCaptureStone(pos) && !canBeLinkedToChainWithLiberties(pos)))
		{
			_message = "Can't add the stone : No liberty at that position";
			return false;
		}

		// Reaching this point we know the only problem that can appear is the superko.
		// We put the stone at the position on the fake board
		_simulatedBoard.setStoneAt(pos, playerToStone(_currentPlayer));

		// We compute the chainID of the new stone. If it's a new chain, we pick _NextChainId
		// If the new stone is connected to other stones of the same color, we pick the one of lowest value for conveniance
		chain = getStoneChainID(pos);

		// We compute the number of direct liberties that are not already liberties for adjacent chains.
		nbDirectLiberties = getDirectLiberties(pos, chain);

		// If we create a new chain, initialize it and increment the nextChainId to prepare the next ones
		if (chain == _simulatedBoard.getNextChainId())
		{
			_simulatedBoard.setNbLibertiesOfChain(chain, 0);
			_simulatedBoard.incrementNextChainId();
		}

		// We update the stone chain.
		_simulatedBoard.setChainAt(pos, chain);

		// We check if some stones could be connected, and we fusion them if they are
		Position northPos = getNorthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(northPos))
			_simulatedBoard.fusionChainsFromPositions(pos, northPos);

		Position southPos = getSouthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(southPos))
			_simulatedBoard.fusionChainsFromPositions(pos, southPos);

		Position westPos = getWestPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(westPos))
			_simulatedBoard.fusionChainsFromPositions(pos, westPos);

		Position eastPos = getEastPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(eastPos))
			_simulatedBoard.fusionChainsFromPositions(pos, eastPos);

		// We update the number of liberty for the current chain
		_simulatedBoard.setNbLibertiesOfChain(chain, _simulatedBoard.getNbLibertiesOfChain(chain) + nbDirectLiberties);

		// We tell other adjacent chains to decrease their liberties
		decreaseLibertiesOfAdjacentChains(pos);

		auto hash = computeHash(_simulatedBoard.getStoneBoard());
		auto search = _oldBoardsHash.find(hash);

		if (search != _oldBoardsHash.end())
		{
			// Board found. We did all of that for nothing. There must be a simpler test
			_message = "Move impossible (Positional Superko rule)";
			return false;
		}

		// If we reach this point, it means we can add a stone at the position safely
		_message = "Click on position to add a stone, [Space] to pass, [N] to start a new game";
		return true;
	}

	bool GameState::canBeLinkedToChainWithLiberties(Position pos)
	{
		// Here we check whether there's a chain with enough (>=2) liberties around a position to be able to place that stone
		// at that position
		bool result = false;
		Stone playerStone = playerToStone(_currentPlayer);

		{
			Position northPos = getNorthPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(northPos) && _simulatedBoard.getStoneAt(northPos) == playerStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(northPos) >= 2)
				result = true;
		}
		{
			Position southPos = getSouthPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(southPos) && _simulatedBoard.getStoneAt(southPos) == playerStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(southPos) >= 2)
				result = true;
		}
		{
			Position westPos = getWestPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(westPos) && _simulatedBoard.getStoneAt(westPos) == playerStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(westPos) >= 2)
				result = true;
		}
		{
			Position eastPos = getEastPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(eastPos) && _simulatedBoard.getStoneAt(eastPos) == playerStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(eastPos) >= 2)
				result = true;
		}
		return result;
	}

	bool GameState::couldCaptureStone(Position pos)
	{
		// For each adjacent position, we check if by placing a stone at the position, we would capture one opponent chain
		// To do that, we simply have to check if the chain currently have only 1 liberty left 

		bool result = false;
		Stone opposingStone = playerToStone(opposingPlayer(_currentPlayer));

		{
			Position northPos = getNorthPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(northPos) && _simulatedBoard.getStoneAt(northPos) == opposingStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(northPos) == 1)
				result = true;
		}
		{
			Position southPos = getSouthPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(southPos) && _simulatedBoard.getStoneAt(southPos) == opposingStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(southPos) == 1)
				result = true;
		}
		{
			Position westPos = getWestPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(westPos) && _simulatedBoard.getStoneAt(westPos) == opposingStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(westPos) == 1)
				result = true;
		}
		{
			Position eastPos = getEastPosition(pos);
			if (_simulatedBoard.isPositionInsideBoard(eastPos) && _simulatedBoard.getStoneAt(eastPos) == opposingStone && _simulatedBoard.getNbLibertiesOfChainAtPosition(eastPos) == 1)
				result = true;
		}
		return result;
	}

	ChainID GameState::getStoneChainID(Position pos)
	{
		// We pick the lowest index of the chain of the cross neighbours 
		// If there is no neighbour (so, no adjacent chain), we use the NextChainId variable

		ChainID result = _simulatedBoard.getNextChainId();

		Position northPos = getNorthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(northPos))
		{
			Stone northStone = _simulatedBoard.getStoneAt(northPos);
			ChainID northChain = _simulatedBoard.getChainAt(northPos);
			if (northStone == playerToStone(_currentPlayer) && northChain > 0 && northChain < result)
				result = northChain;
		}

		Position southPos = getSouthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(southPos))
		{
			Stone southStone = _simulatedBoard.getStoneAt(southPos);
			ChainID southChain = _simulatedBoard.getChainAt(southPos);
			if (southStone == playerToStone(_currentPlayer) && southChain > 0 && southChain < result)
				result = southChain;
		}

		Position westPos = getWestPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(westPos))
		{
			Stone westStone = _simulatedBoard.getStoneAt(westPos);
			ChainID westChain = _simulatedBoard.getChainAt(westPos);
			if (westStone == playerToStone(_currentPlayer) && westChain > 0 && westChain < result)
				result = westChain;
		}

		Position eastPos = getEastPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(eastPos))
		{
			Stone eastStone = _simulatedBoard.getStoneAt(eastPos);
			ChainID eastChain = _simulatedBoard.getChainAt(eastPos);
			if (eastStone == playerToStone(_currentPlayer) && eastChain > 0 && eastChain < result)
				result = eastChain;
		}

		return result;
	}

	bool GameState::putStoneAtPosition(Position pos)
	{
		_simulatedBoard = _board;

		// Check if we can safely add the stone at the position
		if (precomputeStonePlacement(pos))
		{
			// The player didn't pass
			_nbConsecutivePass = 0;

			// Board not found, this one is unique. We can add this board hash to the set, and place this stone
			auto hash = computeHash(_simulatedBoard.getStoneBoard());
			_oldBoardsHash.insert(hash);
			_board = _simulatedBoard;

			// The play is done. It's the other player turn
			changePlayer();

			return true;
		}

		return false;
	}

	void GameState::decreaseLiberty(ChainID chain)
	{
		// Decrease the number of liberty of the chain
		_simulatedBoard.decrementNbLibertiesOfChain(chain);

		// If the number reaches zero, the chain must be deleted
		if (_simulatedBoard.getNbLibertiesOfChain(chain) == 0)
		{
			// We compute the list of stones belonging the chain, and delete them
			_simulatedBoard.removeChain(chain);

			// For each of this stones we deleted, we have to increase the number of liberties of the adjacent stones
			const std::vector<Position>& removedPositions = _simulatedBoard.getLastRemovedStones();
			std::for_each(std::begin(removedPositions), std::end(removedPositions), [&](const Position& pos) {increaseLibertiesOfAdjacentChains(pos); });
		}
	}

	void GameState::decreaseLibertiesOfAdjacentChains(Position pos)
	{
		// For each adjacent position, we check if the chain is the same as our own. If it is, we decrement 
		// our chain number of liberties. If the chain of the adjacent position is different, we need to decrement that one.
		// The tricky part is it's possible that adjacent stones and the central stone share the same chain

		bool ownLibertyMustDecreased = false;
		ChainID centralChain = _simulatedBoard.getChainAt(pos);

		Position northPos = getNorthPosition(pos);
		ChainID northChain = _simulatedBoard.getChainAt(northPos);
		if (northChain == centralChain)
			ownLibertyMustDecreased = true;
		else if (northChain != 0)
			decreaseLiberty(northChain);

		Position southPos = getSouthPosition(pos);
		ChainID southChain = _simulatedBoard.getChainAt(southPos);
		if (southChain == centralChain)
			ownLibertyMustDecreased = true;
		else if (southChain != 0 && southChain != northChain)
			decreaseLiberty(southChain);

		Position westPos = getWestPosition(pos);
		ChainID westChain = _simulatedBoard.getChainAt(westPos);
		if (westChain == centralChain)
			ownLibertyMustDecreased = true;
		else if (westChain != 0 && westChain != northChain && westChain != southChain)
			decreaseLiberty(westChain);

		Position eastPos = getEastPosition(pos);
		ChainID eastChain = _simulatedBoard.getChainAt(eastPos);
		if (eastChain == centralChain)
			ownLibertyMustDecreased = true;
		else if (eastChain != 0 && eastChain != northChain && eastChain != southChain && eastChain != westChain)
			decreaseLiberty(eastChain);

		if (ownLibertyMustDecreased)
			decreaseLiberty(centralChain);
	}

	const std::string& GameState::getMessage() const
	{
		return _message;
	}

	unsigned int GameState::getScoreBlack() const
	{
		return _scoreBlack;
	}

	unsigned int GameState::getScoreWhite() const
	{
		return _scoreWhite;
	}

	int GameState::getDirectLiberties(Position pos, ChainID chain)
	{
		int nbDirectLiberties = 0;

		// For a given position and chain, we compute the number of direct liberties  by checking if there's no stone in place 
		// in each adjacent position and if that position is not already a liberty for our chain.

		Position northPos = getNorthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(northPos) && _simulatedBoard.noStoneAtPosition(northPos) && !_simulatedBoard.isNextToChain(northPos, chain))
			nbDirectLiberties++;

		Position southPos = getSouthPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(southPos) && _simulatedBoard.noStoneAtPosition(southPos) && !_simulatedBoard.isNextToChain(southPos, chain))
			nbDirectLiberties++;

		Position westPos = getWestPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(westPos) && _simulatedBoard.noStoneAtPosition(westPos) && !_simulatedBoard.isNextToChain(westPos, chain))
			nbDirectLiberties++;

		Position eastPos = getEastPosition(pos);
		if (_simulatedBoard.isPositionInsideBoard(eastPos) && _simulatedBoard.noStoneAtPosition(eastPos) && !_simulatedBoard.isNextToChain(eastPos, chain))
			nbDirectLiberties++;

		return nbDirectLiberties;
	}

	void GameState::increaseLibertiesOfAdjacentChains(Position pos)
	{
		// Dual function of decreaseLibertiesOfAdjacentChains
		Position northPos = getNorthPosition(pos);
		ChainID northChain = _simulatedBoard.getChainAt(northPos);
		if (northChain != 0)
			_simulatedBoard.incrementNbLibertiesOfChain(northChain);

		Position southPos = getSouthPosition(pos);
		ChainID southChain = _simulatedBoard.getChainAt(southPos);
		if (southChain != 0 && southChain != northChain)
			_simulatedBoard.incrementNbLibertiesOfChain(southChain);

		Position westPos = getWestPosition(pos);
		ChainID westChain = _simulatedBoard.getChainAt(westPos);
		if (westChain != 0 && westChain != northChain && westChain != southChain)
			_simulatedBoard.incrementNbLibertiesOfChain(westChain);

		Position eastPos = getEastPosition(pos);
		ChainID eastChain = _simulatedBoard.getChainAt(eastPos);
		if (eastChain != 0 && eastChain != northChain && eastChain != southChain && eastChain != westChain)
			_simulatedBoard.incrementNbLibertiesOfChain(eastChain);
	}

	unsigned long long int GameState::randomInt()
	{
		std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
		return dist(prng);
	}

	// Initializes the table
	void GameState::initTable()
	{
		const int sizeBoard = _board.getDimensionX() * _board.getDimensionY();
		for (int i = 0; i < sizeBoard; i++)
			for (int k = 0; k < 2; k++)
				ZobristTable[i][k] = randomInt();
	}

	// Computes the hash value of a given board
	// Acconrding to Google, we don't actually need to recompute the hash for the whole board, but only with XOR of specific 
	// positions that changed. Might give it a try if I have some time 
	unsigned long long int GameState::computeHash(const std::vector<Stone>& stoneBoard)
	{
		const int sizeBoard = _board.getDimensionX() * _board.getDimensionY();
		unsigned long long int h = 0;
		for (int i = 0; i < sizeBoard; i++)
		{
			if (stoneBoard[i] != Stone::NONE)
			{
				int piece = static_cast<int>(stoneBoard[i]);
				h ^= ZobristTable[i][piece];
			}
		}
		return h;
	}

}