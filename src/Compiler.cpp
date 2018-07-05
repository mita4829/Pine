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
    if(EXPR_TYPE == VAR || EXPR_TYPE == INTEGER || EXPR_TYPE == FLOAT || EXPR_TYPE == DOUBLE || EXPR_TYPE == STRING || EXPR_TYPE == BOOLEAN){
        return expr;
    }
    /* Unary */
    else if(EXPR_TYPE == UNARY){
        Unary* u = Safe_Cast<Unary*>(expr);
        Object* flat = flatten(u->getVal());
        string tmp = requestTmpVarName();
        int newtype = getType(flat);
        Let* l = new Let(tmp, newtype, new Unary(flat, u->getOperation()));
        addFlatStmtToStack(l);
        return new Var(tmp, newtype);
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
        Object* f_left   = flatten(bin_expr->getLeft());
        Object* f_right  = flatten(bin_expr->getRight());
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
        int newType = getType(f_left);
        Let* l = new Let(tmp, newType, new Binary(operation, f_left, f_right));
        addFlatStmtToStack(l);
        return new Var(tmp, newType);
    }
    /* Function */
    else if(EXPR_TYPE == FUNCTION){
        Function* f = Safe_Cast<Function*>(expr);
        Seq* flatBody = (Seq*)flatten(f->getBody());
        /* Func body only contains non-const code to remove dead instructions */

        addFlatStmtToStack(new Function(f->getName(), f->getArgv(), flatBody, f->getReturnType()));
        return nullptr;
    }
    /* Print */
    else if(EXPR_TYPE == PRINT){
        Print* p = Safe_Cast<Print*>(expr);
        Object* f_print = flatten(p->getVal());
        addFlatStmtToStack(new Print(f_print));
        return nullptr;
    }/* Seq */
    else if(EXPR_TYPE == SEQ){
        Seq* s = Safe_Cast<Seq*>(expr);
         /* Expand function stack */
        pushNewFlatStack();
        vector<Object*> body_stmt = s->getStatements();
        for(Object* stmt : body_stmt){
            flatten(stmt);
        }
        vector<Object*> flatBody = popFlatStack();
        return new Seq(flatBody);
    }
    /* If */
    else if(EXPR_TYPE == IF){
        If* i = Safe_Cast<If*>(expr);
        Object* con = flatten(i->getCondition());
        Seq* seq = (Seq*)flatten(i->getBody());
        Seq* seqe = (Seq*)flatten(i->getElse());
        addFlatStmtToStack(new If(con, seq, seqe));
        return nullptr;
    }
    /*Compare*/
    else if(EXPR_TYPE == COMPARE){
        Compare* c = Safe_Cast<Compare*>(expr);
        int operation = c->getOperation();
        Object* left = flatten(c->getLeft());
        Object* right = flatten(c->getRight());
        string tmp = requestTmpVarName();
        Let* flatten_let = new Let(tmp, BOOLEAN, new Compare(operation, left, right));
        addFlatStmtToStack(flatten_let);
        return new Var(tmp, BOOLEAN);
    }
    /*Logical*/
    else if(EXPR_TYPE == LOGICAL){
        Logical* l = Safe_Cast<Logical*>(expr);
        int operation = l->getOperation();
        Object* left = flatten(l->getLeft());
        Object* right = flatten(l->getRight());
        string tmp = requestTmpVarName();
        Let* flatten_let = new Let(tmp, BOOLEAN, new Logical(operation, left, right));
        addFlatStmtToStack(flatten_let);
        return new Var(tmp, BOOLEAN);
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
    /*Float*/
    else if(EXPR_TYPE == FLOAT){
        Float* f = Safe_Cast<Float*>(expr);
        float val = f->getVal();
        union{
            float f_bits;
            int i_bits;
        };
        f_bits = val;
        int id = requestFloatID();
        string stmt = "FLOAT_"+to_string(id)+":";
        string value= ".long " + to_string(i_bits) + " #" +to_string(f_bits);
        string movq = "movq FLOAT_"+to_string(id)+"(%rip), %xmm0";
        header.push_back(stmt);
        header.push_back(value);
        addCompileStmt(movq);
        return make_tuple("%xmm0", REG);
    }
    /*Double*/
    else if(EXPR_TYPE == DOUBLE){
        Double* d = Safe_Cast<Double*>(expr);
        double val = d->getVal();
        union{
            double d_bits;
            int64_t i_bits;
        };
        d_bits = val;
        int id = requestFloatID();
        string stmt = "DOUBLE_"+to_string(id)+":";
        string value= ".quad " + to_string(i_bits) + " #" +to_string(d_bits);
        string movq = "movsd DOUBLE_"+to_string(id)+"(%rip), %xmm0";
        header.push_back(stmt);
        header.push_back(value);
        addCompileStmt(movq);
        return make_tuple("%xmm0", REG);
    }
    /*Boolean*/
    else if(EXPR_TYPE == BOOLEAN){
        Boolean* boolean = Safe_Cast<Boolean*>(expr);
        bool val = boolean->getVal();
        int cast = (int)val;
        string operand = "$"+to_string(cast);
        return make_tuple(operand, BOOLEAN);
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
        int ltype = getType(b->getLeft());
        int rtype = getType(b->getRight());
        if(operation == ADD){
            if(ltype == INTEGER && rtype == INTEGER){
                string stmt = "addq %"+regleft+", %"+regright;
                addCompileStmt(stmt);
            }else if((ltype == FLOAT && rtype == FLOAT) || (ltype == DOUBLE && rtype == DOUBLE)){
                string x0 = "movq %"+regleft+", %xmm0";
                string x1 = "movq %"+regright+", %xmm1";
                string add = "addss %xmm0, %xmm1";
                string mov = "movq %xmm1, %"+regright;
                addCompileStmt(x0);
                addCompileStmt(x1);
                addCompileStmt(add);
                addCompileStmt(mov);
            }else if(ltype != rtype){
                RaisePineException("Binary operation + on non-matching type operands.");
            }
        }else if(operation == SUB){
            if(ltype == INTEGER && rtype == INTEGER){
                string stmt = "subq %"+regright+", %"+regleft;
                string orient = "movq %"+regleft+", %"+regright;
                addCompileStmt(stmt);
                addCompileStmt(orient);
            }else if((ltype == FLOAT && rtype == FLOAT) || (ltype == DOUBLE && rtype == DOUBLE)){
                string x0 = "movq %"+regleft+", %xmm0";
                string x1 = "movq %"+regright+", %xmm1";
                string sub = "subss %xmm1, %xmm0";
                string mov = "movq %xmm0, %"+regright;
                addCompileStmt(x0);
                addCompileStmt(x1);
                addCompileStmt(sub);
                addCompileStmt(mov);
            }else if(ltype != rtype){
                RaisePineException("Binary operation - on non-matching type operands.");
            }
        }else if(operation == MUL){
            if(ltype == INTEGER && rtype == INTEGER){
                string stmt = "imulq %"+regleft+", %"+regright;
                addCompileStmt(stmt);
            }else if((ltype == FLOAT && rtype == FLOAT) || (ltype == DOUBLE && rtype == DOUBLE)){
                string x0 = "movq %"+regleft+", %xmm0";
                string x1 = "movq %"+regright+", %xmm1";
                string mul = "mulss %xmm0, %xmm1";
                string mov = "movq %xmm1, %"+regright;
                addCompileStmt(x0);
                addCompileStmt(x1);
                addCompileStmt(mul);
                addCompileStmt(mov);
            }else if(ltype != rtype){
                RaisePineException("Binary operation * on non-matching type operands.");
            }
        }else if(operation == DIV){
            if(ltype == INTEGER && rtype == INTEGER){
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
            }else if((ltype == FLOAT && rtype == FLOAT) || (ltype == DOUBLE && rtype == DOUBLE)){
                string movq = "movq %"+regleft+", %xmm0";
                string movq2 = "movq %"+regright+", %xmm1";
                string stmt = "divss %xmm1, %xmm0";
                string movq3 = "movq %xmm0, %"+regright;
                addCompileStmt(movq);
                addCompileStmt(movq2);
                addCompileStmt(stmt);
                addCompileStmt(movq3);
            }else if(ltype != rtype){
                RaisePineException("Binary operation / on non-matching type operands.");
            }
        }
        registerManager.releaseRegister(regleft);
        return make_tuple(regright, REG);
    }
    /*If*/
    else if(EXPR_TYPE == IF){
        If* i = Safe_Cast<If*>(expr);
        tuple<string, int> con = compile(i->getCondition());
        int conType = getType(i->getCondition());
        if(conType != BOOLEAN && conType != COMPARE && conType != LOGICAL){
            RaisePineException("If statement conditions must be evaluable to a Boolean type.");
        }
        string reg = IntoRegister(get<0>(con));
        string shortL= "SHORT_"+to_string(requestShortID());
        string elseL = "ELSE_"+to_string(requestJumpID());
        string cmpq = "cmpq $1, %"+reg;
        string jmp  = "jne "+elseL;
        
        addCompileStmt(cmpq);
        addCompileStmt(jmp);
        compile(i->getBody());
        addCompileStmt("jmp "+shortL);
        addCompileStmt(elseL+":");
        compile(i->getElse());
        addCompileStmt(shortL+":");
        return make_tuple("", -1);
    }
    /*Compare*/
    else if(EXPR_TYPE == COMPARE){
        Compare* c = Safe_Cast<Compare*>(expr);
        int operation = c->getOperation();
        tuple<string, int> left = compile(c->getLeft());
        tuple<string, int> right = compile(c->getRight());
        int ltype = getType(c->getLeft());
        int rtype = getType(c->getRight());
        if(ltype != rtype){
            RaisePineException("Comparison operation require like-type operands.");
        }
        string instruction;
        /* Jump if comparison is false */
        if(operation == EQU){
            instruction = "jne";
        }else if(operation == NEQ){
            instruction = "je";
        }else if(operation == LT){
            instruction = "jl";
        }else if(operation == LTE){
            instruction = "jle";
        }else if(operation == GTE){
            instruction = "jge";
        }else if(operation == GT){
            instruction = "jg";
        }
        string regleft = IntoRegister(get<0>(left));
        string regright = IntoRegister(get<0>(right));
        string cmp = "cmpq %"+regleft+", %"+regright;
        addCompileStmt(cmp);
        string label = "COMPARE_T"+to_string(requestJumpID());
        string Short = "COMPARE_F"+to_string(requestShortID());
        /* Move $1 into the right register */
        string jmp = instruction+" "+label;
        addCompileStmt(jmp);
        addCompileStmt("movq $1, %"+regright);
        addCompileStmt("jmp "+Short);
        addCompileStmt(label+":");
        addCompileStmt("movq $0, %"+regright);
        addCompileStmt(Short+":");
        registerManager.releaseRegister(regleft);
        return make_tuple(regright, BOOLEAN);
    }
    /*Print*/
    else if(EXPR_TYPE == PRINT){
        Print* print = Safe_Cast<Print*>(expr);
        tuple<string, int> result = compile(print->getVal());
        PolymorphicPrint(print->getVal(), result);
        return make_tuple("", -1);
    }
    /*Logical*/
    else if(EXPR_TYPE == LOGICAL){
        Logical* l = Safe_Cast<Logical*>(expr);
        int operation = l->getOperation();
        string regleft = IntoRegister(get<0>(compile(l->getLeft())));
        string regright = IntoRegister(get<0>(compile(l->getRight())));
        string End = "LOGICAL_"+to_string(requestShortID());
        if(operation == AND){
            string False = "LOGICAL_"+to_string(requestShortID());
            string cmpq = "cmpq $1, %"+regleft;
            string jne = "jne "+False;
            string cmpq2 = "cmpq $1, %"+regright;
            string movq = "movq $1, %"+regright;
            string jmp = "jmp "+End;
            string fmov = "movq $0, %"+regright;
            addCompileStmt(cmpq);
            addCompileStmt(jne);
            addCompileStmt(cmpq2);
            addCompileStmt(jne);
            addCompileStmt(movq);
            addCompileStmt(jmp);
            addCompileStmt(False+":");
            addCompileStmt(fmov);
            addCompileStmt(End+":");
        }
        else if(operation == OR){
            string True = "LOGICAL_"+to_string(requestShortID());
            string cmpq = "cmpq $1, %"+regleft;
            string je = "je "+True;
            string cmpq2 = "cmpq $1, %"+regright;
            string movq = "movq $0, %"+regright;
            string jmp = "jmp "+End;
            string tmov = "movq $1, %"+regright;
            addCompileStmt(cmpq);
            addCompileStmt(je);
            addCompileStmt(cmpq2);
            addCompileStmt(je);
            addCompileStmt(movq);
            addCompileStmt(jmp);
            addCompileStmt(True+":");
            addCompileStmt(tmov);
            addCompileStmt(End+":");
        }
        registerManager.releaseRegister(regleft);
        return make_tuple(regright, REG);
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
    return expr_type == INTEGER || expr_type == VAR || expr_type == FLOAT || expr_type == DOUBLE || expr_type == BOOLEAN || expr_type == STRING;
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
    vector<string> last = (compileStmt.top());
    last.insert(last.begin(), second_last.begin(), second_last.end());
    compileStmt.pop();
    compileStmt.push(last);
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
    header.insert(header.end(), compileStmt.top().begin(), compileStmt.top().end());
    return header;
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

int Compiler::getType(Object* expr){
    int EXPR_TYPE = expr->getType();
    if(EXPR_TYPE == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        return v->getType();
    }
    return EXPR_TYPE;
}
int align32ByteStack(int varCount){
    int byteCount = varCount << 3; /* Multiple by 4: 4-byte for each var */
    int stackSize = ((byteCount >> 3) + 1) << 5;
    return stackSize;
}
int requestFloatID(){
    static int id = -1;
    id += 1;
    return id;
}

int requestJumpID(){
    static int id = -1;
    id += 1;
    return id;
}

int requestShortID(){
    static int id = -1;
    id += 1;
    return id;
}
