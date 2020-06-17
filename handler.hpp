#ifndef HANDLER_H
#define HANDLER_H

#include <iostream>
#include <string>
#include <vector>

#include "hw3_output.hpp"
#include "offsetstack.hpp"
#include "symboltable.hpp"
#include "types.hpp"
#include "codebufferhandler.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

extern int yylineno;

class Handler {
    CodeBufferHandler cb_handler;
    SymbolTable symbol_table;
    OffsetStack offset_stack;
    string expected_ret_type;
    int current_while_count;
    bool was_main_defined;

    void handleStatmentTypeId(Basictype *type, Basictype *id);

    void handleStatmentTypeIdAssignExp(Basictype *type, Basictype *id,
                                       Basictype *exp);

    void handleFunctionDeclartion(Basictype *ret_type, Basictype *id,
                                  Basictype *args);

    Basictype *handleFormalDeclTypeId(Basictype *type, Basictype *id);

    Basictype *handleFormalDecl(Basictype *formal_decl);

    Basictype *handleFormalDeclFormalList(Basictype *formal_decl,
                                          Basictype *formal_list);

    Basictype *handleFormalsEpsilon();

    Basictype *handleFormalsFormalList(Basictype *formals);

    Basictype *handleRettypeType(Basictype *type);

    Basictype *handleRettypeVoid(Basictype *type_void);

    Basictype *handleIdAssignExp(Basictype *id, Basictype *exp);

    void handleReturnVoid();

    void handleReturnWithType(Basictype *ret_type);

    void handleBreak();

    void handleContinue();

    void handleWhileEnd();

    Basictype *handleExpRelopExp(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleExpReleqExp(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleExpOrExp(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleExpAndExp(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleNotExp(Basictype *exp);

    Basictype *handleNumB(Basictype *num);

    Basictype *handleString(Basictype *string);

    Basictype *handleTrue(Basictype *bool_true);

    Basictype *handleFalse(Basictype *bool_false);

    Basictype *handleNum(Basictype *num);

    Basictype *handleIfStart(Basictype *exp);

    Basictype *handleWhileStart(Basictype *exp);

    Basictype *handleCallWithParams(Basictype *id, Basictype *exp_list);

    Basictype *handleCallNoParams(Basictype *id);

    Basictype *handleExplistExp(Basictype *exp);

    Basictype *handleExpExplist(Basictype *exp, Basictype *exp_list);

    Basictype *handleInt(Basictype *some_int);

    Basictype *handleByte(Basictype *some_byte);

    Basictype *handleBool(Basictype *some_bool);

    Basictype *handleBracketExpBracket(Basictype *exp);

    Basictype *handleExpBinopH(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleExpBinopL(Basictype *exp_left, Basictype *exp_right);

    Basictype *handleId(Basictype *id);

    Basictype *handleCallToExp(Basictype *call);


    void finalize();

    bool isNum(const string &type);

    bool assignmentIsLegal(const string &assign_to, const string &assign_from);

    bool isMain(Basictype *ret_type, Basictype *id, Basictype *args);

    void insertPrintFunctions();

public:
    Handler();

    Basictype *handleRule(int rule_number, vector<Basictype *> params);

    void initialize();

    void setExpectedRetType(string ret_type);

    void insertScope();

    void removeScope();

    void checkDefined(Basictype *id);

};

#endif