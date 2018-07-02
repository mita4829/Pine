#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "Foundation.hpp"

enum {
    /* Caller save */
    rax,
    rcx,
    rdx,
    r8,
    r9,
    r10,
    r11,
    /* Callee Save */
    rbx,
    rdi,
    rsi,
    r12,
    r13,
    r14,
    r15,
};

class Register {
private:
    set<int> callee_save_reg;
    set<int> caller_save_reg;
    string translateEnumIDToString(int);
public:
    Register();
    /* Returns a register name, volitile nature, enum id */
    string getRegister();
    void releaseRegister(int enumID);
    void releaseRegister(string reg);
};

#endif
