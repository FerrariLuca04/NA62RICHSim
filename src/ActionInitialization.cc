#include "na62rich/ActionInitialization.hh"
#include "na62rich/PrimaryGeneratorAction.hh"

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());
}