#include "akinator.h"

int akinatorCtor(Akinator_t *akinator, short needVoice) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    akinator->root =  nodeCtor(strdup("Это неизвестно кто"), nullptr, nullptr, nullptr, printElemT, &err);
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
        node->left  = nodeCtor(strdup("Это неизвестно кто"), nullptr, nullptr, node, printElemT);
        node->right = nodeCtor(strdup("Это неизвестно кто"), nullptr, nullptr, node, printElemT);
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
    akiPrint("Ладно, ты победил. Кто это был?\nСкажи пж: ", akinator->needVoice);
    mGetline(leftName, MAX_FILE_NAME, stdin);

    Node_t *newRight = nodeCtor(node->value, nullptr, nullptr, node, printElemT);
    Node_t *newLeft  = nodeCtor(strdup(leftName), nullptr, nullptr, node, printElemT);

    akiPrint("Чем ", akinator->needVoice);
    akiPrint(leftName, akinator->needVoice);
    akiPrint(" отличается от ", akinator->needVoice);
    akiPrint(newRight->value, akinator->needVoice);
    akiPrint("?\n", akinator->needVoice);

    char nodeQuest[MAX_FILE_NAME] = "";
    mGetline(nodeQuest, MAX_FILE_NAME, stdin);

    node->value = strdup(nodeQuest);
    node->right = newRight;
    node->left  = newLeft;

    int err = AKINATOR_OK;
    err |= akinatorToFile(akinator);
    if (err == AKINATOR_OK) akiPrint("Добавил :) Теперь я чуточку умнее.\n\n", akinator->needVoice);

    return err;
}

int akiAsk(Akinator_t *akinator, Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    if (!(node->left && node->right)) {
        akiPrint("Это ", akinator->needVoice);
    }
    akiPrint(node->value, akinator->needVoice);
    akiPrint("? (да/нет)\n", akinator->needVoice);

    int failure = 1;
    int err = AKINATOR_OK;
    while (failure) {
        char answer[MAX_FILE_NAME] = "";
        mGetline(answer, MAX_FILE_NAME, stdin);

        failure = 0;
        if (strcasecmp(answer, "да") == 0) {

            if (node->left) akiAsk(akinator, node->left);
            else akiPrint("Ха-ха, я умнее тебя! У меня памяти 16 Мегабайт!\n", akinator->needVoice);

        } else if (strcasecmp(answer, "нет") == 0) {

            if (node->right) akiAsk(akinator, node->right);
            else err |= addNewNode(akinator, node);

        } else {
            akiPrint("Не понял, ещё раз:\n", akinator->needVoice);
            failure = 1;
        }
    }

    return err;
}

int akiPlay(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    err |= akiAsk(akinator, akinator->root);

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

    akiPrint(node->value, akinator->needVoice);
    int err = AKINATOR_OK;
    err |= getObjStack(akinator, node, stack);

    akiPrint(" это", akinator->needVoice);
    size_t counter = stack->size;
    Node_t *current = (Node_t *) stackPop(stack);
    Node_t *next    = (Node_t *) stackPop(stack);
    while (counter > 0) {
        if (current->right == next) {
            akiPrint(" не ", akinator->needVoice);
            akiPrint(current->value, akinator->needVoice);
        } else {
            akiPrint(" ", akinator->needVoice);
            akiPrint(current->value, akinator->needVoice);
        }
        counter--;

        current = next;
        if (stack->size > 0) next = (Node_t *) stackPop(stack);
    }

    akiPrint("\n\n", akinator->needVoice);

    return err;
}

int akiGiveDef(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akiPrint("Ну и кого мне искать?: ", akinator->needVoice);
    char who[MAX_FILE_NAME] = "";
    mGetline(who, MAX_FILE_NAME, stdin);

    Node_t *foundNode = akiNodeDef(akinator->root, (const char*) who);
    int err = AKINATOR_OK;
    if (!foundNode) {
        akiPrint("Таких не имеем, у нас все нормальные, культурные ребята.\n\n", akinator->needVoice);
    } else {
        Stack_t stack = {};
        _stackCtor(&stack, 1);
        err |= printObjectDef(akinator, foundNode, &stack);

        stackDtor(&stack);
    }

    return err;
}

// COMPARE

