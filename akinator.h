#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mlibs/BinaryTree/tree.h"
#include "mlibs/Onegin/onegin.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

enum AkinatorErrors {
    AKINATOR_OK   = 1 << 0,
    AKINATOR_NULL = 1 << 1,

    FILE_NULL     = 1 << 2,
};

struct Akinator_t {
    Node_t *root = nullptr;
};

#define CHECK_AKI(expression, errCode) {   \
    if (expression) {                       \
        return errCode;                      \
    }                                         \
}                                              \

int akinatorCtor(Akinator_t *akinator);

int readNode(Node_t *node, FILE *akiFile);

int readNodes(Node_t *node, FILE *akiFile);

int parseFile(Akinator_t *akinator, const char *fileName);

int akinatorDtor(Akinator_t *akinator);