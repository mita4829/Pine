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
    set<Int32> callee_save_reg;
    set<Int32> caller_save_reg;
    string translateEnumIDToString(Int32);
public:
    Register();
    /* Returns a register name, volitile nature, enum id */
    string getRegister();
    void releaseRegister(Int32 enumID);
    void releaseRegister(string reg);
};

#endif

