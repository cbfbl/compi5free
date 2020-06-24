

#include "regmanager.hpp"

RegManager::RegManager() :next_reg(0) {}

string RegManager::getNextReg() {
    string reg = "%t"+std::to_string(next_reg);
    next_reg++;
    return reg;
}

string RegManager::getNextGlobal() {
    string global = "@.g"+std::to_string(next_global);
    next_global++;
    return global;
}
