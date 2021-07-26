#pragma once

#include <stdint.h>
#include <NeoC/Base/Object.h>
#include <NeoC/Base/Memory.h>
#include <NeoC/Base/Exception/Signal.h>
#include <NeoC/Base/Exception/Exception.h>
#include <NeoC/Base/Defer.h>
#include <NeoC/String.h>
#include <NeoC/Socket.h>

#include "Chat.h"

class ChatClient_t {
	private String_t *_Host;
	private in_port_t _Port;
	private Socket_t *_Socket;
	private String_t *_Handle;

	public void (* Join)(struct ChatClient_t *, String_t *handle) throws (Chat.HandleException, Chat.NetworkException);
	public String_t *(* GetHandle)(struct ChatClient_t *);
	public void (* Post)(struct ChatClient_t *, String_t *message) throws (Chat.NetworkException);
	public bool (* UpdateExists)(struct ChatClient_t *);
	public String_t *(* GetMessage)(struct ChatClient_t *) throws (Chat.Exception, Chat.NetworkException);
	public void (* Quit)(struct ChatClient_t *) throws (Chat.NetworkException);
} ChatClient_t;

class _ChatClient {
	ChatClient_t *(* New)(String_t *host, const in_port_t port);
	void (* Delete)(ChatClient_t *);

	public void (* Join)(ChatClient_t *, String_t *handle) throws (Chat.HandleException, Chat.NetworkException);
	public String_t *(* GetHandle)(ChatClient_t *);
	public void (* Post)(ChatClient_t *, String_t *message) throws (Chat.NetworkException);
	public bool (* UpdateExists)(ChatClient_t *);
	public String_t *(* GetMessage)(ChatClient_t *) throws (Chat.Exception, Chat.NetworkException);
	public void (* Quit)(ChatClient_t *) throws (Chat.NetworkException);
} _ChatClient;

extern _ChatClient ChatClient;
