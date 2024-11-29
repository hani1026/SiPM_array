// SteppingAction.hh
#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "EventAction.hh"

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  virtual ~SteppingAction();

  virtual void UserSteppingAction(const G4Step*) override;

private:
  EventAction* fEventAction;
};

#endif
