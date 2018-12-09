#include "Parser.hpp"

template <typename T, typename V>
T absDiff(T x, V y) {
    LOG("Parser:absDiff");
    LOG("    x:"+to_string(x)+", y:"+to_string(y));
    T result = x - y;
    result = result < 0 ? -result : result;
    LOG("Parser:-absDiff");
    return result;
}

Parser::Parser(){
    LOG("Parser:Parser");
    map<string, Int32> m; typeEnv.push(m);
    map<string, Object*> s; varBindings.push(s);
}

Parser::~Parser(){
    LOG("Parser:~Parser");
    LOG("Parser:    deleting ast");
    for(auto& node : abstract_syntax_tree){
        if(node != nullptr){
            deleteObject(node);
            node = nullptr;
        }
    }

    LOG("Parser:    deleting varBindings");
    while(varBindings.size() != 0){
        popVarBindingEnv();
    }
    LOG("Parser:-~Parser");
}

Parser::Parser(vector<vector<string>> _lexems) : Parser() {
    LOG("Parser:Parser");
    LOG("    lexem: 0x"+AddressOf(&_lexems));
    lexems = _lexems;
    lineIndex = 0;
    if(lexems.size() > 0){
        line = lexems[0];
        curr = line[0];
    }
    LOG("Parser:-Parser");
}

void Parser::next(){
    LOG("Parser:next");
    LOG("    curr: "+curr);
    if(lineIndex+1 < line.size()){
        lineIndex += 1;
        curr = line[lineIndex];
    }
}

string Parser::peek(){
    LOG("Parser:peek");
    if(lineIndex+1 < line.size()){
        return line[lineIndex+1];
    }
    return "";
}

vector<Object*> Parser::getAST(){
    LOG("Paser::getAST");
    return abstract_syntax_tree;
}

void Parser::parse(){
    LOG("Parser:parse");
    for(const auto& _line : lexems){
        lineIndex = 0;
        curr = _line[lineIndex];
        line = _line;
        Object* tree = generic_parse();
        abstract_syntax_tree.push_back(tree);
    }
    LOG("Parser:-parse");
}

Object* Parser::generic_parse(){
    LOG("Parser:generic_parse");
    Object* result = nullptr;
    string default_func_tokens[] = {"print"};
    set<string> default_func(begin(default_func_tokens),
                             end(default_func_tokens));
    /*Default functions*/
    if(default_func.find(curr) != default_func.end()){
        if(curr == "print"){
            next();
            return print_parse();
        }
    }else if(curr == "let"){
        next();
        return let_parse();
    }else if(curr == "if"){
        next();
        return if_parse();
    }else if(curr == "func"){
        next();
        return function_parse();
    }else if(curr == "for"){
        next();
        return for_parse();
    }
    else if(curr == "while"){
        next();
        return while_parse();
    }
    /*Atomic token*/
    else{
        Object* rtn = static_analysis();
        return rtn;
    }
    /*Unexpected lexem*/
    RaisePineWarning("Unknown lexem found during parse");
    return result;
}

/* Print token [Stops on semi-colon] */
Object* Parser::print_parse(){
    LOG("Parser:print_parse");
    next(); /* ( */
    Object* val = generic_parse();
    next(); /* ; */
    LOG("Parser::-print_parse");
    return new Print(val);
}

Object* Parser::let_parse(){
    LOG("Parser:let_parse");
    string lval = curr;
    next(); // :
    int type = OBJECT;
    if(curr == ":"){
        next();
        type = translateType(curr);
        next();
    }
    next(); // =
    Object* rval = generic_parse();
    /*
     Infer the type of if no type was given
     */
    if(type == OBJECT){
        type = rval->getType();
    }
    /*
     If rval isPrimative, add a binding for future
     static analysis
     */
    if(isPrimative(type)){
        bindVar(lval, rval);
    }
    bindType(lval, type);
    
    LOG("Parser::-let_parse");
    return new Let(lval, type, rval);
}

