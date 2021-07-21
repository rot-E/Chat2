#pragma once

#include <NeoC/Annotation.h>
#include <NeoC/Exception/Signal.h>
#include <NeoC/Exception/Exception.h>

#include <stdint.h>

typedef struct {
	private void (*Setup)();

	public final SignalCode_t Exception;
	public final SignalCode_t HandleException;
	public final SignalCode_t NetworkException;

	public final int32_t HANDLE_LENGTH_MAX;
} _Chat;

extern _Chat Chat;
