

#ifndef HW5_CODEBUFFERHANDLER_H
#define HW5_CODEBUFFERHANDLER_H

#include <string>
#include "bp.hpp"
#include "types.hpp"


class CodeBufferHandler {
    CodeBuffer& cb_inst;
    string typeConvert(const string& type_str);
public :
    CodeBufferHandler();
    void emitPrintFunctions();
    void printLocalBuffer();
    void printGlobalBuffer();
    void emitFunctionStart(Basictype* type_str,Basictype* id,Basictype* args_container);
    void emitFunctionEnd();
    void emitVariableDecl(Basictype* type,Basictype* id);
    void ifStart(Basictype* exp);
};


#endif //HW5_CODEBUFFERHANDLER_H