int printCompared(Akinator_t *akinator, Node_t *objNode1, char object1[MAX_FILE_NAME], Node_t *objNode2, char object2[MAX_FILE_NAME]) {
    CHECK_AKI(!akinator || !objNode1 || !objNode2, AKINATOR_NULL);

    Stack_t stack1 = {}, stack2 = {};
    stackCtor(&stack1, 1);
    stackCtor(&stack2, 1);

    int err = AKINATOR_OK;
    err |= getObjStack(akinator, objNode1, &stack1);
    err |= getObjStack(akinator, objNode2, &stack2);

    akiPrint(object1, akinator->needVoice);
    akiPrint(" похоже на ", akinator->needVoice);
    akiPrint(object2, akinator->needVoice);
    akiPrint(" тем, что они оба", akinator->needVoice);

    size_t simCounter = 0, startSize1 = stack1.size, startSize2 = stack2.size;
    Node_t *cur1 = (Node_t*) stackPop(&stack1);
    Node_t *next1 = (Node_t*) stackPop(&stack1);
    Node_t *cur2 = (Node_t*) stackPop(&stack2);
    Node_t *next2 = (Node_t*) stackPop(&stack2);
    while (next1 == next2 && next1 && next2) {
        if (next1 == next2) {
            if (cur1) {
                if (cur1->right == next1) {
                    akiPrint(" не ", akinator->needVoice);
                    akiPrint(cur1->value, akinator->needVoice);
                } else {
                    akiPrint(" ", akinator->needVoice);
                    akiPrint(cur1->value, akinator->needVoice);
                }
            }
        }

        cur1 = next1;
        next1 = (Node_t*) stackPop(&stack1);
        cur2 = next2;
        next2 = (Node_t*) stackPop(&stack2);

        simCounter++;
    }

    akiPrint(", но ", akinator->needVoice);
    akiPrint(object1, akinator->needVoice);

    size_t counter = startSize1 - simCounter;
    while (counter > 0) {
        if (!next1) next1 = objNode1;

        if (cur1->right == next1) {
            akiPrint(" не ", akinator->needVoice);
            akiPrint(cur1->value, akinator->needVoice);
        } else {
            akiPrint(" ", akinator->needVoice);
            akiPrint(cur1->value, akinator->needVoice);
        }
        counter--;

        cur1 = next1;
        if (stack1.size > 0) next1 = (Node_t*) stackPop(&stack1);
        else next1 = objNode1;
    }

    akiPrint(", а ", akinator->needVoice);
    akiPrint(object2, akinator->needVoice);

    counter = startSize2 - simCounter;
    while (counter > 0) {
        if (!next2) next2 = objNode2;

        if (cur2->right == next2) {
            akiPrint(" не ", akinator->needVoice);
            akiPrint(cur2->value, akinator->needVoice);
        } else {
            akiPrint(" ", akinator->needVoice);
            akiPrint(cur2->value, akinator->needVoice);
        }
        counter--;

        cur2 = next2;
        if (stack2.size > 0) next2 = (Node_t*) stackPop(&stack2);
        else next2 = objNode2;
    }
    akiPrint("\n\n", akinator->needVoice);

    stackDtor(&stack1);
    stackDtor(&stack2);

    return err;
}

int akiCompare(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    char object1[MAX_FILE_NAME] = "", object2[MAX_FILE_NAME] = "";
    akiPrint("Введите первый объект: ", akinator->needVoice);
    mGetline(object1, MAX_FILE_NAME, stdin);
    akiPrint("Введите второй объект: ", akinator->needVoice);
    mGetline(object2, MAX_FILE_NAME, stdin);

    Node_t *objNode1 = akiNodeDef(akinator->root, object1);
    Node_t *objNode2 = akiNodeDef(akinator->root, object2);
    if (!objNode1 || !objNode2) {
        akiPrint("Чувствую скам...\n", akinator->needVoice);

        return AKINATOR_OK;
    }

    if (objNode1 == objNode2) {
        akiPrint("Это одно и то же, дружище.\n", akinator->needVoice);

        return AKINATOR_OK;
    }

    int err = AKINATOR_OK;
    err |= printCompared(akinator, objNode1, object1, objNode2, object2);
     
    return err;
}

//

void mGetline(char buf[], int lineLen, FILE *stream) {
    char* backSlashN = nullptr;
    do {
        fgets(buf, lineLen, stream);

        backSlashN = strchr(buf, '\n');
        if (backSlashN != nullptr)
            *backSlashN = '\0';
    }
    while(backSlashN == nullptr || strlen(buf) == 0);
}

int akiReadFile(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int success = 0;
    while(!success) {
        akiPrint("Введите название файла с деревом: ", 0);
        char fileName[MAX_FILE_NAME] = "";
        mGetline(fileName, MAX_FILE_NAME, stdin);

        akinator->fileName = strdup(fileName);
        
        int err = parseFile(akinator);
        if (!(err & FILE_NULL)) {
            success = 1;
        } else {
            akiPrint("Файл не найден, давай по новой, Вася!\n", 0);
        }
    }

    return AKINATOR_OK;
}

int controlSound(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    akinator->needVoice = !akinator->needVoice;
    if (akinator->needVoice) {
        akiPrint("Звук включён\n", 1);
    } else {
        akiPrint("Звук отключён\n", 0);
    }

    return AKINATOR_OK;
}

void akiPrint(const char *message, short needVoice) {
    printf("%s", message);
    if (needVoice) {
        char command[MAX_FILE_NAME] = "";
        sprintf(command, "echo \"%s\" | festival --language russian --tts > /dev/null 2> /dev/null", message);
        system(command);
    }
}

int chooseMode(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    if (!akinator->fileName) akiReadFile(akinator);

    int err = AKINATOR_OK;
    int failure = 1;
    while (failure) {
        akiPrint("Выберите режим:\n\
            0 - выход из программы\n\
            1 - играть\n\
            2 - дать определение\n\
            3 - сравнение\n\
            4 - графический дамп\n\
            5 - отключить(включить) звук\n\
            6 - прочитать новый файл\n", 0);

        int mode = -1;
        printf("Введите номер комманды: ");
        scanf("%d", &mode);

        switch (mode) {
            case EXIT:
                err |= akinatorDtor(akinator);
                failure = 0;
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
            case GRAPHICS:
                graphDump(akinator->root);
                break;
            case SOUND:
                err |= controlSound(akinator);
                break;
            case NEW_FILE:
                err |= akinatorDtor(akinator);
                err |= akinatorCtor(akinator);
                err |= akiReadFile(akinator);
                break;
            default:
                akiPrint("Неизвестная комманда, попробуйте ещё раз.\n", akinator->needVoice);
                break;
        }
    }
    
    return err;
}

int akiNodeDtor(Node_t *node) {
    CHECK(!node, node, NULL_PTR);

    if (node->left)  akiNodeDtor(node->left);
    if (node->right) akiNodeDtor(node->right);

    free(node->left);
    node->left = nullptr;
    free(node->right);
    node->right = nullptr;

    return AKINATOR_OK;
}

int akinatorDtor(Akinator_t *akinator) {
    CHECK_AKI(!akinator, AKINATOR_NULL);

    int err = AKINATOR_OK;
    err |= akiNodeDtor(akinator->root);

    return err;
}