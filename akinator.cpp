#include "akinator.h"

int akinatorCtor(Akinator_t *akinator, short needVoice) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    akinator->root =  nodeCtor("Это неизвестно кто", nullptr, nullptr, nullptr, printElemT, &err);
    CHECK(!akinator->root, akinator->root, NULL_PTR);

    akinator->needVoice = needVoice;

    return err;
}

int readNode(Node_t *node, FILE *akiFile) {
    CHECK_AKI(!node, AKINATOR_NULL);
    CHECK_AKI(!akiFile, FILE_NULL);

    int symb = getc(akiFile);
    while (symb != '{' && symb != '}' && symb != EOF) {
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

        node->value = (char*) calloc((size_t) nodeTextInd, sizeof(char));
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
    graphDump(node);

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

int akiAsk(Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    if (!(node->left && node->right)) {
        akiPrint("Это ");
    }
    akiPrint(node->value);
    akiPrint("? (да/нет)\n");

    int failure = 1;
    while (failure) {
        char answer[10] = "";
        scanf("%s", answer);

        failure = 0;
        if (strcasecmp(answer, "да") == 0) {
            if (node->left) akiAsk(node->left);
            else akiPrint("Ха-ха, я умнее тебя! У меня памяти 16 Мегабайт!\n");
        } else if (strcasecmp(answer, "нет") == 0) {
            akiAsk(node->right);
        } else {
            akiPrint("Не понял, ещё раз:\n");
            failure = 1;
        }
    }

    return AKINATOR_OK;
}

int akiPlay(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akiPrint("Введите название файла с деревом: ");
    char fileName[MAX_FILE_NAME] = "";
    scanf("%s", fileName);
    
    int err = AKINATOR_OK;
    err |= parseFile(akinator, fileName);
    if (err != AKINATOR_OK) return err;

    err |= akiAsk(akinator->root);
    if (err == AKINATOR_OK) chooseMode(akinator);

    return err;
}

void akiPrint(const char *message) {
    printf("%s", message);
}

int chooseMode(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akiPrint("Выберите режим:\n\
              0 - выход из программы\n\
              1 - играть\n");

    int err = AKINATOR_OK;
    int failure = 1;
    while (failure) {
        int mode = -1;
        printf("Введите номер комманды: ");
        scanf("%d", &mode);
        failure = 0;

        switch (mode) {
            case EXIT:
                akinatorDtor(akinator);
                exit(0);
                break;
            case PLAY:
                err |= akiPlay(akinator);
                break;
            default:
                akiPrint("Неизвестная комманда, попробуйте ещё раз.\n");
                failure = 1;
                break;
        }
    }
    
    return AKINATOR_OK;
}

int akiNodeDtor(Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    if (node->left)  akiNodeDtor(node->left);
    if (node->right) akiNodeDtor(node->right);

    if (node->value) free(node->value);
    free(node);

    return AKINATOR_OK;
}

int akinatorDtor(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = akiNodeDtor(akinator->root);

    return err;
}