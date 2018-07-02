#ifndef MEMORY_HPP
#define MEMORY_HPP
#include "Foundation.hpp"

/*  The Memory class stores variables and their values
    in a given function's stack frame through the usage of
    vectors and maps.
 
    This class follows the Singleton design pattern to prevent
    multiple instances during the interpretation step.
 */
class Memory {
private:
    static Memory* memory;
    Memory();
    ~Memory();
    /*  A vector of maps to mimic the represenation of the memory stack
        frame during execution.
     */
    std::vector<std::map<std::string, Object*>> funcStackFrame;
public:
    static Memory* getInstance();
    /*  set takes a string 'name' and maps it to an Object* 'o'
        and places it in the current stack frame.
     
        set exit and gives an error if 'name' already exists in the
        current stack frame.
     */
    void set(std::string name, Object* o);
    /*  get takes a string 'name' and returns the Object* it is
        mapped to in the current stack frame. If 'name' does not
        exist in the current stack frame, a nullptr will be returned.
     */
    Object* get(std::string name);
};

#endif
