#include "akinator.h"

int akinatorCtor(Akinator_t *akinator, short needVoice) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    akinator->root =  nodeCtor("Это неизвестно кто", nullptr, nullptr, nullptr, printElemT, &err);
    CHECK(!akinator->root, akinator->root, NULL_PTR);

    akinator->needVoice = needVoice;
    akinator->fileName  = nullptr;

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

int parseFile(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);
    CHECK_AKI(!akinator->fileName, FILE_NULL);

    FILE *akiFile  = fopen(akinator->fileName, "rb+");
    CHECK_AKI(!akiFile, FILE_NULL);
    
    int err = AKINATOR_OK;
    err |= readNodes(akinator->root, akiFile);

    fclose(akiFile);

    return err;
}

// PLAY SECTION

void akiNodeToFile(Node_t *node, FILE *file) {
    if (!node || !file) return;

    fprintf(file, "\n{ \"%s\"", node->value);

    int addedBracket = 1;
    if (node->left)  akiNodeToFile(node->left, file);
    if (!node->left) {
        fprintf(file, " }\n");
        addedBracket = 0;
    }

    if (node->right)  akiNodeToFile(node->right, file);
    if (!node->right && addedBracket) fprintf(file, " }\n");
}

int akinatorToFile(Akinator_t *akinator) {
    CHECK_AKI(!akinator || !akinator->fileName, AKINATOR_NULL);

    FILE *writeFile = fopen(akinator->fileName, "w");
    CHECK_AKI(!writeFile, FILE_NULL);

    akiNodeToFile(akinator->root, writeFile);
    fclose(writeFile);

    return AKINATOR_OK;
}

int addNewNode(Akinator_t *akinator, Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    char leftName[MAX_FILE_NAME] = "";
    akiPrint("Ладно, ты победил. Кто это был?\nСкажи пж: ");
    scanf("%s", leftName);

    Node_t *newRight = nodeCtor(node->value, nullptr, nullptr, node, printElemT);
    Node_t *newLeft  = nodeCtor(leftName, nullptr, nullptr, node, printElemT);

    akiPrint("Чем ");
    akiPrint(leftName);
    akiPrint(" отличается от ");
    akiPrint(newRight->value);
    akiPrint("?\n");

    char nodeQuest[MAX_FILE_NAME] = "";
    scanf("%s", nodeQuest);

    node->value = nodeQuest;
    node->right = newRight;
    node->left  = newLeft;

    int err = AKINATOR_OK;
    err |= akinatorToFile(akinator);
    if (err == AKINATOR_OK) akiPrint("Добавил :) Теперь я чуточку умнее.\n\n");

    return err;
}

int akiAsk(Akinator_t *akinator, Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    if (!(node->left && node->right)) {
        akiPrint("Это ");
    }
    akiPrint(node->value);
    akiPrint("? (да/нет)\n");

    int failure = 1;
    int err = AKINATOR_OK;
    while (failure) {
        char answer[10] = "";
        scanf("%s", answer);

        failure = 0;
        if (strcasecmp(answer, "да") == 0) {

            if (node->left) akiAsk(akinator, node->left);
            else akiPrint("Ха-ха, я умнее тебя! У меня памяти 16 Мегабайт!\n");

        } else if (strcasecmp(answer, "нет") == 0) {

            if (node->right) akiAsk(akinator, node->right);
            else err |= addNewNode(akinator, node);

        } else {
            akiPrint("Не понял, ещё раз:\n");
            failure = 1;
        }
    }

    return err;
}

int akiPlay(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    err |= akiAsk(akinator, akinator->root);
    if (err == AKINATOR_OK) chooseMode(akinator);

    return err;
}

// DEFINITION SECTION

Node_t* akiNodeDef(Node_t *node, const char *object) {
    if (!node || !object) return nullptr;

    if (strcasecmp(node->value, object) == 0) {
        return node;
    }

    if (node->left)  {
        Node_t *foundNode = akiNodeDef(node->left, object);
        if (foundNode) return foundNode;
    }
    if (node->right) {
        Node_t *foundNode = akiNodeDef(node->right, object);
        if (foundNode) return foundNode;
    }

    return nullptr;
}

int getObjStack(Akinator_t *akinator, Node_t *node, Stack_t *stack) {
    CHECK_AKI(!akinator || !node || !stack, AKINATOR_NULL);

    while (node != akinator->root) {
        node = node->previous;
        stackPush(stack, node);
    }

    return AKINATOR_OK;
}

int printObjectDef(Akinator_t *akinator, Node_t *node, Stack_t *stack) {
    CHECK_AKI(!akinator || !node || !stack, AKINATOR_NULL);

    akiPrint(node->value);
    int err = AKINATOR_OK;
    err |= getObjStack(akinator, node, stack);

    akiPrint(" это");
    int counter = stack->size;
    Node_t *current = (Node_t *) stackPop(stack);
    Node_t *next    = (Node_t *) stackPop(stack);
    while (counter > 0) {
        if (current->right == next) {
            akiPrint(" не ");
            akiPrint(current->value);
        } else {
            akiPrint(" ");
            akiPrint(current->value);
        }
        counter--;

        current = next;
        if (stack->size > 0) next = (Node_t *) stackPop(stack);
    }

    akiPrint("\n\n");

    return err;
}

