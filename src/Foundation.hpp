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

extern vector<string> Trace;

#define ARCH64 8
#define ARCH32 4
#define LOG(str) Trace.push_back(str)
#define Printf(str) cout << str << endl
#define STR(x) to_string(x)
#define DEBUG(x) debug(STR(x))
#define VOID_ASM_OPERAND ""

#define TAG_DEPTH(i, d) (((Int64)(d) << 32) | i)
#define GET_DEPTH(i) ((UInt64)i) >> 32
#define GET_LEGNTH(i) (0x00000000FFFFFFFF & i)

#define GREEN   "\e[0;32m"
#define RED     "\e[1;31m"
#define YELLOW  "\e[1;33m"
#define WHITE   "\e[0m"

struct TypeContext {
    
    // An Explicit Type represents the type on a node prior to
    // type analysis. i.e. VAR("foo", INTEGER) is a node of explicit type VAR
    Int32 explicitType;
    
    // An Implicit Type represents the type on a node post type analysis.
    // i.e. VAR("foo", INTEGER) is a explicit type VAR node, but it implicitly
    // an INTEGER type.
    Int32 implicitType;
    
    // Array Depth is a positive integer to show the depth of a nested array.
    // If the arrayDepth is zero, then the node is not an array.
    // e.g.
    //     42;     => Depth 0
    //     [42]    => Depth 1
    //     [[42]]  => Depth 2
    //     ...
    //
    UInt32 arrayDepth;
    
    // Array Primative Element Type is the type of elements
    // that the array holds after nesting
    // e.g. [[[1,2], [2]]] is a 3D array, but the primative type
    // is INTEGER
    Int32 arrayPrimativeElementType;
    
    // stackLocation holds a integer that's a multiple of either 4 or 8
    // to indicate the stack offset location for the given node at the compile
    // phase
    Int32 stackLocation;
    
    string indexInstruction;
};

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
    BAND,
    BOR,
    XOR,
    LS,
    RS
};

enum PINE_TYPES {
    Void    = VOID,
    Int     = INTEGER,
    Float   = FLOAT,
    Double  = DOUBLE,
    Bool    = BOOLEAN,
    String  = STRING
};

void    debug(string);
void    RaisePineException(string);
void    RaisePineWarning(string);
string  AddressOf(void*);

bool isPrimative(Int32 Type);

class Object {
protected:
    Int32  Type = OBJECT;
    UInt32 poolID;
public:
    TypeContext typeContext;
    Object();
    virtual ~Object();
    Int32  getExplicitType();
    Int32  getImplicitType();
    UInt32 getMemoryPoolID();
    virtual void print();
    virtual Object* clone();
};

struct Bindings {
    Int32   type;
    Object* obj;
};

extern map<Int32, Object*> memoryPool;

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
    Int64  val;
public:
    Integer();
    Integer(Int64  val);
    ~Integer();
    Int64  getVal();
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
    TypeContext context;
public:
    Let();
    Let(string lval, TypeContext context, Object* rval);
    ~Let();
    Object* getVal();
    TypeContext getContext();
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
    Int32  getVarType();
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
    vector<tuple<Object*, Seq*>> ifStmt;
    Seq* Else = nullptr;
public:
    If();
    ~If();
    If(vector<tuple<Object*, Seq*>> ifStmt, Seq* Else);
    vector<tuple<Object*, Seq*>> getIfStmt();
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

class Array : public Object {
private:
    vector<Object*> array;
    vector<Int32> offsetTable;
    Int32 length;
    Int32 elementType;
    Int32 offsetSize;
public:
    Array();
    ~Array();
    Array(vector<Object*> array, Int32 length, Int32 elementType);
    vector<Object*> getArray();
    Int32 getLength();
    Int32 getElementType();
    Int32 getIndexOffsetSize();
    vector<Int32> getOffsetTable();
    virtual void print();
    virtual Object* clone();
};

class Index : public Object {
private:
    string  arrayName;
    vector<Object*> index;
    Int32   elementType;
public:
    Index();
    Index(string arrayName, vector<Object*> index, Int32 elementType);
    ~Index();
    Int32 getElementType();
    string getArrayName();
    vector<Object*> getIndex();
    void setIndex(Object* index, Int32 location);
    virtual void print();
    virtual Object* clone();
};

template <typename T>
T Safe_Cast(Object* obj) {
    return (T)obj;
}

string getTypeName(Int32 type);
void deleteObject(Object* ptr);
Int32 findPrimativeType(Object* obj);

#endif
