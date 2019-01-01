#include "Compiler.hpp"

CompilerResult EmptyResult;

string requestTmpVarName(){
    LOG("Compiler:requestTmpVarName");
    static Int32 id = -1;
    id += 1;
    return "var" + to_string(id);
}

Int32 align32ByteStack(Int32 varCount){
    const Int32 ALIGNMENT = 32;
    Int32    byteCount = varCount * ARCH64;
    Int32 next32Factor = (byteCount + ALIGNMENT) / ALIGNMENT;
    Int32    stackSize = next32Factor * ALIGNMENT;
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
Int32 Compiler::getImplicitType(Object* expr){
    LOG("Compiler:getImplicitType");
    LOG("    expr: 0x"+AddressOf(&expr));
    if (expr == nullptr) {
        return NIT;
    }
    Int32 Type = expr->getExplicitType();
    if(Type == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        return v->getVarType();
    }
    else if(Type == INDEX){
        Index* i = Safe_Cast<Index*>(expr);
        if (isPrimative(i->getElementType())) {
            return i->getElementType();
        }
        string arrayName = i->getArrayName();
        CompileBinding binding = getBindings(arrayName);
        return getImplicitType(binding.obj);
    }
    else if(Type == BINARY){
        Binary* b = Safe_Cast<Binary*>(expr);
        return b->typeContext.implicitType;
    }
    else if(Type == ARRAY){
        Array* a = Safe_Cast<Array*>(expr);
        vector<Object*> array = a->getArray();
        return getImplicitType(array.size() > 1 ? array[1] : nullptr);
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
            expr_type == STRING  ||
            expr_type == INDEX);
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
    
    Int32 Type = expr->getExplicitType();
    LOG("    Type: "+getTypeName(Type));
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
        
        TypeContext context;
        context.explicitType = rval->getExplicitType();
        context.implicitType = getImplicitType(rval);;
        
        Let* l          = new Let(lval, context, new Unary(rval, u->getOperation()));
        addFlatStmtToStack(l);
        return new Var(lval, context.implicitType);
    }
    /* Let */
    else if(Type == LET){
        Let* l          = Safe_Cast<Let*>(expr);
        Object* rval    = flatten(l->getVal());
        
        TypeContext context;
        context.explicitType = rval->getExplicitType();
        context.implicitType = getImplicitType(rval);
        
        Let* fl         = new Let(l->getName(), context, rval);
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
        Int32 lt         = fl->getExplicitType();
        Int32 rt         = fr->getExplicitType();
        Int32 operation  = b->getOperation();
        string name = requestTmpVarName();
        while(isValue(fl->getExplicitType()) == false){
            fl = flatten(fl);
        }
        while(isValue(fr->getExplicitType()) == false){
            fr = flatten(fr);
        }
        /*
         We let the type of the binary expr be the evaluated left
         operand.
         */
        TypeContext context;
        context.explicitType = fl->getExplicitType();
        context.implicitType = getImplicitType(fl);
        
        Let* l = new Let(name, context, new Binary(operation, fl, fr));
        addFlatStmtToStack(l);
        
        return new Var(name, context.implicitType);
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
        
        TypeContext context;
        context.explicitType = BOOLEAN;
        
        Let* fl          = new Let(name, context, new Compare(operation, l, r));
        addFlatStmtToStack(fl);
        return new Var(name, BOOLEAN);
    }
    /*Logical*/
    else if(Type == LOGICAL){
        Logical* l       = Safe_Cast<Logical*>(expr);
        Int32 operation  = l->getOperation();
        Object* lf       = flatten(l->getLeft());
        Object* rf       = flatten(l->getRight());
        string name      = requestTmpVarName();
        
        TypeContext context;
        context.explicitType = BOOLEAN;
        
        Let* fl          = new Let(name, context, new Logical(operation, lf, rf));
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
    /* Array */
    else if(Type == ARRAY){
        Array* a = Safe_Cast<Array*>(expr);
        vector<Object*> array = a->getArray();
        vector<Object*> flatArray;
        for(const auto& element : array){
            flatArray.push_back(flatten(element));
        }
        Array* flatArrayObj = new Array(flatArray, a->getLength(), a->getElementType());
        
        flatArrayObj->typeContext.arrayDepth         = a->typeContext.arrayDepth;
        flatArrayObj->typeContext.arrayPrimativeElementType = a->typeContext.arrayPrimativeElementType;
        
        return flatArrayObj;
    }
    /* Index */
    else if(Type == INDEX){
        Index* i = Safe_Cast<Index*>(expr);
        vector<Object*> indices = i->getIndex();
        vector<Object*> flatIndices;
        for (const auto& i : indices) {
            flatIndices.push_back(flatten(i));
        }
        string name = requestTmpVarName();
        
        Index* flat = new Index(i->getArrayName(), flatIndices, i->getElementType());
        
        return flat;
    }
    RaisePineWarning("Flatten reached end of token matching ("+getTypeName(Type)+").");
    return nullptr;
}

CompilerResult Compiler::compile(Object* expr){
    LOG("Compiler:compile");
    LOG("    expr: 0x"+AddressOf(&expr));
    
    Int32 Type = expr->getExplicitType();
    LOG("    Type: "+getTypeName(Type));
    /* Integer */
    if(Type == INTEGER){
        Integer*       i = Safe_Cast<Integer*>(expr);
        Int64          v = i->getVal();
        string   operand = "$"+STR(v);
        
        CompilerResult result = EmptyResult;
        result.resultType = INTEGER;
        result.data       = operand;
        
        return result;
    }
    /* Float */
    else if(Type == FLOAT){
        class Float* f = Safe_Cast<class Float*>(expr);
        float val = f->getVal();
    
        union {
            double f_bits;
            Int64 i_bits;
        };
        
        f_bits = (double)val;
        Int32 id = requestFloatID();
        
        string ins1 = "FLOAT_"+STR(id)+":";
        string ins2 = ".quad " + STR(i_bits) + " #" + STR(f_bits);
        string ins4 = "movq FLOAT_"+STR(id)+"(%rip), %xmm0";
        header.push_back(ins1);
        header.push_back(ins2);
        addCompileStmt(ins4);
        
        CompilerResult result = EmptyResult;
        result.resultType = REG;
        /* TODO: Update reg manager to use other SIMD registers */
        result.data       = "%xmm0";
        return result;
    }
    /* Double */
    else if(Type == DOUBLE){
        class Double* d = Safe_Cast<class Double*>(expr);
        double val = d->getVal();
    
        union {
            double d_bits;
            Int64  i_bits;
        };
        
        d_bits = val;
        Int32 id = requestFloatID();
        
        string ins1 = "DOUBLE_"+STR(id)+":";
        string ins2 = ".quad " + STR(i_bits) + " #" + STR(d_bits);
        string ins3 = ".align " + STR(ARCH32);
        string ins4 = "movq DOUBLE_"+STR(id)+"(%rip), %xmm0";
        header.push_back(ins1);
        header.push_back(ins2);
        header.push_back(ins3);
        addCompileStmt(ins4);
        
        CompilerResult result = EmptyResult;
        result.resultType = REG;
        /* TODO: Update reg manager to use other SIMD registers */
        result.data       = "%xmm0";
        return result;
    }
    /* Var */
    else if(Type == VAR){
        Var*            var = Safe_Cast<Var*>(expr);
        Int32          type = var->getVarType();
        string         name = var->getName();
        Int32 stackLocation = retrieveStackLocation(name);
        string         ins1 = "-"+to_string(stackLocation)+"(%rbp)";
        
        CompilerResult result = EmptyResult;
        result.resultType     = STACKLOC;
        result.stackLocation  = stackLocation;
        result.data           = "-" + STR(stackLocation) + "(%rbp)";
        
        return result;
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
        
        CompilerResult result = EmptyResult;
        result.resultType     = REG;
        result.data           = ins3;
    
        return result;
    }
    /*Boolean*/
    else if(Type == BOOLEAN){
        Boolean* boolean = Safe_Cast<Boolean*>(expr);
        bool val = boolean->getVal();
        Int32 cast = (Int32)val;
        string operand = "$"+STR(cast);
        
        CompilerResult result = EmptyResult;
        
        result.resultType     = BOOLEAN;
        result.data           = operand;
        
        return result;
    }
    /* Binary */
    else if(Type == BINARY){
        Binary*                  b = Safe_Cast<Binary*>(expr);
        Int32            operation = b->getOperation();
        CompilerResult        left = compile(b->getLeft());
        string             regleft = IntoRegister(left.data);
        CompilerResult       right = compile(b->getRight());
        string            regright = IntoRegister(right.data);
        Int32                ltype = getImplicitType(b->getLeft());
        Int32                rtype = getImplicitType(b->getRight());
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
        else if(operation == BAND){
            if(ltype == INTEGER && rtype == INTEGER){
                string ins1 = "movq %"+regleft+", %rcx";
                string ins2 = "movq %"+regright+", %rax";
                string ins3 = "andq %rcx, %rax";
                string ins4 = "movq %rax, %"+regright;
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
                addCompileStmt(ins4);
            }else{
                RaisePineException("Bit-wise And is not defined for non-integer operands.");
            }
        }
        else if(operation == BOR){
            if(ltype == INTEGER && rtype == INTEGER){
                string ins1 = "movq %"+regleft+", %rcx";
                string ins2 = "movq %"+regright+", %rax";
                string ins3 = "orq %rcx, %rax";
                string ins4 = "movq %rax, %"+regright;
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
                addCompileStmt(ins4);
            }else{
                RaisePineException("Bit-wise Or is not defined for non-integer operands.");
            }
        }
        else if(operation == XOR){
            if(ltype == INTEGER && rtype == INTEGER){
                string ins1 = "movq %"+regleft+", %rcx";
                string ins2 = "movq %"+regright+", %rax";
                string ins3 = "xorq %rcx, %rax";
                string ins4 = "movq %rax, %"+regright;
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
                addCompileStmt(ins4);
            }else{
                RaisePineException("Bit-wise Xor is not defined for non-integer operands.");
            }
        }
        else if(operation == LS){
            if(ltype == INTEGER && rtype == INTEGER){
                string ins1 = "movq %"+regright+", %rcx";
                string ins2 = "movq %"+regleft+", %rax";
                string ins3 = "shlq %cl, %rax";
                string ins4 = "movq %rax, %"+regright;
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
                addCompileStmt(ins4);
            }else{
                RaisePineException("Left shift is not defined for non-integer operands.");
            }
        }
        else if(operation == RS){
            if(ltype == INTEGER && rtype == INTEGER){
                string ins1 = "movq %"+regright+", %rcx";
                string ins2 = "movq %"+regleft+", %rax";
                string ins3 = "sarq %cl, %rax";
                string ins4 = "movq %rax, %"+regright;
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
                addCompileStmt(ins4);
            }else{
                RaisePineException("Right shift is not defined for non-integer operands.");
            }
        }
        else{
            RaisePineWarning("Uncaught Binary operation in compile "+STR(operation));
        }
        registerManager.releaseRegister(regleft);
        
        b->typeContext.implicitType = ltype;
        CompilerResult result = EmptyResult;
        
        result.resultType     = REG;
        result.data           = regright;
        return result;
    }
    /* Function */
    else if(Type == FUNCTION){
        Function* f = Safe_Cast<Function*>(expr);
        pushNewStackFrame();
        /* TODO: Expand args here */
        Seq* body = f->getBody();
        compile(body);
        /* Create stack space dependent on variable count */
        Int32 varCount  = bindings.top().size();
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
        
        string ins7    = "_"+f->getName()+":";
        string ins8    = ".global _"+f->getName();
        
        addFrontCompileStmt(ins7);
        addFrontCompileStmt(ins8);
        
        popStackFrame();
        
        CompilerResult result = EmptyResult;
        result.resultType     = VOID;
        return result;
    }
    /* Seq */
    else if(Type == SEQ){
        Seq* seq             = Safe_Cast<Seq*>(expr);
        vector<Object*> body = seq->getStatements();
        for(const auto& stmt : body){
            compile(stmt);
        }
        
        CompilerResult result = EmptyResult;
        result.resultType     = VOID;
        return result;
    }
    /* Let */
    else if(Type == LET){
        Let*                    let = Safe_Cast<Let*>(expr);
        Object*                rval = let->getVal();
        string                 name = let->getName();
        CompilerResult compiledResult = compile(rval);
        string                  reg = IntoRegister(compiledResult.data);
        Int32         stackLocation;
        CompileBinding binding;
        
        CompilerResult result = EmptyResult;
        
        if(rval->getExplicitType() == ARRAY){
            string   ins1 = "movq %"+reg+", "+compiledResult.data;
            addCompileStmt(ins1);
            string location = compiledResult.data;
            
            binding.stackLocation = compiledResult.stackLocation;
            
            result.stackLocation           = compiledResult.stackLocation;
            let->typeContext.stackLocation = compiledResult.stackLocation;
        }else{
            setBindings(name);
            stackLocation = retrieveStackLocation(name);
            binding.stackLocation = stackLocation;

            string   ins1 = "movq %"+reg+", -"+STR(stackLocation)+"(%rbp)";
            
            result.stackLocation           = stackLocation;
            let->typeContext.stackLocation = stackLocation;
            
            addCompileStmt(ins1);
        }
        binding.obj = rval;
        setBindings(name, binding);

        registerManager.releaseRegister(reg);
        
        result.resultType     = VOID;
        return result;
    }
    /* Assign */
    else if(Type == ASSIGN){
        Assign*           a = Safe_Cast<Assign*>(expr);
        Object*        rval = a->getVal();
        string          reg = IntoRegister(compile(rval).data);
        string      varName = Safe_Cast<Var*>(a->getVar())->getName();
        Int32 stackLocation = retrieveStackLocation(varName);
        string stmt = "movq %"+reg+", -"+to_string(stackLocation)+"(%rbp)";
        addCompileStmt(stmt);
        
        registerManager.releaseRegister(reg);
        
        CompilerResult result = EmptyResult;
        result.resultType     = VOID;
        return result;
    }
    /* Print */
    else if(Type == PRINT){
        Print* p = Safe_Cast<Print*>(expr);
        CompilerResult compiledResult = compile(p->getVal());
        PolymorphicPrint(p->getVal(), compiledResult);
        CompilerResult result = EmptyResult;
        
        result.resultType     = VOID;
        return result;
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
            Int32 conType = getImplicitType(condition);
            if(conType != BOOLEAN &&
               conType != COMPARE &&
               conType != LOGICAL)
            {
                RaisePineException("If statement conditions must be evaluable to a Boolean type.");
            }
            CompilerResult con = compile(condition);
            string ins1 = IntoRegister(con.data);
            
            string ins3 = "ELSE_ELIF_"+STR(requestJumpID());
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
        
        CompilerResult result = EmptyResult;
        
        result.resultType     = VOID;
        return result;
    }
    /* Compare */
    else if(Type == COMPARE){
        Compare* c = Safe_Cast<Compare*>(expr);
        Int32 operation = c->getOperation();
        CompilerResult left  = compile(c->getLeft());
        CompilerResult right = compile(c->getRight());
        Int32 ltype = getImplicitType(c->getLeft());
        Int32 rtype = getImplicitType(c->getRight());
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
        string regleft = IntoRegister(left.data);
        string regright = IntoRegister(right.data);
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
        
        registerManager.releaseRegister(regleft);
        
        CompilerResult result = EmptyResult;
        
        result.resultType = REG;
        result.data       = regright;
        return result;
    }
    /* For */
    else if(Type == FOR){
        For* f = Safe_Cast<For*>(expr);
        compile(f->getDeclare());
        string ins1 = "FOR_" + STR(requestJumpID());
        addCompileStmt(ins1 + ":");
        string ins2 = "SHORT_" + STR(requestShortID());
        CompilerResult compiledResult = compile(f->getCondition());
        string reg  = IntoRegister(compiledResult.data);
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
        
        CompilerResult result = EmptyResult;
        result.resultType     = VOID;
        return result;
    }
    /*While*/
    else if(Type == WHILE){
        While* w = Safe_Cast<While*>(expr);
        
        string ins1 = "WHILE_" + STR(requestJumpID());
        addCompileStmt(ins1 + ":");
        
        CompilerResult compiledResult = compile(w->getCondition());
        string reg = IntoRegister(compiledResult.data);
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
        
        CompilerResult result = EmptyResult;
        result.resultType     = VOID;
        return result;
    }
    /* Array */
    else if(Type == ARRAY){
        Array* a = Safe_Cast<Array*>(expr);
        vector<Object*>       array = a->getArray();
        static Int32 elementCounter = 0;
        const  Int32    NO_LOCATION = -1;
        Int32        pointerToArray = NO_LOCATION;
        Int32           elementType = NIT;
        Int32         stackLocation = NO_LOCATION;
        
        for(const auto& element : array){
            const string elementName = "ARRAY_ELEMENT_"+STR(elementCounter);
            elementCounter += 1;
            
            CompilerResult compiledResult = compile(element);
            
            if(element->getExplicitType() != ARRAY){
                string    reg = IntoRegister(compiledResult.data);
                setBindings(elementName);
                stackLocation = retrieveStackLocation(elementName);
                
                string   ins1 = "movq %"+reg+", -"+STR(stackLocation)+"(%rbp)";
                addCompileStmt(ins1);
                registerManager.releaseRegister(reg);
                
            }else{
                stackLocation = compiledResult.stackLocation;
            }
            
            if(pointerToArray == NO_LOCATION){
                /* Assign the first element of the array as the pointerToArray */
                pointerToArray = stackLocation;
            }
        }
        
        CompilerResult result = EmptyResult;
        result.resultType     = ARRAY;
        result.stackLocation  = pointerToArray;
        result.data           = "-" + STR(pointerToArray) + "(%rbp)";
        
        a->typeContext.stackLocation = pointerToArray;
        return result;
    }
    /* Index */
    else if(Type == INDEX){
        Index*                 i = Safe_Cast<Index*>(expr);
        string         arrayName = i->getArrayName();
        Array*                 a = Safe_Cast<Array*>(getBindings(arrayName).obj);
        Int32        indexOffset = a->getIndexOffsetSize();
        Int32 arrayStackLocation = retrieveStackLocation(arrayName);
        vector<Int32> offsetTable = a->getOffsetTable();

        vector<Object*>  indices = i->getIndex();

        string               reg;
        string             toReg = registerManager.getRegister();
        string              ins0 = "xorq %"+toReg+", %"+toReg;
        addCompileStmt(ins0);
        
        for (Int32 i = 0; i < indices.size(); i++) {
            Object* nthIndex = indices[i];
            CompilerResult compiledResult = compile(nthIndex);
                                      reg = IntoRegister(compiledResult.data);
            string                   ins1 = "imulq $"+STR(offsetTable[offsetTable.size()-1-i])+", %"+reg;
            string                   ins2 = "addq  %"+reg+", %"+toReg;
            string                   ins3 = "incq %"+toReg;
            addCompileStmt(ins1);
            addCompileStmt(ins2);
            addCompileStmt(ins3);
        }
        
        string ins3 = "negq %"+toReg;
        string ins4 = "movq -"+STR(arrayStackLocation)+"(%rbp, %"+toReg+
                      ", "+STR(ARCH64)+"), %"+reg;
        
        addCompileStmt(ins3);
        addCompileStmt(ins4);

        CompilerResult result   = EmptyResult;
        result.resultType       = REG;
        result.data             = reg;

        i->typeContext.indexInstruction = "-"+STR(arrayStackLocation)+"(%rbp, %"+toReg+
                                          ", "+STR(ARCH64)+")";
        return result;
    }
    RaisePineWarning("Compiler reached end of token matching. Type: "+getTypeName(Type));
    LOG("Compile:compile escaped type: "+STR(Type));
    CompilerResult result = EmptyResult;
    result.resultType     = VOID;
    return result;
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
    pushNewBindings();
}

void Compiler::popStackFrame(){
    vector<string> second_last = (compileStmt.top());
    compileStmt.pop();
    vector<string> last = (compileStmt.top());
    last.insert(last.begin(), second_last.begin(), second_last.end());
    compileStmt.pop();
    compileStmt.push(last);
    popBindings();
}

void Compiler::PolymorphicPrint(Object* expr, CompilerResult result){
    LOG("Compiler:PolymorphicPrint");
    LOG("    expr: 0x"+AddressOf(&expr));
    LOG("    result: ("+result.data+", "+getTypeName(result.resultType)+")");
    
    Int32 explicitType = expr->getExplicitType();
    LOG("    explicitType: "+getTypeName(explicitType));
    switch (explicitType) {
      case VAR:
        {
            Var*         v = Safe_Cast<Var*>(expr);
            string varName = v->getName();
            Int32 stackLocation = retrieveStackLocation(varName);
            
            if (isPrimative(v->getVarType())) {
                string ins1 = "leaq -"+STR(stackLocation)+"(%rbp), %rsi";
                string ins2 = "movq $"+STR(v->getVarType())+", %rdi";
                string ins3 = "callq _PinePrint";
                
                addCompileStmt(ins1);
                addCompileStmt(ins2);
                addCompileStmt(ins3);
            }
            else {
                CompileBinding binding = getBindings(varName);
                Object*     bindedExpr = binding.obj;
                PolymorphicPrint(bindedExpr, result);
            }
        }
        break;
      case ARRAY:
        {
            Array* a = Safe_Cast<Array*>(expr);
            vector<Object*> array = a->getArray();
            Int32 arrayLength     = a->getLength();
            
            string ins1 = "leaq "+result.data+", %rsi";
            string ins2;
            
            /* If the explicit type is still not primative, retrieve the implicit type */
            Int32 primativeType = a->typeContext.arrayPrimativeElementType;
            if (isPrimative(primativeType) != true) {
                primativeType = getImplicitType(array.size() > 1 ? array[1] : nullptr);
            }
            ins2 = "movq $"+STR(primativeType)+", %rdi";
            
            string ins3 = "callq _PinePrintArray";

            addCompileStmt(ins1);
            addCompileStmt(ins2);
            addCompileStmt(ins3);
        }
        break;
      case INTEGER:
        {
            string ins1 = "movq "+result.data+", %rdi";
            string ins2 = "callq _PinePrintInt";
            addCompileStmt(ins1);
            addCompileStmt(ins2);
        }
        break;
      case STRING:
        {
            string ins1 = "movq %"+result.data+", %rdi";
            string ins2 = "callq _PinePrintString";
            addCompileStmt(ins1);
            addCompileStmt(ins2);
            
            registerManager.releaseRegister(result.data);
        }
        break;
      case INDEX:
        {
            Index* i = Safe_Cast<Index*>(expr);
            string arrayName = i->getArrayName();
            Int32 elementType = i->getElementType();
            
            CompileBinding binding = getBindings(arrayName);
            Array* a = Safe_Cast<Array*>(binding.obj);
            
            string ins1;
            string ins2;
            string ins3;
            
            if (isPrimative(elementType)) {
                ins1 = "leaq "+i->typeContext.indexInstruction+", %rsi";
                ins2 = "movq $"+STR(elementType)+", %rdi";
                ins3 = "callq _PinePrint";
                
            }
            else if (i->getIndex().size() == a->typeContext.arrayDepth){
                ins1 = "leaq "+i->typeContext.indexInstruction+", %rsi";
                ins2 = "movq $"+STR(a->typeContext.arrayPrimativeElementType)+", %rdi";
                ins3 = "callq _PinePrint";
            }
            else {
                ins1 = "leaq "+i->typeContext.indexInstruction+", %rsi";
                ins2 = "movq $"+STR(a->typeContext.arrayPrimativeElementType)+", %rdi";
                ins3 = "callq _PinePrintArray";
            }
            
            addCompileStmt(ins1);
            addCompileStmt(ins2);
            addCompileStmt(ins3);

        }
        break;
      case BOOLEAN:
        {
            string ins1 = "movq "+result.data+", %rdi";
            string ins2 = "callq _PinePrintBoolean";
            addCompileStmt(ins1);
            addCompileStmt(ins2);
        }
        break;
      case FLOAT:
        {
            string ins1 = "movq "+result.data+", %rdi";
            string ins2 = "callq _PinePrintFloat";
            addCompileStmt(ins1);
            addCompileStmt(ins2);
        }
        break;
      case DOUBLE:
        {
            string ins1 = "movq "+result.data+", %rdi";
            string ins2 = "callq _PinePrintDouble";
            addCompileStmt(ins1);
            addCompileStmt(ins2);
        }
        break;
      default:
        RaisePineWarning("Print function caught unknown type ("+getTypeName(explicitType)+")");
        break;
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

// ---
void Compiler::pushNewBindings(){
    map<string, CompileBinding> m;
    bindings.push(m);
}

void Compiler::popBindings(){
    map<string, CompileBinding>* last = &(bindings.top());
    bindings.pop();
}

void Compiler::setBindings(string varName, CompileBinding binding){
    map<string, CompileBinding>* last = &(bindings.top());
    (*last)[varName] = binding;
}

void Compiler::setBindings(string varName){
    map<string, CompileBinding>* frame = &(bindings.top());
    if(frame->count(varName) > 0){
        RaisePineException("Redeclarion of variable named: "+varName);
    }
    /* Stack address are aligned by 0x8. Give the Var the next location based
     on the given size of the scope's variable count.
     */
    Int32 stackLocation = (frame->size() + 1) << 3;
    
    CompileBinding binding;
    binding.stackLocation = stackLocation;
    binding.obj           = nullptr;
    
    setBindings(varName, binding);
}

CompileBinding Compiler::getBindings(string varName){
    map<string, CompileBinding>* frame = &(bindings.top());
    return (*frame)[varName];
}

Int32 Compiler::retrieveStackLocation(string varName){
    map<string, CompileBinding>* frame = &(bindings.top());
    if (frame->find(varName) == frame->end()) {
        RaisePineException("Stack location could not be found for: "+varName);
    }
    CompileBinding binding = (*frame)[varName];
    return binding.stackLocation;
}
