

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
        s+=typeConvert(args[i]->getType());
        if (i!=args.size()-1){
            s+=", ";
        }
    }
    s += "){\n";
    cb_inst.emit(s);
    allocateStack();
}

void CodeBufferHandler::emitFunctionEnd(Basictype* ret_type){
    if (ret_type->getType()=="VOID"){
        cb_inst.emit("ret void");
    }
    cb_inst.emit("}");
}

string CodeBufferHandler::typeConvert(const string& type_str) {
    if (type_str == "INT"){
        return "i32";
    }
    if (type_str=="BYTE"){
        return "i8";
    }
    if (type_str == "BOOL"){
        return "i1";
    }
    if (type_str=="VOID"){
        return "void";
    }
    if (type_str=="STRING"){
        return "i8*";
    }
}

void CodeBufferHandler::emitVariableDecl(Basictype* type,Basictype* id) {
    storeRegToLocal(type->getType(),"0",id->getGlobalOffset());
}

void CodeBufferHandler::ifStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getTrueList(),lb);
}

void CodeBufferHandler::elseStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getFalseList(),lb);
}

void CodeBufferHandler::ifMiddle(Basictype* ret,Basictype *exp) {
    //int bp_loc = cb_inst.emit("br label @");
    //ret->mergeList(false,CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)));
    ret->mergeList(false,exp->getFalseList());
}

void CodeBufferHandler::ifEnd(Basictype* bp_var) {
    int bp_loc = cb_inst.emit("br label @");
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),lb);
    cb_inst.bpatch(bp_var->getFalseList(),lb);
}

void CodeBufferHandler::ifEnd() {
    int bp_loc = cb_inst.emit("br label @");
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),lb);
}

void CodeBufferHandler::ifElse(Basictype *if_start,Basictype* m) {
    string lb = ((Label*)m)->getLabel();
    //if_start->mergeList(false,CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)));
    cb_inst.bpatch(if_start->getFalseList(),lb);
}

void CodeBufferHandler::ifElseEnd(Basictype *bp_var) {
    int bp_loc = cb_inst.emit("br label @");
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),lb);
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(((Goto*)bp_var)->getGotoLoc(),FIRST)),lb);
}

void CodeBufferHandler::expRel(Basictype *ret, Basictype *exp_l,
                                    Basictype* op,Basictype *exp_r) {
    string reg_l = getReg(exp_l);
    string reg_r = getReg(exp_r);
    if (exp_l->getType()=="BYTE"){
        reg_l = emitSxt(reg_l,8,32);
    }
    if (exp_r->getType()=="BYTE"){
        reg_r = emitSxt(reg_r,8,32);
    }
    string reg = reg_manager.getNextReg();
    string action = relConvert(op->getLexeme());
    string s1 = reg +  " = icmp " +  action + " i32 " + reg_l +", " + reg_r;
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
    string s = "";
    string reg = "";
    if (op->getLexeme()=="/"){
        handleDiv(exp_r);
        div_add = (ret->getType()=="INT" ? "s" : "u");
    }
    string reg_l = getReg(exp_l);
    string reg_r = getReg(exp_r);
    if (exp_l->getType() == "BYTE"){
        reg_l = emitSxt(reg_l,8,32);
    }
    if (exp_r->getType() == "BYTE"){
        reg_r = emitSxt(reg_r,8,32);
    }
    string type = " i32 ";
    reg = reg_manager.getNextReg();
    s += reg + " = " + div_add + binConvert(op->getLexeme()) + type  + reg_l + ", " + reg_r;
    cb_inst.emit(s);
    if (ret->getType() == "BYTE"){
        reg = emitTrunc(reg,32,8);
    }
    ret->setReg(reg);
}

