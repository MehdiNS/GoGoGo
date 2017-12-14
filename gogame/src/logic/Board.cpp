#include "Board.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>

namespace logic
{
	Board::Board(int sizeX, int sizeY) :
		_sizeX{ sizeX },
		_sizeY{ sizeY },
		_nextChainID{ 1 },
		_stoneBoard(sizeX * sizeY, Stone::NONE),
		_chainBoard(sizeX * sizeY, 0u),
		_nbLibertiesOfChains{},
		_lastRemovedStones{}
	{
		// We probably won't need to remove 81 stones at once but just to be safe...
		_lastRemovedStones.reserve(_sizeX * _sizeY);
	}


	void Board::reset()
	{
		// _sizeX and _sizeY stays the same
		_nextChainID = 1;
		std::fill(std::begin(_stoneBoard), std::end(_stoneBoard), Stone::NONE);
		std::fill(std::begin(_chainBoard), std::end(_chainBoard), 0u);
		_nbLibertiesOfChains.clear();
		_lastRemovedStones.clear();
	}

	void Board::incrementNextChainId()
	{
		_nextChainID++;
	}

	ChainID Board::getNextChainId() const
	{
		return _nextChainID;
	}

	bool Board::isPositionInsideBoard(Position pos) const
	{
		return (pos.x >= 0 && pos.x < _sizeX && pos.y >= 0 && pos.y < _sizeY);
	}

	bool Board::isPositionInsideBoard(int i) const
	{
		int x = i % _sizeX;
		int y = i / _sizeX;
		return (x >= 0 && x < _sizeX && y >= 0 && y < _sizeY);
	}

	bool Board::noStoneAtPosition(Position pos) const
	{
		return (getStoneAt(pos) == Stone::NONE);
	}

	int Board::getDimensionX() const
	{
		return _sizeX;
	}

	int Board::getDimensionY() const
	{
		return _sizeY;
	}

	Stone Board::getStoneAt(Position pos) const
	{
		return _stoneBoard[pos.x + pos.y*_sizeX];
	}

	Stone Board::getStoneAt(int i) const
	{
		return _stoneBoard[i];
	}

	void Board::setStoneAt(int i, Stone stone)
	{
		_stoneBoard[i] = stone;
	}

	void Board::setStoneAt(Position pos, Stone stone)
	{
		_stoneBoard[pos.x + pos.y*_sizeX] = stone;
	}

	ChainID Board::getChainAt(Position pos) const
	{
		// The ChainID 0 is reserved for empty positions and out of the board positions 
		if (isPositionInsideBoard(pos))
			return _chainBoard[pos.x + pos.y*_sizeX];
		else
			return 0;
	}

	ChainID Board::getChainAt(int i) const
	{
		return _chainBoard[i];
	}

	void Board::setChainAt(Position pos, ChainID chain)
	{
		_chainBoard[pos.x + pos.y*_sizeX] = chain;
	}

	void Board::setChainAt(int i, ChainID chain)
	{
		_chainBoard[i] = chain;
	}

	unsigned int Board::getNbLibertiesOfChain(ChainID chain) const
	{
		return _nbLibertiesOfChains.at(chain);
	}

	void Board::decrementNbLibertiesOfChain(ChainID chain)
	{
		_nbLibertiesOfChains[chain]--;
	}

	void Board::incrementNbLibertiesOfChain(ChainID chain)
	{
		_nbLibertiesOfChains[chain]++;
	}

	unsigned int Board::getNbLibertiesOfChainAtPosition(Position position) const
	{
		return _nbLibertiesOfChains.at(getChainAt(position));
	}

	const std::vector<Position>& Board::getLastRemovedStones()
	{
		return _lastRemovedStones;
	}

	void Board::setNbLibertiesOfChain(ChainID chain, unsigned int newValue)
	{
		_nbLibertiesOfChains[chain] = newValue;
	}

