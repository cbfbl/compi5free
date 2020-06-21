

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
    string emitSxt(const string& reg,int from_bytes,int to_bytes);
    string emitTrunc(const string& reg,int from_bytes,int to_bytes);
    void handleDiv(Basictype* exp_r);
    void allocateStack();
    string loadLocalToReg(const string& reg_type,int offset);
    void storeRegToLocal(const string& reg_type,const string& reg,int offset);
    string getReg(Basictype* basic_type);
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
    void emitVariableDeclExp(Basictype* type,Basictype* id,Basictype* exp);
    void idAssignExp(Basictype* id,Basictype* exp);
    void expString(Basictype* ret);
};


#endif //HW5_CODEBUFFERHANDLER_H
