#include "Foundation.hpp"

#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Register.hpp"

struct CompilerResult {
    Int32   resultType;
    Int32   stackLocation;
    Int32   dataType;
    string  reg;
    string  data;
};

class Compiler {
private:
    vector<Object*> ast;
    vector<Object*> flatAst;
    
    stack<vector<Object*>> flattenStmt;
    stack<vector<string>> compileStmt;
    stack<map<string, Int32>> stackLoc;
    stack<map<string, Object*>>  varBindings;
    vector<string> header;
    Register registerManager;
    
    Object* flatten(Object* expr);
    CompilerResult compile(Object* expr);
    
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
    void setVarStackLocation(string name, Int32 stackLocation);
    void PolymorphicPrint(Object* expr, CompilerResult result);
    void pushNewVarBindingEnv();
    void popVarBindingEnv();
    void bindVar(string name, Object*);
    
public:
    Compiler();
    Compiler(vector<Object*> ast);
    ~Compiler();
    
    string getAssembly();
    void generateBinary();
};

#endif
