#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mlibs/BinaryTree/tree.h"
#include "mlibs/Onegin/onegin.h"
#include "mlibs/Secured-Stack/stack.h"

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
    EXIT       = 0,
    PLAY       = 1,
    DEFINITION = 2,
    COMPARE    = 3,
    GRAPHICS   = 4,
    SOUND      = 5,
};

struct Akinator_t {
    Node_t *root = nullptr;

    const char *fileName  = nullptr;
    short       needVoice = 1;
};

#define CHECK_AKI(expression, errCode) {   \
    if (expression) {                       \
        return errCode;                      \
    }                                         \
}                                              \

int akinatorCtor(Akinator_t *akinator, short needVoice = 1);

int readNode(Node_t *node, FILE *akiFile);

int readNodes(Node_t *node, FILE *akiFile);

int parseFile(Akinator_t *akinator);

// PLAY SECTION

void akiNodeToFile(Node_t *node, FILE *file);

int akinatorToFile(Akinator_t *akinator);

int addNewNode(Akinator_t *akinator, Node_t *node);

int akiAsk(Akinator_t *akinator, Node_t *node);

int akiPlay(Akinator_t *akinator);

// DEFINITION SECTION

Node_t* akiNodeDef(Node_t *node, const char *object);

int getObjStack(Akinator_t *akinator, Node_t *node, Stack_t *stack);

int printObjectDef(Akinator_t *akinator, Node_t *node, Stack_t *stack);

int akiGiveDef(Akinator_t *akinator);

// COMPARE SECTION

int printCompared(Akinator_t *akinator, Node_t *objNode1, char object1[MAX_FILE_NAME], Node_t *objNode2, char object2[MAX_FILE_NAME]);

int akiCompare(Akinator_t *akinator);

//

int akiReadFile(Akinator_t *akinator);

int controlSound(Akinator_t *akinator);

void akiPrint(const char *message, short needVoice);

int chooseMode(Akinator_t *akinator);

int akiNodeDtor(Node_t *node);

int akinatorDtor(Akinator_t *akinator);