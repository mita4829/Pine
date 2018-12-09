#ifndef FOUNDATION_HPP
#define FOUNDATION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <stack>
#include <cstdint>
#include <fstream>
#include <sstream>
using namespace std;

extern vector<string> Trace;
#define LOG(str) Trace.push_back(str)
#define Printf(str) cout << str << endl
#define tuple(T,V) make_tuple(T, V)
#define Tuple(T,V) tuple<T, V>
#define STR(x) to_string(x)
#define VOID_ASM_OPERAND ""

#define GREEN   "\e[0;32m"
#define RED     "\e[1;31m"
#define YELLOW  "\e[1;33m"
#define WHITE   "\e[0m"
#define CYAN    "\x1B3[36m"

/*
    Global types for constant width for cross-platform compilation
*/
typedef int8_t      Int8;
typedef int16_t     Int16;
typedef int32_t     Int32;
typedef int64_t     Int64;
typedef uint8_t     UInt8;
typedef uint16_t    UInt16;
typedef uint32_t    UInt32;
typedef uint32_t    UInt;
typedef uint64_t    UInt64;
typedef char        Char;

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

enum OPERATION {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQU,
    NEQ,
    LT,
    LTE,
    GTE,
    GT,
    NEG,
    NOT,
    IVT,
    AND,
    OR,
};

enum PINE_TYPES {
    Void    = VOID,
    Int     = INTEGER,
    Float   = FLOAT,
    Double  = DOUBLE,
    Bool    = BOOLEAN,
    String  = STRING
};

void    DEBUG(string);
void    RaisePineException(string);
void    RaisePineWarning(string);
string  AddressOf(void*);

bool isPrimative(Int32 Type);

class Object {
protected:
    Int32  Type = OBJECT;
public:
    Object();
    virtual ~Object();
    Int32  getType();
    virtual void print();
    virtual Object* clone();
};

class Number : public Object {
public:
    Number();
    ~Number();
    virtual void print();
    virtual Object* clone();
};

class Boolean : public Object {
private:
    bool val;
public:
    Boolean();
    Boolean(bool val);
    ~Boolean();
    bool getVal();
    virtual void print();
    virtual Object* clone();
};

class Integer : public Number {
private:
    Int32  val;
public:
    Integer();
    Integer(Int32  val);
    ~Integer();
    Int32  getVal();
    virtual void print();
    virtual Object* clone();
};

class Float : public Number {
private:
    float val;
public:
    Float();
    Float(float val);
    ~Float();
    float getVal();
    virtual void print();
    virtual Object* clone();
};

class Double : public Number {
private:
    double val;
public:
    Double();
    Double(double val);
    ~Double();
    double getVal();
    virtual void print();
    virtual Object* clone();
};

class String : public Object {
private:
    string val;
public:
    String();
    String(string val);
    ~String();
    string getVal();
    virtual void print();
    virtual Object* clone();
};

class Let : public Object {
private:
    string lval;
    Object* rval = nullptr;
    Int32  expectedType;
public:
    Let();
    Let(string lval, Int32  expectedType, Object* rval);
    ~Let();
    Object* getVal();
    Int32  getExpectedType();
    string getName();
    virtual void print();
    virtual Object* clone();
};

class Binary : public Object {
private:
    Object* left;
    Object* right;
    Int32  operation;
public:
    Binary();
    Binary(Int32  operation, Object* left, Object* right);
    ~Binary();
    Object* getLeft();
    Object* getRight();
    void setLeft(Object*);
    void setRight(Object*);
    Int32  getOperation();
    virtual void print();
    virtual Object* clone();
};

class Compare : public Object {
private:
    Object* left;
    Object* right;
    Int32  operation;
public:
    Compare();
    Compare(Int32  operation, Object* left, Object* right);
    ~Compare();
    Object* getLeft();
    Object* getRight();
    void setLeft(Object*);
    void setRight(Object*);
    Int32  getOperation();
    virtual void print();
    virtual Object* clone();
};

class Var : public Object {
private:
    string name;
    Int32  type;
public:
    Var();
    ~Var();
    Var(string name, Int32  type);
    string getName();
    Int32  getType();
    virtual void print();
    virtual Object* clone();
};


class Print : public Object {
private:
    Object* val = nullptr;
public:
    Print();
    Print(Object* val);
    ~Print();
    Object* getVal();
    virtual void print();
    virtual Object* clone();
};

class Seq : public Object {
private:
    vector<Object*> stmt;
public:
    Seq();
    ~Seq();
    Seq(vector<Object*>);
    vector<Object*> getStatements();
    virtual void print();
    virtual Object* clone();
};

class Function : public Object {
private:
    string name;
    vector<Object*> argv;
    Seq* body;
    Int32  return_type;
public:
    Function();
    Function(string name, vector<Object*> argv, Seq* body, Int32  return_type);
    ~Function();
    string getName();
    vector<Object*> getArgv();
    Seq* getBody();
    Int32  getReturnType();
    virtual void print();
    virtual Object* clone();
};

class Unary : public Object {
private:
    Object* val;
    Int32  operation;
public:
    Unary();
    ~Unary();
    Unary(Object* val, Int32  operation);
    Object* getVal();
    Int32  getOperation();
    virtual void print();
    virtual Object* clone();
};

class If : public Object {
private:
    Object* condition;
    Seq* body;
    Seq* Else = nullptr;
public:
    If();
    ~If();
    If(Object* condition, Seq* body, Seq* Else);
    Object* getCondition();
    Seq* getBody();
    Seq* getElse();
    virtual void print();
    virtual Object* clone();
};

class Logical : public Object {
private:
    Int32  operation;
    Object* left = nullptr;
    Object* right = nullptr;
public:
    Logical();
    ~Logical();
    Logical(Int32, Object*, Object*);
    Int32  getOperation();
    Object* getLeft();
    Object* getRight();
    void setLeft(Object*);
    void setRight(Object*);
    virtual void print();
    virtual Object* clone();
};

class Assign : public Object {
private:
    Object* name;
    Object* val = nullptr;
public:
    Assign();
    ~Assign();
    Assign(Object*, Object*);
    Object* getVar();
    Object* getVal();
    void replaceVal(Object*);
    virtual void print();
    virtual Object* clone();
};

class For : public Object {
private:
    Object* declare;
    Object* condition;
    Object* incrementor;
    Seq* body;
public:
    For();
    ~For();
    For(Object*, Object*, Object*, Seq*);
    Object* getDeclare();
    Object* getCondition();
    Object* getIncl();
    Seq* getBody();
    virtual void print();
    virtual Object* clone();
};

class While : public Object {
private:
    Object* condition;
    Seq* body;
public:
    While();
    ~While();
    While(Object* condition, Seq* body);
    Object* getCondition();
    Seq* getBody();
    virtual void print();
    virtual Object* clone();
};

template <typename T>
T Safe_Cast(Object* obj) {
    return (T)obj;
}

string getTypeName(Int32 type);
#endif
