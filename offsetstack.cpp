#include "offsetstack.hpp"

void OffsetStack::duplicateLastItem() { stack.push_back(stack.back()); }
void OffsetStack::removeLastItem() { stack.pop_back(); }
void OffsetStack::increaseOffset() { stack.back()++; }
void OffsetStack::decreaseOffset() { stack.back()--; }
int OffsetStack::getTopOffset() { return stack.back(); }
void OffsetStack::initialize() { stack.push_back(0); }