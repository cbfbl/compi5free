

#include "codebufferhandler.hpp"

void CodeBufferHandler::emitPrintFunctions() {

    cb_inst.emitGlobal("declare i32 @printf(i8*, ...)");
    cb_inst.emitGlobal(R"(@.int_specifier = global [4 x i8] c"12\0A\00")");
    cb_inst.emitGlobal("define void @printi(i32) {\n"
                           "call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8]," \
                           " [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)\n" \
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
    string s = "";
    s += "%" + id->getLexeme() + " = add " + typeConvert(type->getType()) + " 0, 0";
    cb_inst.emit(s);
}

void CodeBufferHandler::ifStart(Basictype *exp) {
    string lb = cb_inst.genLabel();
    cb_inst.bpatch(exp->getTrueList(),lb);
}

