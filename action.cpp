#include "action.h"

Action::Action() : name("Generic Name"), description("Generic Description") {}

Action::Action(const std::string& name_, const std::string& description_)
    : name(name_), description(description_)
{
}