	int Board::computeNbLibertiesInCommonBetweenChains(ChainID chain1, ChainID chain2)
	{
		// We create a set of liberties of each chain
		// To do so, we iterate over the grid, couldn't find a better way...
		// After the two sets are constructed, we compute the intersection of them 
		// and return the number of liberty common to the two set

		std::set<int> libertyPositionsOfChain1;
		std::set<int> libertyPositionsOfChain2;

		for (int i = 0; i < _chainBoard.size(); ++i)
		{
			// Ignore chains different that the two we are treating
			if (getChainAt(i) != chain1 &&getChainAt(i) != chain2)
				continue;

			// Construct 2D position from index
			int x = i % _sizeX;
			int y = i / _sizeX;
			Position pos{ x, y };

			std::set<int>& currentSet = getChainAt(i) == chain1 ? libertyPositionsOfChain1 : libertyPositionsOfChain2;

			Position northPos = getNorthPosition(pos);
			if (isPositionInsideBoard(northPos) && noStoneAtPosition(northPos))
				currentSet.insert(northPos.x + northPos.y*_sizeX);

			Position southPos = getSouthPosition(pos);
			if (isPositionInsideBoard(southPos) && noStoneAtPosition(southPos))
				currentSet.insert(southPos.x + southPos.y*_sizeX);

			Position westPos = getWestPosition(pos);
			if (isPositionInsideBoard(westPos) && noStoneAtPosition(westPos))
				currentSet.insert(westPos.x + westPos.y*_sizeX);

			Position eastPos = getEastPosition(pos);
			if (isPositionInsideBoard(eastPos) && noStoneAtPosition(eastPos))
				currentSet.insert(eastPos.x + eastPos.y*_sizeX);
		}

		std::vector<int> libertyIntersectionOfChains;
		libertyIntersectionOfChains.reserve(std::max(libertyPositionsOfChain1.size(), libertyPositionsOfChain2.size()));

		std::set_intersection(libertyPositionsOfChain1.begin(), libertyPositionsOfChain1.end(),
			libertyPositionsOfChain2.begin(), libertyPositionsOfChain2.end(), std::back_inserter(libertyIntersectionOfChains));
	
		return static_cast<int>(libertyIntersectionOfChains.size());
	}

	void Board::fusionChainsFromPositions(Position pos, Position neighbourPos)
	{
		// We check if the two stones in paramteers could be connected, and we fusion them in a unique chain if they are
		// First, we check if they have the same color
		Stone stoneAtPos = getStoneAt(pos);
		Stone stoneAtOtherPos = getStoneAt(neighbourPos);
		if (stoneAtPos != stoneAtOtherPos)
			return;

		// Next, we check if both belong to a chain. No point in doing the fusion if not
		ChainID chainAtPos = getChainAt(pos);
		if (chainAtPos == 0)
			return;

		ChainID chainAtOtherPos = getChainAt(neighbourPos);
		if (chainAtOtherPos == 0)
			return;

		// If they belong to two different chains, we combine them. How so ?
		// The key point is to compute the number of liberties their respectives chains have in common
		if (chainAtPos != chainAtOtherPos)
		{
			int nbLibertyInCommon = computeNbLibertiesInCommonBetweenChains(chainAtPos, chainAtOtherPos);

			for (int i = 0; i < _chainBoard.size(); i++)
			{
				if (getChainAt(i) == chainAtOtherPos)
					setChainAt(i, chainAtPos);
			}

			_nbLibertiesOfChains[chainAtPos] = _nbLibertiesOfChains[chainAtPos] + _nbLibertiesOfChains[chainAtOtherPos] - nbLibertyInCommon - 1;
			_nbLibertiesOfChains.erase(chainAtOtherPos);
		}
	}

	bool Board::isNextToChain(Position pos, ChainID chain)
	{
		// Simple function to check if a chain is adjacent to a position on the board

		Position northPos = getNorthPosition(pos);
		if (getChainAt(northPos) == chain)
			return true;

		Position southPos = getSouthPosition(pos);
		if (getChainAt(southPos) == chain)
			return true;

		Position westPos = getWestPosition(pos);
		if (getChainAt(westPos) == chain)
			return true;

		Position eastPos = getEastPosition(pos);
		if (getChainAt(eastPos) == chain)
			return true;

		return false;
	}


	void Board::removeChain(ChainID chain)
	{
		// This function constructs a list of stone positions belonging to the given chain we have to remove

		// We start by emptying our vector
		_lastRemovedStones.clear();

		// We iterate over the board and each time we found a stone of the wanted chain, we delete it
		for (int i = 0; i < _stoneBoard.size(); ++i)
		{
			int x = i % _sizeX;
			int y = i / _sizeX;

			if (getStoneAt(i) != Stone::NONE  && getChainAt(i) == chain)
			{
				setStoneAt(i, Stone::NONE);
				setChainAt(i, 0);
				_lastRemovedStones.emplace_back(x, y);
			}
		}
	}
}