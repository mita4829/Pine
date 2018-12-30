#include "Parser.hpp"

Parser::Parser(){
    LOG("Parser:Parser");
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
    while(bindings.size() != 0){
        popBindings();
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
    else if(curr == "["){
        next();
        return array_parse();
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
    
    Bindings binding;
    TypeContext context;
    string lval = curr;
    Int32 inferredType = OBJECT;
    Int32 arrayDepth = 0;
    
    next(); // :
    
    if(curr == ":"){
        next();
        inferredType = translateType(curr, arrayDepth);
        next();
    }
    next(); // =
    Object* rval = generic_parse();
    
    binding.type = rval->getExplicitType();
    binding.obj  = rval;
    setBindings(lval, binding);
    
    context.explicitType = rval->getExplicitType();
    switch (context.explicitType) {
        case VAR: {
            Var* v = Safe_Cast<Var*>(rval);
            context.implicitType = v->getVarType();
            break;
            }
        case ARRAY: {
            Array* a = Safe_Cast<Array*>(rval);
            context.arrayPrimativeElementType = a->typeContext.arrayPrimativeElementType;
            context.arrayDepth                = a->typeContext.arrayDepth;
            break;
            }
        default:
            break;
    }
    
    /* If a type was given, verify it matches the type rval has */
    if(inferredType != OBJECT) {
        /* If rval is an array, preform additional checks */
        if((context.explicitType == ARRAY && arrayDepth == 0) ||
            (context.explicitType != ARRAY && arrayDepth >= 1 &&
                rval->typeContext.explicitType != INDEX)){
            RaisePineException("Variable "+lval+" does not match its type definition.");
        }
        if(context.explicitType == ARRAY){
            Array* a = Safe_Cast<Array*>(rval);
            if(arrayDepth != a->typeContext.arrayDepth){
                RaisePineException("Variable "+lval+" does not match its type definition "
                                   "of correct level of nesting.");
            }
            if(inferredType != a->typeContext.arrayPrimativeElementType){
                RaisePineException("Array "+lval+" does not match its type definition "
                                   "for its elements.");
            }
        }
        
        if(context.explicitType != ARRAY &&
           context.explicitType != INDEX &&
           inferredType != context.explicitType){
           RaisePineException("Variable "+lval+" defined of type "+getTypeName(inferredType)+
                              " does not match what was assigned to it, which is of type "
                              +getTypeName(context.explicitType));
        }
    }
    
    LOG("Parser::-let_parse");
    return new Let(lval, context, rval);
}

Object* Parser::if_parse(){
    LOG("Parser:if_parse");
    vector<tuple<Object*, Seq*>> ifStmt;
    vector<Object*> Else;
    
    do {
        // If were on an else if branch, skip those two tokens
        if(curr == "else" && peek() == "if"){
            next(); // else
            next(); // if
        }
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
        
        ifStmt.push_back(make_tuple(condition,
                                    new Seq(body)));
        
    } while (curr == "else" && peek() == "if");
    
    if (curr == "else"){
        next(); // else
        next(); // {
        while(curr != "}"){
            Object* stmt = generic_parse();
            Else.push_back(stmt);
            next();
        }
        next(); // }
    }
    
    if(curr != ";"){
        RaisePineException("If statement expected semi-colon at ending.");
    }
    
    LOG("Parser::-if_parse");
    return new If(ifStmt, new Seq(Else));
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
    Bindings binding;
    
    pushNewBindings();
    string fname = curr;
    next(); /* ( */
    next();
    /* TODO: Capture argv list */
    vector<Object*> argv;
    Int32 arrayDepth = 0;
    
    next(); /* ) */
    next(); /* -> */
    Int32 returnType = translateType(curr, arrayDepth);
    
    binding.type = returnType;
    binding.obj  = nullptr;
    setBindings(fname, binding);
    
    next(); /* { */
    next();
    vector<Object*> body;
    while(curr != "}"){
        Object* stmt = generic_parse();
        body.push_back(stmt);
        next();
    }
    next(); /* } */
    popBindings();
    LOG("Parser::-function_parse");
    return new Function(fname, argv, new Seq(body), returnType);
}

Object* Parser::array_parse(){
    LOG("Paser:array_parse");
    vector<Object*> array;
    const Int32 NONE = -1;
    const bool NOT_END_OF_ARRAY = true;
    Int32 elementType = NONE;
    Int32 primativeType = NONE;
    Int32 arrayDepth = 0;
    Integer* arrayLengthHeader;
    
    while(NOT_END_OF_ARRAY && curr != "]"){
        Object* element = generic_parse();
        array.push_back(element);
        if(elementType == NONE){
            elementType = element->getExplicitType();
            if(elementType == ARRAY){
                arrayDepth      = element->typeContext.arrayDepth;
                primativeType   = element->typeContext.arrayPrimativeElementType;
            }else{
                primativeType = elementType;
            }
        }
        else if(element->getExplicitType() == ARRAY){
            if ((arrayDepth != element->typeContext.arrayDepth) ||
                 (primativeType != element->typeContext.arrayPrimativeElementType)){
                RaisePineException("Arrays can only contant elements of same type.");
            }
        }
        if(curr == "]"){
            break;
        }
        next(); // ,
    }
    next(); // ]
    
    /*  Add the header int to the front of the array
        The upper 32-bit is tagged with the depth, and the
        lower 32-bit is tagged with the length.
    */
    
    Int64 header = TAG_DEPTH(array.size(), arrayDepth + 1);
    
    arrayLengthHeader = new Integer(header);
    array.insert(array.begin(), arrayLengthHeader);
    
    Array* a = new Array(array, array.size() - 1, elementType);
    a->typeContext.arrayDepth = arrayDepth + 1;
    a->typeContext.arrayPrimativeElementType = primativeType;
    
    LOG("Paser:-array_parse");
    return a;
}

Object* Parser::static_analysis(){
    LOG("Parser::static_analysis");
    Object* result = logical_or();
    
    /*
     Preform static analysis on the result to see if
     optimizations can be applied
     */
    
    result = analyzer.ConstantFold(volatileVars != 0 ? nullptr : &bindings, result);

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
        if (peek() == "[") {
            string arrayName   = curr;
            
            Bindings binding = getBindings(arrayName);
            Array* a = Safe_Cast<Array*>(binding.obj);
            
            Int32  elementType = a->getElementType();
            next(); // [
            next();
            Object* index = static_analysis();
            next(); // ]
            result = new Index(arrayName, index, elementType);
        }
        else {
            result = new Var(curr, getTypeForVar(curr));
            next();
        }
    }
    /* String */
    else if(curr[0] == '"'){
        result = new class String(curr);
        next();
    }
    /* Numeric value */
    else if((result = is_numeric(curr))){
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
        long long int i = stoi(val);
        float f = stof(val);
        double d = stod(val);
        num = new Integer(i);
        if(f != i || (val.find(".") != std::string::npos)){
            deleteObject(num);
            num = new class Float(f);
        }
        if(val.length() - val.rfind(".") > 6 &&
           (val.find(".") != std::string::npos)){
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

int Parser::translateType(string type, Int32& depth){
    LOG("Parser:translateType");
    LOG("    type: "+type);
    if(type == "["){
        depth += 1;
        next(); // Type
        Int32 type = translateType(curr, depth);
        next(); // ]
        return type;
    }
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
    }else if(type == "Void"){
        return Void;
    }
    RaisePineException("Missing or invalid type definition "+type);
    return 0;
}

bool Parser::isVar(string varName){
    map<string, Bindings>* last = &(bindings.top());
    if(last->find(varName) != last->end()){
        return true;
    }
    return false;
}

Int32 Parser::getTypeForVar(string name){
    Bindings binding = getBindings(name);
    return binding.type;
}

void Parser::pushNewBindings(){
    map<string, Bindings> m;
    bindings.push(m);
}

void Parser::popBindings(){
    map<string, Bindings>* last = &(bindings.top());
    bindings.pop();
}

Bindings Parser::getBindings(string varName){
    map<string, Bindings>* last = &(bindings.top());
    if(last->find(varName) == last->end()){
        RaisePineException("Undefined variable ("+varName+") in varBindings.");
    }
    return (*last)[varName];
}

void Parser::setBindings(string varName, Bindings binding){
    map<string, Bindings>* s = &(bindings.top());
    (*s)[varName] = binding;
}
