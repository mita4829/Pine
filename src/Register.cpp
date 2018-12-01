
#include "Register.hpp"

Register::Register(){
    LOG("Register:Register");
    for(Int32 i = 0; i <= 6; i++){
        caller_save_reg.insert(i);
        callee_save_reg.insert(i + 7);
    }
    LOG("Register:-Register");
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
    LOG("Register:getRegister");
    if(caller_save_reg.size() == 0 && caller_save_reg.size() == 0){
        /* Out of registers, need to spill */
        return "";
    }
    if(callee_save_reg.size() > 0){
        Int32 id = *(callee_save_reg.begin());
        callee_save_reg.erase(id);
        return translateEnumIDToString(id);
    }
    Int32 id = *(caller_save_reg.begin());
    caller_save_reg.erase(id);
    LOG("Register:-getRegister");
    return translateEnumIDToString(id);
}

void Register::releaseRegister(Int32 id){
    LOG("Register:releaseRegister(Int32)");
    LOG("    id:"+STR(id));
    if(id <= 6){
        caller_save_reg.insert(id);
    }else{
        callee_save_reg.insert(id);
    }
    LOG("Register:-releaseRegister(Int32)");
}

void Register::releaseRegister(string reg){
    LOG("Register:releaseRegister(string)");
    LOG("    id:"+reg);
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
    for(Int32 i = 0; i < 14; i++){
        if(translationTable[i] == reg){
            releaseRegister(i);
            LOG("Register:-releaseRegister(string)");
            return;
        }
    }
    RaisePineWarning("Unknowing freeing of register "+reg);
}
