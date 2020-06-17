#include "types.hpp"

Basictype::Basictype(const char *yytext) : lexeme(yytext) {
    if (lexeme == "int") {
        setType("INT");
    } else if (lexeme == "bool") {
        setType("BOOL");
    } else if (lexeme == "b" || lexeme == "byte") {
        setType("BYTE");
    } else if (lexeme == "void") {
        setType("VOID");
    } else {
        setType("UNKNOWN");
    }
}

Basictype::Basictype() {};

string &Basictype::getLexeme() { return lexeme; }

int Basictype::getGlobalOffset() { return global_offset; }

int Basictype::getLocalOffset() { return local_offset; }

void Basictype::setGlobalOffset(int global_set) { global_offset = global_set; }

void Basictype::setLocalOffset(int local_set) { local_offset = local_set; }

string &Basictype::getType() { return type; }

void Basictype::setType(string type_set) { type = type_set; }

Num::Num(const char *yytext)
        : Basictype(yytext), lexeme_value(std::stoi(string(yytext))) {
    this->setType("INT OR BYTE");
}

int Num::getVal() {
    return lexeme_value;
}

Id::Id(const char *yytext) : Basictype(yytext), name(yytext) {}

string Id::getName() { return name; }

Bool::Bool(const char *yytext) : Basictype(yytext) {
    if (this->getLexeme() == "true") {
        bool_val = true;
    } else if (this->getLexeme() == "false") {
        bool_val = false;
    }
    this->setType("BOOL");
}

Container::Container(const char *yytext) : Basictype(yytext) {}

vector<Basictype *> Container::getVariables() { return variables_list; }

void Container::addVariable(Basictype *var) { variables_list.push_back(var); }

void Container::addVariableToFront(Basictype *var) {
    variables_list.insert(variables_list.begin(), var);
}

vector<string> Container::getVariablesTypes() {
    vector<string> out_vec;
    for (auto basic_type : variables_list) {
        out_vec.push_back(basic_type->getType());
    }
    return out_vec;
}

Function::Function(const char *yytext) : Container(yytext) { setType("FUNC"); }

Function::Function(string yytext) : Container(yytext.c_str()) {
    setType("FUNC");
}

void Function::setRetType(string set_ret_type) { ret_type = set_ret_type; }

string Function::getFunctionType() { return function_type; }

void Function::setFunctionType(string set_func_type) {
    function_type = set_func_type;
}

string Function::getRetType() { return ret_type; }

String::String(const char *yytext) : Basictype(yytext) {
    this->setType("STRING");
}