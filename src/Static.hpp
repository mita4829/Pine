#ifndef STATIC_HPP
#define STATIC_HPP
#include "Foundation.hpp"
/*
 ConstantFold attempts to preform constant folding on the ast
 
 Parameters
    varBindings - A stack of maps binding previous defined variable names to Const*
    ast - The abstract syntax tree to preform constant folding on
    modified - A reference bool to let the caller know if the ast was optimized
 
 Return
    A constant folded ast
 */

class Static {
private:
    stack<map<string, Bindings>>* bindings = nullptr;
    Static() {};
    bool isVar(string name);
public:
    static Static& getStaticAnalyzer(){
        static Static instance;
        return instance;
    }
    
    Object* ConstantFold(stack<map<string, Bindings>>* bindings,
                        Object* ast);
    Object* Fold(Object* ast);
    void printBindings();
};


#endif
