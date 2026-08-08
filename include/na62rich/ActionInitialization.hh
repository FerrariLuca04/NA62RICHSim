#ifndef NA62RICH_ACTION_INITIALIZATION_HH
#define NA62RICH_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization {
    public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

    void Build() const override;
};

#endif