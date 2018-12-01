#include "Foundation.hpp"

#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Register.hpp"

class Compiler {
private:
    vector<Object*> ast;
    
    stack<vector<Object*>> flattenStmt;
    stack<vector<string>> compileStmt;
    stack<map<string, Int32>> stackLoc;
    vector<string> header;
    Register registerManager;
    
    Object* flatten(Object* expr);
    Tuple(string, Int32) compile(Object* expr);
    
    string IntoRegister(string operand);
    void addFlatStmtToStack(Object* stmt);
    void pushNewFlatStack();
    vector<Object*> popFlatStack();
    void addCompileStmt(string stmt);
    void addFrontCompileStmt(string stmt);
    void pushNewStackFrame();
    void popStackFrame();
    void pushNewStackLocationMap();
    void popStackLocationMap();
    void mapVarToStackLocation(string name);
    Int32 getStackLocationForVar(string name);
    void PolymorphicPrint(Object* expr, Tuple(string, Int32) result);
    
public:
    Compiler();
    Compiler(vector<Object*> ast);
    ~Compiler();
    
    string getAssembly();
    void generateBinary();
};

#endif
