// SteppingAction.cc                                                                                                                                                                             

#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4UnitsTable.hh"
#include "G4Exception.hh"

#include <iostream> // 디버그 출력을 위해 추가                                                                                                                                                   

SteppingAction::SteppingAction(EventAction* eventAction)
  : G4UserSteppingAction(), fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!IsOpticalPhoton(step)) return;
  
  ProcessSiPMHit(step);
  RecordStartPosition(step);
}

bool SteppingAction::IsOpticalPhoton(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  return (track->GetDefinition()->GetParticleName() == "opticalphoton");
}

void SteppingAction::ProcessSiPMHit(const G4Step* step)
{
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  if(volumeName.substr(0,4) != "SiPM") return;

  try {
    G4int sipmID = std::stoi(volumeName.substr(5));
    if(IsValidSiPMID(sipmID)) {
      fEventAction->AddCount_SiPM(sipmID);
    }
  } catch (const std::exception& e) {
    ReportInvalidSiPMName(volumeName);
  }
}

bool SteppingAction::IsValidSiPMID(G4int sipmID)
{
  return (sipmID >= 0 && sipmID < 40);
}

void SteppingAction::ReportInvalidSiPMName(const G4String& volumeName)
{
  G4ExceptionDescription msg;
  msg << "Invalid SiPM volume name: " << volumeName;
  G4Exception("SteppingAction::ProcessSiPMHit",
              "MyCode001", JustWarning, msg);
}

void SteppingAction::RecordStartPosition(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  if(track->GetCurrentStepNumber() == 1) {
    G4ThreeVector startPos = track->GetVertexPosition();
    fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
  }
}
