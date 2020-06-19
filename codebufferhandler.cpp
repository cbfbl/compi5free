

#include "codebufferhandler.hpp"

void CodeBufferHandler::emitPrintFunctions() {

    cb_inst.emitGlobal("declare i32 @printf(i8*, ...)");
    cb_inst.emitGlobal("declare void @exit(i32)");
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
    allocateStack();
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
    storeRegToLocal("0",id->getGlobalOffset());
}

void CodeBufferHandler::ifStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getTrueList(),lb);
}

void CodeBufferHandler::elseStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getFalseList(),lb);
}

void CodeBufferHandler::expRel(Basictype *ret, Basictype *exp_l,
                                    Basictype* op,Basictype *exp_r) {
    string reg = reg_manager.getNextReg();
    string action = relConvert(op->getLexeme());
    string s1 = reg +  " = icmp " +  action + " i32 " + exp_l->getReg() +", " + exp_r->getReg();
    cb_inst.emit(s1);
    string s2 = "br i1 " + reg +  ", label @, label @";
    int ln = cb_inst.emit(s2);
    ret->makeList(true,pair<int,BranchLabelIndex>(ln,FIRST));
    ret->makeList(false,pair<int,BranchLabelIndex>(ln,SECOND));
}

string CodeBufferHandler::relConvert(const string& op_str){
    if (op_str == "<"){
        return "slt";
    }
    if (op_str == ">"){
        return "sgt";
    }
    if (op_str == "=="){
        return "eq";
    }
    if (op_str == "!="){
        return "ne";
    }
    if (op_str == "<="){
        return "sle";
    }
    if (op_str == ">="){
        return "sge";
    }
    return "Unknown relative operation";
}

void
CodeBufferHandler::expBinop(Basictype* ret,Basictype *exp_l, Basictype *op, Basictype *exp_r) {
    string div_add = "";
    if (op->getLexeme()=="/"){
        handleDiv(exp_r);
        div_add = (ret->getType()=="INT" ? "s" : "u");
    }
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
    s += reg + " = " + div_add + binConvert(op->getLexeme()) + type  + reg_l + ", " + reg_r;
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
        return "div";
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

void CodeBufferHandler::handleDiv(Basictype *exp_r) {
    string reg = exp_r->getReg();
    string chk_reg = reg_manager.getNextReg();
    string s1 = chk_reg + " = icmp eq "+ typeConvert(exp_r->getType())+ " 0, " + reg ;
    string s2 = "br i1 " + chk_reg + " label @,label @";
    cb_inst.emit(s1);
    int bp_loc1 = cb_inst.emit(s2);
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc1,FIRST)),lb);
    // TODO call print and exit
    cb_inst.emit("PRINT ERROR AND EXIT");
    lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc1,SECOND)),lb);
}

void CodeBufferHandler::expNot(Basictype* ret,Basictype *exp) {
    ret->mergeList(false,exp->getTrueList());
    ret->mergeList(true,exp->getFalseList());
}

void CodeBufferHandler::expAnd(Basictype* ret,Basictype* exp_l,Basictype* m, Basictype* exp_r) {
    cb_inst.bpatch(exp_l->getTrueList(),((Label*)m)->getLabel());
    ret->mergeList(true,exp_r->getTrueList());
    ret->mergeList(false,exp_l->getFalseList());
    ret->mergeList(false,exp_r->getFalseList());
}

void CodeBufferHandler::expAndStart(Basictype *exp_l) {

}

void CodeBufferHandler::expOrStart(Basictype *exp_l) {

}

void
CodeBufferHandler::expOr(Basictype *ret, Basictype *exp_l,Basictype* m, Basictype *exp_r) {
    cb_inst.bpatch(exp_l->getFalseList(),((Label*)m)->getLabel());
    ret->mergeList(true,exp_l->getTrueList());
    ret->mergeList(true,exp_r->getTrueList());
    ret->mergeList(false,exp_r->getFalseList());
}

string CodeBufferHandler::newLabel() {
    string lb = cb_inst.genLabel();
    return lb;
}

void CodeBufferHandler::ifEnd(Basictype *exp) {
    cb_inst.bpatch(exp->getFalseList(), cb_inst.genLabel());
}

void CodeBufferHandler::createBr(bool which_list,Basictype* ret) {
    int bp_loc = cb_inst.emit("br @");
    ret->mergeList(which_list,CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)));
}

void CodeBufferHandler::allocateStack() {
    cb_inst.emit("%stack = alloca [ 50 x i32 ] ");
}

string CodeBufferHandler::loadLocalToReg(int offset) {
    string reg = reg_manager.getNextReg();
    string s = reg + " = " + "getelementptr [50 x i32], [50 x i32]* %stack, i32 0, " + std::to_string(offset);
    cb_inst.emit(s);
    string out_reg = reg_manager.getNextReg();
    cb_inst.emit("load i32, i32* "+reg);
    return out_reg;
}

void CodeBufferHandler::storeRegToLocal(const string& reg,int offset) {
    string var_reg = reg_manager.getNextReg();
    string s = var_reg + " = " + "getelementptr [50 x i32], [50 x i32]* %stack, i32 0, i32 " + std::to_string(offset);
    cb_inst.emit(s);
    cb_inst.emit("store i32 " + reg + ", i32* " + var_reg);
}
