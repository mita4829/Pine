#include "Compiler.hpp"

/*
 crossPlatformFunctionCall
 
 Parameters:
    functionName - The string of the function name to be called in assembly
 
 Return:
    A function name that will be called with the correct syntax depending
    on the platform.
 */
string crossPlatformFunctionCall(string functionName){
#ifdef __APPLE__
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_MAC
    return "_" + functionName;
    #endif // _TARGET_OS_MAC
#endif // __APPLE__
   
#ifdef __linux__
    return functionName;
#endif // __linux__
    RaisePineWarning("Pine is being compiled on an unsupported platform.");
    return functionName;
}

string requestTmpVarName(){
    LOG("Compiler:requestTmpVarName");
    static Int32 id = -1;
    id += 1;
    return "var" + to_string(id);
}

Int32 align32ByteStack(Int32 varCount){
    Int32 byteCount = varCount << 3; /* Multiple by 4: 4-byte for each var */
    Int32 stackSize = ((byteCount >> 3) + 1) << 5;
    return stackSize;
}

Int32 requestStringID(){
    static Int32 id = -1;
    id += 1;
    return id;
}

Int32 requestFloatID(){
    static Int32 id = -1;
    id += 1;
    return id;
}


Int32 requestJumpID(){
    static Int32 id = -1;
    id += 1;
    return id;
}

Int32 requestShortID(){
    static Int32 id = -1;
    id += 1;
    return id;
}


/*
 Returns the type of the expr. If it's a VAR type, cast
 to VAR can get its type.
 */
Int32 getType(Object* expr){
    LOG("Compiler:getType");
    LOG("    expr: "+AddressOf(&expr));
    Int32 Type = expr->getType();
    if(Type == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        return v->getType();
    }
    return Type;
}

bool isValue(Int32 expr_type){
    LOG("Compiler:isValue");
    LOG("    expr_type: "+to_string(expr_type));
    return (expr_type == INTEGER ||
            expr_type == VAR     ||
            expr_type == FLOAT   ||
            expr_type == DOUBLE  ||
            expr_type == BOOLEAN ||
            expr_type == STRING);
}

Compiler::Compiler(){
    LOG("Compiler:Compiler");
    pushNewFlatStack();
    pushNewStackFrame();
}

Compiler::Compiler(vector<Object*> _ast) : Compiler() {
    LOG("Compiler:Compiler");
    LOG("    _ast: 0x"+AddressOf(&_ast));
    LOG("    _ast.size():"+to_string(_ast.size()));
    
    ast = _ast;
    
    for(const auto& tree : ast){
        flatten(tree);
    }
    
    flatAst = popFlatStack();
    LOG("    flatAst.size():"+to_string(flatAst.size()));

    for(const auto& tree : flatAst){
        compile(tree);
    }

    LOG("    Completed compile");
}

Compiler::~Compiler(){
    LOG("Compiler:~Compiler");
    while(flattenStmt.size() != 0){
        vector<Object*>* frame = &(flattenStmt.top());
        for(auto& stmt : *frame){
            if(stmt != nullptr){
                deleteObject(stmt);
                stmt = nullptr;
            }
        }
        flattenStmt.pop();
    }
    LOG("Compiler:    Deleting flatAst");
    for(auto& tree : flatAst){
        if(tree != nullptr){
            deleteObject(tree);
            tree = nullptr;
        }
    }
    
    LOG("Compiler:    Deleting ast");
    for(auto& tree : ast){
        if(tree != nullptr){
            deleteObject(tree);
            tree = nullptr;
        }
    }
}

