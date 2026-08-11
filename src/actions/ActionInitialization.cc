#include "na62rich/actions/ActionInitialization.hh"
#include "na62rich/actions/PrimaryGeneratorAction.hh"
#include "na62rich/actions/EventAction.hh"

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());

    SetUserAction(new EventAction());
}