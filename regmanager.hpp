

#ifndef COMPI_HW3_REGMANAGER_H
#define COMPI_HW3_REGMANAGER_H

#include <string>

using std::string;

class regmanager {
    int next_reg;
    int next_global;
public :
    regmanager();
    string getNextReg();
    string getNextGlobal();
};


#endif //COMPI_HW3_REGMANAGER_H
