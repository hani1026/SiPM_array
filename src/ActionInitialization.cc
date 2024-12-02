// ActionInitialization.cc
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "Construction.hh"

ActionInitialization::ActionInitialization(G4String fileName)
  : G4VUserActionInitialization(), fFileName(fileName)
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const {
  SetupRunAction();
}

void ActionInitialization::Build() const {
  SetupGeneratorAction();
  RunAction* runAction = SetupRunAction();
  EventAction* eventAction = SetupEventAction(runAction);
  SetupSteppingAction(eventAction);
}

void ActionInitialization::SetupGeneratorAction() const {
  SetUserAction(new PrimaryGeneratorAction());
}

RunAction* ActionInitialization::SetupRunAction() const {
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);
  return runAction;
}

EventAction* ActionInitialization::SetupEventAction(RunAction* runAction) const {
  EventAction* eventAction = new EventAction(runAction);
  SetUserAction(eventAction);
  return eventAction;
}

void ActionInitialization::SetupSteppingAction(EventAction* eventAction) const {
  SetUserAction(new SteppingAction(eventAction));
}
