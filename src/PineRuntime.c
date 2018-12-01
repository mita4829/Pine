#include <stdio.h>

enum Expr {
    OBJECT,
    VAR,
    NUMBER,
    BOOLEAN,
    INTEGER,
    FLOAT,
    DOUBLE,
    STRING,
    VOID,
    LET,
    BINARY,
    COMPARE,
    PRINT,
    FUNCTION,
    UNARY,
    SEQ,
    IF,
    LOGICAL,
    ASSIGN,
    FOR,
    WHILE,
    STACKLOC,
    REG,
};

void PinePrint(int id, void* val){
    if(id == INTEGER){
        printf("%d", *((int*)val));
        return;
    }else if(id == FLOAT){
        printf("%f", *((float*)val));
        return;
    }else if(id == DOUBLE){
        printf("%lf", *((double*)val));
        return;
    }else if(id == BOOLEAN){
        int r = *((int*)val);
        if(r == 1){
            printf("True");
        }else{
            printf("False");
        }
        return;
    }
    else if(id == STRING){
        printf("%s", *((char**)val));
        return;
    }
    printf("Unknown id: %d\n", id);
}

void PinePrintInt(int val){
    printf("%d", val);
}

void PinePrintString(char* val){
    printf("%s", val);
}

void PinePrintBoolean(int val){
    if(val){
        printf("True");
    }
    else{
        printf("False");
    }
}

void PinePrintFloat(float val){
    printf("%f", val);
}
