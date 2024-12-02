// ActionInitialization.hh
#ifndef ActionInitialization_h
#define ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

class RunAction;
class EventAction;

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(G4String fileName);
  virtual ~ActionInitialization();

  virtual void BuildForMaster() const override;
  virtual void Build() const override;

private:
  void SetupGeneratorAction() const;
  RunAction* SetupRunAction() const;
  EventAction* SetupEventAction(RunAction* runAction) const;
  void SetupSteppingAction(EventAction* eventAction) const;

  G4String fFileName;
};

#endif
