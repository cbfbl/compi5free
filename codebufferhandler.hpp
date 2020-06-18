

#ifndef HW5_CODEBUFFERHANDLER_H
#define HW5_CODEBUFFERHANDLER_H

#include <string>
#include "bp.hpp"
#include "types.hpp"
#include "regmanager.hpp"


class CodeBufferHandler {
    CodeBuffer& cb_inst;
    string typeConvert(const string& type_str);
    string opConvert(const string& op_str);
public :
    CodeBufferHandler();
    void emitPrintFunctions();
    void printLocalBuffer();
    void printGlobalBuffer();
    void emitFunctionStart(Basictype* type_str,Basictype* id,Basictype* args_container);
    void emitFunctionEnd();
    void emitVariableDecl(Basictype* type,Basictype* id);
    void ifStart(Basictype* exp);
    void elseStart(Basictype* exp);
    void expReleq(Basictype* ret,Basictype* exp_l,Basictype* op,Basictype* exp_r);
};


#endif //HW5_CODEBUFFERHANDLER_H
