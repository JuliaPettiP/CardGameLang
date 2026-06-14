#ifndef SEMANTIC_VALIDATOR_HEADER
#define SEMANTIC_VALIDATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/CompilationStatus.h"

CompilationStatus validateSemantics(Program * program);

#endif