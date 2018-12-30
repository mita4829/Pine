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
    INDEX,
    ARRAY,
    NIT
};

#define GET_DEPTH(i) ((unsigned long long int)i) >> 32
#define GET_LEGNTH(i) (0x00000000FFFFFFFF & i)

void PinePrint(int id, void* val){
    if(id == INTEGER){
        printf("%lld", *((long long int*)val));
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

void PinePrintDouble(double val){
    printf("%lf", val);
}

long long int* PinePrintArrayHelper(int type, int length, long long int* array){
    
    for (int i = 0; i < length; i++){
        if (type == INTEGER){
            printf("%lld", *array);
        }
        else if (type == FLOAT){
            printf("%f", (float)(*array));
        }
        else if (type == BOOLEAN){
            if (*array != 0) {
                printf("True");
            }else{
                printf("False");
            }
        }
        else if (type == STRING){
            printf("\"%s\"", (char*)(*array));
        }
        else if(type == DOUBLE){
            printf("%lf", (double)(*array));
        }
        else {
            printf("Unknown type: %d\n", type);
        }
        array--;
        if (i != length - 1) {
            printf(", ");
        }
    }
    
    return array;
}

long long int* PinePrintArray(int type, void* buffer){
    long long int* array = (long long int*)buffer;
    unsigned long long int intLength = GET_LEGNTH(array[0]);
    unsigned long long int depth = GET_DEPTH(array[0]);
    
    unsigned long long int length;
    printf("[");
    if (depth > 1) {
        array--;
        
        for (int i = 0; i < intLength; i++) {
            array = PinePrintArray(type, array);
            if (i != intLength - 1){
                printf(", ");
            }
        }
        
    }
    else {
        array--;
        array = PinePrintArrayHelper(type, intLength, array);
    }
    printf("]");
    return array;
}

