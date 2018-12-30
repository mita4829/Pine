#include "Foundation.hpp"

#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Register.hpp"

struct CompilerResult {
    Int32   resultType;
    Int32   stackLocation;
    string  data;
};

struct CompileBinding {
    Int32   stackLocation;
    Object* obj;
};

class Compiler {
private:
    vector<Object*> ast;
    vector<Object*> flatAst;
    
    stack<vector<Object*>> flattenStmt;
    stack<vector<string>> compileStmt;
    stack<map<string, Int32>> stackLoc;         // +
    stack<map<string, Object*>>  varBindings;   // +
    
    stack<map<string, CompileBinding>> bindings;
    void pushNewBindings();
    void popBindings();
    void setBindings(string varName, CompileBinding binding);
    void setBindings(string varName);
    CompileBinding getBindings(string varName);
    Int32 retrieveStackLocation(string varName);
    
    
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

    void PolymorphicPrint(Object* expr, CompilerResult result);
    
    void pushNewStackFrame();
    void popStackFrame();
    
    
public:
    Compiler();
    Compiler(vector<Object*> ast);
    ~Compiler();
    
    string getAssembly();
    void generateBinary();
};

#endif