int akiGiveDef(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akiPrint("Ну и кого мне искать?: ");
    char who[MAX_FILE_NAME] = "";
    scanf("%s", who);

    Node_t *foundNode = akiNodeDef(akinator->root, (const char*) who);
    int err = AKINATOR_OK;
    if (!foundNode) {
        akiPrint("Таких не имеем, у нас все нормальные, культурные ребята.\n\n");
    } else {
        Stack_t stack = {};
        _stackCtor(&stack, 1);
        err |= printObjectDef(akinator, foundNode, &stack);

        stackDtor(&stack);
    }

    chooseMode(akinator);

    return err;
}

// COMPARE

int akiCompare(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    char object1[MAX_FILE_NAME] = "", object2[MAX_FILE_NAME] = "";
    akiPrint("Введите первый объект: ");
    scanf("%s", object1);
    akiPrint("Введите второй объект: ");
    scanf("%s", object2);

    Node_t *objNode1 = akiNodeDef(akinator->root, object1);
    Node_t *objNode2 = akiNodeDef(akinator->root, object2);
    if (!objNode1 || !objNode2) {
        akiPrint("Чувствую скам...\n");
        chooseMode(akinator);

        return AKINATOR_OK;
    }

    if (objNode1 == objNode2) {
        akiPrint("Это одно и то же, дружище.\n");
        chooseMode(akinator);

        return AKINATOR_OK;
    }

    Stack_t stack1 = {}, stack2 = {};
    stackCtor(&stack1, 1);
    stackCtor(&stack2, 1);

    int err = AKINATOR_OK;
    err |= getObjStack(akinator, objNode1, &stack1);
    err |= getObjStack(akinator, objNode2, &stack2);

    akiPrint(object1);
    akiPrint(" похоже на ");
    akiPrint(object2);
    akiPrint(" тем, что они оба");

    int simCounter = 0, startSize1 = stack1.size, startSize2 = stack2.size;
    Node_t *cur1 = (Node_t*) stackPop(&stack1);
    Node_t *next1 = (Node_t*) stackPop(&stack1);
    Node_t *cur2 = (Node_t*) stackPop(&stack2);
    Node_t *next2 = (Node_t*) stackPop(&stack2);
    while (next1 == next2) {
        if (next1 == next2) {
            if (cur1->right == next1) {
                akiPrint(" не ");
                akiPrint(cur1->value);
            } else {
                akiPrint(" ");
                akiPrint(cur1->value);
            }
        }

        cur1 = next1;
        next1 = (Node_t*) stackPop(&stack1);
        cur2 = next2;
        next2 = (Node_t*) stackPop(&stack2);

        simCounter++;
    }

    akiPrint(", но ");
    akiPrint(object1);

    int counter = startSize1 - simCounter;
    while (counter > 0) {
        if (cur1->right == next1) {
            akiPrint(" не ");
            akiPrint(cur1->value);
        } else {
            akiPrint(" ");
            akiPrint(cur1->value);
        }
        counter--;

        cur1 = next1;
        if (stack1.size > 0) next1 = (Node_t*) stackPop(&stack1);
    }

    akiPrint(", а ");
    akiPrint(object2);

    counter = startSize2 - simCounter;
    while (counter > 0) {
        if (cur2->right == next2) {
            akiPrint(" не ");
            akiPrint(cur2->value);
        } else {
            akiPrint(" ");
            akiPrint(cur2->value);
        }
        counter--;

        cur2 = next2;
        if (stack2.size > 0) next2 = (Node_t*) stackPop(&stack2);
    }
    akiPrint("\n\n");

    stackDtor(&stack1);
    stackDtor(&stack2);

    chooseMode(akinator);
     
    return AKINATOR_OK;
}

//

int akiReadFile(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akiPrint("Введите название файла с деревом: ");
    char fileName[MAX_FILE_NAME] = "";
    scanf("%s", fileName);

    akinator->fileName = fileName;
    
    int err = AKINATOR_OK;
    err |= parseFile(akinator);

    return err;
}

void akiPrint(const char *message) {
    printf("%s", message);
}

int chooseMode(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    if (!akinator->fileName) akiReadFile(akinator);
    graphDump(akinator->root);

    akiPrint("Выберите режим:\n\
              0 - выход из программы\n\
              1 - играть\n\
              2 - дать определение\n\
              3 - сравнение\n");

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
            case DEFINITION:
                err |= akiGiveDef(akinator);
                break;
            case COMPARE:
                err |= akiCompare(akinator);
                break;
            default:
                akiPrint("Неизвестная комманда, попробуйте ещё раз.\n");
                failure = 1;
                break;
        }
    }
    
    return err;
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