Object* Parser::if_parse(){
    LOG("Parser:if_parse");
    next(); // (
    Object* condition = generic_parse();
    next(); // )
    next(); // {
    vector<Object*> body;
    while(curr != "}"){
        Object* stmt = generic_parse();
        body.push_back(stmt);
        next();
    }
    next(); // }
    vector<Object*> Else;
    if(curr == "else"){
        next();
        next();
        while(curr != "}"){
            Object* stmt = generic_parse();
            Else.push_back(stmt);
            next();
        }
        next();
    }
    if(curr != ";"){
        RaisePineException("If statement expected semi-colon at ending.");
    }
    LOG("Parser::-if_parse");
    return new If(condition, new Seq(body), new Seq(Else));
}

Object* Parser::for_parse(){
    LOG("Parser:for_parse");
    volatileVars += 1;
    next(); // (
    Object* decl = generic_parse();
    next(); // ;
    Object* cond = generic_parse();
    next(); // ;
    Object* incl = generic_parse();
    next(); // )
    next(); // {
    vector<Object*> body;
    while(curr != "}"){
        Object* stmt = generic_parse();
        body.push_back(stmt);
        next();
    }
    next(); /* } */
    volatileVars -= 1;
    if(curr != ";"){
        RaisePineException("For loop expected semi-colon at ending.");
    }
    LOG("Parser::-for_parse");
    return new For(decl, cond, incl, new Seq(body));
}

Object* Parser::while_parse(){
    LOG("Parser::while_parse");
    volatileVars += 1;
    next(); // (
    Object* cond = generic_parse();
    next(); // )
    next(); // {
    vector<Object*> body;
    while(curr != "}"){
        Object* stmt = generic_parse();
        body.push_back(stmt);
        next();
    }
    next(); // }
    volatileVars -= 1;
    if(curr != ";"){
         RaisePineException("While loop expected semi-colon at ending.");
    }
    LOG("Parser:-while_parse");
    return new While(cond, new Seq(body));
}

Object* Parser::function_parse(){
    LOG("Paser::function_parse");
    pushNewTypeEnv();
    string fname = curr;
    next(); /* ( */
    next();
    /* TODO: Capture argv list */
    vector<Object*> argv;
    
    next(); /* ) */
    next(); /* -> */
    Int32 return_type = translateType(curr);
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
    LOG("Parser::-function_parse");
    return new Function(fname, argv, new Seq(body), return_type);
}

Object* Parser::static_analysis(){
    LOG("Parser::static_analysis");
    Object* result = logical_or();
    
    /*
     Preform static analysis on the result to see if
     optimizations can be applied
     */
    
    result = analyzer.ConstantFold(volatileVars != 0 ? nullptr : &varBindings, result);

    LOG("Parser::-static_analysis");
    return result;
}

Object* Parser::logical_or(){
    LOG("Paser::logical_or");
    Object* result = logical_and();
    while(curr == "||"){
        next();
        result = new Logical(OR, result, logical_and());
    }
    LOG("Parser::-logical_or");
    return result;
}

Object* Parser::logical_and(){
    LOG("Paser::logical_and");
    Object* result = logical_parse();
    while(curr == "&&"){
        next();
        result = new Logical(AND, result, logical_parse());
    }
    LOG("Parser::-logical_and");
    return result;
}

/* Numerical tokens for expression [Stops on semi-colon] */
Object* Parser::logical_parse(){
    LOG("Paser::logical_parse");
    Object* result = union_parse();
    string logical[] = {"==", "!=", "=", "<=", ">=", "<", ">"};
    set<string> logical_tokens(begin(logical),
                        end(logical));
    
    while(logical_tokens.find(curr) != logical_tokens.end()){
        if(curr == "=="){
            next();
            result = new Compare(EQU, result, union_parse());
        }else if(curr == "!="){
            next();
            result = new Compare(NEQ, result, union_parse());
        }else if(curr == "="){
            next();
            result = new Assign(result, generic_parse());
        }else if(curr == "<"){
            next();
            result = new Compare(LT, result, union_parse());
        }else if(curr == "<="){
            next();
            result = new Compare(LTE, result, union_parse());
        }else if(curr == ">="){
            next();
            result = new Compare(GTE, result, union_parse());
        }else if(curr == ">"){
            next();
            result = new Compare(GT, result, union_parse());
        }
    }
    LOG("Parser::-logical_parse");
    return result;
}

