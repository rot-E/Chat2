#pragma once

typedef struct {
	Error *E;
	int Int;
} intE;

typedef struct {
	Error *E;
	char *String;
} StringE;

typedef struct {
	Error *E;
	struct sockaddr *sockaddr;
} sockaddrE;
