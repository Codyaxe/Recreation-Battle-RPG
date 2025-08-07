#include "action.h"

Action::Action() :
    name("Generic Name"), description("Generic Description") 
{}

Action::Action(std::string name_, std::string description_) :
    name(name_), description(description_) 
{}