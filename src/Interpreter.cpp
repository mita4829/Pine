#include "Interpreter.hpp"

bool isValue(int expr_type){
    return expr_type == INTEGER || expr_type == FLOAT || expr_type == DOUBLE || expr_type == STRING;
}
void printHelper(Object* val){
    switch (val->getType()) {
        case INTEGER:
            std::cout << Safe_Cast<Integer*>(val)->getVal() << std::endl;
            break;
        case FLOAT:
            std::cout << Safe_Cast<Float*>(val)->getVal() << std::endl;
            break;
        case DOUBLE:
            std::cout << Safe_Cast<Double*>(val)->getVal() << std::endl;
            break;
        case STRING:
            std::cout << Safe_Cast<String*>(val)->getVal() << std::endl;
            break;
        default:
            std::cout << std::endl;
            break;
    }
}

Memory* Env = Memory::getInstance();

Object* step(Object* expr){
    const int EXPR_TYPE = expr->getType();
    /* Atom tokens */
    if(EXPR_TYPE == INTEGER || EXPR_TYPE == FLOAT || EXPR_TYPE == DOUBLE || EXPR_TYPE == STRING){
        return expr;
    }
    else if(EXPR_TYPE == VAR){
        Var* v = Safe_Cast<Var*>(expr);
        Object* value = Env->get(v->getName());
        return value;
    }
    /* Binary token */
    else if(EXPR_TYPE == BINARY){
        Binary* bin_expr = Safe_Cast<Binary*>(expr);
        Object* left   = bin_expr->getLeft();
        Object* right  = bin_expr->getRight();
        int left_type  = left->getType();
        int right_type = right->getType();
        int operation  = bin_expr->getOperation();
        /*Determine types of operand*/
        if(left_type == INTEGER && right_type == INTEGER){
            if(operation == ADD){
                Object* result = new Integer(Safe_Cast<Integer*>(left)->getVal() + Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == SUB){
                Object* result = new Integer(Safe_Cast<Integer*>(left)->getVal() - Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == MUL){
                Object* result = new Integer(Safe_Cast<Integer*>(left)->getVal() * Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == DIV){
                int denominator = Safe_Cast<Integer*>(right)->getVal();
                if(denominator == 0){
                    RaisePineException("Division by zero.");
                    return nullptr;
                }
                Object* result = new Integer(Safe_Cast<Integer*>(left)->getVal() / denominator);
                return result;
            }else if(operation == MOD){
                int denominator = Safe_Cast<Integer*>(right)->getVal();
                if(denominator == 0){
                    RaisePineException("Modulo by zero.");
                    return nullptr;
                }
                Object* result = new Integer(Safe_Cast<Integer*>(left)->getVal() % denominator);
                return result;
            }
        }else if(left_type == FLOAT && right_type == FLOAT){
            if(operation == ADD){
                Object* result = new Float(Safe_Cast<Float*>(left)->getVal() + Safe_Cast<Float*>(right)->getVal());
                return result;
            }else if(operation == SUB){
                Object* result = new Float(Safe_Cast<Float*>(left)->getVal() - Safe_Cast<Float*>(right)->getVal());
                return result;
            }else if(operation == MUL){
                Object* result = new Float(Safe_Cast<Float*>(left)->getVal() * Safe_Cast<Float*>(right)->getVal());
                return result;
            }else if(operation == DIV){
                float denominator = Safe_Cast<Float*>(right)->getVal();
                if(denominator == 0){
                    RaisePineException("Division by zero.");
                    return nullptr;
                }
                Object* result = new Float(Safe_Cast<Float*>(left)->getVal() / denominator);
                return result;
            }else if(operation == MOD){
                RaisePineException("Cannot evaulate modulo with float operands.");
                return nullptr;
            }
        }else if(left_type == DOUBLE && right_type == DOUBLE){
            if(operation == ADD){
                Object* result = new Double(Safe_Cast<Double*>(left)->getVal() + Safe_Cast<Double*>(right)->getVal());
                return result;
            }else if(operation == SUB){
                Object* result = new Double(Safe_Cast<Double*>(left)->getVal() - Safe_Cast<Double*>(right)->getVal());
                return result;
            }else if(operation == MUL){
                Object* result = new Double(Safe_Cast<Double*>(left)->getVal() * Safe_Cast<Double*>(right)->getVal());
                return result;
            }else if(operation == DIV){
                double denominator = Safe_Cast<Double*>(right)->getVal();
                if(denominator == 0){
                    RaisePineException("Division by zero.");
                    return nullptr;
                }
                Object* result = new Double(Safe_Cast<Double*>(left)->getVal() / denominator);
                return result;
            }else if(operation == MOD){
                RaisePineException("Cannot evaulate modulo with double operands.");
                return nullptr;
            }
        }else if(left_type == STRING && right_type == STRING){
            if(operation == ADD){
                Object* result = new String(Safe_Cast<String*>(left)->getVal() + Safe_Cast<String*>(right)->getVal());
                return result;
            }else{
                RaisePineException("Cannot evaulate expression with String operands.");
                return nullptr;
            }
        }
        /*Types for operands not the same but reducible*/
        else if((!isValue(left_type) || !isValue(right_type))){
            if(!isValue(left_type)){
                left = step(left);
            }
            if(!isValue(right_type)){
                right = step(right);
            }
            Binary* bop = new Binary(operation, left, right);
            Object* result = step(bop);
            delete bop;
            return result;
        }
        /*Irreduciable operands type that are different will thrown an exception*/
        else if(left_type != right_type){
            RaisePineException("Binary operation received non-matching type operands.");
            return nullptr;
        }
    }
    /* Compare token */
    else if(EXPR_TYPE == COMPARE){
        Binary* bin_expr = Safe_Cast<Binary*>(expr);
        Object* left   = bin_expr->getLeft();
        Object* right  = bin_expr->getRight();
        int left_type  = left->getType();
        int right_type = right->getType();
        int operation  = bin_expr->getOperation();
        if(left_type == INTEGER && right_type == INTEGER){
            if(operation == EQU){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() == Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == NEQ){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() != Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == LT){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() < Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == GT){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() > Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == LTE){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() <= Safe_Cast<Integer*>(right)->getVal());
                return result;
            }else if(operation == GTE){
                Object* result = new Boolean(Safe_Cast<Integer*>(left)->getVal() >= Safe_Cast<Integer*>(right)->getVal());
                return result;
            }
        }else if(left_type == FLOAT && right_type == FLOAT){
            float a = Safe_Cast<Float*>(left)->getVal();
            float b = Safe_Cast<Float*>(right)->getVal();
            /*Allow Pine Float values to be equatible to true if both are equivlent up to seven decimal places*/
            const float eps = 0.0000001;
            float diff = a - b;
            diff = ((diff < 0) ? -1 * diff : diff);
            if(operation == EQU){
                if(diff < eps){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }else if(operation == NEQ){
                if(diff < eps){
                    return new Boolean(false);
                }
                return new Boolean(true);
            }else if(operation == LT){
                return new Boolean(a < b);
            }else if(operation == LTE){
                if(diff < eps || a < b){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }else if(operation == GT){
                return new Boolean(a > b);
            }else if(operation == GTE){
                if(diff < eps || a > b){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }
        }else if(left_type == DOUBLE && right_type == DOUBLE){
            double a = Safe_Cast<Double*>(left)->getVal();
            double b = Safe_Cast<Double*>(right)->getVal();
            /*Allow Pine Double values to be equatible to true if both are equivlent up to eleven decimal places*/
            const double eps = 0.000000000001;
            double diff = a - b;
            diff = ((diff < 0) ? -1 * diff : diff);
            if(operation == EQU){
                if(diff < eps){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }else if(operation == NEQ){
                if(diff < eps){
                    return new Boolean(false);
                }
                return new Boolean(true);
            }else if(operation == LT){
                return new Boolean(a < b);
            }else if(operation == LTE){
                if(diff < eps || a < b){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }else if(operation == GT){
                return new Boolean(a > b);
            }else if(operation == GTE){
                if(diff < eps || a > b){
                    return new Boolean(true);
                }
                return new Boolean(false);
            }
        }else if(left_type == STRING && right_type == STRING){
            std::string a = Safe_Cast<String*>(left)->getVal();
            std::string b = Safe_Cast<String*>(right)->getVal();
            if(operation == EQU){
                return new Boolean(a == b);
            }else if(operation == NEQ){
                return new Boolean(a != b);
            }
        }/*Types for operands not the same but reducible*/
        else if((!isValue(left_type) || !isValue(right_type))){
            if(!isValue(left_type)){
                left = step(left);
            }
            if(!isValue(right_type)){
                right = step(right);
            }
            Compare* cop = new Compare(operation, left, right);
            Object* result = step(cop);
            delete cop;
            return result;
        }
        /*Irreduciable operands type that are different will thrown an exception*/
        else if(left_type != right_type){
            RaisePineException("Compare operation received non-matching type operands.");
            return nullptr;
        }
    }
    /*Let token*/
    else if(EXPR_TYPE == LET){
        Let* l = Safe_Cast<Let*>(expr);
        Object* rval = step(l->getVal());
        std::string lval = l->getName();
        if(rval->getType() != l->getExpectedType()){
            RaisePineException("Let constant received incompatible type value.");
            return nullptr;
        }
        Env->set(lval, rval);
        return nullptr;
    }
    /*Print token*/
    else if(EXPR_TYPE == PRINT){
        Print* p = Safe_Cast<Print*>(expr);
        Object* val = p->getVal();
        while(!isValue(val->getType())){
            val = step(val);
        }
        printHelper(val);
        return nullptr;
    }
    RaisePineWarning("Interpreter reached end of token pattern matching.");
    return nullptr;
}
