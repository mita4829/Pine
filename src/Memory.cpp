#include "Memory.hpp"

Memory* Memory::memory = nullptr;

Memory::Memory(){}
Memory::~Memory(){}
Memory* Memory::getInstance(){
    if(!memory){
        memory = new Memory();
    }
    return memory;
}
void Memory::set(std::string name, Object* o){
    
    if(funcStackFrame.empty()){
        std::map<std::string, Object*> m;
        funcStackFrame.push_back(m);
    }
    std::map<std::string, Object*>* frame = &(funcStackFrame.back());
    if(frame->count(name) > 0){
        RaisePineException(std::string("Redeclaration of the same variable ") + "\e[1;33m" + name + "\e[1;33m" + "\e[0m");
        return;
    }
    (*frame)[name] = o;
}

Object* Memory::get(std::string name){
    if(funcStackFrame.empty()){
        return nullptr;
    }
    std::map<std::string, Object*> frame = funcStackFrame.back();
    if(frame.count(name) > 0){
        return frame[name];
    }

    return nullptr;
}
