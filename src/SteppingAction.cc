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
  G4Track* track = step->GetTrack();
  G4String particleName = track->GetDefinition()->GetParticleName();

  // 뮤온인 경우 시작 위치 기록
  if (particleName == "mu-") {
    if(track->GetCurrentStepNumber() == 1) {
      G4ThreeVector startPos = track->GetVertexPosition();
      fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
    }
    return;
  }

  // 광자가 아닌 경우 무시
  if (particleName != "opticalphoton") return;

  // 볼륨 이름 확인
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  // SiPM 볼륨인 경우에만 처리
  if(volumeName.substr(0,4) != "SiPM") return;

  try {
    G4int sipmID = std::stoi(volumeName.substr(5));
    if(sipmID >= 0 && sipmID < 40) {
      fEventAction->AddCount_SiPM(sipmID);
    }
  } catch (const std::exception& e) {
    G4ExceptionDescription msg;
    msg << "Invalid SiPM volume name: " << volumeName;
    G4Exception("SteppingAction::UserSteppingAction",
                "MyCode001", JustWarning, msg);
  }
}
