

#ifndef HW5_CODEBUFFERHANDLER_H
#define HW5_CODEBUFFERHANDLER_H

#include <string>
#include "bp.hpp"
#include "types.hpp"
#include "regmanager.hpp"


class CodeBufferHandler {
    CodeBuffer& cb_inst;
    regmanager reg_manager;
    string typeConvert(const string& type_str);
    string opConvert(const string& op_str);
    string binConvert(const string& op_str);
    string emitSxt(const string& reg);
    string emitTrunc(const string& reg);
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
    void expBinop(Basictype* ret,Basictype* exp_l,Basictype* op,Basictype* exp_r);
    void expNum(Basictype* num);
    void expNumB(Basictype* num);
};


#endif //HW5_CODEBUFFERHANDLER_H
