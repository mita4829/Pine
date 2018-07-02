#include "Lexer.hpp"

std::vector<std::vector<std::string>> Lexer::lexem;

void Lexer::lex(std::string file){
    std::regex spaces("\\s+");
    std::string stream = regex_replace(file, spaces, " ");
    std::vector<std::string> tokens;
    
    std::string ops[] =  {"==","!=","||","&&","<=",">=","->","<<",">>"};
    char sig[] =  {':','+','-','*','/','=','%','>','<','!','[',']',',','&','|','~'};
    
    int inScope = 0;
    bool inString = false;
    int l = 0;
    int r = 0;
    const int stream_length = stream.length();
    
    while(r < stream_length){
        /* Accept a non-empty token */
        if(stream[r] == ' ' && !inString){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            r += 1;
            l = r;
        }
        /* Accept token + ; */
        else if(stream[r] == ';' && !inString){
            if(stream.substr(l, r-l) != ""){
                tokens.push_back(stream.substr(l, r-l));
            }
            tokens.push_back(";");
            r += 1;
            l = r;
            if(!inScope){
                Lexer::lexem.push_back(tokens);
                tokens.clear();
            }
        }
        /* Strings */
        else if(stream[r] == '"'){
            if(inString){
                inString = false;
                tokens.push_back(stream.substr(l, r-l));
            }else{
                inString = true;
            }
            r += 1;
            l = r;
        }/*Paren matching*/
        else if(stream[r] == '(' && !inString){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("(");
            inScope += 1;
            r += 1;
            l = r;
        }else if(stream[r] == ')' && !inString){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back(")");
            inScope -= 1;
            r += 1;
            l = r;
        }
        /*Curry bracket matching*/
        else if(stream[r] == '{' && !inString){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("{");
            inScope += 1;
            r += 1;
            l = r;
        }else if(stream[r] == '}' && !inString){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("}");
            inScope -= 1;
            r += 1;
            l = r;
        }
        /*Double character tokens*/
        else if(!inString && in<std::string>(stream.substr(r,2),ops,sizeof(ops)/sizeof(ops[0]))){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back(stream.substr(r,2));
            r += 2;
            l = r;
        }
        /*Single character tokens*/
        else if(!inString && in<char>(stream[r],sig,sizeof(sig)/sizeof(sig[0]))){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back(stream.substr(r,1));
            r += 1;
            l = r;
        }
        else{
            r += 1;
        }
    }
}
