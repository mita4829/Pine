#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Foundation.hpp"
#include "Register.hpp"

class Compiler {
private:
    /* ast given from parser */
    vector<Object*> ast;
    /* Flatten stmt */
    stack<vector<Object*>> flattenStmt;
    /* Compile stack location */
    stack<map<string, int>> stackLoc;
    
    stack<vector<string>> compileStmt;
    
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
    void pushNewStackFrame();
    void popStackFrame();
    void pushNewStackLocationMap();
    void mapVarToStackLocation(string);
    int getStackLocationForVar(string);
    void popStackLocationMap();
    /*Method to move operand regardless of compile type into an
     free register given from the registerManager.
     */
    string IntoRegister(string operand);
    
};

#endif
