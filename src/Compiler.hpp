#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Foundation.hpp"
#include "Register.hpp"

/*
 Returns a multiple of 32 integer dependent on varCount.
 This number n = 32x, where n is the first value of x that satifies
 varCount * 8 <= 32 * x.
 MacOS requires 32-byte alignment for SIMD optimization on some function calls.
 */
int align32ByteStack(int varCount);
int requestFloatID();

class Compiler {
private:
    /* ast given from parser */
    vector<Object*> ast;
    /* Flatten stmt */
    stack<vector<Object*>> flattenStmt;
    /* Compile stack location */
    stack<map<string, int>> stackLoc;
    stack<vector<string>> compileStmt;
    vector<string> header;
    
    Register registerManager;
    
public:
    Compiler();
    Compiler(vector<Object*> _ast);
    
    vector<string> getAssembly();
    
    /*Flatten methods*/
    Object* flatten(Object* expr);
    string requestTmpVarName();
    void addFlatStmtToStack(Object* stmt);
    void pushNewFlatStack();
    vector<Object*> popFlatStack();
    bool isValue(int);
    
    /*Type checker method*/
    
    /*Compile methods*/
    tuple<string, int> compile(Object* expr);
    int getStackLocationFor(string var);
    void addCompileStmt(string stmt);
    void addFrontCompileStmt(string stmt);
    void pushNewStackFrame();
    void popStackFrame();
    void pushNewStackLocationMap();
    void mapVarToStackLocation(string);
    int getStackLocationForVar(string);
    void popStackLocationMap();
    void PolymorphicPrint(Object* expr, tuple<string, int>);
    
    int getType(Object* expr);
    /*Method to move operand regardless of compile type into an
     free register given from the registerManager.
     */
    string IntoRegister(string operand);
    
};

#endif
