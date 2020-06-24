#include "handler.hpp"

#define THE_ANSWER 42

Basictype *Handler::handleRule(int rule_number, vector<Basictype *> params) {
    switch (rule_number) {
        case 1:
            finalize();
            break;
        case 4:
            handleFunctionDeclartion(params[0], params[1], params[2]);
            break;
        case 5:
            return handleRettypeType(params[0]);
        case 6:
            return handleRettypeVoid(params[0]);
        case 7:
            return handleFormalsEpsilon();
        case 8:
            return handleFormalsFormalList(params[0]);
        case 9:
            return handleFormalDecl(params[0]);
        case 10:
            return handleFormalDeclFormalList(params[0], params[1]);
        case 11:
            return handleFormalDeclTypeId(params[0], params[1]);
        case 14:
            removeScope();
            break;
        case 15:
            handleStatmentTypeId(params[0], params[1]);
            break;
        case 16:
            handleStatmentTypeIdAssignExp(params[0], params[1], params[2]);
            break;
        case 17:
            return handleIdAssignExp(params[0], params[1]);
        case 19:
            handleReturnVoid();
            break;
        case 20:
            handleReturnWithType(params[0]);
            break;
        case 23:
        case 24:
            handleWhileEnd();
            break;
        case 25:
            handleBreak();
            break;
        case 26:
            handleContinue();
            break;
        case 27:
            return handleCallWithParams(params[0], params[1]);
        case 28:
            return handleCallNoParams(params[0]);
        case 29:
            return handleExplistExp(params[0]);
        case 30:
            return handleExpExplist(params[0], params[1]);
        case 31:
            return handleInt(params[0]);
        case 32:
            return handleByte(params[0]);
        case 33:
            return handleBool(params[0]);
        case 34:
            return handleBracketExpBracket(params[0]);
        case 35:
            return handleExpBinopH(params[0], params[1]);
        case 36:
            return handleExpBinopL(params[0], params[1]);
        case 37:
            return handleId(params[0]);
        case 38:
            return handleCallToExp(params[0]);
        case 39:
            return handleNum(params[0]);
        case 40:
            return handleNumB(params[0]);
        case 41:
            return handleString(params[0]);
        case THE_ANSWER:
            return handleTrue(params[0]);
        case 43:
            return handleFalse(params[0]);
        case 44:
            return handleNotExp(params[0]);
        case 45:
            return handleExpAndExp(params[0], params[1]);
        case 46:
            return handleExpOrExp(params[0], params[1]);
        case 47:
            return handleExpRelopExp(params[0], params[1]);
        case 48:
            return handleExpReleqExp(params[0], params[1]);
        case 49:
            return handleIfStart(params[0]);
        case 50:
            return handleWhileStart(params[0]);
        default:
            break;
    }
    return NULL;
}

Handler::Handler() : expected_ret_type("NONE") {}

void Handler::initialize() {
    offset_stack.initialize();
    symbol_table.insertScope();
    current_while_count = 0;
    was_main_defined = false;
    insertPrintFunctions();

}

void Handler::insertPrintFunctions() {
    Function *print = new Function("print");
    Function *printi = new Function("printi");
    vector<string> print_params{"STRING"};
    vector<string> printi_params{"INT"};
    String *print_variable = new String("");
    Num *printi_variable = new Num("0");
    printi_variable->setType("INT");
    print->addVariable(print_variable);
    printi->addVariable(printi_variable);
    string print_type = output::makeFunctionType("VOID", print_params);
    string printi_type = output::makeFunctionType("VOID", printi_params);
    print->setFunctionType(print_type);
    printi->setFunctionType(printi_type);
    print->setRetType("VOID");
    printi->setRetType("VOID");
    symbol_table.insertItem(print);
    symbol_table.insertItem(printi);
}

void Handler::removeScope() {
    /*
    output::endScope();
    vector<Basictype *> last_scope = symbol_table.getLastScopeData();
    for (Basictype *basic_p : last_scope) {
        if (basic_p->getType() == "FUNC") {
            int item_scope = symbol_table.getItemLocation(basic_p->getLexeme()).first;
            if (item_scope != 0){
                continue;
            }
            output::printID(basic_p->getLexeme(), basic_p->getGlobalOffset(),
                            ((Function *) basic_p)->getFunctionType());
        } else {
            output::printID(basic_p->getLexeme(), basic_p->getGlobalOffset(),
                            basic_p->getType());
        }
    }
    */
    symbol_table.removeScope();
    offset_stack.removeLastItem();
}

