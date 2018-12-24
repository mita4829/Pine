#include "Foundation.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#include "Compiler.hpp"

#include <signal.h>
#include <string.h>
using namespace std;

void segfault_sigaction(int signal, siginfo_t *si, void *arg){
    LOG("Main:segfault_sigaction");
    RaisePineException("Segmentation fault event.");
}

Int32 main(Int32 argc, Char** argv){
    try {
        LOG("Main:Main");
        struct sigaction sa;
        string pineSourceFile;
        Lexer lexer;
        
        memset(&sa, 0, sizeof(struct sigaction));
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = segfault_sigaction;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, NULL);
        
        if(argc < 2){
            RaisePineException("Expected Pine file.");
        }
        
        /* Lexer */
        pineSourceFile = argv[1];
        lexer.lex(pineSourceFile);
        
        /* Parser */
        Parser p = Parser(Lexer::lexem);
        p.parse();
        
        vector<Object*> ast = p.getAST();
        /* Compiler */
        vector<Object*> astCopy;
        for(auto& tree : ast){
            astCopy.push_back(tree->clone());
        }
        Compiler c = Compiler(astCopy);
        c.generateBinary();
        
        /* Dump log if debug flag is given */
        if(argc >= 3 && string(argv[argc-1]) == "-d"){
            RaisePineWarning("Breaking on runtime for debug mode.");
            throw 0;
        }
    } catch (...) {
        /* Log failure in the event of a main error */
        LOG("Main:Main");
        RaisePineException("Event in Main failed.");
    }
    return 0;
}

