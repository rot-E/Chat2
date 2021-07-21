#pragma once

#include <NeoC/Annotation.h>
#include <NeoC/Memory.h>
#include <NeoC/String.h>
#include <NeoC/Console.h>
#include <NeoC/Socket.h>
#include <NeoC/Data/Map.h>
#include <NeoC/System.h>

#include <NeoC/Debug.h>

#include <threads.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "Chat.h"

typedef struct ChatServer_t {
	private String_t *_Handle;
	private in_port_t _Port;
	private Socket_t *_Socket;

	private Map_t in (Socket_t *, String_t *) *_User;
	private fd_set _State;
	private int32_t _Fd_Max;

	private bool _V;

	public void (* StartResponder)(struct ChatServer_t *) throws (Chat.Exception);
	public void (* StartServer)(struct ChatServer_t *) throws (Chat.Exception);

	public void (* Post)(struct ChatServer_t *, String_t *message) throws ();
} ChatServer_t;

typedef struct {
	public ChatServer_t *(* New)(String_t *handle, const in_port_t port, const bool v);
	public void (* Delete)(ChatServer_t *);

	public void (* StartResponder)(ChatServer_t *) throws (Chat.Exception);
	public void (* StartServer)(ChatServer_t *) throws (Chat.Exception);

	public void (* Post)(ChatServer_t *, String_t *message) throws ();
} _ChatServer;

extern _ChatServer ChatServer;
