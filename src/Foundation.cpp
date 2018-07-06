#include "Foundation.hpp"

Object::Object(){}
Object::~Object(){}
int Object::getType(){
    return Type;
}
void Object::print(){
    cout<<"Obj()"<<endl;
}

Number::Number(){
    Type = NUMBER;
}
Number::~Number(){}
void Number::print(){
    cout<<"N()";
}

Boolean::Boolean(){
    Type = BOOLEAN;
}
Boolean::Boolean(bool _val){
    Type = BOOLEAN;
    val = _val;
}
Boolean::~Boolean(){}
bool Boolean::getVal(){
    return val;
}
void Boolean::print(){
    cout<<"Bool("<<val<<")";
}

Integer::Integer(){
    Type = INTEGER;
}
Integer::Integer(int _val){
    Type = INTEGER;
    val = _val;
}
Integer::~Integer(){}
int Integer::getVal(){
    return val;
}
void Integer::print(){
    cout<<"Int("<<val<<")";
}

Float::Float(){
    Type = FLOAT;
}
Float::Float(float _val){
    Type = FLOAT;
    val = _val;
}
Float::~Float(){}
float Float::getVal(){
    return val;
}
void Float::print(){
    cout<<"Float("<<val<<")";
}

Double::Double(){
    Type = DOUBLE;
}
Double::Double(double _val){
    Type = DOUBLE;
    val = _val;
}
Double::~Double(){}
double Double::getVal(){
    return val;
}
void Double::print(){
    cout<<"Double("<<val<<")";
}

String::String(){
    Type = STRING;
}
String::String(string _val){
    Type = STRING;
    val = _val;
}
String::~String(){}
string String::getVal(){
    return val;
}
void String::print(){
    cout<<"S(\"" <<val<< "\")";
}

Let::Let(){
    Type = LET;
}
Let::Let(string _lval, int _expectedType, Object* _rval){
    Type = LET;
    expectedType = _expectedType;
    lval = _lval;
    rval = _rval;
}
Let::~Let(){
    if(!rval){
        delete rval;
        rval = nullptr;
    }
}
Object* Let::getVal(){
    return rval;
}
int Let::getExpectedType(){
    return expectedType;
}
string Let::getName(){
    return lval;
}
void Let::print(){
    cout << "Let(\"" << lval <<"\","<<expectedType<<",";
    rval->print();
    cout << ")";
}

Binary::Binary(){
    Type = BINARY;
}
Binary::Binary(int _operation, Object* l, Object* r){
    Type = BINARY;
    operation = _operation;
    left = l;
    right = r;
}
Binary::~Binary(){
    if(!left){
        delete left;
        left = nullptr;
    }
    if(!right){
        delete right;
        right = nullptr;
    }
}
Object* Binary::getLeft(){
    return left;
}
Object* Binary::getRight(){
    return right;
}
int Binary::getOperation(){
    return operation;
}
void Binary::print(){
    string translate;
    switch (operation) {
        case ADD:
            translate = "Add";
            break;
        case SUB:
            translate = "Sub";
            break;
        case MUL:
            translate = "Mul";
            break;
        case DIV:
            translate = "Div";
            break;
        case MOD:
            translate = "Mod";
            break;
        default:
            translate = "NULL";
            break;
    }
    cout << "Binary(" << translate << ",";
    
    left->print();
    cout << ",";
    right->print();
    cout << ")";
}

Compare::Compare(){
    Type = COMPARE;
}
Compare::Compare(int _operation, Object* l, Object* r){
    Type = COMPARE;
    operation = _operation;
    left = l;
    right = r;
}
Compare::~Compare(){}
Object* Compare::getLeft(){
    return left;
}
Object* Compare::getRight(){
    return right;
}
int Compare::getOperation(){
    return operation;
}
void Compare::print(){
    string translate;
    switch (operation) {
        case EQU:
            translate = "Equ";
            break;
        case NEQ:
            translate = "Neq";
            break;
        case LT:
            translate = "Lt";
            break;
        case GT:
            translate = "Gt";
            break;
        case LTE:
            translate = "Lte";
            break;
        case GTE:
            translate = "Gte";
            break;
        default:
            translate = "NULL";
            break;
    }
    cout << "Compare(" << translate << ",";
    
    left->print();
    cout << ",";
    right->print();
    cout << ")";
}

