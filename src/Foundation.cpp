#include "Foundation.hpp"

vector<string> Trace;
map<Int32, Object*> memoryPool;

void DEBUG(string message){
    LOG("Foundation:DEBUG");
    cout << GREEN << "DEBUG: " << WHITE << message << endl;
}

void RaisePineException(string message){
    LOG("Foundation:RaisePineException");
    ofstream log;
    log.open("log.txt");
    
    /* Dump traces to log file */
    for(int i = 0; i < Trace.size(); i++){
        log << Trace[i] << "\n";
    }
    
    log.close();
    cout << RED << "Pine Exception: " << WHITE << message << endl;
    exit(1);
}
void RaisePineWarning(string message){
    LOG("Foundation:RaisePineWarning");
    cout << YELLOW << "Pine Warning: " << WHITE << message << endl;
}

string AddressOf(void* ptr){
    stringstream ss;
    ss << hex << ((long long)ptr);
    return ss.str();
}

bool isPrimative(Int32 Type){
    return (Type == INTEGER ||
            Type == FLOAT ||
            Type == DOUBLE ||
            Type == STRING ||
            Type == BOOLEAN);
}

Object::Object(){
    static UInt32 memoryPoolID = 0;
    LOG("Foundation:Object");
    LOG("    memoryPoolID: "+STR(memoryPoolID));
    
    poolID = memoryPoolID;
    memoryPool[memoryPoolID] = this;
    
    memoryPoolID += 1;
}
Object::~Object(){LOG("Foundation:~Object");}
Int32 Object::getType(){
    return Type;
}
UInt32 Object::getMemoryPoolID(){
    return poolID;
}
void Object::print(){
    cout<<"Obj()"<<endl;
}

Object* Object::clone(){
    return nullptr;
}

Number::Number(){
    LOG("Foundation:Number");
    Type = NUMBER;
}
Number::~Number(){LOG("Foundation:~Number");}
void Number::print(){
    cout<<"N()";
}

Object* Number::clone(){
    return nullptr;
}

Boolean::Boolean(){
    LOG("Foundation:Boolean");
    Type = BOOLEAN;
}
Boolean::Boolean(bool _val){
    LOG("Foundation:Boolean");
    LOG("    _val: "+to_string(val));
    Type = BOOLEAN;
    val = _val;
}
Boolean::~Boolean(){LOG("Foundation:~Boolean");}
bool Boolean::getVal(){
    return val;
}
void Boolean::print(){
    cout<<"Bool("<<val<<")";
}

Object* Boolean::clone(){
    Boolean* copy = new Boolean(val);
    return copy;
}

Integer::Integer(){
    LOG("Foundation:Integer");
    Type = INTEGER;
}
Integer::Integer(Int32 _val){
    LOG("Foundation:Integer");
    LOG("    _val: "+to_string(_val));
    Type = INTEGER;
    val = _val;
}
Integer::~Integer(){LOG("Foundation:~Integer");}
Int32 Integer::getVal(){
    return val;
}
void Integer::print(){
    cout<<"Int("<<val<<")";
}

