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
    LET,
    BINARY,
    COMPARE,
    PRINT,
    FUNCTION,
    UNARY,
    SEQ,
    
    
    REG,
    STACKLOC,
};

void PinePrint(int id, void* val){
    if(id == INTEGER){
        printf("%d\n", *((int*)val));
        return;
    }else if(id == FLOAT){
        printf("%f\n", *((float*)val));
        return;
    }else if(id == DOUBLE){
        printf("%lf\n", *((double*)val));
        return;
    }else if(id == BOOLEAN){
        int r = *((int*)val);
        if(r == 1){
            printf("True\n");
        }else{
            printf("False\n");
        }
        return;
    }
    else if(id == STRING){
        printf("%s\n", *((char**)val));
        return;
    }
    printf("Unknown id: %d\n", id);
}
