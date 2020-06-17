

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
    void emitFunctionStart(const string& ret_type,const string& function_name,const vector<Basictype*>& args);
    void emitFunctionEnd();
    void emitVariableDecl(string type_str,string variable_name);
};


#endif //HW5_CODEBUFFERHANDLER_H