Object* Integer::clone(){
    Integer* copy = new Integer(val);
    return copy;
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

Object* Float::clone(){
    Float* copy = new Float(val);
    return copy;
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

Object* Double::clone(){
    Double* copy = new Double(val);
    return copy;
}

String::String(){
    Type = STRING;
}
String::String(string _val){
    Type = STRING;
    val = _val;
}
String::~String(){LOG("Foundation:~String");}
string String::getVal(){
    return val;
}
void String::print(){
    cout<<"String(" <<val<< ")";
}

Object* String::clone(){
    String* copy = new String(val);
    return copy;
}

Let::Let(){
    Type = LET;
}

Let::Let(string _lval, Int32 _expectedType, Object* _rval){
    Type = LET;
    expectedType = _expectedType;
    lval = _lval;
    rval = _rval;
}

Let::~Let(){
    if(rval != nullptr){
        deleteObject(rval);
        rval = nullptr;
    }
}

Object* Let::getVal(){
    return rval;
}

Int32 Let::getExpectedType(){
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

Object* Let::clone(){
    Let* copy = new Let(lval, expectedType, rval->clone());
    return copy;
}

Binary::Binary(){
    Type = BINARY;
}

Binary::Binary(Int32 _operation, Object* l, Object* r){
    Type = BINARY;
    operation = _operation;
    left = l;
    right = r;
}

Binary::~Binary(){
    LOG("Foundation:~Binary");
    if(left != nullptr){
        deleteObject(left);
        left = nullptr;
    }
    if(right != nullptr){
        deleteObject(right);
        right = nullptr;
    }
}

Object* Binary::getLeft(){
    return left;
}

Object* Binary::getRight(){
    return right;
}

void Binary::setLeft(Object* val){
    left = val;
}

void Binary::setRight(Object* val){
    right = val;
}

Int32 Binary::getOperation(){
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

Object* Binary::clone(){
    Binary* copy = new Binary(operation, left->clone(), right->clone());
    return copy;
}

Compare::Compare(){
    Type = COMPARE;
    left = nullptr;
    right = nullptr;
}

Compare::Compare(Int32 _operation, Object* l, Object* r){
    Type = COMPARE;
    operation = _operation;
    left = l;
    right = r;
}

Compare::~Compare(){
    if(left != nullptr){
        deleteObject(left);
        left = nullptr;
    }
    if(right != nullptr){
        deleteObject(right);
        right = nullptr;
    }
}

Object* Compare::getLeft(){
    return left;
}

Object* Compare::getRight(){
    return right;
}

void Compare::setLeft(Object* val){
    left = val;
}

void Compare::setRight(Object* val){
    right = val;
}

Int32 Compare::getOperation(){
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

Object* Compare::clone(){
    Compare* copy = new Compare(operation, left->clone(), right->clone());
    return copy;
}

Var::Var(){
    Type = VAR;
}

Var::~Var(){}

Var::Var(string _name, Int32 _type){
    Type = VAR;
    name = _name;
    type = _type;
}

string Var::getName(){
    return name;
}

Int32 Var::getType(){
    return type;
}

void Var::print(){
    cout << "Var(\"" << name << "\", " << type << ")";
}

Object* Var::clone(){
    Var* copy = new Var(name, type);
    return copy;
}

Print::Print(){
    Type = PRINT;
    val = nullptr;
}

Print::Print(Object* _val){
    Type = PRINT;
    val = _val;
}

Print::~Print(){
    LOG("Foundation:~Print");
    if(val != nullptr){
        deleteObject(val);
        val = nullptr;
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

Object* Print::clone(){
    Print* copy = new Print(val->clone());
    return copy;
}

Function::Function(){
    Type = FUNCTION;
    body = nullptr;
}

Function::~Function(){
    LOG("Foundation:~Function");
    for(const auto &elem : argv){
        if(elem != nullptr){
            deleteObject(elem);
        }
    }
    argv.clear();
    if(body != nullptr){
        deleteObject(body);
        body = nullptr;
    }
}

Function::Function(string _name, vector<Object*> _argv, Seq* _body, Int32 _return_type){
    Type = FUNCTION;
    name = _name;
    argv = _argv;
    body = _body;
    return_type = _return_type;
}

string Function::getName(){return name;}

vector<Object*> Function::getArgv(){return argv;}

Seq* Function::getBody(){return body;}

Int32 Function::getReturnType(){return return_type;}

void Function::print(){
    cout << "Function(\"" + name + "\", [";
    for(const auto& arg : argv){
        arg->print();
    }
    cout << "], ";
    body->print();
    cout << ", " + to_string(return_type) + ")";
}

Object* Function::clone(){
    Function* copy;
    
    vector<Object*> argvCopy;
    for(const auto& arg : argv){
        Object* argCopy = arg->clone();
        argvCopy.push_back(argCopy);
    }
    
    copy = new Function(name,
                        argvCopy,
                        Safe_Cast<Seq*>(body->clone()),
                        return_type);
    
    return copy;
}

Unary::Unary(){
    Type = UNARY;
    val = nullptr;
}

Unary::~Unary(){
    if(val != nullptr){
        deleteObject(val);
        val = nullptr;
    }
}

Unary::Unary(Object* _val, Int32 _op){
    Type = UNARY;
    val = _val;
    operation = _op;
}

Object* Unary::getVal(){return val;}

Int32 Unary::getOperation(){return operation;}

void Unary::print(){
    cout << "Unary(";
    val->print();
    cout << ", " << operation << ")";
}

Object* Unary::clone(){
    Unary* copy = new Unary(val->clone(),
                            operation);
    return copy;
}

Seq::Seq(){
    Type = SEQ;
}

Seq::~Seq(){
    for(const auto &elem : stmt){
        if(elem != nullptr){
            deleteObject(elem);
        }
    }
}

Seq::Seq(vector<Object*> _seq){
    Type = SEQ;
    stmt = _seq;
}

vector<Object*> Seq::getStatements(){
    return stmt;
}

void Seq::print(){
    cout << "Seq(";
    for(const auto &s : stmt){
        s->print();
        cout << ", ";
    }
    cout << ")";
}

Object* Seq::clone(){
    Seq* copy;
    vector<Object*> stmtCopy;
    for(const auto& statement : stmt){
        Object* statementCopy = statement->clone();
        stmtCopy.push_back(statementCopy);
    }
    
    copy = new Seq(stmtCopy);
    
    return copy;
}

If::If(){
    Type        = IF;
}

If::~If(){
    for(const auto& stmt : ifStmt){
        deleteObject(get<0>(stmt));
        deleteObject(get<1>(stmt));
    }
    if(Else != nullptr){
        deleteObject(Else);
        Else = nullptr;
    }
}

If::If(vector<tuple<Object*, Seq*>> _ifStmt, Seq* _Else) : If() {
    ifStmt = _ifStmt;
    Else = _Else;
}

vector<tuple<Object*, Seq*>> If::getIfStmt(){
    return ifStmt;
}

Seq* If::getElse(){
    return Else;
}

void If::print(){
    cout << "If(";
    for(const auto& stmt : ifStmt){
        cout << "[";
        get<0>(stmt)->print();
        cout << ":";
        get<1>(stmt)->print();
        cout << "],";
    }
    cout << ", ";
    if(Else != nullptr){
        Else->print();
    }
    cout << ")";
}

Object* If::clone(){
    vector<tuple<Object*, Seq*>> copyStmt;
    for(const auto& stmt : ifStmt){
        tuple<Object*, Seq*> copyIfStmt =
        make_tuple((get<0>(stmt))->clone(),
                   Safe_Cast<Seq*>( (get<1>(stmt))->clone() ) );
        
        copyStmt.push_back(copyIfStmt);
    }
    If* copy = new If(copyStmt,
                      Safe_Cast<Seq*>(Else->clone()));
    return copy;
}

Logical::Logical(){
    Type = LOGICAL;
    left = nullptr;
    right = nullptr;
}

Logical::~Logical(){
    if(left != nullptr){
        deleteObject(left);
        left = nullptr;
    }
    if(right != nullptr){
        deleteObject(right);
        right = nullptr;
    }
}

Logical::Logical(Int32 o, Object* l, Object* r) : Logical() {
    operation = o;
    left = l;
    right = r;
}

Int32 Logical::getOperation(){
    return operation;
}

Object* Logical::getLeft(){
    return left;
}

Object* Logical::getRight(){
    return right;
}

void Logical::setLeft(Object* val){
    left = val;
}

void Logical::setRight(Object* val){
    right = val;
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

Object* Logical::clone(){
    Logical* copy = new Logical(operation,
                                left->clone(),
                                right->clone());
    return copy;
}

Assign::Assign(){
    Type = ASSIGN;
    name = nullptr;
    val = nullptr;
}

Assign::~Assign(){
    if(name != nullptr){
        deleteObject(name);
        name = nullptr;
    }
    if(val != nullptr){
        deleteObject(val);
        val = nullptr;
    }
}

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

void Assign::replaceVal(Object* nval){
    if(val != nullptr){
        deleteObject(val);
    }
    val = nval;
}

void Assign::print(){
    cout << "Assign(";
    name->print();
    cout << ", ";
    val->print();
    cout << ")";
}

Object* Assign::clone(){
    Assign* copy = new Assign(name->clone(), val->clone());
    return copy;
}

For::For(){
    Type = FOR;
    declare = nullptr;
    condition = nullptr;
    incrementor = nullptr;
    body = nullptr;
}

For::~For(){
    if(declare != nullptr){
        deleteObject(declare);
        declare = nullptr;
    }
    if(condition != nullptr){
        deleteObject(condition);
        condition = nullptr;
    }
    if(incrementor != nullptr){
        deleteObject(incrementor);
        incrementor = nullptr;
    }
    if(body != nullptr){
        deleteObject(body);
        body = nullptr;
    }
}

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

Object* For::clone(){
    For* copy = new For(declare->clone(),
                        condition->clone(),
                        incrementor->clone(),
                        Safe_Cast<Seq*>(body->clone()));
    return copy;
}

While::While(){
    Type = WHILE;
    condition = nullptr;
    body = nullptr;
}

While::While(Object* _condition, Seq* _body) : While() {
    LOG("Foundation:While");
    LOG("    condition: 0x"+(AddressOf(&_condition)));
    LOG("    body: 0x"+(AddressOf(&_body)));
    condition = _condition;
    body = _body;
}

While::~While(){
    if(condition != nullptr){
        deleteObject(condition);
    }
    
    if(body != nullptr){
        deleteObject(body);
    }
}

Object* While::getCondition(){
    return condition;
}

Seq* While::getBody(){
    return body;
}

void While::print(){
    cout << "While(";
    condition->print();
    cout << ", ";
    body->print();
    cout << ")";
}

Object* While::clone(){
    While* copy = new While(condition->clone(),
                            Safe_Cast<Seq*>(body->clone()));
    return copy;
}


string getTypeName(Int32 type){
    string translation[] = {
        "OBJECT",
        "VAR",
        "NUMBER",
        "BOOLEAN",
        "INTEGER",
        "FLOAT",
        "DOUBLE",
        "STRING",
        "VOID",
        "LET",
        "BINARY",
        "COMPARE",
        "PRINT",
        "FUNCTION",
        "UNARY",
        "SEQ",
        "IF",
        "LOGICAL",
        "ASSIGN",
        "FOR",
        "WHILE",
        "STACKLOC",
        "REG",
    };
    
    return translation[type];
}

void deleteObject(Object* o){
    UInt32 memoryPoolID = o->getMemoryPoolID();
    if(memoryPool[memoryPoolID] != nullptr){
        delete memoryPool[memoryPoolID];
        memoryPool[memoryPoolID] = nullptr;
    }
}


