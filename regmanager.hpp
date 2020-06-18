

#ifndef COMPI_HW3_REGMANAGER_H
#define COMPI_HW3_REGMANAGER_H

#include <string>

using std::string;

class regmanager {
    int next_reg;
public :
    regmanager();
    string getNextReg();
};


#endif //COMPI_HW3_REGMANAGER_H
