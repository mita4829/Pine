#include "Register.hpp"

Register::Register(){
    for(int i = 0; i <= 6; i++){
        caller_save_reg.insert(i);
        callee_save_reg.insert(i + 7);
    }
}

string Register::translateEnumIDToString(int id){
    string translationTable[] = {
        "rax",
        "rcx",
        "rdx",
        "r8",
        "r9",
        "r10",
        "r11",
        "rbx",
        "rdi",
        "rsi",
        "r12",
        "r13",
        "r14",
        "r15",
    };
    return translationTable[id];
}

string Register::getRegister(){
    if(caller_save_reg.size() == 0 && caller_save_reg.size() == 0){
        /* Out of registers, need to spill */
        return "";
    }
    if(callee_save_reg.size() > 0){
        int id = *(callee_save_reg.begin());
        callee_save_reg.erase(id);
        return translateEnumIDToString(id);
    }
    int id = *(caller_save_reg.begin());
    caller_save_reg.erase(id);
    return translateEnumIDToString(id);
}

void Register::releaseRegister(int id){
    if(id <= 6){
        caller_save_reg.insert(id);
    }else{
        callee_save_reg.insert(id);
    }
}

void Register::releaseRegister(string reg){
    string translationTable[] = {
        "rax",
        "rcx",
        "rdx",
        "r8",
        "r9",
        "r10",
        "r11",
        "rbx",
        "rdi",
        "rsi",
        "r12",
        "r13",
        "r14",
        "r15",
    };
    for(int i = 0; i < 14; i++){
        if(translationTable[i] == reg){
            releaseRegister(i);
            return;
        }
    }
    RaisePineWarning("Unknowing freeing of register "+reg);
}
