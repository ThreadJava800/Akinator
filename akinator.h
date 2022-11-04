#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mlibs/BinaryTree/tree.h"
#include "mlibs/Onegin/onegin.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

const int MAX_FILE_NAME = 4096;

enum AkinatorErrors {
    AKINATOR_OK   = 1 << 0,
    AKINATOR_NULL = 1 << 1,

    FILE_NULL     = 1 << 2,
};

enum MODES {
    EXIT    = 0,
    PLAY    = 1,
};

struct Akinator_t {
    Node_t *root = nullptr;

    short   needVoice = 1;
};

#define CHECK_AKI(expression, errCode) {   \
    if (expression) {                       \
        return errCode;                      \
    }                                         \
}                                              \

int akinatorCtor(Akinator_t *akinator, short needVoice = 1);

int readNode(Node_t *node, FILE *akiFile);

int readNodes(Node_t *node, FILE *akiFile);

int parseFile(Akinator_t *akinator, const char *fileName);

int akiAsk(Node_t *node);

int akiPlay(Akinator_t *akinator);

void akiPrint(const char *message);

int chooseMode(Akinator_t *akinator);

int akiNodeDtor(Node_t *node);

int akinatorDtor(Akinator_t *akinator);