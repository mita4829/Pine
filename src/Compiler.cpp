#include "Compiler.hpp"

Compiler::Compiler(){
    pushNewFlatStack();
    pushNewStackFrame();
}
Compiler::Compiler(vector<Object*> _ast) : Compiler() {
    ast = _ast;
}
/* ==================================== Flatten ======================================= */
Object* Compiler::flatten(Object* expr){
    int EXPR_TYPE = expr->getType();
    /* Primative */
    if(EXPR_TYPE == INTEGER || EXPR_TYPE == FLOAT || EXPR_TYPE == DOUBLE || EXPR_TYPE == STRING || EXPR_TYPE == VAR){
        return expr;
    }
    /* Unary */
    else if(EXPR_TYPE == UNARY){
        Unary* u = Safe_Cast<Unary*>(expr);
        Object* flat = flatten(u->getVal());
        string tmp = requestTmpVarName();
        Let* l = new Let(tmp, flat->getType(), new Unary(flat, u->getOperation()));
        addFlatStmtToStack(l);
        return new Var(tmp, OBJECT);
    }
    /* Let */
    else if(EXPR_TYPE == LET){
        Let* let = Safe_Cast<Let*>(expr);
        Object* rval = flatten(let->getVal());
        Let* flatten_let = new Let(let->getName(), let->getExpectedType(), rval);
        addFlatStmtToStack(flatten_let);
        return nullptr;
    }
    /* Binary */
    else if(EXPR_TYPE == BINARY){
        Binary* bin_expr = Safe_Cast<Binary*>(expr);
        Object* f_left     = flatten(bin_expr->getLeft());
        Object* f_right    = flatten(bin_expr->getRight());
        int left_type    = f_left->getType();
        int right_type   = f_right->getType();
        int operation    = bin_expr->getOperation();
        string tmp = requestTmpVarName();
        while(!isValue(f_left->getType())){
            f_left = flatten(f_left);
        }
        while(!isValue(f_right->getType())){
            f_right = flatten(f_right);
        }
        /*TODO: Research if it's safe to assume ltype as the binary result?*/
        Let* l = new Let(tmp, f_left->getType(), new Binary(operation, f_left, f_right));
        addFlatStmtToStack(l);
        return new Var(tmp, OBJECT);
    }
    /* Function */
    else if(EXPR_TYPE == FUNCTION){
        Function* f = Safe_Cast<Function*>(expr);
        /* Expand function stack */
        pushNewFlatStack();
        
        Seq* body = f->getBody();
        vector<Object*> body_stmt = body->getStatements();
        for(Object* s : body_stmt){
            flatten(s);
        }
        /* Func body only contains non-const code to remove dead instructions */
        vector<Object*> flatBody = popFlatStack();
        addFlatStmtToStack(new Function(f->getName(), f->getArgv(), new Seq(flatBody), f->getReturnType()));
        return nullptr;
    }
    /* Print */
    else if(EXPR_TYPE == PRINT){
        Print* p = Safe_Cast<Print*>(expr);
        Object* f_print = flatten(p->getVal());
        addFlatStmtToStack(new Print(f_print));
        return nullptr;
    }
    RaisePineWarning("Flatten reached end of token matching.");
    return nullptr;
}

