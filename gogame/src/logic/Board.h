#pragma once
#include <cassert>
#include <vector>

namespace logic
{
	enum class neighbour : unsigned char
	{
		NORTH,
		SOUTH,
		WEST,
		EAST
	};

	enum class Stone : unsigned char
	{
		BLACK,
		WHITE,
		NONE
	};

	enum class Player : unsigned char
	{
		BLACK,
		WHITE
	};

	Stone playerToStone(Player player)
	{
		return (player == Player::BLACK) ? Stone::BLACK : Stone::WHITE;
	}

	Player opposingPlayer(Player player)
	{
		return (player == Player::BLACK) ? Player::WHITE : Player::BLACK;
	}

	class Board
	{
	public:
		int _sizeX;
		int _sizeY;
		std::vector<Stone> _stoneBoard;

		using ChainID = unsigned int; // It should be safe
		ChainID _nextChainID;
		std::vector<ChainID> _chainBoard;


		Board(int sizeX, int sizeY) :
			_sizeX{ sizeX },
			_sizeY{ sizeY },
			_nextChainID{ 0 },
			_stoneBoard(_sizeX * _sizeY, Stone::NONE),
			_chainBoard(_sizeX * _sizeY, 0u)
		{
		}

		void reset()
		{
			// _sizeX and _sizeY stays the same
			_nextChainID = 0;
			std::fill(std::begin(_stoneBoard), std::end(_stoneBoard), Stone::NONE);
			std::fill(std::begin(_chainBoard), std::end(_chainBoard), 0u);
		}

		void incrementNextChainId()
		{
			++_nextChainID;
		}

		bool isPositionInsideBoard(int x, int y) const
		{
			return (x >= 0 && x < _sizeX && y >= 0 && y < _sizeY);
		}

		bool noStoneAtPosition(int x, int y) const
		{
			return (stoneAt(x, y) == Stone::NONE);
		}

		Stone stoneAt(int x, int y) const
		{
			return _stoneBoard[x + y*_sizeX];
		}

		ChainID chainAt(int x, int y) const
		{
			return _chainBoard[x + y*_sizeX];
		}

		unsigned int directLibertiesAtPosition(int x, int y, Player player)
		{
			Stone oppositeStoneColor = playerToStone(opposingPlayer(player));

			unsigned int libertiesNorth = 0;
			if (isPositionInsideBoard(x, y - 1))
				libertiesNorth = stoneAt(x, y - 1) == oppositeStoneColor ? libertiesNorth : 1;

			unsigned int libertiesSouth = 0;
			if (isPositionInsideBoard(x, y + 1))
				libertiesSouth = stoneAt(x, y + 1) == oppositeStoneColor ? libertiesSouth : 1;

			unsigned int libertiesWest = 0;
			if (isPositionInsideBoard(x - 1, y))
				libertiesWest = stoneAt(x - 1, y) == oppositeStoneColor ? libertiesWest : 1;

			unsigned int libertiesEast = 0;
			if (isPositionInsideBoard(x + 1, y))
				libertiesEast = stoneAt(x + 1, y) == oppositeStoneColor ? libertiesEast : 1;

			return libertiesNorth + libertiesSouth + libertiesWest + libertiesEast;
		}

		ChainID getChainID(int x, int y, Player player)
		{
			// Look up direct neighbouroud
			ChainID nextChainId = _nextChainID;

			if (isPositionInsideBoard(x, y - 1))
			{
				ChainID chainIdNorthNeighbour = chainAt(x, y - 1);
				if (stoneAt(x, y - 1) == playerToStone(player) && chainIdNorthNeighbour > 0 && chainIdNorthNeighbour < nextChainId)
					nextChainId = chainIdNorthNeighbour;
			}

			if (isPositionInsideBoard(x, y + 1))
			{
				ChainID chainIdSouthNeighbour = chainAt(x, y + 1);
				if (stoneAt(x, y + 1) == playerToStone(player) && chainIdSouthNeighbour > 0 && chainIdSouthNeighbour < nextChainId)
					nextChainId = chainIdSouthNeighbour;
			}

			if (isPositionInsideBoard(x - 1, y))
			{
				ChainID chainIdWestNeighbour = chainAt(x - 1, y);
				if (stoneAt(x - 1, y) == playerToStone(player) && chainIdWestNeighbour > 0 && chainIdWestNeighbour < nextChainId)
					nextChainId = chainIdWestNeighbour;
			}

			if (isPositionInsideBoard(x + 1, y))
			{
				ChainID chainIdEastNeighbour = chainAt(x + 1, y);
				if (stoneAt(x + 1, y) == playerToStone(player) && chainIdEastNeighbour > 0 && chainIdEastNeighbour < nextChainId)
					nextChainId = chainIdEastNeighbour;
			}

			return nextChainId;
		}

		bool isNextToChain(int x, int y, ChainID chain)
		{
			// Look up direct neighbouroud

			ChainID chainIdNorthNeighbour = chainAt(x, y - 1);
			if (chainAt(x, y - 1) == chain)
				return true;

			ChainID chainIdSouthNeighbour = chainAt(x, y + 1);
			if (chainAt(x, y + 1) == chain)
				return true;

			ChainID chainIdWestNeighbour = chainAt(x - 1, y);
			if (chainAt(x - 1, y) == chain)
				return true;

			ChainID chainIdEastNeighbour = chainAt(x + 1, y);
			if (chainAt(x + 1, y) == chain)
				return true;

			// Nothing found
			return false;
		}

		void putStoneAtPosition(int x, int y, Stone stone)
		{
			_stoneBoard[x + y*_sizeX] = stone;
		}
	};
}