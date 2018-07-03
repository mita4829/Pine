#include "Parser.hpp"

Parser::Parser(){
    map<string, int> m; typeEnv.push(m);
}
Parser::~Parser(){}
Parser::Parser(vector<vector<string>> _lexems) : Parser() {
    lexems = _lexems;
    lineIndex = 0;
    if(lexems.size() > 0){
        line = lexems[0];
        curr = line[0];
    }
}
void Parser::next(){
    if(lineIndex+1 < line.size()){
        lineIndex += 1;
        curr = line[lineIndex];
    }
}
vector<Object*> Parser::getAST(){
    return abstract_syntax_tree;
}

void Parser::parse(){
    for(int i = 0; i<lexems.size(); i++){
        line = lexems[i];
        lineIndex = 0;
        curr = line[lineIndex];
        Object* tree = generic_parse();
        abstract_syntax_tree.push_back(tree);
    }
}

Object* Parser::generic_parse(){
    Object* result = nullptr;
    string default_func[] = {"print"};
    /*Default functions*/
    if(in<string>(curr, default_func, sizeof(default_func)/sizeof(default_func[0]))){
        if(curr == "print"){
            next();
            return print_parse();
        }
    }else if(curr == "let"){
        next();
        return let_parse();
    }else if(curr == "func"){
        next();
        return function_parse();
    }
    /*Atomic token*/
    else{
        Object* rtn = union_parse();
        return rtn;
    }
    /*Unexpected lexem*/
    RaisePineWarning("Unknown lexem found during parse");
    return result;
}

/* Print token [Stops on semi-colon] */
Object* Parser::print_parse(){
    next(); /* ( */
    Object* val = generic_parse();
    next(); /* ; */
    return new Print(val);
}

Object* Parser::let_parse(){
    string lval = curr;
    next(); // :
    next();
    int type = translateType(curr);
    bindType(lval, type);
    next(); // =
    next();
    Object* rval = generic_parse();
    return new Let(lval, type, rval);
}

Object* Parser::function_parse(){
    pushNewTypeEnv();
    string fname = curr;
    next(); /* ( */
    next();
    /* TODO: Capture argv list */
    vector<Object*> argv;
    
    next(); /* ) */
    next(); /* -> */
    int return_type = translateType(curr);
    bindType(fname, return_type);
    next(); /* { */
    next();
    vector<Object*> body;
    while(curr != "}"){
        Object* stmt = generic_parse();
        body.push_back(stmt);
        next();
    }
    next(); /* } */
    popTypeEnv();
    return new Function(fname, argv, new Seq(body), return_type);
}

/* Numerical tokens for expression [Stops on semi-colon] */
Object* Parser::union_parse(){
    Object* result = intersect_parse();
    string union_tokens[] = {"+", "-"};
    while(in(curr, union_tokens, sizeof(union_tokens)/sizeof(union_tokens[0]))){
        if(curr == "+"){
            next();
            result = new Binary(ADD, result, intersect_parse());
        }else if(curr == "-"){
            next();
            result = new Binary(SUB, result, intersect_parse());
        }
    }
    return result;
}

Object* Parser::intersect_parse(){
    Object* result = nots_parse();
    string intersect_tokens[] = {"*", "/", "%"};
    while(in(curr, intersect_tokens, sizeof(intersect_tokens)/sizeof(intersect_tokens[0]))){
        if(curr == "*"){
            next();
            result = new Binary(MUL, result, nots_parse());
        }else if(curr == "/"){
            next();
            result = new Binary(DIV, result, nots_parse());
        }else if(curr == "%"){
            next();
            result = new Binary(MOD, result, nots_parse());
        }
    }
    return result;
}

Object* Parser::nots_parse(){
    Object* result = atom_parse();
    string nots_tokens[] = {"-", "!", "~"};
    while(in(curr, nots_tokens, sizeof(nots_tokens)/sizeof(nots_tokens[0])) && result == nullptr){
        if(curr == "-"){
            next();
            result = new Unary(atom_parse(), NEG);
        }else if(curr == "!"){
            next();
            result = new Unary(atom_parse(), NOT);
        }else if(curr == "~"){
            next();
            result = new Unary(atom_parse(), IVT);
        }
    }
    return result;
}

Object* Parser::atom_parse(){
    Object* result = nullptr;
    string nots_tokens[] = {"-", "!", "~"};
    /* Unary operation */
    if(in(curr, nots_tokens, sizeof(nots_tokens)/sizeof(nots_tokens[0]))){
        return result;
    }
    /* Paren */
    else if(curr == "("){
        next(); /* ( */
        result = generic_parse();
        next(); /* ) */
    }
    /* Booleans */
    else if(curr == "True" || curr == "False"){
        if(curr == "True"){result = new Boolean(true);}
        else{result = new Boolean(false);}
        next();
    }
    /* Variable */
    else if(isVar(curr)){
        result = new Var(curr, getTypeForVar(curr));
        next();
    }
    /* Numeric value */
    else if((result = is_numeric(curr))){
        next();
    }
    
    return result;
}

Object* Parser::is_numeric(string val){
    try {
        int i = stoi(val);
        float f = stof(val);
        if(f != i){
            return new Float(f);
        }
        return new Integer(i);
    } catch (...) { /* Catch all exceptions for non-numeric value */
        return nullptr;
    }
    return nullptr;
}

int Parser::translateType(string type){
    if(type == "Int"){
        return INTEGER;
    }else if(type == "Float"){
        return FLOAT;
    }else if(type == "Double"){
        return DOUBLE;
    }else if(type == "String"){
        return STRING;
    }else if(type == "Bool"){
        return BOOLEAN;
    }
    RaisePineException("Missing or invalid type definition "+type);
    return 0;
}

bool Parser::isVar(string name){
    map<string, int>* s = &(typeEnv.top());
    if(s->find(name) != s->end()){
        return true;
    }
    return false;
}

void Parser::pushNewTypeEnv(){
    map<string, int> m;
    typeEnv.push(m);
}

map<string, int> Parser::popTypeEnv(){
    map<string, int> last = (typeEnv.top());
    typeEnv.pop();
    return last;
}

void Parser::bindType(string name, int type){
    map<string,int>* s = &(typeEnv.top());
    (*s)[name] = type;
}

int Parser::getTypeForVar(string name){
    map<string, int>* last = &(typeEnv.top());
    if(last->find(name) == last->end()){
        RaisePineException("Undefined variable type: "+name);
    }
    return (*last)[name];
}
