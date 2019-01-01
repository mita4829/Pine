#ifndef PARSER_HPP
#define PARSER_HPP
#include "Foundation.hpp"
#include "Static.hpp"

class Parser {
private:
    vector<Object*> abstract_syntax_tree;
    vector<vector<string>> lexems;
    vector<string> line;
    
    stack<map<string, Bindings>> bindings;
    string curr;
    /* volatileVar when greater than zero prevent variable names from being
       replaced with its value. This is useful when optimizating
       inside a loop to assume all variable to be volatile.
     */
    Int32 volatileVars = 0;
    Static analyzer = Static::getStaticAnalyzer();
    
    Int32 lineIndex;
    void next();
    string peek();
    
    Object* is_numeric(string val);
    Int32 translateType(string type, Int32& arrayDepth);
    
public:
    Parser();
    ~Parser();
    Parser(vector<vector<string>> lexems);
    
    /* Collection of parse methods */
    void parse();
    Object* generic_parse();
    Object* print_parse();
    Object* let_parse();
    Object* if_parse();
    Object* for_parse();
    Object* while_parse();
    Object* function_parse();
    Object* array_parse();
    Object* static_analysis();
    Object* bit_or();
    Object* bit_and();
    Object* bit_xor();
    Object* logical_or();
    Object* logical_and();
    Object* logical_parse();
    Object* shift_parse();
    Object* union_parse();
    Object* intersect_parse();
    Object* nots_parse();
    Object* atom_parse();
    
    bool isVar(string);
   
    void pushNewTypeEnv();
    void pushNewVarBindingEnv();
    map<string, Int32> popTypeEnv();
    void popVarBindingEnv();
    void bindType(string name, Int32 type);
    void bindVar(string name, Object*);
    
    
    void pushNewBindings();
    void popBindings();
    Bindings getBindings(string varName);
    Int32 getTypeForVar(string name);
    void setBindings(string, Bindings);
    vector<Object*> getAST();
};

#endif
