#include "Lexer.hpp"

vector<vector<string>> Lexer::lexem;

/*
 lex
 Routine
    Lexes a Pine source file into lexems
 Parameters
    file: The file name of a Pine source file
 Return:
    None
 */
void Lexer::lex(string filename){
    LOG("Lexer:lex("+filename+")");
    ifstream ifs(filename);
    string sourceCode((istreambuf_iterator<Char>(ifs)),
                      (istreambuf_iterator<Char>()));
    ifs.close();
    
    regex spaces("\\s+");
    string stream = regex_replace(sourceCode, spaces, " ");
    vector<string> tokens;
    
    string ops_token[] =  {"==","!=","||","&&","<=",">=","->","<<",">>"};
    Char   sig_token[] =  {':','+','-','*','/','=','%','>','<','!','[',']',',','&','|','~'};
    
    set<string> ops(begin(ops_token), end(ops_token));
    set<Char>   sig(begin(sig_token), end(sig_token));
    
    UInt32   inScope = 0;
    bool    inString = false;
    bool   inComment = false;
    UInt32         l = 0;
    UInt32         r = 0;
    const UInt32 stream_length = stream.length();
    
    while(r < stream_length){
        /* Accept a non-empty token */
        if(stream[r] == ' ' && !inString && !inComment){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            r += 1;
            l = r;
        }
        /* Accept token + ; */
        else if(stream[r] == ';' && !inString && !inComment){
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
        else if(stream[r] == '"' && !inComment){
            if(inString == false){
                inString = true;
                l = r;
                r += 1;
            }else{
                inString = false;
                tokens.push_back(stream.substr(l, r-l+1));
                r += 1;
                l = r;
            }
        }/*Paren matching*/
        else if(stream[r] == '(' && !inString && !inComment){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("(");
            inScope += 1;
            r += 1;
            l = r;
        }else if(stream[r] == ')' && !inString && !inComment){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back(")");
            inScope -= 1;
            r += 1;
            l = r;
        }
        /*Curry bracket matching*/
        else if(stream[r] == '{' && !inString && !inComment){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("{");
            inScope += 1;
            r += 1;
            l = r;
        }else if(stream[r] == '}' && !inString && !inComment){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back("}");
            inScope -= 1;
            r += 1;
            l = r;
        }
        /* Comment */
        else if(!inString &&
                 (stream.substr(r,2) == "/*" ||
                  stream.substr(r,2) == "*/"))
        {
            if(stream.substr(r,2) == "/*"){
                inComment = true;
                r += 1;
            }else{
                if(!inComment){
                    /* There could be a case where the source contains
                       an end comment token, which is ignore, but still
                       invalid in syntax. Thrown an exception.
                     */
                    RaisePineException("Unmatching */ token found.");
                }
                r += 2;
                l = r;
                inComment = false;
            }
        }
        /*Double character tokens*/
        else if(!inString && !inComment && ops.find(stream.substr(r,2)) != ops.end()){
            if(stream.substr(l,r-l) != ""){
                tokens.push_back(stream.substr(l,r-l));
            }
            tokens.push_back(stream.substr(r,2));
            r += 2;
            l = r;
        }
        /*Single character tokens*/
        else if(!inString && !inComment && sig.find(stream[r]) != sig.end()){
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
