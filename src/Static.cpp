#include "Static.hpp"

Object* Static::Fold(Object* ast){
    LOG("Static:Fold");
    LOG("    ast: 0x"+AddressOf((void*)ast));
    LOG("   Type: "+STR(ast->getExplicitType()));
    Int32 Type = ast->getExplicitType();
    if(Type == INTEGER || Type == FLOAT || Type == DOUBLE || Type == STRING ||
       Type == BOOLEAN){
        return ast;
    }
    else if(Type == VAR){
        Var* v = Safe_Cast<Var*>(ast);
        string name = v->getName();
        if(isVar(name)){
            map<string, Bindings> bindingMap = bindings->top();
            Bindings binding = bindingMap[name];
            Object* obj = binding.obj;
            if(isPrimative(obj->getExplicitType())){
                return obj->clone();
            }
        }
        return ast;
    }
    else if(Type == UNARY){
        Unary*  u = Safe_Cast<Unary*>(ast);
        Object* o = Fold(u->getVal());
        if(isPrimative(o->getExplicitType())){
            if(o->getExplicitType() == INTEGER){
                Int32 neg = -(Safe_Cast<Integer*>(o)->getVal());
                Integer* result = new Integer(neg);
                deleteObject(ast);
                return result;
            }
            else if(o->getExplicitType() == FLOAT){
                float neg = -(Safe_Cast<class Float*>(o)->getVal());
                class Float* result = new class Float(neg);
                deleteObject(ast);
                return result;
            }
            else if(o->getExplicitType() == Double){
                double neg = -(Safe_Cast<class Double*>(o)->getVal());
                class Double* result = new class Double(neg);
                deleteObject(ast);
                return result;
            }
        }
    }
    else if(Type == BINARY){
        Binary* b = Safe_Cast<Binary*>(ast);
        Object* l = Fold(b->getLeft());
        Object* r = Fold(b->getRight());
        
        /* We must check if the l and r pointer are the same as the original.
         If they are different, then someone lower in the tree already
         deleted that portion of the tree, so we must set the correct child to
         NULL so as to not cause a double free.
         */
        if(b->getLeft() != l){
            b->setLeft(nullptr);
        }
        if(b->getRight() != r){
            b->setRight(nullptr);
        }
        
        if(isPrimative(l->getExplicitType()) && isPrimative(r->getExplicitType()) &&
           (l->getExplicitType() == r->getExplicitType())){
            if(b->getOperation() == ADD){
                if(l->getExplicitType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() +
                                Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() +
                                Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() +
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    deleteObject(ast);
                    return foldResult;
                }
            }
            else if(b->getOperation() == SUB){
                if(l->getExplicitType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() -
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() -
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() -
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    deleteObject(ast);
                    return foldResult;
                }
            }
            else if(b->getOperation() == MUL){
                if(l->getExplicitType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() *
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() *
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() *
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    deleteObject(ast);
                    return foldResult;
                }
            }
            else if(b->getOperation() == DIV){
                if(l->getExplicitType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() /
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == FLOAT){
                    float sum = Safe_Cast<class Float*>(l)->getVal() /
                    Safe_Cast<class Float*>(r)->getVal();
                    class Float* foldResult = new class Float(sum);
                    deleteObject(ast);
                    return foldResult;
                }
                else if(l->getExplicitType() == DOUBLE){
                    double sum = Safe_Cast<class Double*>(l)->getVal() /
                    Safe_Cast<class Double*>(r)->getVal();
                    class Double* foldResult = new class Double(sum);
                    deleteObject(ast);
                    return foldResult;
                }
            }
            else if(b->getOperation() == MOD){
                if(l->getExplicitType() == INTEGER){
                    Int32 sum = Safe_Cast<Integer*>(l)->getVal() %
                    Safe_Cast<Integer*>(r)->getVal();
                    Integer* foldResult = new Integer(sum);
                    deleteObject(ast);
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
        
        /* We must check if the l and r pointer are the same as the original.
           If they are different, then someone lower in the tree already
           deleted that portion of the tree, so we must set the correct child to
           NULL so as to not cause a double free.
         */
        if(c->getLeft() != l){
            c->setLeft(nullptr);
        }
        if(c->getRight() != r){
            c->setRight(nullptr);
        }
        
        if(isPrimative(l->getExplicitType()) &&
           isPrimative(r->getExplicitType()) &&
           (l->getExplicitType() == r->getExplicitType()))
        {
            if(c->getOperation() == LT){
                Int32 lt = Safe_Cast<Integer*>(l)->getVal() <
                           Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(lt);
                deleteObject(ast);
                return result;
            }
            else if(c->getOperation() == LTE){
                Int32 lte = Safe_Cast<Integer*>(l)->getVal() <=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(lte);
                deleteObject(ast);
                return result;
            }
            else if(c->getOperation() == EQU){
                Int32 eq = Safe_Cast<Integer*>(l)->getVal() ==
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(eq);
                deleteObject(ast);
                return result;
            }
            else if(c->getOperation() == GTE){
                Int32 gte = Safe_Cast<Integer*>(l)->getVal() >=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(gte);
                deleteObject(ast);
                return result;
            }
            else if(c->getOperation() == GT){
                Int32 gt = Safe_Cast<Integer*>(l)->getVal() >
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(gt);
                deleteObject(ast);
                return result;
            }
            else if(c->getOperation() == NEQ){
                Int32 neq = Safe_Cast<Integer*>(l)->getVal() !=
                Safe_Cast<Integer*>(r)->getVal();
                Boolean* result = new Boolean(neq);
                deleteObject(ast);
                return result;
            }
        }
        return ast;
    }
    else if(Type == LOGICAL){
        Logical* o = Safe_Cast<Logical*>(ast);
        Object*  l = Fold(o->getLeft());
        Object*  r = Fold(o->getRight());
        
        /* We must check if the l and r pointer are the same as the original.
         If they are different, then someone lower in the tree already
         deleted that portion of the tree, so we must set the correct child to
         NULL so as to not cause a double free.
         */
        if(o->getLeft() != l){
            o->setLeft(nullptr);
        }
        if(o->getRight() != r){
            o->setRight(nullptr);
        }
        
        if(isPrimative(l->getExplicitType()) &&
           isPrimative(r->getExplicitType()) &&
           (l->getExplicitType() == r->getExplicitType()))
        {
            if(o->getOperation() == OR){
                Int32 res = Safe_Cast<Boolean*>(l)->getVal() ||
                          Safe_Cast<Boolean*>(r)->getVal();
                Boolean* result = new Boolean(res);
                deleteObject(ast);
                return result;
            }
            else if(o->getOperation() == AND){
                Int32 res = Safe_Cast<Boolean*>(l)->getVal() &&
                          Safe_Cast<Boolean*>(r)->getVal();
                Boolean* result = new Boolean(res);
                deleteObject(ast);
                return result;
            }
        }
    }
    else if(Type == ASSIGN){
        return ast;
    }
    else if(Type == INDEX){
        Index*      index = Safe_Cast<Index*>(ast);
        Object* foldIndex = Fold(index->getIndex());
        string arrayName  = index->getArrayName();
        Int32  elementType= index->getElementType();
        
        if (index->getIndex() != foldIndex) {
            deleteObject(ast);
            return new Index(arrayName, foldIndex, elementType);
        }
        return ast;
    }
    RaisePineWarning("Static analysis did not catch case for: "+getTypeName(Type));
    return ast;
}

Object* Static::ConstantFold(stack<map<string, Bindings>>* _bindings,
                             Object* ast){
    bindings = _bindings;
    Object* foldResult = Fold(ast);
    return foldResult;
}

bool Static::isVar(string name){
    if(bindings == nullptr){
        return false;
    }
    map<string, Bindings>* bindingMap = &(bindings->top());
    if(bindingMap->find(name) != bindingMap->end()){
        return true;
    }
    return false;
}

void Static::printBindings(){
    LOG("Static:printBindings");
    if(bindings == nullptr){
        cout << "{\n}" << endl;
    }else{
        map<string, Bindings>* s = &(bindings->top());
        cout << "{" << endl;
        for(const auto& value : *s){
            if (value.second.obj != nullptr){
                cout << "    " << value.first;
                cout << ": (" << getTypeName(value.second.type) << ", ";
                value.second.obj->print();
                cout << ")";
                cout << endl;
            }
        }
        cout << "}" << endl;
    }
    LOG("Static:-printBindings");
}
