#pragma once

#include "Error/Error.h"
#include "Error/EType.h"
#include "String/String.h"
#include "Socket/Socket.h"
#include "Chat/ChatClient.h"
//#include "Chat/ChatServer.h"

extern void InspectE(E);

#define UnwrapE(X) _Generic((X),				\
	E:					InspectE,				\
	bool_E:				UnwrapBoolE,			\
	int32_t_E:			UnwrapInt32E,			\
	uint8_t_E:			UnwrapUInt8E,			\
	uint8_t_ptr_E:		UnwrapUInt8PtrE,		\
	String_t_E:			UnwrapStringE,			\
	Socket_t_E:			UnwrapSocketE,			\
	ChatClient_t_E:		UnwrapChatClientE		\
)(X)
//	ChatServer_t_E:		UnwrapChaTServerE		\

extern bool UnwrapBoolE(bool_E);
extern int32_t UnwrapInt32E(int32_t_E);
extern uint8_t UnwrapUInt8E(uint8_t_E);
extern uint8_t *UnwrapUInt8PtrE(uint8_t_ptr_E);
extern String_t *UnwrapStringE(String_t_E);
extern Socket_t *UnwrapSocketE(Socket_t_E);
extern ChatClient_t *UnwrapChatClientE(ChatClient_t_E);
//extern ChatServer_t *UnwrapChatServerE(ChatServer_t_E);


#define UnwrapXE_DEC(X, XE);			\
	extern X Unwrap ## XE(XE);

#define UnwrapXE_DEF(X, XE);			\
	extern X Unwrap ## XE(XE ret) {		\
		InspectE(ret.E);				\
		return ret.V;					\
	}
