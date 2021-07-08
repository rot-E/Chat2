#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include <stdlib.h>

#include "Error.h"

typedef struct Failure_t {
	bool _Failure;
	int32_t _CaseNum;

	bool (* IsFailed)(struct Failure_t *);
	int32_t (* GetCaseNum)(struct Failure_t *);

	void (* Handle)(struct Failure_t *, const void (* handler)(const int32_t caseNum));
} Failure_t;


typedef struct {
	Failure_t *(* New)(const bool failure);
	Failure_t *(* Build)(const bool failure, const int32_t caseNum);
	void (* Release)(Failure_t *);
	void (* Delete)(Failure_t *);
} _Failure;

extern _Failure Failure;


typedef Failure_t *F;
