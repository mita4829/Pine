#include "Foundation.hpp"
#ifndef PARSER_HPP
#define PARSER_HPP

enum Numeric {
    Int_Num,
    Float_Num,
    Double_Num,
    Non_Numeric,
};


class Parser {
private:
    vector<Object*> abstract_syntax_tree;
    vector<vector<string>> lexems;
    vector<string> line;
    vector<set<string>> varScope;
    stack<map<string, int>> typeEnv;
    
    string curr;
    int lineIndex;
    void next();
    
    Object* is_numeric(string val);
    int translateType(string type);
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
    Object* function_parse();
    Object* logical_or();
    Object* logical_and();
    Object* logical_parse();
    Object* union_parse();
    Object* intersect_parse();
    Object* nots_parse();
    Object* atom_parse();
    
    bool isVar(string);
   
    void pushNewTypeEnv();
    map<string, int> popTypeEnv();
    void bindType(string name, int type);
    int getTypeForVar(string name);
    
    vector<Object*> getAST();
};

#endif
