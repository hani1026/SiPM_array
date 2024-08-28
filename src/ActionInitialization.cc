#include "ActionInitialization.hh"

ActionInitialization::ActionInitialization(G4String fileName):G4VUserActionInitialization(),fFileName(fileName)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);
}


void ActionInitialization::Build() const
{

  PrimaryGeneratorAction* P_generator = new PrimaryGeneratorAction();
  SetUserAction(P_generator);
  
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);
  
  EventAction* eventAction = new EventAction(runAction);
  SetUserAction(eventAction);
  
  SteppingAction* StepAction = new SteppingAction(eventAction);
  SetUserAction(StepAction);
}
