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
		_currentPlayer{ Player::BLACK },
		_scoreWhite{ 0 },
		_scoreBlack{ 0 },
		_nbConsecutivePass{ 0 },
		_message{ "Click on position to add a stone, [Espace] to pass, [N] to start a new game" }
	{}

	void GameState::reset()
	{
		_isGameOver = false;
		_board.reset();
		_currentPlayer = Player::BLACK;
		_scoreWhite = 0;
		_scoreBlack = 0;
		_nbConsecutivePass = 0;
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

	Player GameState::getCurrentPlayer() const
	{
		return _currentPlayer;
	}

	void GameState::pass()
	{
		_nbConsecutivePass++;

		if (!_isGameOver && _nbConsecutivePass == 2)
			_isGameOver = true;

		if (!_isGameOver)
			changePlayer();
	}

	bool GameState::canPutStoneAtPosition(Position pos)
	{
		// If the game is already over, early exit
		if (_isGameOver)
		{
			_message = "Game over";
			return false;
		}

		// If the position is outside the board, early exit
		if (!_board.isPositionInsideBoard(pos))
		{
			_message = "Can't add the stone : Outside of board";
			return false;
		}

		// If there's already a stone at the position, early exit
		if (!_board.noStoneAtPosition(pos))
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
		if (chain != _board.getNextChainId())
			nbTotalLiberties += _board.getNbLibertiesOfChain(chain) - 1;

		// If there's zero liberty at the position, let's check if placing a stone there could capture other stone(s)
		// and creating at least one new liberty, or if he can grab a liberty (at least 2 actually, 1 to connect with it, 
		// and 1 still a liberty) from an adjacent stone 
		if (nbTotalLiberties == 0 && (!couldCaptureStone(pos) && !canBeLinkedToChainWithLiberties(pos)))
		{
			_message = "No liberty at that position";
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
			if (_board.isPositionInsideBoard(northPos) && _board.getStoneAt(northPos) == playerStone && _board.getNbLibertiesOfChainAtPosition(northPos) >=2 )
				result = true;
		}
		{
			Position southPos = getSouthPosition(pos);
			if (_board.isPositionInsideBoard(southPos) && _board.getStoneAt(southPos) == playerStone && _board.getNbLibertiesOfChainAtPosition(southPos) >= 2)
				result = true;
		}
		{
			Position westPos = getWestPosition(pos);
			if (_board.isPositionInsideBoard(westPos) && _board.getStoneAt(westPos) == playerStone && _board.getNbLibertiesOfChainAtPosition(westPos) >= 2)
				result = true;
		}
		{
			Position eastPos = getEastPosition(pos);
			if (_board.isPositionInsideBoard(eastPos) && _board.getStoneAt(eastPos) == playerStone && _board.getNbLibertiesOfChainAtPosition(eastPos) >= 2)
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
			if (_board.isPositionInsideBoard(northPos) && _board.getStoneAt(northPos) == opposingStone && _board.getNbLibertiesOfChainAtPosition(northPos) == 1)
				result = true;
		}
		{
			Position southPos = getSouthPosition(pos);
			if (_board.isPositionInsideBoard(southPos) && _board.getStoneAt(southPos) == opposingStone && _board.getNbLibertiesOfChainAtPosition(southPos) == 1)
				result = true;
		}
		{
			Position westPos = getWestPosition(pos);
			if (_board.isPositionInsideBoard(westPos) && _board.getStoneAt(westPos) == opposingStone && _board.getNbLibertiesOfChainAtPosition(westPos) == 1)
				result = true;
		}
		{
			Position eastPos = getEastPosition(pos);
			if (_board.isPositionInsideBoard(eastPos) && _board.getStoneAt(eastPos) == opposingStone && _board.getNbLibertiesOfChainAtPosition(eastPos) == 1)
				result = true;
		}
		return result;
	}

	ChainID GameState::getStoneChainID(Position pos)
	{
		// We pick the lowest index of the chain of the cross neighbours 
		// If there is no neighbour (so, no adjacent chain), we use the NextChainId variable

		ChainID result = _board.getNextChainId();

		Position northPos = getNorthPosition(pos);
		if (_board.isPositionInsideBoard(northPos))
		{
			Stone northStone = _board.getStoneAt(northPos);
			ChainID northChain = _board.getChainAt(northPos);
			if (northStone == playerToStone(_currentPlayer) && northChain > 0 && northChain < result)
				result = northChain;
		}

		Position southPos = getSouthPosition(pos);
		if (_board.isPositionInsideBoard(southPos))
		{
			Stone southStone = _board.getStoneAt(southPos);
			ChainID southChain = _board.getChainAt(southPos);
			if (southStone == playerToStone(_currentPlayer) && southChain > 0 && southChain < result)
				result = southChain;
		}

		Position westPos = getWestPosition(pos);
		if (_board.isPositionInsideBoard(westPos))
		{
			Stone westStone = _board.getStoneAt(westPos);
			ChainID westChain = _board.getChainAt(westPos);
			if (westStone == playerToStone(_currentPlayer) && westChain > 0 && westChain < result)
				result = westChain;
		}

		Position eastPos = getEastPosition(pos);
		if (_board.isPositionInsideBoard(eastPos))
		{
			Stone eastStone = _board.getStoneAt(eastPos);
			ChainID eastChain = _board.getChainAt(eastPos);
			if (eastStone == playerToStone(_currentPlayer) && eastChain > 0 && eastChain < result)
				result = eastChain;
		}

		return result;
	}

	bool GameState::putStoneAtPosition(Position pos)
	{
		// Check if we can safely add the stone at the position
		if (canPutStoneAtPosition(pos))
		{
			// Put the stone at the position
			_board.setStoneAt(pos, playerToStone(_currentPlayer));

			// The player didn't pass
			_nbConsecutivePass = 0;

			// We compute the chainID of the new stone. If it's a new chain, we pick _NextChainId
			// If the new stone is connected to other stones of the same color, we pick 
			ChainID chain = getStoneChainID(pos);

			// We compute the number of direct liberties that are not already liberties for adjacent chains.
			int nbDirectLiberties = getDirectLiberties(pos, chain);

			// If we create a new chain, intialize it and increment the nextChainId to prepare the next ones
			if (chain == _board.getNextChainId())
			{
				_board.setNbLibertiesOfChain(chain, 0);
				_board.incrementNextChainId();
			}

			// We update the stone chain.
			_board.setChainAt(pos, chain);

			// We check if some stones could be connected, and we fusion them if they are
			Position northPos = getNorthPosition(pos);
			if (_board.isPositionInsideBoard(northPos))
				_board.fusionChainsFromPositions(pos, northPos);

			Position southPos = getSouthPosition(pos);
			if (_board.isPositionInsideBoard(southPos))
				_board.fusionChainsFromPositions(pos, southPos);

			Position westPos = getWestPosition(pos);
			if (_board.isPositionInsideBoard(westPos))
				_board.fusionChainsFromPositions(pos, westPos);

			Position eastPos = getEastPosition(pos);
			if (_board.isPositionInsideBoard(eastPos))
				_board.fusionChainsFromPositions(pos, eastPos);

			// We update the number of liberty for the current chain
			_board.setNbLibertiesOfChain(chain, _board.getNbLibertiesOfChain(chain) + nbDirectLiberties);

			// We tell other adjacent chains to decrease their liberties
			decreaseLibertiesOfAdjacentChains(pos);
			std::cout << "New nb liberty of chain : " << _board.getNbLibertiesOfChain(chain) << std::endl;

			// The play is done. It's the other player turn
			changePlayer();

			return true;
		}

		return false;
	}

	void GameState::decreaseLiberty(ChainID chain)
	{
		// Decrease the number of liberty of the chain
		_board.decrementNbLibertiesOfChain(chain);

		// If the number reaches zero, the chain must be deleted
		if (_board.getNbLibertiesOfChain(chain) == 0)
		{
			// We compute the list of stones belonging the chain, and delete them
			_board.removeChain(chain);

			// For each of this stones we deleted, we have to increase the number of liberties of the adjacent stones
			const std::vector<Position>& removedPositions = _board.getLastRemovedStones();
			std::for_each(std::begin(removedPositions), std::end(removedPositions), [&](const Position& pos) {increaseLibertiesOfAdjacentChains(pos); });
		}
	}

	void GameState::decreaseLibertiesOfAdjacentChains(Position pos)
	{
		// For each adjacent position, we check if the chain is the same as our own. If it is, we decrement 
		// our chain number of liberties. If the chain of the adjacent position is different, we need to decrement that one
		// The tricky part is it's possible that adjacent stones share the same chain

		bool ownLibertyMustDecreased = false;
		ChainID centralChain = _board.getChainAt(pos);

		Position northPos = getNorthPosition(pos);
		ChainID northChain = _board.getChainAt(northPos);
		if (northChain == centralChain)
		{
			ownLibertyMustDecreased = true;
		}
		else if (northChain != 0)
		{
			decreaseLiberty(northChain);
		}

		Position southPos = getSouthPosition(pos);
		ChainID southChain = _board.getChainAt(southPos);
		if (southChain == centralChain)
		{
			ownLibertyMustDecreased = true;
		}
		else if (southChain != 0 && southChain != northChain)
		{
			decreaseLiberty(southChain);
		}

		Position westPos = getWestPosition(pos);
		ChainID westChain = _board.getChainAt(westPos);
		if (westChain == centralChain)
		{
			ownLibertyMustDecreased = true;
		}
		else if (westChain != 0 && westChain != northChain && westChain != southChain)
		{
			decreaseLiberty(westChain);
		}

		Position eastPos = getEastPosition(pos);
		ChainID eastChain = _board.getChainAt(eastPos);
		if (eastChain == centralChain)
		{
			ownLibertyMustDecreased = true;
		}
		else if (eastChain != 0 && eastChain != northChain && eastChain != southChain && eastChain != westChain)
		{
			decreaseLiberty(eastChain);
		}

		if (ownLibertyMustDecreased)
			decreaseLiberty(centralChain);
	}

	const std::string& GameState::getMessage() const
	{
		return _message;
	}

	int GameState::getDirectLiberties(Position pos, ChainID chain)
	{
		int nbDirectLiberties = 0;

		// For a given position and chain, we compute the number of direct liberties  by checking if there's no stone in place 
		// in each adjacent position and if that position is not already a liberty for our chain.

		Position northPos = getNorthPosition(pos);
		if (_board.isPositionInsideBoard(northPos) && _board.noStoneAtPosition(northPos) && !_board.isNextToChain(northPos, chain))
			nbDirectLiberties++;

		Position southPos = getSouthPosition(pos);
		if (_board.isPositionInsideBoard(southPos) && _board.noStoneAtPosition(southPos) && !_board.isNextToChain(southPos, chain))
			nbDirectLiberties++;

		Position westPos = getWestPosition(pos);
		if (_board.isPositionInsideBoard(westPos) && _board.noStoneAtPosition(westPos) && !_board.isNextToChain(westPos, chain))
			nbDirectLiberties++;

		Position eastPos = getEastPosition(pos);
		if (_board.isPositionInsideBoard(eastPos) && _board.noStoneAtPosition(eastPos) && !_board.isNextToChain(eastPos, chain))
			nbDirectLiberties++;

		return nbDirectLiberties;
	}

	void GameState::increaseLibertiesOfAdjacentChains(Position pos)
	{
		// Dual function of decreaseLibertiesOfAdjacentChains
		Position northPos = getNorthPosition(pos);
		ChainID northChain = _board.getChainAt(northPos);
		if (northChain != 0)
			_board.incrementNbLibertiesOfChain(northChain);

		Position southPos = getSouthPosition(pos);
		ChainID southChain = _board.getChainAt(southPos);
		if (southChain != 0 && southChain != northChain)
			_board.incrementNbLibertiesOfChain(southChain);

		Position westPos = getWestPosition(pos);
		ChainID westChain = _board.getChainAt(westPos);
		if (westChain != 0 && westChain != northChain && westChain != southChain)
			_board.incrementNbLibertiesOfChain(westChain);

		Position eastPos = getEastPosition(pos);
		ChainID eastChain = _board.getChainAt(eastPos);
		if (eastChain != 0 && eastChain != northChain && eastChain != southChain && eastChain != westChain)
			_board.incrementNbLibertiesOfChain(eastChain);
	}
}