/* ==================================== Compile ======================================= */
tuple<string, int> Compiler::compile(Object* expr){
    int EXPR_TYPE = expr->getType();
    /*Integer*/
    if(EXPR_TYPE == INTEGER){
        Integer* integer = Safe_Cast<Integer*>(expr);
        int i = integer->getVal();
        string operand = "$"+to_string(i);
        return make_tuple(operand, INTEGER);
    }
    /*Var*/
    else if(EXPR_TYPE == VAR){
        Var* var = Safe_Cast<Var*>(expr);
        int type = var->getType();
        string name = var->getName();
        int stackLocation = getStackLocationForVar(name);
        string operand = "-"+to_string(stackLocation)+"(%rbp)";
        return make_tuple(operand, STACKLOC);
    }
    /*Unary*/
    else if(EXPR_TYPE == UNARY){
        Unary* unary = Safe_Cast<Unary*>(expr);
        int operation = unary->getOperation();
        tuple<string, int> operand = compile(unary->getVal());
        string reg = IntoRegister(get<0>(operand));
        if(operation == NEG){
            string stmt = "negq %" + reg;
            addCompileStmt(stmt);
            return make_tuple(reg, REG);
        }
    }
    /*Function*/
    else if(EXPR_TYPE == FUNCTION){
        Function* f = Safe_Cast<Function*>(expr);
        pushNewStackFrame();
        /* TODO: Expand args here */
        Seq* body = f->getBody();
        compile(body);
        /* Create stack space dependent on variable count */
        int varCount = stackLoc.top().size();
        int stackSize = align32ByteStack(varCount);
        string collapse = "addq $"+to_string(stackSize)+", %rsp";
        string pop = "popq %rbp";
        string ret = "retq";
        string expand = "subq $"+to_string(stackSize)+", %rsp";
        string newStack = "movq %rsp, %rbp";
        string pushRbp = "pushq %rbp";
        
        addCompileStmt(collapse);
        addCompileStmt(pop);
        addCompileStmt(ret);
        addFrontCompileStmt(expand);
        addFrontCompileStmt(newStack);
        addFrontCompileStmt(pushRbp);
        
        string fname = "_" + f->getName() + ":";
        string globl = ".global _"+f->getName();
        addFrontCompileStmt(fname);
        addFrontCompileStmt(globl);
        
        popStackFrame();
        return make_tuple("", -1);
    }
    /*Seq*/
    else if(EXPR_TYPE == SEQ){
        Seq* seq = Safe_Cast<Seq*>(expr);
        vector<Object*> body = seq->getStatements();
        for(Object* statement : body){
            compile(statement);
        }
        return make_tuple("", -1);
    }
    /*Let*/
    else if(EXPR_TYPE == LET){
        Let* let = Safe_Cast<Let*>(expr);
        Object* rval = let->getVal();
        string name = let->getName();
        tuple<string, int> result = compile(rval);
        string reg = IntoRegister(get<0>(result));
        mapVarToStackLocation(name);
        int stackLocation = getStackLocationForVar(name);
        string stmt = "movq %" + reg + ", -" + to_string(stackLocation) + "(%rbp)";
        addCompileStmt(stmt);
        registerManager.releaseRegister(reg);
        return make_tuple("", -1);
    }
    /*Binary*/
    else if(EXPR_TYPE == BINARY){
        Binary* b = Safe_Cast<Binary*>(expr);
        int operation = b->getOperation();
        tuple<string, int> left = compile(b->getLeft());
        string regleft = IntoRegister(get<0>(left));
        tuple<string, int> right = compile(b->getRight());
        string regright = IntoRegister(get<0>(right));
        if(operation == ADD){
            string stmt = "addq %"+regleft+", %"+regright;
            addCompileStmt(stmt);
        }else if(operation == SUB){
            string stmt = "subq %"+regright+", %"+regleft;
            string orient = "movq %"+regleft+", %"+regright;
            addCompileStmt(stmt);
            addCompileStmt(orient);
        }else if(operation == MUL){
            string stmt = "imulq %"+regleft+", %"+regright;
            addCompileStmt(stmt);
        }else if(operation == DIV){
            string movq = "movq %"+regleft+", %rcx";
            string movq2 = "movq %"+regright+", %rax";
            string movq3 = "movq %rcx, %rax";
            string cqld = "cltd";
            string movq4 = "movq %"+regright+", %rcx";
            string stmt = "idivq %rcx";
            string movq5 = "movq %rax, %"+regright;
            addCompileStmt(movq);
            addCompileStmt(movq2);
            addCompileStmt(movq3);
            addCompileStmt(cqld);
            addCompileStmt(movq4);
            addCompileStmt(stmt);
            addCompileStmt(movq5);
        }
        registerManager.releaseRegister(regleft);
        return make_tuple(regright, REG);
    }
    /*Print*/
    else if(EXPR_TYPE == PRINT){
        Print* print = Safe_Cast<Print*>(expr);
        tuple<string, int> result = compile(print->getVal());
        PolymorphicPrint(print->getVal(), result);
        return make_tuple("", -1);
    }
    RaisePineWarning("Compiler reached end of token matching: "+to_string(EXPR_TYPE));
    return make_tuple("", -1);
}

