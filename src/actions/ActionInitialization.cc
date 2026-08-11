#include "na62rich/actions/ActionInitialization.hh"
#include "na62rich/actions/PrimaryGeneratorAction.hh"

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());
}