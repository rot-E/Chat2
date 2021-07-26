#pragma once

#include <stdint.h>
#include <NeoC/Base/Object.h>
#include <NeoC/Base/Exception/Signal.h>
#include <NeoC/Base/Exception/Exception.h>

class _Chat {
	private void (*Setup)();

	public final SignalCode_t Exception;
	public final SignalCode_t HandleException;
	public final SignalCode_t NetworkException;

	public final int32_t HANDLE_LENGTH_MAX;
} _Chat;

extern _Chat Chat;
