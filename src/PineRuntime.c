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
    ARRAY = (1 << 8)
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

void PinePrintArray(int length, int type, void* buffer){
    printf("[");
    if(type == INTEGER){
        long long int* array = (long long int*)buffer;
        for(int i = 0; i < length; i++){
            printf("%lld", (*array));
            if(i != length-1){
                printf(", ");
            }
            array--;
        }
    }
    else if(type == BOOLEAN){
        long long int* array = (long long int*)buffer;
        for(int i = 0; i < length; i++){
            long long int boolValue = *array;
            if(boolValue == 1){
                printf("True");
            }else{
                printf("False");
            }
            if(i != length-1){
                printf(", ");
            }
            array--;
        }
    }
    else if(type == FLOAT){
        float* array = (float*)buffer;
        for(int i = 0; i < length; i++){
            printf("%f", *array);
            if(i != length-1){
                printf(", ");
            }
            array--;
        }
    }
    printf("]");
}

void PinePrintLeftBracket(){
    printf("[");
}

void PinePrintRightBracket(){
    printf("]");
}

void PinePrintComma(){
    printf(", ");
}