Object* Parser::union_parse(){
    LOG("Paser::union_parse");
    Object* result = intersect_parse();
    string unions[] = {"+", "-"};
    set<string> union_tokens(begin(unions),
                             end(unions));
    while(union_tokens.find(curr) != union_tokens.end()){
        if(curr == "+"){
            next();
            result = new Binary(ADD, result, intersect_parse());
        }else if(curr == "-"){
            next();
            result = new Binary(SUB, result, intersect_parse());
        }
    }
    LOG("Parser::-union_parse");
    return result;
}

Object* Parser::intersect_parse(){
    LOG("Paser::intersect_parse");
    Object* result = nots_parse();
    string intersect[] = {"*", "/", "%"};
    set<string> intersect_tokens(begin(intersect),
                                 end(intersect));
    while(intersect_tokens.find(curr) != intersect_tokens.end()){
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
    LOG("Parser:-intersect_parse");
    return result;
}

Object* Parser::nots_parse(){
    LOG("Paser::nots_parse");
    Object* result = atom_parse();
    string nots[] = {"-", "!", "~"};
    set<string> nots_tokens(begin(nots),
                            end(nots));
    while((nots_tokens.find(curr) != nots_tokens.end()) &&
           result == nullptr){
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
    LOG("Parser:-nots_parse");
    return result;
}

Object* Parser::atom_parse(){
    LOG("Paser::atom_parse");
    Object* result = nullptr;
    string nots[] = {"-", "!", "~"};
    set<string> nots_tokens(begin(nots),
                            end(nots));
    /* Unary operation */
    if(nots_tokens.find(curr) != nots_tokens.end()){
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
    /* String */
    else if(curr[0] == '"'){
        result = new class String(curr);
        next();
    }
    /* Uncaught ERROR */
    else {
        LOG("    Unknown_atomic: "+curr);
        RaisePineWarning("Variable \""+curr+"\" was not defined.");
    }
    LOG("Parser:-atom_parse");
    return result;
}

Object* Parser::is_numeric(string val){
    LOG("Parser::is_numeric");
    LOG("    val: "+val);
    try {
        Object* num = nullptr;
        int i = stoi(val);
        float f = stof(val);
        double d = stod(val);
        num = new Integer(i);
        if(f != i || ((val.find(".") != std::string::npos))){
            deleteObject(num);
            num = new class Float(f);
        }
        if(val.length() - val.rfind(".") > 6){
            deleteObject(num);
            num = new class Double(d);
        }
        return num;
    } catch (...) { /* Catch all exceptions for non-numeric value */
        LOG("Parser:is_numericAssert");
        return nullptr;
    }
    return nullptr;
}

int Parser::translateType(string type){
    LOG("Parser:translateType");
    LOG("    type: "+type);
    if(type == "Int"){
        return Int;
    }else if(type == "Float"){
        return Float;
    }else if(type == "Double"){
        return Double;
    }else if(type == "String"){
        return String;
    }else if(type == "Bool"){
        return Bool;
    }
    RaisePineException("Missing or invalid type definition "+type);
    return 0;
}

bool Parser::isVar(string name){
    map<string, Int32>* s = &(typeEnv.top());
    if(s->find(name) != s->end()){
        return true;
    }
    return false;
}

void Parser::pushNewTypeEnv(){
    map<string, Int32> m;
    typeEnv.push(m);
}

void Parser::pushNewVarBindingEnv(){
    map<string, Object*> m;
    varBindings.push(m);
}

map<string, Int32> Parser::popTypeEnv(){
    map<string, Int32> last = (typeEnv.top());
    typeEnv.pop();
    return last;
}

void Parser::popVarBindingEnv(){
    map<string, Object*>* last = &(varBindings.top());
    for(const auto& binding : (*last)){
        deleteObject(binding.second);
    }
    varBindings.pop();
}

void Parser::bindType(string name, int type){
    map<string, Int32>* s = &(typeEnv.top());
    (*s)[name] = type;
}

int Parser::getTypeForVar(string name){
    map<string, Int32>* last = &(typeEnv.top());
    if(last->find(name) == last->end()){
        RaisePineException("Undefined variable type: "+name);
    }
    return (*last)[name];
}

void Parser::bindVar(string name, Object* obj){
    map<string, Object*>* last = &(varBindings.top());
    (*last)[name] = obj->clone();
}
