// SteppingAction.cc                                                                                                                                                                             

#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4UnitsTable.hh"

#include <iostream> // 디버그 출력을 위해 추가                                                                                                                                                   

SteppingAction::SteppingAction(EventAction* eventAction)
  : G4UserSteppingAction(), fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // 광자의 현재 트랙 가져오기
  G4Track* track = step->GetTrack();
  
  // 광자만 처리
  if(track->GetDefinition()->GetParticleName() != "opticalphoton") return;
  
  // 현재 스텝의 물리적 볼륨 가져오기
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  // SiPM 볼륨 이름이 "SiPM_0", "SiPM_1", ..., "SiPM_399"과 같은 형식인지 확인
  if(volumeName.substr(0,4) == "SiPM") {
    // 언더스코어를 포함하지 않도록 수정
    G4int sipmID = std::stoi(volumeName.substr(5)); // 'SiPM_' 다음부터 숫자 추출
    if(sipmID >=0 && sipmID < 40) {
      // SiPM 카운트 증가
      fEventAction->AddCount_SiPM(sipmID);
      
      // 디버그 출력
      //std::cout << "Photon reached SiPM ID: " << sipmID << std::endl;
    }
  }

  // 첫 스텝에서 시작 위치 기록
  if(track->GetCurrentStepNumber() == 1) {
    G4ThreeVector startPos = track->GetVertexPosition();
    fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
  }
  
}