string CodeBufferHandler::binConvert(const string &op_str) { //TODO unsigned and signed?
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

string CodeBufferHandler::emitSxt(const string &reg,int from_bytes,int to_bytes) {
    string out_reg = reg_manager.getNextReg();
    string from_str = "i"+std::to_string(from_bytes);
    string to_str = "i"+std::to_string(to_bytes);
    string s = out_reg + " = sext " + from_str + " " + reg + " to " + to_str;
    cb_inst.emit(s);
    return out_reg;
}

string CodeBufferHandler::emitTrunc(const string &reg,int from_bytes,int to_bytes) {
    string out_reg = reg_manager.getNextReg();
    string from_str = "i"+std::to_string(from_bytes);
    string to_str = "i"+std::to_string(to_bytes);
    string s = out_reg + " = trunc " + from_str + " " + reg + " to " + to_str;
    cb_inst.emit(s);
    return out_reg;
}

void CodeBufferHandler::handleDiv(Basictype *exp_r) {
    string reg = getReg(exp_r);
    string chk_reg = reg_manager.getNextReg();
    string s1 = chk_reg + " = icmp eq "+ typeConvert(exp_r->getType())+ " 0, " + reg ;
    string s2 = "br i1 " + chk_reg + ", label @,label @";
    cb_inst.emit(s1);
    int bp_loc1 = cb_inst.emit(s2);
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc1,FIRST)),lb);
    divError(lb);
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

void CodeBufferHandler::createBr(bool which_list,Basictype* ret) {
    int bp_loc = cb_inst.emit("br label @");
    ret->mergeList(which_list,CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)));
}

void CodeBufferHandler::allocateStack() {
    cb_inst.emit("%stack = alloca [ 50 x i32 ] ");
}

string CodeBufferHandler::loadLocalToReg(const string& reg_type,int offset) {
    string reg = reg_manager.getNextReg();
    string out_reg = reg_manager.getNextReg();
    string s = reg + " = " + "getelementptr [50 x i32], [50 x i32]* %stack, i32 0, i32 " + std::to_string(offset);
    cb_inst.emit(s);
    cb_inst.emit(out_reg + " = load i32, i32* "+reg);
    if (reg_type == "BYTE"){
        out_reg = emitTrunc(out_reg,32,8);
    }
    else if (reg_type == "BOOL"){
        out_reg = emitTrunc(out_reg,32,1);
    }
    return out_reg;
}

void CodeBufferHandler::storeRegToLocal(const string& reg_type,const string& reg,int offset) {
    string out_reg = reg;
    if (reg_type == "BYTE"){
        out_reg = emitSxt(reg,8,32);
    }
    else if (reg_type == "BOOL"){
        out_reg = emitSxt(reg,1,32);
    }
    string var_reg = reg_manager.getNextReg();
    string s = var_reg + " = " + "getelementptr [50 x i32], [50 x i32]* %stack, i32 0, i32 " + std::to_string(offset);
    cb_inst.emit(s);
    cb_inst.emit("store i32 " + out_reg + ", i32* " + var_reg);
}

void CodeBufferHandler::emitVariableDeclExp(Basictype* type,Basictype* id,Basictype* exp) {
    string reg = "";
    if (type->getType()=="BOOL"){
        reg = handleBoolHelper(exp);
        storeRegToLocal("INT",reg,id->getGlobalOffset());
        return;
    }
    reg = getReg(exp);
    storeRegToLocal(exp->getType(),reg,id->getGlobalOffset());
}

string CodeBufferHandler::getReg(Basictype *basic_type) {
    string reg = basic_type->getReg();
    if (reg == "&" && basic_type->getGlobalOffset() >= 0){
        reg = loadLocalToReg(basic_type->getType(),basic_type->getGlobalOffset());
    }
    else if (reg == "&" && basic_type->getGlobalOffset() < 0){
        reg = "%" + std::to_string(-1*(basic_type->getGlobalOffset() + 1));
    }
    else if (basic_type->getType()=="STRING"){
        string str_size = "[" + std::to_string(basic_type->getLexeme().length()-1)+" x i8]";
        reg = "getelementptr("+str_size+" , "+str_size+"* "+reg+", i32 0, i32 0)";
    }
    return reg;
}