Var::Var(){Type = VAR;}
Var::~Var(){}
Var::Var(string _name, int _type){
    Type = VAR;
    name = _name;
    type = _type;
}
string Var::getName(){
    return name;
}
int Var::getType(){
    return type;
}
void Var::print(){
    cout << "Var(\"" << name << "\", " << type << ")";
}

Print::Print(){
    Type = PRINT;
}
Print::Print(Object* _val){
    Type = PRINT;
    val = _val;
}
Print::~Print(){
    if(!val){
        delete val;
    }
}
Object* Print::getVal(){
    return val;
}
void Print::print(){
    cout << "Print(";
    val->print();
    cout << ")";
}

Function::Function(){Type = FUNCTION;}
Function::Function(string _name, vector<Object*> _argv, Seq* _body, int _return_type){
    Type = FUNCTION;
    name = _name;
    argv = _argv;
    body = _body;
    return_type = _return_type;
}
string Function::getName(){return name;}
vector<Object*> Function::getArgv(){return argv;}
Seq* Function::getBody(){return body;}
int Function::getReturnType(){return return_type;}
void Function::print(){
    cout << "Function(\"" + name + "\", [";
    //for(Object* o : argv){
        //o->print();
    //}
    cout << "], ";
    body->print();
    cout << ", " + to_string(return_type) + ")";
}

Unary::Unary(){Type = UNARY;}
Unary::Unary(Object* _val, int _op){
    Type = UNARY;
    val = _val;
    operation = _op;
}
Object* Unary::getVal(){return val;}
int Unary::getOperation(){return operation;}
void Unary::print(){
    cout << "Unary(";
    val->print();
    cout << ", " << operation << ")";
}

Seq::Seq(){Type = SEQ;}
Seq::Seq(vector<Object*> _seq){
    Type = SEQ;
    stmt = _seq;
}
vector<Object*> Seq::getStatements(){
    return stmt;
}
void Seq::print(){
    cout << "Seq(";
    for(Object* s : stmt){
        s->print();
        cout << ", ";
    }
    cout << ")";
}

If::If(){Type = IF;}
If::If(Object* _condition, Seq* _body, Seq* _Else) : If() {
    condition = _condition;
    body = _body;
    Else = _Else;
}
Object* If::getCondition(){
    return condition;
}
Seq* If::getBody(){
    return body;
}
Seq* If::getElse(){
    return Else;
}
void If::print(){
    cout << "If(";
    condition->print();
    cout << ", ";
    body->print();
    cout << ", ";
    if(Else != nullptr){
        Else->print();
    }
    cout << ")";
}

Logical::Logical(){
    Type = LOGICAL;
}
Logical::Logical(int o, Object* l, Object* r) : Logical() {
    operation = o;
    left = l;
    right = r;
}
int Logical::getOperation(){
    return operation;
}
Object* Logical::getLeft(){
    return left;
}
Object* Logical::getRight(){
    return right;
}
void Logical::print(){
    cout << "Logical(";
    cout << operation;
    cout << ",";
    left->print();
    cout << ",";
    right->print();
    cout << ")";
}

Assign::Assign(){Type = ASSIGN;}
Assign::Assign(Object* _name, Object* _val) : Assign() {
    name = _name;
    val = _val;
}
Object* Assign::getVar(){
    return name;
}
Object* Assign::getVal(){
    return val;
}
void Assign::print(){
    cout << "Assign(";
    name->print();
    cout << ", ";
    val->print();
    cout << ")";
}

For::For(){Type = FOR;}
For::For(Object* decl, Object* cond, Object* incl, Seq* _body) : For() {
    declare = decl;
    condition = cond;
    incrementor = incl;
    body = _body;
}
Object* For::getDeclare(){
    return declare;
}
Object* For::getCondition(){
    return condition;
}
Object* For::getIncl(){
    return incrementor;
}
Seq* For::getBody(){
    return body;
}
void For::print(){
    cout << "For(";
    declare->print();
    cout << ", ";
    condition->print();
    cout << ", ";
    incrementor->print();
    cout << ", ";
    body->print();
    cout << ")";
}


void DEBUG(string message){
    cout << "\e[0;32m" << "DEBUG: " << "\e[0m" << message << endl;
}

void RaisePineException(string message){
    cout << "\e[1;31m" << "Pine Exception: " << "\e[0m" << message << endl;
    exit(1);
}
void RaisePineWarning(string message){
    cout << "\e[1;33m" << "Pine Warning: " << "\e[0m" << message << endl;
}


