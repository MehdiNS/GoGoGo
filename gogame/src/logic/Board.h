#pragma once
#include <cassert>
#include <vector>

namespace logic
{
	enum class neighbor : unsigned char
	{
		NORTH,
		SOUTH,
		WEST,
		EAST
	};

	enum class Color : unsigned char
	{
		BLACK,
		WHITE,
		NONE
	};

	using Stone = Color;
	using Player = Color;

	class BoardLogic
	{
	public:
		int _sizeX;
		int _sizeY;
		std::vector<Stone> _stoneBoard;

		using ChainID = unsigned int; // It should be safe
		ChainID _nextChainID;
		std::vector<ChainID> _chainBoard;


		BoardLogic(int sizeX, int sizeY) : _sizeX{ sizeX }, _sizeY{ sizeY }, _nextChainID{ 0 }
		{
			_stoneBoard.reserve(_sizeX * _sizeY);
			_stoneBoard = { Stone::NONE };

			_chainBoard.reserve(_sizeX * _sizeY);
			_chainBoard = { 0 };
		}

		void incrementNextChainId()
		{
			++_nextChainID;
		}

		bool isValidPosition(int x, int y) const
		{
			return (x >= 0 && x < _sizeX && y >= 0 && y < _sizeY);
		}

		Stone stoneAt(int x, int y) const
		{
			assert(isValidPosition(x, y));
			return _stoneBoard[x + y*_sizeX];
		}

		Stone& stoneAt(int x, int y)
		{
			assert(isValidPosition(x, y));
			return _stoneBoard[x + y*_sizeX];
		}


		ChainID chainAt(int x, int y) const
		{
			assert(isValidPosition(x, y));
			return _chainBoard[x + y*_sizeX];
		}

		ChainID& chainAt(int x, int y)
		{
			assert(isValidPosition(x, y));
			return _chainBoard[x + y*_sizeX];
		}

		unsigned int nbIndividualLibertiesAtPosition(int x, int y) const
		{
			int nbLibertiesNorth = stoneAt(x, y - 1) == Stone::NONE ? 1 : 0;
			int nbLibertiesSouth = stoneAt(x, y + 1) == Stone::NONE ? 1 : 0;
			int nbLibertiesWest = stoneAt(x - 1, y) == Stone::NONE ? 1 : 0;
			int nbLibertiesEast = stoneAt(x + 1, y + 1) == Stone::NONE ? 1 : 0;
			return nbLibertiesNorth + nbLibertiesSouth + nbLibertiesWest + nbLibertiesEast;
		}

		bool tryToAddStoneAtPosition(int x, int y, Stone color)
		{
			if (isValidPosition(x, y)
				&& stoneAt(x, y) == Stone::NONE
				&& nbIndividualLibertiesAtPosition(x, y) > 0)
				stoneAt(x, y) = color;
		}

		ChainID getChainID(int x, int y, Player playerColor)
		{
			// Look up direct neighbouroud
			ChainID nextChainId = _nextChainID;

			ChainID chainIdNorthNeighbour = chainAt(x, y - 1);
			if (stoneAt(x, y - 1) == playerColor && chainIdNorthNeighbour > 0 && chainIdNorthNeighbour < nextChainId)
				nextChainId = chainIdNorthNeighbour;

			ChainID chainIdSouthNeighbour = chainAt(x, y + 1);
			if (stoneAt(x, y + 1) == playerColor  && chainIdSouthNeighbour > 0 && chainIdSouthNeighbour < nextChainId)
				nextChainId = chainIdSouthNeighbour;

			ChainID chainIdWestNeighbour = chainAt(x - 1, y);
			if (stoneAt(x - 1, y) == playerColor && chainIdWestNeighbour > 0 && chainIdWestNeighbour < nextChainId)
				nextChainId = chainIdWestNeighbour;

			ChainID chainIdEastNeighbour = chainAt(x + 1, y);
			if (stoneAt(x + 1, y) == playerColor && chainIdEastNeighbour > 0 && chainIdEastNeighbour < nextChainId)
				nextChainId = chainIdEastNeighbour;

			return nextChainId;
		}
	};
}