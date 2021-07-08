#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	Failure_t *F;
	bool V;
} bool_F;

typedef struct {
	Failure_t *F;
	int32_t V;
} int32_t_F;

typedef struct {
	Failure_t *F;
	uint8_t V;
} uint8_t_F;

typedef struct {
	Failure_t *F;
	uint8_t *V;
} uint8_t_ptr_F;


#define XF_DEF(X)			\
	typedef struct {		\
		Failure_t *F;		\
		X V;				\
	} X ## F;

#define X_F_DEF(X)			\
	typedef struct {		\
		Failure_t *F;		\
		X V;				\
	} X ## _F;

#define X_ptr_F_DEF(X)		\
	typedef struct {		\
		Failure_t *F;		\
		X *V;				\
	} X ## _ptr_F;

#define ADTF_DEF(X)			\
	typedef struct {		\
		Failure_t *F;		\
		X *V;				\
	} X ## F;
