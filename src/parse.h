#ifndef CODECRAFTERS_SHELL_PARSE_H_INCLUDED
#define CODECRAFTERS_SHELL_PARSE_H_INCLUDED

#include "ptr_array.h"

// Parses an array of tokens into an array of arguments (C strings).
PtrArray *parse(const PtrArray *tokens);

#endif
