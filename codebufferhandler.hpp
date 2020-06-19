

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
    string relConvert(const string& op_str);
    string binConvert(const string& op_str);
    string emitSxt(const string& reg);
    string emitTrunc(const string& reg);
    void handleDiv(Basictype* exp_r);
    void allocateStack();
    string loadLocalToReg(int offset);
    void storeRegToLocal(const string& reg,int offset);
public :
    CodeBufferHandler();
    string newLabel();
    void emitPrintFunctions();
    void printLocalBuffer();
    void printGlobalBuffer();
    void emitFunctionStart(Basictype* type_str,Basictype* id,Basictype* args_container);
    void emitFunctionEnd();
    void emitVariableDecl(Basictype* type,Basictype* id);
    void ifStart(Basictype* exp);
    void ifEnd(Basictype* exp);
    void elseStart(Basictype* exp);
    void expRel(Basictype* ret,Basictype* exp_l,Basictype* op,Basictype* exp_r);
    void expBinop(Basictype* ret,Basictype* exp_l,Basictype* op,Basictype* exp_r);
    void expNum(Basictype* num);
    void expNumB(Basictype* num);
    void expNot(Basictype* ret, Basictype* exp);
    void expAndStart(Basictype* exp_l);
    void expAnd(Basictype* ret,Basictype* exp_l,Basictype* m, Basictype* exp_r);
    void expOrStart(Basictype* exp_l);
    void expOr(Basictype* ret,Basictype* exp_l,Basictype* m,Basictype* exp_r);
    void createBr(bool which_list,Basictype* ret);
};


#endif //HW5_CODEBUFFERHANDLER_H