void CodeBufferHandler::idAssignExp(Basictype *id, Basictype *exp) {
    storeRegToLocal(exp->getType(),getReg(exp),id->getGlobalOffset());
}

void CodeBufferHandler::expString(Basictype *ret) {
    // @.int_specifier = global [4 x i8] c"%d\0A\00"
    string lexeme = ret->getLexeme();
    string str_len = std::to_string(ret->getLexeme().length()-1);
    string reg = reg_manager.getNextGlobal();
    string new_str = lexeme.substr(0,lexeme.length()-1);
    string s = reg + " = global ["+str_len;
    s+= " x i8] c"+new_str+R"(\00")";
    ret->setReg(reg);
    cb_inst.emitGlobal(s);
}

string
CodeBufferHandler::emitPhi(vector<string> values, vector<string> labels) {
    //  %indvar = phi i32 [ 0, %LoopHeader ], [ %nextindvar, %Loop ]
    string reg = reg_manager.getNextReg();
    string s = reg + " = phi i32 ";
    for (int i = 0 ; i < values.size() ; i++){
        s+= "[ " + values[i] +", " + labels[i] +" ]";
        if (i != values.size() - 1){
            s+= ", ";
        }
    }
    cb_inst.emit(s);
    return reg;
}

string CodeBufferHandler::handleBoolHelper(Basictype* exp) {
    // br i1 <cond>, label <iftrue>, label <iffalse>
    string true_label = cb_inst.genLabel();
    int bp_loc1 = cb_inst.emit("br label @");
    string false_label = cb_inst.genLabel();
    int bp_loc2 = cb_inst.emit("br label @");
    string jmp_at_end = cb_inst.genLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc1,FIRST)),jmp_at_end);
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc2,FIRST)),jmp_at_end);
    cb_inst.bpatch(exp->getTrueList(),true_label);
    cb_inst.bpatch(exp->getFalseList(),false_label);
    vector<string> true_false;
    true_false.emplace_back("1");
    true_false.emplace_back("0");
    vector<string> labels_vec;
    labels_vec.emplace_back("%"+true_label);
    labels_vec.emplace_back("%"+false_label);
    return emitPhi(true_false,labels_vec);
}

void CodeBufferHandler::expId(Basictype *ret) {
    if (ret->getType()=="BOOL"){
        int stack_helper_loc = cb_inst.emit("br i1 "+getReg(ret)+", label @, label @");
        ret->mergeList(true,CodeBuffer::makelist(pair<int,BranchLabelIndex>(stack_helper_loc,FIRST)));
        ret->mergeList(false,CodeBuffer::makelist(pair<int,BranchLabelIndex>(stack_helper_loc,SECOND)));
    }
}

void CodeBufferHandler::whileStart(Basictype *exp,Basictype* m_while_start) {
    break_positions.push_back(vector<int>());
    string loop_label = ((Label*)m_while_start)->getLabel();
    while_labels.push_back(loop_label);
    cb_inst.bpatch(exp->getTrueList(),loop_label);
}

void CodeBufferHandler::whileEnd(Basictype* to_bp) {
    string lb = breakControl();
    cb_inst.bpatch(to_bp->getFalseList(),lb);
    break_positions.pop_back();
    while_labels.pop_back();
}

Basictype* CodeBufferHandler::emitMWhile() {
    int bp_loc = cb_inst.emit("br label @");
    Label* ret = new Label(newLabel());
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),ret->getLabel());
    return ret;
}

void CodeBufferHandler::whileEnd(Basictype* while_start,Basictype *n_goto) {
    int bp_loc = ((Goto*)n_goto)->getGotoLoc();
    n_goto->mergeList(false,CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)));
    //string lb = cb_inst.genLabel();
    //cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),lb);
    //cb_inst.bpatch(while_start->getFalseList(),lb);
    whileEnd(n_goto);
}

