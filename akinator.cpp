#include "akinator.h"

int akinatorCtor(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    akinator->root =  nodeCtor("Это неизвестно кто", nullptr, nullptr, nullptr, printElemT, &err);
    CHECK(!akinator->root, akinator->root, NULL_PTR);

    return err;
}

int readNode(Node_t *node, FILE *akiFile) {
    CHECK_AKI(!node, AKINATOR_NULL);
    CHECK_AKI(!akiFile, FILE_NULL);

    int symb = getc(akiFile);
    while (symb != '{' && symb != '}' && symb != '\0') {
        symb = getc(akiFile);
    }

    if (symb == EOF) return AKINATOR_OK;

    if (symb == '{') {
        int caveCounter = 0;
        char nodeText[MAX_NODE_LENGTH] = {};
        int nodeTextInd = 0;

        while (caveCounter < 2) {
            if (symb == '\"') caveCounter++;
            symb = getc(akiFile);

            if (caveCounter == 1) {
                nodeText[nodeTextInd] = (char) symb;
                nodeTextInd++;
            }
        }

        nodeText[nodeTextInd - 1] = '\0';
        node->value = (char*) calloc(nodeTextInd, sizeof(char));
        strcpy(node->value, nodeText);
    } 

    while (symb == ' ' || symb == '\n') {
        symb = getc(akiFile);
    }
    
    if (symb == '}') {
        node->left = node->right = nullptr;
        symb = getc(akiFile);
    } else {
        node->left  = nodeCtor("Это неизвестно кто", nullptr, nullptr, node, printElemT);
        node->right = nodeCtor("Это неизвестно кто", nullptr, nullptr, node, printElemT);
        ungetc(symb, akiFile);
    }

    return AKINATOR_OK;
}

int readNodes(Node_t *node, FILE *akiFile) {
    CHECK(!node, node, NULL_PTR);
    CHECK_AKI(!akiFile, FILE_NULL);

    int err = AKINATOR_OK;
    err |= readNode(node, akiFile);
    if (node->left)  readNodes(node->left,  akiFile);
    if (node->right) readNodes(node->right, akiFile);

    return err;
}

int parseFile(Akinator_t *akinator, const char *fileName) {
    CHECK_AKI(!akinator, AKINATOR_NULL);
    CHECK_AKI(!fileName, FILE_NULL);

    FILE *akiFile  = fopen(fileName, "rb+");
    CHECK_AKI(!akiFile, FILE_NULL);
    
    int err = AKINATOR_OK;
    err |= readNodes(akinator->root, akiFile);

    fclose(akiFile);

    return err;
}

int akinatorDtor(Akinator_t *akinator) {

    return AKINATOR_OK;
}