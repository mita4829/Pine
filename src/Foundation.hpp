#ifndef FOUNDATION_HPP
#define FOUNDATION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <stack>
using namespace std;

/* Expr enum holds constants that are used with instances
 to distingush the type of an object value at runtime. */
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
};

class Object {
protected:
    int Type = OBJECT;
public:
    Object();
    ~Object();
    int getType();
    virtual void print();
};

class Number : public Object {
public:
    Number();
    ~Number();
    virtual void print();
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
};

class Integer : public Number {
private:
    int val;
public:
    Integer();
    Integer(int val);
    ~Integer();
    int getVal();
    virtual void print();
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
};

class Let : public Object {
private:
    string lval;
    Object* rval = nullptr;
    int expectedType;
public:
    Let();
    Let(string lval, int expectedType, Object* rval);
    ~Let();
    Object* getVal();
    int getExpectedType();
    string getName();
    virtual void print();
};

class Binary : public Object {
private:
    Object* left;
    Object* right;
    int operation;
public:
    Binary();
    Binary(int operation, Object* left, Object* right);
    ~Binary();
    Object* getLeft();
    Object* getRight();
    int getOperation();
    virtual void print();
};

class Compare : public Object {
private:
    Object* left;
    Object* right;
    int operation;
public:
    Compare();
    Compare(int operation, Object* left, Object* right);
    ~Compare();
    Object* getLeft();
    Object* getRight();
    int getOperation();
    virtual void print();
};

class Var : public Object {
private:
    string name;
    int type;
public:
    Var();
    ~Var();
    Var(string name, int type);
    string getName();
    int getType();
    virtual void print();
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
};

class Seq : public Object {
private:
    vector<Object*> stmt;
public:
    Seq();
    Seq(vector<Object*>);
    vector<Object*> getStatements();
    virtual void print();
};

class Function : public Object {
private:
    string name;
    vector<Object*> argv;
    Seq* body;
    int return_type;
public:
    Function();
    Function(string name, vector<Object*> argv, Seq* body, int return_type);
    ~Function();
    string getName();
    vector<Object*> getArgv();
    Seq* getBody();
    int getReturnType();
    virtual void print();
};

class Unary : public Object {
private:
    Object* val;
    int operation;
public:
    Unary();
    Unary(Object* val, int operation);
    Object* getVal();
    int getOperation();
    virtual void print();
};



template <typename T>
T Safe_Cast(Object* obj) {
    return (T)obj;
}

template <typename T>
bool in(T str, T tokens[], int length) {
    for(int i = 0; i<length; i++){
        if(str == tokens[i]){
            return true;
        }
    }
    return false;
}

void DEBUG(std::string);
void RaisePineException(std::string);
void RaisePineWarning(std::string);
#endif
