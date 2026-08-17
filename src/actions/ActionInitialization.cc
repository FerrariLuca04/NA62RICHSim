#include "na62rich/actions/ActionInitialization.hh"

#include "na62rich/actions/PrimaryGeneratorAction.hh"
#include "na62rich/actions/RunAction.hh"
#include "na62rich/actions/EventAction.hh"

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());

    auto* runAction = new RunAction();
    SetUserAction(runAction);

    SetUserAction(new EventAction(runAction));
}