#pragma once

namespace logic
{
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

	// Could have used std::pair but I really don't like the 'first', 'second' syntax. Make the code less pexpressive imo
	struct Position
	{
		int x, y;
		Position(int x0 = 0, int y0 = 0) : x{x0}, y{y0} {}
	};

	static Position getNorthPosition(Position pos) { return { pos.x, pos.y - 1 }; }
	static Position getSouthPosition(Position pos) { return { pos.x, pos.y + 1 }; }
	static Position getWestPosition(Position pos) { return { pos.x - 1, pos.y };}
	static Position getEastPosition(Position pos) { return { pos.x + 1, pos.y }; }

	static Stone playerToStone(Player player)
	{
		return (player == Player::BLACK) ? Stone::BLACK : Stone::WHITE;
	}

	static Player opposingPlayer(Player player)
	{
		return (player == Player::BLACK) ? Player::WHITE : Player::BLACK;
	}

	using ChainID = unsigned int;
}