void Handler::insertScope() {
    symbol_table.insertScope();
    offset_stack.duplicateLastItem();
}

void Handler::checkDefined(Basictype *id) {
    if (symbol_table.exists(id->getLexeme())) {
        output::errorDef(yylineno, id->getLexeme());
        exit(0);
    }
}

void Handler::setExpectedRetType(string ret_type) {
    expected_ret_type = ret_type;
}

// rule 1
void Handler::finalize() {
    if (!was_main_defined) {
        output::errorMainMissing();
        exit(0);
    }
    removeScope();
}

// rule 4
void Handler::handleFunctionDeclartion(Basictype *ret_type, Basictype *id,
                                       Basictype *args) {
    setExpectedRetType("NONE");
    removeScope();
    handleFunctionStart(ret_type,id,args);
}

// rule 5
Basictype *Handler::handleRettypeType(Basictype *type) { return type; }

// rule 6
Basictype *Handler::handleRettypeVoid(Basictype *void_type) {
    return void_type;
}

// rule 7
Basictype *Handler::handleFormalsEpsilon() { return new Container("epsilon"); }

// rule 8
Basictype *Handler::handleFormalsFormalList(Basictype *formals) {
    return formals;
}

// rule 9
Basictype *Handler::handleFormalDecl(Basictype *formal_decl) {
    return formal_decl;
}

// rule 10
Basictype *Handler::handleFormalDeclFormalList(Basictype *formal_decl,
                                               Basictype *formal_list) {
    ((Container *) formal_list)->addVariableToFront(formal_decl);
    return formal_list;
}

// rule 11
Basictype *Handler::handleFormalDeclTypeId(Basictype *type, Basictype *id) {
    if (symbol_table.exists(id->getLexeme())) {
        output::errorDef(yylineno, id->getLexeme());
        exit(0);
    }
    Container *con = new Container(id->getLexeme().c_str());
    con->addVariable(con);
    con->setType(type->getType());
    symbol_table.insertItem(con);
    return con;
}

// rule 15
void Handler::handleStatmentTypeId(Basictype *type, Basictype *id) {
    if (symbol_table.exists(id->getLexeme())) {
        output::errorDef(yylineno, id->getLexeme());
        exit(0);
    }
    id->setGlobalOffset(offset_stack.getTopOffset());
    id->setLocalOffset(symbol_table.getLastScopeSize());
    id->setType(type->getType());
    symbol_table.insertItem(id);
    offset_stack.increaseOffset();
}

