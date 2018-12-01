#include "Static.hpp"

Object* Static::Fold(Object* ast){
    LOG("Static:Fold");
    LOG("    ast: 0x"+AddressOf((void*)ast));
    LOG("   Type: "+STR(ast->getType()));
    Int32 Type = ast->getType();
    if(Type == INTEGER || Type == FLOAT || Type == DOUBLE || Type == STRING ||
       Type == BOOLEAN){
        return ast;
    }
    else if(Type == VAR){
        Var* v = Safe_Cast<Var*>(ast);
        string name = v->getName();
        if(isVar(name)){
            Object* obj = varBindings->top()[name];
            return obj;
        }
        return ast;
    }
    else if(Type == UNARY){
        Unary*  u = Safe_Cast<Unary*>(ast);
        Object* o = Fold(u->getVal());
        if(isPrimative(o->getType())){
            if(o->getType() == INTEGER){
                Int32 neg = -(Safe_Cast<Integer*>(o)->getVal());
                Integer* result = new Integer(neg);
                delete ast;
                return result;
            }
            else if(o->getType() == FLOAT){
                float neg = -(Safe_Cast<class Float*>(o)->getVal());
                class Float* result = new class Float(neg);
                delete ast;
                return result;
            }
            else if(o->getType() == Double){
                double neg = -(Safe_Cast<class Double*>(o)->getVal());
                class Double* result = new class Double(neg);
                delete ast;
                return result;
            }
        }
    }
    else if(Type == BINARY){
        Binary* b = Safe_Cast<Binary*>(ast);
        Object* l = Fold(b->getLeft());
        Object* r = Fold(b->getRight());
        if(isPrimative(l->getType()) && isPrimative(r->getType()) &&
           (l->getType() == r->getType())){
            if(b->getOperation() == ADD){
                if(l->getType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() +
                                Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() +
                                Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() +
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    delete ast;
                    return foldResult;
                }
            }
            else if(b->getOperation() == SUB){
                if(l->getType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() -
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() -
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() -
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    delete ast;
                    return foldResult;
                }
            }
            else if(b->getOperation() == MUL){
                if(l->getType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() *
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() *
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() *
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    delete ast;
                    return foldResult;
                }
            }
            else if(b->getOperation() == DIV){
                if(l->getType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() /
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() /
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    delete ast;
                    return foldResult;
                }
                else if(l->getType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() /
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    delete ast;
                    return foldResult;
                }
            }
            else if(b->getOperation() == MOD){
                if(l->getType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() %
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    delete ast;
                    return foldResult;
                }
            }
        }
        return ast;
    }
    else if(Type == COMPARE){
        Compare* c = Safe_Cast<Compare*>(ast);
        Object*  l = Fold(c->getLeft());
        Object*  r = Fold(c->getRight());
        if(isPrimative(l->getType()) &&
           isPrimative(r->getType()) &&
           (l->getType() == r->getType()))
        {
            if(c->getOperation() == LT){
                Int32 lt = Safe_Cast<Integer*>(l)->getVal() <
                           Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(lt);
                delete ast;
                return result;
            }
            else if(c->getOperation() == LTE){
                Int32 lte = Safe_Cast<Integer*>(l)->getVal() <=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(lte);
                delete ast;
                return result;
            }
            else if(c->getOperation() == EQU){
                Int32 eq = Safe_Cast<Integer*>(l)->getVal() ==
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(eq);
                delete ast;
                return result;
            }
            else if(c->getOperation() == GTE){
                Int32 gte = Safe_Cast<Integer*>(l)->getVal() >=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(gte);
                delete ast;
                return result;
            }
            else if(c->getOperation() == GT){
                Int32 gt = Safe_Cast<Integer*>(l)->getVal() >
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(gt);
                delete ast;
                return result;
            }
            else if(c->getOperation() == NEQ){
                Int32 neq = Safe_Cast<Integer*>(l)->getVal() !=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(neq);
                delete ast;
                return result;
            }
        }
        return ast;
    }
    else if(Type == LOGICAL){
        Logical* o = Safe_Cast<Logical*>(ast);
        Object*  l = Fold(o->getLeft());
        Object*  r = Fold(o->getRight());
        if(isPrimative(l->getType()) &&
           isPrimative(r->getType()) &&
           (l->getType() == r->getType()))
        {
            if(o->getOperation() == OR){
                Int32 res = Safe_Cast<Boolean*>(l)->getVal() ||
                          Safe_Cast<Boolean*>(r)->getVal();
                Boolean* result = new Boolean(res);
                delete ast;
                return result;
            }
            else if(o->getOperation() == AND){
                Int32 res = Safe_Cast<Boolean*>(l)->getVal() &&
                          Safe_Cast<Boolean*>(r)->getVal();
                Boolean* result = new Boolean(res);
                delete ast;
                return result;
            }
        }
    }
    else if(Type == ASSIGN){
//        Assign* a = Safe_Cast<Assign*>(ast);
//        Object* v = Fold(a->getVal());
//        a->replaceVal(v);
        return ast;
    }
    RaisePineWarning("Static analysis did not catch case for: "+STR(Type));
    return ast;
}

Object* Static::ConstantFold(stack<map<string, Object*>>* _varBindings,
                             Object* ast){
    varBindings = _varBindings;
    Object* foldResult = Fold(ast);
    return foldResult;
}

bool Static::isVar(string name){
    if(varBindings == nullptr){
        return false;
    }
    map<string, Object*>* s = &(varBindings->top());
    if(s->find(name) != s->end()){
        return true;
    }
    return false;
}