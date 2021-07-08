#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	Error_t *E;
	bool V;
} bool_E;

typedef struct {
	Error_t *E;
	int32_t V;
} int32_t_E;

typedef struct {
	Error_t *E;
	uint8_t V;
} uint8_t_E;

typedef struct {
	Error_t *E;
	uint8_t *V;
} uint8_t_ptr_E;


#define XE_DEF(X)			\
	typedef struct {		\
		Error_t *E;			\
		X V;				\
	} X ## E;

#define X_E_DEF(X)			\
	typedef struct {		\
		Error_t *E;			\
		X V;				\
	} X ## _E;

#define X_ptr_E_DEF(X)		\
	typedef struct {		\
		Error_t *E;			\
		X *V;				\
	} X ## _ptr_E;

#define ADTE_DEF(X)			\
	typedef struct {		\
		Error_t *E;			\
		X *V;				\
	} X ## E;