/* ================================== Helper Methods ===================================== */

string Compiler::requestTmpVarName(){
    static int id = -1;
    id += 1;
    return "var" + to_string(id);
}

bool Compiler::isValue(int expr_type){
    return expr_type == INTEGER || expr_type == VAR || expr_type == FLOAT || expr_type == DOUBLE || expr_type == STRING;
}

void Compiler::addFlatStmtToStack(Object* stmt){
    vector<Object*>* last = &(flattenStmt.top());
    last->push_back(stmt);
}

void Compiler::pushNewFlatStack(){
    vector<Object*> frame;
    flattenStmt.push(frame);
}

vector<Object*> Compiler::popFlatStack(){
    vector<Object*> last = (flattenStmt.top());
    flattenStmt.pop();
    return last;
}

string Compiler::IntoRegister(string operand){
    char first = operand[0];
    /* If register, return it */
    if(first == 'r' || first == 'e'){
        return operand;
    }
    string reg = registerManager.getRegister();
    /* If stack location, request a register */
    if(first == '-' || ((first - '0' >= 0) || (first - '0' <= 9)) || first == '$'){
        string stmt = "movq " + operand + ", %" + reg;
        addCompileStmt(stmt);
        return reg;
    }
    RaisePineException("Compiler reached fatel error when assign registers.");
    return "";
}

void Compiler::addCompileStmt(string stmt){
    vector<string>* last = &(compileStmt.top());
    last->push_back(stmt);
}

void Compiler::addFrontCompileStmt(string stmt){
    vector<string>* last = &(compileStmt.top());
    last->insert(last->begin(), stmt);
}

void Compiler::pushNewStackFrame(){
    vector<string> frame;
    compileStmt.push(frame);
    pushNewStackLocationMap();
}

void Compiler::popStackFrame(){
    vector<string> second_last = (compileStmt.top());
    compileStmt.pop();
    vector<string>* last = &(compileStmt.top());
    second_last.insert(second_last.end(), last->begin(), last->end());
    compileStmt.pop();
    compileStmt.push(second_last);
    popStackLocationMap();
}

void Compiler::pushNewStackLocationMap(){
    map<string, int> Slm;
    stackLoc.push(Slm);
}

void Compiler::mapVarToStackLocation(string name){
    map<string, int>* frame = &(stackLoc.top());
    if(frame->count(name) > 0){
        RaisePineException("Redeclarion of variable named: "+name);
    }
    int loc = (frame->size() + 1) << 3; /* Stack address are aligned by 0x8.
                                         Give the Var the next location based
                                         on the given size of the scope's variable
                                         count.
                                         */
    (*frame)[name] = loc;
}

int Compiler::getStackLocationForVar(string name){
    map<string, int>* frame = &(stackLoc.top());
    return (*frame)[name];
}

void Compiler::popStackLocationMap(){
    stackLoc.pop();
}

vector<string> Compiler::getAssembly(){
    return compileStmt.top();
}

void Compiler::PolymorphicPrint(Object* expr, tuple<string, int> result){
    int EXPR_TYPE = expr->getType();
    if(EXPR_TYPE == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        int stackLocation = getStackLocationForVar(v->getName());
        EXPR_TYPE = v->getType();
        string val = "leaq -"+to_string(stackLocation)+"(%rbp), %rsi";
        addCompileStmt(val);
    }else if(get<1>(result) == STACKLOC){
        string val = "leaq "+get<0>(result)+", %rsi";
        addCompileStmt(val);
    }else{
        string val = "movq "+get<0>(result)+", %rsi";
        addCompileStmt(val);
    }
    
    string type = "movq $"+to_string(EXPR_TYPE)+", %rdi";
    string call = "callq _PinePrint";
    addCompileStmt(type);
    addCompileStmt(call);
}

int align32ByteStack(int varCount){
    int byteCount = varCount << 3; /* Multiple by 4: 4-byte for each var */
    int stackSize = ((byteCount >> 3) + 1) << 5;
    return stackSize;
}
