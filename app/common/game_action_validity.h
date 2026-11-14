// Earth & Beyond client (reconstruction): the object at client 0x83C870 (INTERIM file name, see game_action_validity.cpp's notes).
#ifndef __GAME_ACTION_VALIDITY_H
#define __GAME_ACTION_VALIDITY_H

#include <string>

// A game action's validity: the GameActionValidityBitFlag bits that stop it (0 when it is valid) and the
// first reason given (0x14 bytes; its members are out of line and the client calls none of them)
class GameActionValidityClass	// NAME 3: the client's 'GameActionValidityBitFlag' (Type_Name 0x83D270), whose bits Flags holds
{
public:
	GameActionValidityClass();	// 0x83C870
	GameActionValidityClass(unsigned int flags, const std::string &reason);	// 0x83C8A0  // NAME 1 [flags, reason]: what they hold
	~GameActionValidityClass();	// 0x83CE30

	bool Is_Valid() const { return(Flags == 0); }	// NAME 1: what it tests

	GameActionValidityClass &operator|=(const GameActionValidityClass &other);	// 0x83CA30

	unsigned int Flags;		// +0x00 (GameActionValidityBitFlag bits)  // NAME 1: what it holds
	std::string Reason;		// +0x04  // NAME 1: what it holds
};

GameActionValidityClass operator|(const GameActionValidityClass &a, const GameActionValidityClass &b);	// 0x83CC00

#endif