void CodeBufferHandler::whileElseEnd(Basictype *while_start,Basictype* m) {
    string lb = ((Label*)m)->getLabel();
    cb_inst.bpatch(while_start->getFalseList(),lb);
}

void CodeBufferHandler::whileMiddle(Basictype* ret,Basictype * n_goto,Basictype* exp,Basictype* before_exp) {
    int bp_loc = ((Goto*)n_goto)->getGotoLoc();
    string while_start = ((Label*)before_exp)->getLabel();
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),while_start);
    ret->mergeList(false,exp->getFalseList());
    //string lb = cb_inst.genLabel();
    //cb_inst.bpatch(exp->getFalseList(),lb);

}

string CodeBufferHandler::breakControl() {
    vector<int> break_branches = break_positions.back();
    int bp_loc = cb_inst.emit("br label @");
    string lb = cb_inst.genLabel();
    for (int break_loc : break_branches){
        cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(break_loc,FIRST)),lb);
    }
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),lb);
    break_branches.pop_back();
    return lb;
}


int CodeBufferHandler::emitGoto() {
    int bp_loc = cb_inst.emit("br label @");
    return bp_loc;
}

void CodeBufferHandler::stmBreak() {
    int bp_loc = cb_inst.emit("br label @");
    break_positions.back().push_back(bp_loc);
}

void CodeBufferHandler::stmContinue() {
    int bp_loc = cb_inst.emit("br label @");
    cb_inst.bpatch(CodeBuffer::makelist(pair<int,BranchLabelIndex>(bp_loc,FIRST)),while_labels.back());
}

void CodeBufferHandler::emitRet(Basictype *exp) {
    string s = "ret ";
    if (exp == NULL){
        s += "void";
    }
    else {
        s += typeConvert(exp->getType()) + " " + getReg(exp);
    }
    cb_inst.emit(s);
}

void CodeBufferHandler::emitCall(Basictype* ret,Basictype *id, Basictype *explist) {
    string s1 = "";
    string ret_reg = "";
    string s2 = "call " + typeConvert(ret->getType())+ " @" + id->getLexeme()+"(";
    vector<Basictype*> simple_bool_vec;
    if (explist != NULL) {
        vector<Basictype *> args = ((Container *) explist)->getVariables();
        vector<pair<string, string>> type_and_reg_vec;
        for (Basictype *arg : args) {
            string llvm_type = typeConvert(arg->getType());
            string llvm_reg = getReg(arg);
            type_and_reg_vec.push_back(
                    pair<string, string>(llvm_type, llvm_reg));
            if (llvm_reg=="true" || llvm_reg=="false"){
                simple_bool_vec.push_back(arg);
            }
        }
        for (int i = 0; i < type_and_reg_vec.size(); i++) {
            s2 += type_and_reg_vec[i].first + " " + type_and_reg_vec[i].second;
            if (i != type_and_reg_vec.size() - 1) {
                s2 += ", ";
            }
        }

    }
    if (ret->getType() != "VOID"){
        ret_reg = reg_manager.getNextReg();
        s1 += ret_reg+ " = ";
        ret->setReg(ret_reg);
    }
    s1 += s2 + ")";

    if (!simple_bool_vec.empty()) {
        string lb = cb_inst.genLabel();
        for (Basictype* arg : simple_bool_vec){
            if (getReg(arg)=="true"){
                cb_inst.bpatch(arg->getTrueList(),lb);
            }
            else {
                cb_inst.bpatch(arg->getFalseList(),lb);
            }
        }
    }
    cb_inst.emit(s1);
}

void CodeBufferHandler::divError(string lb) {
    string err_str = R"(c"Error division by zero\00")";
    cb_inst.emitGlobal("@.div_error = global [23 x i8] "+err_str);
    cb_inst.emit("call void @print(i8* getelementptr([23 x i8] , [23 x i8]* @.div_error, i32 0, i32 0))");
    cb_inst.emit("call void @exit(i32 0)");
    cb_inst.emit("br label %"+lb);
}








