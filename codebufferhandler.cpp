

#include "codebufferhandler.hpp"

void CodeBufferHandler::emitPrintFunctions() {

    cb_inst.emitGlobal("declare i32 @printf(i8*, ...)");
    cb_inst.emitGlobal(R"(@.int_specifier = global [4 x i8] c"%d\0A\00")");
    cb_inst.emitGlobal(R"(@.str_specifier = global [4 x i8] c"%s\0A\00")");
    cb_inst.emitGlobal("define void @printi(i32) {\n"
                           "call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8]," \
                           " [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)\n" \
                           "ret void\n" \
                           "}");
    cb_inst.emitGlobal("define void @print(i8*) {\n"
                       "call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8]," \
                           " [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)\n" \
                           "ret void\n" \
                           "}");
}

void CodeBufferHandler::printGlobalBuffer() {
    cb_inst.printGlobalBuffer();
}

void CodeBufferHandler::printLocalBuffer() {
    cb_inst.printCodeBuffer();
}

CodeBufferHandler::CodeBufferHandler() : cb_inst(CodeBuffer::instance())  {

}

void CodeBufferHandler::emitFunctionStart(Basictype* ret_type,Basictype* id,Basictype* args_container) {
    vector<Basictype*> args = ((Container*)args_container)->getVariables();
    string s = "define " + typeConvert(ret_type->getType()) + " @" + id->getLexeme() + "(";
    for (int i=0;i<args.size();i++){
        s+=typeConvert(args[i]->getType())+" "+args[i]->getLexeme();
        if (i!=args.size()-1){
            s+=", ";
        }
    }
    s += "){\n";
    cb_inst.emit(s);
}

void CodeBufferHandler::emitFunctionEnd(){
    cb_inst.emit("}");
}

string CodeBufferHandler::typeConvert(const string& type_str) {
    if (type_str == "INT" || type_str == "BYTE"){
        return "i32";
    }
    if (type_str == "BOOL"){
        return "i1";
    }
    if (type_str=="VOID"){
        return "void";
    }
}

void CodeBufferHandler::emitVariableDecl(Basictype* type,Basictype* id) {
    /*
    string s = "";
    s += "%" + id->getLexeme() + " = add " + typeConvert(type->getType()) + " 0, 0";
    cb_inst.emit(s);
     */
}

void CodeBufferHandler::ifStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getTrueList(),lb);
}

void CodeBufferHandler::elseStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getFalseList(),lb);
}

void CodeBufferHandler::expReleq(Basictype *ret, Basictype *exp_l,
                                    Basictype* op,Basictype *exp_r) {
    string s = "if " + exp_l->getReg() + + " " + opConvert(op->getLexeme()) + " " + exp_r->getReg();
    s += " goto label @, label @";
    int ln = cb_inst.emit(s);
    ret->makeList(true,pair<int,BranchLabelIndex>(ln,FIRST));
    ret->makeList(false,pair<int,BranchLabelIndex>(ln,SECOND));
}

string CodeBufferHandler::opConvert(const string& op_str){
    return "3";
}

void
CodeBufferHandler::expBinop(Basictype* ret,Basictype *exp_l, Basictype *op, Basictype *exp_r) {
    string s = "";
    string reg = "";
    string reg_l = exp_l->getReg();
    string reg_r = exp_r->getReg();
    if (exp_l->getType() == "BYTE"){
        reg_l = emitSxt(reg_l);
    }
    if (exp_r->getType() == "BYTE"){
        reg_r = emitSxt(reg_r);
    }
    string type = " " + typeConvert(ret->getType()) + " ";
    reg = reg_manager.getNextReg();
    s += reg + " = " +binConvert(op->getLexeme()) + type  + reg_l + ", " + reg_r;
    cb_inst.emit(s);
    if (ret->getType() == "BYTE"){
        reg = emitTrunc(reg);
    }
    ret->setReg(reg);
}

string CodeBufferHandler::binConvert(const string &op_str) {
    if (op_str=="+"){
        return "add";
    }
    if (op_str=="-"){
        return "sub";
    }
    if (op_str=="/"){
        return "sdiv";
    }
    if (op_str=="*"){
        return "mul";
    }
    return "err";
}

void CodeBufferHandler::expNum(Basictype *num) {
    num->setReg(num->getLexeme());
}

void CodeBufferHandler::expNumB(Basictype *num) {
    num->setReg(num->getLexeme());
}

string CodeBufferHandler::emitSxt(const string &reg) {
    string out_reg = reg_manager.getNextReg();
    string s = out_reg + " = sext i8 " + reg + " to i32" ;
    cb_inst.emit(s);
    return out_reg;
}

string CodeBufferHandler::emitTrunc(const string &reg) {
    string out_reg = reg_manager.getNextReg();
    string s = out_reg + " = trunc i32 " + reg + " to i8";
    cb_inst.emit(s);
    return out_reg;
}
