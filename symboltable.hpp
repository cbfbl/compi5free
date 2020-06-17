#ifndef SYMBOLETABLE_H
#define SYMBOLETABLE_H

#include <unordered_map>
#include <vector>

#include "types.hpp"

using std::unordered_map;
using std::vector;

class SymbolTable {
    vector <vector<Basictype *>> scopes;
    unordered_map <string, vector<unsigned long>> id_map;

public:
    void insertScope();

    void removeScope();

    void insertItem(Basictype *item, bool insert_to_scope = true);

    bool exists(string id);

    int getLastScopeSize();

    vector<Basictype *> getLastScopeData();

    Basictype *getItemById(string id);
};

#endif