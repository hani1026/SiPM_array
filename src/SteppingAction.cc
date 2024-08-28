#include "SteppingAction.hh"

SteppingAction::SteppingAction(EventAction * eventAction): fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
 
  if (!fSiPMScoringVolume){
    const MyDetectorConstruction* detConstruction
        = static_cast<const MyDetectorConstruction*>
      (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    fSiPMScoringVolume = detConstruction->GetSiPMVolume();
  }

  G4RootAnalysisManager* analysisManager = G4RootAnalysisManager::Instance();
  G4double stepLength = 0. ;
  stepLength = aStep->GetStepLength()/CLHEP::cm;
  fEventAction->AddCount_length(stepLength);
  
  if(aStep->GetTrack()->GetCurrentStepNumber() == 1)
    {
  
      fEventAction->AddCount_WholePhoton(1);
      G4double xposition = aStep->GetPreStepPoint()->GetPosition().x();
      G4double yposition = aStep->GetPreStepPoint()->GetPosition().y();
      G4double zposition = aStep->GetPreStepPoint()->GetPosition().z();
      fEventAction->AddXposition(xposition);
      fEventAction->AddYposition(yposition);
      fEventAction->AddZposition(zposition);
    } 
  
  
  G4LogicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  if (volume != fSiPMScoringVolume)
    {
      return;
    } 

  G4int copyNo = 0;
  
  if (volume == fSiPMScoringVolume)
    {
  
      copyNo = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetCopyNo();  
      fEventAction->AddCount_SiPM(fEventAction->f_SiPM_Count, copyNo); 
      fEventAction->AddCount_WholeSiPM(1);
  
      aStep->GetTrack()->SetTrackStatus(fStopAndKill); 
    }



}
