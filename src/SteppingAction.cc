// SteppingAction.cc                                                                                                                                                                             

#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4UnitsTable.hh"
#include "G4Exception.hh"
#include "G4VProcess.hh"

#include <iostream> // 디버그 출력을 위해 추가                                                                                                                                                   

SteppingAction::SteppingAction(EventAction* eventAction)
  : G4UserSteppingAction(), fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  if (track->GetDefinition()->GetParticleName() == "opticalphoton") {
    G4StepPoint* postStepPoint = step->GetPostStepPoint();
    if (postStepPoint) {
      G4VPhysicalVolume* postVolume = postStepPoint->GetPhysicalVolume();
      if (postVolume && IsSiPMVolume(postVolume->GetName())) {
        G4String volName = postVolume->GetName(); 
        G4int sipmID = std::stoi(volName.substr(5));
        if (sipmID >= 0 && sipmID < 40) {
          G4double hitTime = postStepPoint->GetGlobalTime();
          fEventAction->AddCount_SiPM(sipmID, hitTime);
          track->SetTrackStatus(fStopAndKill);
        }
      }
    }
  }
  
  // 뮤온 시작 위치 기록
  if (IsMuon(step) && step->GetTrack()->GetCurrentStepNumber() == 1) {
    G4ThreeVector startPos = step->GetTrack()->GetVertexPosition();
    fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
  }
}

G4bool SteppingAction::SimulateSiPMResponse(const G4Step* step)
{
  // 널 포인터 체크 추가
  if (!step) return false;
  
  // 물질 속성 테이블 접근 전 유효성 검사
  G4MaterialPropertiesTable* mpt = 
    step->GetPreStepPoint()->GetMaterial()->GetMaterialPropertiesTable();
  if (!mpt) return false;
  
  G4MaterialPropertyVector* effVector = mpt->GetProperty("EFFICIENCY");
  if (!effVector) return false;
  
  // 온도에 따른 PDE 보정
  G4double temperature = 293.15*kelvin;  // 작동 온도
  G4double referenceTemp = 293.15*kelvin;  // 데이터시트 기준 온도
  G4double tempCoeff = -0.0016;  // PDE 온도 계수
  G4double tempCorrection = 1.0 + tempCoeff * (temperature - referenceTemp);
  
  // 과전압 효과 추가
  G4double overVoltage = 2.7*volt;  // 동작 과전압
  G4double nominalVoltage = 2.7*volt;  // 권장 과전압
  G4double voltageCoeff = 0.1;  // 과전압 계수
  G4double voltageCorrection = 1.0 + voltageCoeff * (overVoltage - nominalVoltage);
  
  // 총 보정 계수
  G4double totalCorrection = tempCorrection * voltageCorrection;
  G4double efficiency = effVector->Value(step->GetTrack()->GetTotalEnergy());
  efficiency *= totalCorrection;
  
  // PDE에 따른 광자 검출 여부 결정
  if(G4UniformRand() > efficiency) return false;

  // 크로스토크 시뮬레이션 (3% 확률, S13360-6075PE 데이터시트 기준)
  if(G4UniformRand() < 0.03) {
    G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
    fEventAction->AddCount_SiPM(GetSiPMID(step), hitTime);  // 추가 펄스
  }

  // 애프터펄싱 시뮬레이션 (약 3% 확률)
  if(G4UniformRand() < 0.03) {
    G4double delay = -50*ns * std::log(G4UniformRand());  // 지수 분포
    if(delay < 1000*ns) {  // 1μs 이내만 고려
      fEventAction->AddDelayedCount_SiPM(GetSiPMID(step), delay);
    }
  }

  // 회복 시간 효과 추가 (약 50ns)
  static G4double lastHitTime = 0;
  G4double currentTime = step->GetTrack()->GetGlobalTime();
  if(currentTime - lastHitTime < 50*ns) return false;
  lastHitTime = currentTime;

  return true;
}

G4int SteppingAction::GetSiPMID(const G4Step* step) const
{
  // SiPM 볼륨의 이름에서 ID 추출
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  return std::stoi(volumeName.substr(5));
}

// 다른 유틸리티 함수들도 구현
bool SteppingAction::IsOpticalPhoton(const G4Step* step) const
{
  return (step->GetTrack()->GetDefinition()->GetParticleName() == "opticalphoton");
}

bool SteppingAction::IsMuon(const G4Step* step) const
{
  return (step->GetTrack()->GetDefinition()->GetParticleName() == "mu-");
}

bool SteppingAction::IsSiPMVolume(const G4String& volumeName) const
{
  return (volumeName.substr(0,4) == "SiPM");
}

void SteppingAction::RecordStartPosition(const G4Step* step)
{
  if (IsMuon(step) && step->GetTrack()->GetCurrentStepNumber() == 1) {
    G4ThreeVector startPos = step->GetTrack()->GetVertexPosition();
    fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
  }
}
