

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

void CodeBufferHandler::emitFunctionStart(const string& ret_type,const string& function_name,const vector<Basictype*>& args) {
    string s = "define " + typeConvert(ret_type) + " @" + function_name + "(";
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
        return "i8";
    }
    if (type_str == "BOOL"){
        return "i1";
    }
    if (type_str=="VOID"){
        return "void";
    }
}

void CodeBufferHandler::emitVariableDecl(string type_str,string variable_name) {
    string s = "";
    s += "%" + variable_name + " = add i32 0, 0";
    cb_inst.emit(s);
}

