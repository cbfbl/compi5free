

#include "regmanager.hpp"

regmanager::regmanager() :next_reg(0) {}

string regmanager::getNextReg() {
    string reg = "%"+std::to_string(next_reg);
    next_reg++;
    return reg;
}