// rule 16
void Handler::handleStatmentTypeIdAssignExp(Basictype *type, Basictype *id,
                                            Basictype *exp) {
    if (symbol_table.exists(id->getLexeme())) {
        output::errorDef(yylineno, id->getLexeme());
        exit(0);
    }
    if (!assignmentIsLegal(type->getType(), exp->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    handleStatmentTypeId(type, id);
}

// rule 17
Basictype *Handler::handleIdAssignExp(Basictype *id, Basictype *exp) {
    if (!symbol_table.exists(id->getLexeme())) {
        output::errorUndef(yylineno, id->getLexeme());
        exit(0);
    }
    Basictype *id_item = symbol_table.getItemById(id->getLexeme());
    if (id_item->getType() == "FUNC") {
        output::errorUndef(yylineno, id->getLexeme());
        exit(0);
    } else if (!assignmentIsLegal(id_item->getType(), exp->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return id_item;
}

// rule 19
void Handler::handleReturnVoid() {
    if (expected_ret_type != "VOID") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}


// rule 20
void Handler::handleReturnWithType(Basictype *ret_type) {
    if (!assignmentIsLegal(expected_ret_type, ret_type->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

// rule 23
// rule 24
void Handler::handleWhileEnd() {
    current_while_count--;
}

// rule 25
void Handler::handleBreak() {
    if (current_while_count == 0) {
        output::errorUnexpectedBreak(yylineno);
        exit(0);
    }
}

// rule 26
void Handler::handleContinue() {
    if (current_while_count == 0) {
        output::errorUnexpectedContinue(yylineno);
        exit(0);
    }
}

// rule 27
Basictype *Handler::handleCallWithParams(Basictype *id, Basictype *exp_list) {
    if (!symbol_table.exists(id->getLexeme())) {
        output::errorUndefFunc(yylineno, id->getLexeme());
        exit(0);
    }
    Basictype *func = symbol_table.getItemById(id->getLexeme());
    if (func->getType() != "FUNC") {
        output::errorUndefFunc(yylineno, id->getLexeme());
        exit(0);
    }
    vector<string> func_args_types = ((Function *) func)->getVariablesTypes();
    vector<Basictype *> func_args = ((Function *) func)->getVariables();
    vector<Basictype *>
            exp_list_params = ((Container *) exp_list)->getVariables();
    if (func_args.size() != exp_list_params.size()) {
        output::errorPrototypeMismatch(yylineno, func->getLexeme(),
                                       func_args_types);
        exit(0);
    }
    for (int i = 0; i < func_args.size(); i++) {
        if (!assignmentIsLegal(func_args[i]->getType(),
                               exp_list_params[i]->getType())) {
            output::errorPrototypeMismatch(yylineno, func->getLexeme(),
                                           func_args_types);
            exit(0);
        }
    }
    Basictype *ret_call = new Basictype("CALL");
    ret_call->setType(((Function *) func)->getRetType());
    return ret_call;
}

// rule 28
Basictype *Handler::handleCallNoParams(Basictype *id) {
    if (!symbol_table.exists(id->getLexeme())) {
        output::errorUndefFunc(yylineno, id->getLexeme());
        exit(0);
    }
    Basictype *func = symbol_table.getItemById(id->getLexeme());
    if (func->getType() != "FUNC") {
        output::errorUndefFunc(yylineno, id->getLexeme());
        exit(0);
    }
    vector<Basictype *> func_args = ((Function *) func)->getVariables();
    vector<string> func_args_types = ((Function *) func)->getVariablesTypes();
    if (!func_args.empty()) {
        output::errorPrototypeMismatch(yylineno, id->getLexeme(),
                                       func_args_types);
        exit(0);
    }
    Basictype *ret_call = new Basictype("CALL");
    ret_call->setType(((Function *) func)->getRetType());
    return ret_call;
}

// rule 29
Basictype *Handler::handleExplistExp(Basictype *exp) {
    Container *con = new Container(exp->getLexeme().c_str());
    con->addVariableToFront(exp);
    return con;
}

// rule 30
Basictype *Handler::handleExpExplist(Basictype *exp, Basictype *exp_list) {
    ((Container *) exp_list)->addVariableToFront(exp);
    return exp_list;
}

// rule 31
Basictype *Handler::handleInt(Basictype *some_int) {
    some_int->setType("INT");
    return some_int;
}

//rule 32
Basictype *Handler::handleByte(Basictype *some_byte) {
    some_byte->setType("BYTE");
    return some_byte;
}

// rule 33
Basictype *Handler::handleBool(Basictype *some_bool) {
    return some_bool;
}


// rule 34
Basictype *Handler::handleBracketExpBracket(Basictype *exp) {
    return exp;
}

// rule 35
Basictype *Handler::handleExpBinopH(Basictype *exp_left, Basictype *exp_right) {
    if (!isNum(exp_left->getType()) || !isNum(exp_right->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (exp_left->getType() == "INT") {
        return exp_left;
    }
    return exp_right;
}

// rule 36
Basictype *Handler::handleExpBinopL(Basictype *exp_left, Basictype *exp_right) {
    return handleExpBinopH(exp_left, exp_right);
}

// rule 37
Basictype *Handler::handleId(Basictype *id) {
    if (!symbol_table.exists(id->getLexeme())) {
        output::errorUndef(yylineno, id->getLexeme());
        exit(0);
    }
    Basictype *id_type = symbol_table.getItemById(id->getLexeme());
    Basictype *ret_type = new Basictype(id_type->getLexeme().c_str());
    if (id_type->getType() == "FUNC") {
        ret_type->setType(((Function *) id_type)->getRetType());
    } else {
        ret_type->setType(id_type->getType());
    }
    ret_type->setGlobalOffset(id_type->getGlobalOffset());
    ret_type->setLocalOffset(id_type->getLocalOffset());
    ret_type->setType(id_type->getType());
    return ret_type;
}

// rule 38
Basictype *Handler::handleCallToExp(Basictype *call) {
    return call;
}

// rule 39
Basictype *Handler::handleNum(Basictype *num) {
    num->setType("INT");
    return num;
}

// rule 40
Basictype *Handler::handleNumB(Basictype *num) {
    if (((Num *) num)->getVal() > 255) {
        output::errorByteTooLarge(yylineno, num->getLexeme());
        exit(0);
    }
    num->setType("BYTE");
    return num;
}

// rule 41
Basictype *Handler::handleString(Basictype *string) {
    return string;
}

// rule THE_ANSWER
Basictype *Handler::handleTrue(Basictype *bool_true) {
    bool_true->setReg("true");
    return bool_true;
}

// rule 43
Basictype *Handler::handleFalse(Basictype *bool_false) {
    bool_false->setReg("false");
    return bool_false;
}

// rule 44
Basictype *Handler::handleNotExp(Basictype *exp) {
    if (exp->getType() != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return new Bool("true");
}

// rule 45
Basictype *Handler::handleExpAndExp(Basictype *exp_left, Basictype *exp_right) {
    if (exp_left->getType() != "BOOL" || exp_right->getType() != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return new Bool("true");
}

// rule 46
Basictype *Handler::handleExpOrExp(Basictype *exp_left, Basictype *exp_right) {
    if (exp_left->getType() != "BOOL" || exp_right->getType() != "BOOL") {
        output::errorMismatch(0);
        exit(0);
    }
    return new Bool("true");
}

// rule 47
Basictype *Handler::handleExpRelopExp(Basictype *exp_left,
                                      Basictype *exp_right) {
    if (!isNum(exp_left->getType()) || !isNum(exp_right->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return new Bool("true");
}

// rule 48
Basictype *Handler::handleExpReleqExp(Basictype *exp_left,
                                      Basictype *exp_right) {
    if (!isNum(exp_left->getType()) || !isNum(exp_right->getType())) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return new Bool("true");
}

// rule 49
Basictype *Handler::handleIfStart(Basictype *exp) {
    if (exp->getType() != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return exp;
}

// rule 50
Basictype *Handler::handleWhileStart(Basictype *exp) {
    current_while_count++;
    return handleIfStart(exp);
}

bool Handler::isNum(const string &type) {
    return (type == "INT" || type == "BYTE");
}

bool Handler::assignmentIsLegal(const string &assign_to,
                                const string &assign_from) {
    if (assign_to == "INT" && assign_from == "BYTE") {
        return true;
    }
    return assign_to == assign_from;

}

bool Handler::isMain(Basictype *ret_type, Basictype *id, Basictype *args) {
    if (ret_type->getType() != "VOID") {
        return false;
    }
    if (id->getLexeme() != "main") {
        return false;
    }
    if (!((Container *) args)->getVariables().empty()) {
        return false;
    }
    return true;
}

void Handler::handleFunctionStart(Basictype *ret_type, Basictype *id,
                                        Basictype *args) {
    if (symbol_table.exists(((Id *) id)->getName())) {
        output::errorDef(yylineno, id->getLexeme());
    }
    if (isMain(ret_type, id, args)) {
        was_main_defined = true;
    }
    vector<string> params_types;
    Function *func = new Function(((Id *) id)->getName());
    func->setType("FUNC");
    func->setRetType(ret_type->getType());
    func->setGlobalOffset(0);
    int i = -1;
    for (Basictype *basic_type : ((Container *) args)->getVariables()) {
        basic_type->setGlobalOffset(i);
        func->addVariable(basic_type);
        i--;
        params_types.push_back(basic_type->getType());
    }
    string func_type =
            output::makeFunctionType(ret_type->getType(), params_types);
    func->setFunctionType(func_type);
    symbol_table.insertItem(func);
}