Object* Compiler::flatten(Object* expr){
    LOG("Compiler:flatten");
    LOG("    expr: 0x"+AddressOf(&expr));
    
    Int32 Type = expr->getType();
    LOG("    Type: "+STR(Type));
    /* Primatives */
    /*
     We cannot just return the primative expr/node, as the original
     AST holds a reference to these nodes. Otherwise, freeing
     the memory of the original AST and the flatAST will cause
     a double free assert.
     */
    if(Type == VAR || Type == INTEGER || Type == FLOAT ||
       Type == DOUBLE || Type == STRING || Type == BOOLEAN){
        return expr->clone();
    }
    /* Unary */
    else if(Type == UNARY){
        Unary* u        = Safe_Cast<Unary*>(expr);
        Object* rval    = flatten(u->getVal());
        string lval     = requestTmpVarName();
        Int32 newType   = getType(rval);
        Let* l          = new Let(lval, newType, new Unary(rval, u->getOperation()));
        addFlatStmtToStack(l);
        return new Var(lval, newType);
    }
    /* Let */
    else if(Type == LET){
        Let* l          = Safe_Cast<Let*>(expr);
        Object* rval    = flatten(l->getVal());
        Let* fl         = new Let(l->getName(), getType(rval), rval);
        addFlatStmtToStack(fl);
        return fl;
    }
    /* Assign */
    else if(Type == ASSIGN){
        Assign* a   = Safe_Cast<Assign*>(expr);
        Object* f   = flatten(a->getVal());
        Assign* fa  = new Assign(a->getVar(), f);
        addFlatStmtToStack(fa);
        return fa;
    }
    /* Binary */
    else if(Type == BINARY){
        Binary* b        = Safe_Cast<Binary*>(expr);
        Object* fl       = flatten(b->getLeft());
        Object* fr       = flatten(b->getRight());
        Int32 lt         = fl->getType();
        Int32 rt         = fr->getType();
        Int32 operation  = b->getOperation();
        string name = requestTmpVarName();
        while(isValue(fl->getType()) == false){
            fl = flatten(fl);
        }
        while(isValue(fr->getType()) == false){
            fr = flatten(fr);
        }
        /*
         We let the type of the binary expr be the evaluated left
         operand.
         */
        Int32 newType = getType(fl);
        
        Let* l = new Let(name, newType, new Binary(operation, fl, fr));
        addFlatStmtToStack(l);
        
        return new Var(name, newType);
    }
    /* Function */
    else if(Type == FUNCTION){
        Function* f     = Safe_Cast<Function*>(expr);
        Seq* flatBody   = Safe_Cast<Seq*>(flatten(f->getBody()));
        /* Func body only contains non-const code to remove dead instructions */
        
        addFlatStmtToStack(new Function(f->getName(), f->getArgv(), flatBody, f->getReturnType()));
        return nullptr;
    }
    /* Print */
    else if(Type == PRINT){
        Print* p        = Safe_Cast<Print*>(expr);
        Object* f_print = flatten(p->getVal());
        addFlatStmtToStack(new Print(f_print));
        return nullptr;
    }
    /* Seq */
    else if(Type == SEQ){
        Seq* s = Safe_Cast<Seq*>(expr);
        /* Expand function stack */
        pushNewFlatStack();
        vector<Object*> body_stmt = s->getStatements();
        for(const auto& stmt : body_stmt){
            flatten(stmt);
        }
        vector<Object*> flatBody  = popFlatStack();
        return new Seq(flatBody);
    }
    /* If */
    else if(Type == IF){
        If*                          ifExpr = Safe_Cast<If*>(expr);
        vector<tuple<Object*, Seq*>> ifStmt = ifExpr->getIfStmt();
        vector<tuple<Object*, Seq*>> flatStmt;
        Object*                    elseBody = nullptr;
        for(const auto& stmt : ifStmt){
            tuple<Object*, Seq*> flatIfStmt = make_tuple(flatten(get<0>(stmt)),
                                                         Safe_Cast<Seq*>(flatten(get<1>(stmt))));
            flatStmt.push_back(flatIfStmt);
        }
        
        elseBody = ifExpr->getElse();
        elseBody = flatten(elseBody);
        
        addFlatStmtToStack(new If(flatStmt, Safe_Cast<Seq*>(elseBody)));
        return nullptr;
    }
    /*Compare*/
    else if(Type == COMPARE){
        Compare* c       = Safe_Cast<Compare*>(expr);
        Int32 operation  = c->getOperation();
        Object* l        = flatten(c->getLeft());
        Object* r        = flatten(c->getRight());
        string name      = requestTmpVarName();
        Let* fl          = new Let(name, BOOLEAN, new Compare(operation, l, r));
        addFlatStmtToStack(fl);
        return new Var(name, BOOLEAN);
    }
    /*Logical*/
    else if(Type == LOGICAL){
        Logical* l       = Safe_Cast<Logical*>(expr);
        int operation    = l->getOperation();
        Object* lf       = flatten(l->getLeft());
        Object* rf       = flatten(l->getRight());
        string name      = requestTmpVarName();
        Let* fl          = new Let(name, BOOLEAN, new Logical(operation, lf, rf));
        addFlatStmtToStack(fl);
        return new Var(name, BOOLEAN);
    }
    /*For*/
    else if(Type == FOR){
        For* f          = Safe_Cast<For*>(expr);
        Object* f_decl  = (f->getDeclare());
        Object* f_cond  = (f->getCondition());
        Object* f_incl  = (f->getIncl());
        Seq* f_body = Safe_Cast<Seq*>(flatten(f->getBody()));
        addFlatStmtToStack(new For(f_decl, f_cond, f_incl, f_body));
        return nullptr;
    }
    /* While */
    else if(Type == WHILE){
        While* w = Safe_Cast<While*>(expr);
        Object* cond = w->getCondition();
        Seq* body = (Seq*)flatten(w->getBody());
        addFlatStmtToStack(new While(cond, body));
        return nullptr;
    }
    RaisePineWarning("Flatten reached end of token matching.");
    return nullptr;
}

