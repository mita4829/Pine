#include "Foundation.hpp"
#include "Interpreter.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Compiler.hpp"
#include <fstream>

using namespace std;

void print(vector<string> v){
    cout << "[";
    for(int i = 0; i<v.size(); i++){
        cout << v[i] << ", ";
    }
    cout << "]" << endl;
}
void print_v(vector<vector<string>> v){
    cout << "[";
    for(int i = 0; i<v.size(); i++){
        cout << "[";
        vector<string> w = v[i];
        for(int i = 0; i<w.size(); i++){
            cout << w[i] << ", ";
        }
        cout << "], ";
    }
    cout << "]" << endl;
}

void printAST(vector<Object*> ast){
    for(int i = 0; i<ast.size(); i++){
        ast[i]->print();
    }
    cout << endl;
}

int main(int argc, char** argv){
    if(argc < 2){
        RaisePineException("Expected a Pine source file.");
    }
    string filename = argv[1];
    std::ifstream ifs(filename);
    std::string sourceCode( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );
    ifs.close();
   
    
    
    
    //DEBUG("Source:");
    //cout << sourceCode;
    Lexer::lex(sourceCode);
    //DEBUG("Lexems:");
    //print_v(Lexer::lexem);
    Parser p = Parser(Lexer::lexem);
    p.parse();
    vector<Object*> ast = p.getAST();
    //DEBUG("AST:");
    //printAST(ast);
    //DEBUG("Flatten:");
    Compiler c = Compiler(ast);
    for(Object* tree : ast){
        c.flatten(tree);
    }
    vector<Object*> flat_ast = c.popFlatStack();
//    for(Object* tree : flat_ast){
//        tree->print();
//        cout << endl;
//    }
//    DEBUG("Compile:");
    for(Object* tree : flat_ast){
        c.compile(tree);
    }
    vector<string> assmcode = c.getAssembly();
    string output = "";
    for(string s : assmcode){
        char first = s[0];
        string tab = "\t";
        if(first != '.' && !(first >= 'A' && first <= 'Z')){
            s = tab + s;
        }
        output += (s + "\n");
    }
    ofstream asmFile;
    asmFile.open("Pine.s");
    asmFile << output;
    asmFile.close();
    system("gcc -std=c11 -o a.out Pine.s PineRuntime.s");
}


