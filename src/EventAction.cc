#include "EventAction.hh"

EventAction::EventAction(RunAction * runAction)
  : fRunAction(runAction)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fCount1 = 0;
  fCount2 = 0;
  fCount3 = 0;
  xpos = 0;
  ypos = 0;
  zpos = 0;
    
  for(int i=0;i<40;i++){
    f_SiPM_Count[i] = {0,};
  }
}

void EventAction::EndOfEventAction(const G4Event*)
{
  fEventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillNtupleDColumn(0, fEventID);

  for(G4int i=0; i<40; i++){
    analysisManager->FillNtupleDColumn(i+1, f_SiPM_Count[i]);    
  }
  analysisManager->FillNtupleDColumn(41, fCount1);
  analysisManager->FillNtupleDColumn(42, fCount2);
  analysisManager->FillNtupleDColumn(43, fCount3 / (fCount1));


  analysisManager->FillNtupleDColumn(44, xpos);
  analysisManager->FillNtupleDColumn(45, ypos);
  analysisManager->FillNtupleDColumn(46, zpos);
      
  analysisManager->AddNtupleRow();  
  fRunAction->printEventproc();
}