Tuple(string, Int32) Compiler::compile(Object* expr){
    LOG("Compiler:compile");
    LOG("    expr: 0x"+AddressOf(&expr));
    
    Int32 Type = expr->getType();
    /* Integer */
    if(Type == INTEGER){
        Integer*       i = Safe_Cast<Integer*>(expr);
        Int32          v = i->getVal();
        string   operand = "$"+STR(v);
        return tuple(operand, INTEGER);
    }
    /* Float */
    else if(Type == FLOAT){
        class Float* f = Safe_Cast<class Float*>(expr);
        float val = f->getVal();
    
        union {
            float f_bits;
            Int32 i_bits;
        };
        
        f_bits = val;
        Int32 id = requestFloatID();
        
        string ins1 = "FLOAT_"+STR(id)+":";
        string ins2 = ".long " + STR(i_bits) + " #" + STR(f_bits);
        string ins3 = "movq FLOAT_"+STR(id)+"(%rip), %xmm0";
        header.push_back(ins1);
        header.push_back(ins2);
        addCompileStmt(ins3);
        
        return tuple("%xmm0", REG);
    }
    /* Var */
    else if(Type == VAR){
        Var*            var = Safe_Cast<Var*>(expr);
        Int32          type = var->getType();
        string         name = var->getName();
        Int32 stackLocation = getStackLocationForVar(name);
        string         ins1 = "-"+to_string(stackLocation)+"(%rbp)";
        return tuple(ins1, STACKLOC);
    }
    /* String */
    else if(Type == STRING){
        class String* s   = Safe_Cast<class String*>(expr);
        string val  = s->getVal();
        Int32 id    = requestStringID();
        
        string ins1 = "STR_"+STR(id)+".str";
        string ins2 = ".asciz " + val;
        
        header.push_back(ins1+":");
        header.push_back(ins2);
        
        string ins3 = registerManager.getRegister();
        string ins4 = "leaq " + ins1 + "(%rip), %"+ins3;
        
        addCompileStmt(ins4);
        return tuple(ins3, REG);
    }
    /*Boolean*/
    else if(Type == BOOLEAN){
        Boolean* boolean = Safe_Cast<Boolean*>(expr);
        bool val = boolean->getVal();
        Int32 cast = (Int32)val;
        string operand = "$"+STR(cast);
        return tuple(operand, BOOLEAN);
    }
    /* Binary */
    else if(Type == BINARY){
        Binary*                  b = Safe_Cast<Binary*>(expr);
        Int32            operation = b->getOperation();
        Tuple(string, Int32)  left = compile(b->getLeft());
        string             regleft = IntoRegister(get<0>(left));
        Tuple(string, Int32) right = compile(b->getRight());
        string            regright = IntoRegister(get<0>(right));
        Int32                ltype = getType(b->getLeft());
        Int32                rtype = getType(b->getRight());
        if(operation == ADD){
            if(ltype == INTEGER && rtype == INTEGER){
                string stmt = "addq %"+regleft+", %"+regright;
                addCompileStmt(stmt);
            }else if((ltype == FLOAT && rtype == FLOAT) ||
                     (ltype == DOUBLE && rtype == DOUBLE)){
                string  x0 = "movq %"+regleft+", %xmm0";
                string  x1 = "movq %"+regright+", %xmm1";
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
                string   stmt = "subq %"+regright+", %"+regleft;
                string orient = "movq %"+regleft+", %"+regright;
                addCompileStmt(stmt);
                addCompileStmt(orient);
            }else if((ltype == FLOAT && rtype == FLOAT) ||
                     (ltype == DOUBLE && rtype == DOUBLE)){
                string  x0 = "movq %"+regleft+", %xmm0";
                string  x1 = "movq %"+regright+", %xmm1";
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
            }else if((ltype == FLOAT && rtype == FLOAT) ||
                     (ltype == DOUBLE && rtype == DOUBLE)){
                string  x0 = "movq %"+regleft+", %xmm0";
                string  x1 = "movq %"+regright+", %xmm1";
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
                string  movq = "movq %"+regleft+", %rcx";
                string movq2 = "movq %"+regright+", %rax";
                string movq3 = "movq %rcx, %rax";
                string  cqld = "cltd";
                string movq4 = "movq %"+regright+", %rcx";
                string  stmt = "idivq %rcx";
                string movq5 = "movq %rax, %"+regright;
                addCompileStmt(movq);
                addCompileStmt(movq2);
                addCompileStmt(movq3);
                addCompileStmt(cqld);
                addCompileStmt(movq4);
                addCompileStmt(stmt);
                addCompileStmt(movq5);
            }else if((ltype == FLOAT && rtype == FLOAT) ||
                     (ltype == DOUBLE && rtype == DOUBLE)){
                string  movq = "movq %"+regleft+", %xmm0";
                string movq2 = "movq %"+regright+", %xmm1";
                string  stmt = "divss %xmm1, %xmm0";
                string movq3 = "movq %xmm0, %"+regright;
                addCompileStmt(movq);
                addCompileStmt(movq2);
                addCompileStmt(stmt);
                addCompileStmt(movq3);
            }else if(ltype != rtype){
                RaisePineException("Binary operation / on non-matching type operands.");
            }
        }
        else if(operation == MOD){
            if(ltype == INTEGER && rtype == INTEGER){
                string  movq = "movq %"+regleft+", %rcx";
                string movq2 = "movq %"+regright+", %rax";
                string movq3 = "movq %rcx, %rax";
                string  cqld = "cltd";
                string movq4 = "movq %"+regright+", %rcx";
                string  stmt = "idivq %rcx";
                /* TODO Task: Check if %rdx is free before taking it*/
                string movq5 = "movq %rdx, %"+regright;
                addCompileStmt(movq);
                addCompileStmt(movq2);
                addCompileStmt(movq3);
                addCompileStmt(cqld);
                addCompileStmt(movq4);
                addCompileStmt(stmt);
                addCompileStmt(movq5);
            }else{
                RaisePineException("Modulo is not defined for non-integer operands.");
            }
        }
        else{
            RaisePineWarning("Uncaught Binary operation in compile "+STR(operation));
        }
        registerManager.releaseRegister(regleft);
        return tuple(regright, REG);
    }
    /* Function */
    else if(Type == FUNCTION){
        Function* f = Safe_Cast<Function*>(expr);
        pushNewStackFrame();
        /* TODO: Expand args here */
        Seq* body = f->getBody();
        compile(body);
        /* Create stack space dependent on variable count */
        Int32 varCount  = stackLoc.top().size();
        Int32 stackSize = align32ByteStack(varCount);
        
        string ins1     = "addq $"+STR(stackSize)+", %rsp";
        string ins2     = "popq %rbp";
        string ins3     = "retq";
        string ins4     = "subq $"+STR(stackSize)+", %rsp";
        string ins5     = "movq %rsp, %rbp";
        string ins6     = "pushq %rbp";
        
        addCompileStmt(ins1);
        addCompileStmt(ins2);
        addCompileStmt(ins3);
        addFrontCompileStmt(ins4);
        addFrontCompileStmt(ins5);
        addFrontCompileStmt(ins6);
        
        string ins7    = crossPlatformFunctionCall(f->getName())+":";
        string ins8    = ".global "+crossPlatformFunctionCall(f->getName());
        
        addFrontCompileStmt(ins7);
        addFrontCompileStmt(ins8);
        
        popStackFrame();
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /* Seq */
    else if(Type == SEQ){
        Seq* seq             = Safe_Cast<Seq*>(expr);
        vector<Object*> body = seq->getStatements();
        for(const auto& stmt : body){
            compile(stmt);
        }
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /* Let */
    else if(Type == LET){
        Let*                    let = Safe_Cast<Let*>(expr);
        Object*                rval = let->getVal();
        string                 name = let->getName();
        Tuple(string, Int32) result = compile(rval);
        string                  reg = IntoRegister(get<0>(result));
        
        mapVarToStackLocation(name);
        Int32         stackLocation = getStackLocationForVar(name);
        
        string                 ins1 = "movq %"+reg+", -"+STR(stackLocation)+"(%rbp)";
        addCompileStmt(ins1);
        
        registerManager.releaseRegister(reg);
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /* Assign */
    else if(Type == ASSIGN){
        Assign*           a = Safe_Cast<Assign*>(expr);
        Object*        rval = a->getVal();
        string          reg = IntoRegister(get<0>(compile(rval)));
        string      varName = Safe_Cast<Var*>(a->getVar())->getName();
        Int32 stackLocation = getStackLocationForVar(varName);
        string stmt = "movq %"+reg+", -"+to_string(stackLocation)+"(%rbp)";
        addCompileStmt(stmt);
        
        registerManager.releaseRegister(reg);
        
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /* Print */
    else if(Type == PRINT){
        Print* p = Safe_Cast<Print*>(expr);
        Tuple(string, Int32) result = compile(p->getVal());
        PolymorphicPrint(p->getVal(), result);
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /*If*/
    else if(Type == IF){
        If* i = Safe_Cast<If*>(expr);
        vector<tuple<Object*, Seq*>> ifStmt = i->getIfStmt();
        /* For each if / if else statement, compile the condition
         and the following body.
         */
        string ins2 = "SHORT_"+to_string(requestShortID());
        for (const auto& stmt : ifStmt){
            Object* condition = get<0>(stmt);
            Int32 conType = getType(condition);
            if(conType != BOOLEAN &&
               conType != COMPARE &&
               conType != LOGICAL)
            {
                RaisePineException("If statement conditions must be evaluable to a Boolean type.");
            }
            Tuple(string, Int32) con = compile(condition);
            string ins1 = IntoRegister(get<0>(con));
            
            string ins3 = "ELSE_ELIF_"+to_string(requestJumpID());
            string ins4 = "cmpq $1, %"+ins1;
            string ins5  = "jne "+ins3;
            addCompileStmt(ins4);
            addCompileStmt(ins5);
            /* Compile then body */
            compile(get<1>(stmt));
            addCompileStmt("jmp "+ins2);
            addCompileStmt(ins3+":");
        }
        
        /* Compile else body if it exist*/
        if(i->getElse() != nullptr){
            compile(i->getElse());
        }
        
        addCompileStmt(ins2+":");
        
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /* Compare */
    else if(Type == COMPARE){
        Compare* c = Safe_Cast<Compare*>(expr);
        Int32 operation = c->getOperation();
        Tuple(string, Int32) left = compile(c->getLeft());
        Tuple(string, Int32) right = compile(c->getRight());
        Int32 ltype = getType(c->getLeft());
        Int32 rtype = getType(c->getRight());
        if(ltype != rtype){
            RaisePineException("Comparison operation require like-type operands.\n"
                               "Recieved ("+getTypeName(ltype)+") and ("+getTypeName(rtype)+")");
        }
        string instruction;
        /* Jump if comparison is false */
        if(operation == EQU){
            instruction = "jne";
        }else if(operation == NEQ){
            instruction = "je";
        }else if(operation == LT){
            instruction = "jge";
        }else if(operation == LTE){
            instruction = "jg";
        }else if(operation == GTE){
            instruction = "jl";
        }else if(operation == GT){
            instruction = "jle";
        }
        string regleft = IntoRegister(get<0>(left));
        string regright = IntoRegister(get<0>(right));
        string cmp = "cmpq %"+regright+", %"+regleft;
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
        
        return tuple(regright, BOOLEAN);
    }
    /* For */
    else if(Type == FOR){
        For* f = Safe_Cast<For*>(expr);
        compile(f->getDeclare());
        string ins1 = "FOR_" + STR(requestJumpID());
        addCompileStmt(ins1 + ":");
        string ins2 = "SHORT_" + STR(requestShortID());
        Tuple(string, Int32) result = compile(f->getCondition());
        string reg  = IntoRegister(get<0>(result));
        string ins3 = "cmpq $1, %" + reg;
        string ins4 = "jne " + ins2;
        addCompileStmt(ins3);
        addCompileStmt(ins4);
        
        compile(f->getBody());
        compile(f->getIncl());
        string ins5 = "jmp " + ins1;
        addCompileStmt(ins5);
        addCompileStmt(ins2 + ":");
        
        registerManager.releaseRegister(reg);
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    /*While*/
    else if(Type == WHILE){
        While* w = Safe_Cast<While*>(expr);
        
        string ins1 = "WHILE_" + STR(requestJumpID());
        addCompileStmt(ins1 + ":");
        
        Tuple(string, Int32) result = compile(w->getCondition());
        string reg = IntoRegister(get<0>(result));
        string ins2 = "cmpq $1, %" + reg;
        string ins3 = "SHORT_" + STR(requestShortID());
        string ins4 = "jne " + ins3;
        
        addCompileStmt(ins2);
        addCompileStmt(ins4);
        
        compile(w->getBody());
        
        string ins5 = "jmp " + ins1;
        addCompileStmt(ins5);
        addCompileStmt(ins3 + ":");
        registerManager.releaseRegister(reg);
        return tuple(VOID_ASM_OPERAND, VOID);
    }
    RaisePineWarning("Compiler reached end of token matching. Type: "+STR(Type));
    LOG("Compile:compile escaped type: "+STR(Type));
    return tuple(VOID_ASM_OPERAND, VOID);
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
    LOG("Compiler:IntoRegister");
    LOG("    operand:"+operand);
    if(operand == VOID_ASM_OPERAND){
        RaisePineException("Compiler reached a fatel error when assigning registers.");
    }
    Char first = operand[0];
    /*
     Check if it's a register i.e. rax or eax by the first char.
     We assume the operand register could be 32 or 64 bits.
     */
    if(first == 'r' || first == 'e'){
        return operand;
    }

    string reg = registerManager.getRegister();
    
    if(reg == VOID_ASM_OPERAND){
        RaisePineWarning("Ran out of registers. Please report this issue.");
    }
    
    if(first == '-' ||
       ((first - '0' >= 0) || (first - '0' <= 9)) ||
       first == '$'){
        string stmt = "movq " + operand + ", %" + reg;
        addCompileStmt(stmt);
        return reg;
    }
    
    RaisePineException("Compiler reached a fatel error when assigning registers.");
    return VOID_ASM_OPERAND;
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
    map<string, Int32> Slm;
    stackLoc.push(Slm);
}

void Compiler::popStackLocationMap(){
    stackLoc.pop();
}

Int32 Compiler::getStackLocationForVar(string name){
    map<string, Int32>* frame = &(stackLoc.top());
    return (*frame)[name];
}

void Compiler::mapVarToStackLocation(string name){
    LOG("Compiler:mapVarToStackLocation");
    LOG("   name: "+name);
    map<string, Int32>* frame = &(stackLoc.top());
    if(frame->count(name) > 0){
        RaisePineException("Redeclarion of variable named: "+name);
    }
    /* Stack address are aligned by 0x8.
     Give the Var the next location based
     on the given size of the scope's variable
     count.
     */
    Int32 loc = (frame->size() + 1) << 3;
    (*frame)[name] = loc;
    LOG("Compiler:-mapVarToStackLocation");
}

void Compiler::PolymorphicPrint(Object* expr, Tuple(string, Int32) result){
    LOG("Compiler:PolymorphicPrint");
    LOG("    expr: 0x"+AddressOf(&expr));
    LOG("    result: ("+get<0>(result)+", "+STR(get<1>(result))+")");
    Int32 EXPR_TYPE = expr->getType();
    if(EXPR_TYPE == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        Int32 stackLocation = getStackLocationForVar(v->getName());
        EXPR_TYPE = v->getType();
        string val = "leaq -"+STR(stackLocation)+"(%rbp), %rsi";
        addCompileStmt(val);
        string type = "movq $"+STR(EXPR_TYPE)+", %rdi";
        string call = "callq " + crossPlatformFunctionCall("PinePrint");
        addCompileStmt(type);
        addCompileStmt(call);
    }else if(get<1>(result) == STACKLOC){
        string val = "leaq "+get<0>(result)+", %rsi";
        addCompileStmt(val);
        string type = "movq $"+STR(EXPR_TYPE)+", %rdi";
        string call = "callq " + crossPlatformFunctionCall("PinePrint");
        addCompileStmt(type);
        addCompileStmt(call);
    }else if(EXPR_TYPE == INTEGER){
        string ins1 = "movq "+get<0>(result)+", %rdi";
        string ins2 = "callq " + crossPlatformFunctionCall("PinePrintInt");
        addCompileStmt(ins1);
        addCompileStmt(ins2);
    }else if(EXPR_TYPE == STRING){
        string ins1 = "movq %"+get<0>(result)+", %rdi";
        addCompileStmt(ins1);
        string ins2 = "callq " + crossPlatformFunctionCall("PinePrintString");
        addCompileStmt(ins2);
    }else if(EXPR_TYPE == BOOLEAN){
        string ins1 = "movq "+get<0>(result)+", %rdi";
        addCompileStmt(ins1);
        string ins2 = "callq " + crossPlatformFunctionCall("PinePrintBoolean");
        addCompileStmt(ins2);
    }else if(EXPR_TYPE == FLOAT){
        string ins1 = "movq "+get<0>(result)+", %rdi";
        addCompileStmt(ins1);
        string ins2 = "callq " + crossPlatformFunctionCall("PinePrintFloat");
        addCompileStmt(ins2);
    }

    LOG("Compiler:-PolymorphicPrint");
}

string Compiler::getAssembly(){
    LOG("Compiler:getAssembly");
    header.insert(header.end(), compileStmt.top().begin(), compileStmt.top().end());
    string output = "";
    for(auto s : header){
        Char first = s[0];
        string tab = "\t";
        if(first != '.' &&
           !(first >= 'A' && first <= 'Z'))
        {
            s = tab + s;
        }
        output += (s + "\n");
    }
    LOG("Compiler:-getAssembly");
    return output;
}

void Compiler::generateBinary(){
    LOG("Compiler:generateBinary");
    string asmCode = getAssembly();
    ofstream asmFile;
    asmFile.open("Pine.s");
    asmFile << asmCode;
    asmFile.close();
    /* TODO: Variable for gcc or llvm */
    system("gcc -std=c11 -o a.out Pine.s PineRuntime.s");
    LOG("Compiler:-generateBinary");
}
