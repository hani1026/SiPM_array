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
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <iostream> // 디버그 출력을 위해 추가                                                                                                                                                   

SteppingAction::SteppingAction(EventAction* eventAction)
  : G4UserSteppingAction(), fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // 포인터 캐싱
  static thread_local G4Track* track = nullptr;
  static thread_local G4StepPoint* postStepPoint = nullptr;
  
  track = step->GetTrack();
  if (track->GetDefinition()->GetParticleName() == "opticalphoton") {
    postStepPoint = step->GetPostStepPoint();
    if (postStepPoint) {
      G4VPhysicalVolume* postVolume = postStepPoint->GetPhysicalVolume();
      if (postVolume && IsSiPMVolume(postVolume->GetName())) {
        try {
          G4String volName = postVolume->GetName();
          G4int sipmID = std::stoi(volName.substr(5));
          if (sipmID >= 0 && sipmID < 40) {
            fEventAction->AddCount_SiPM(sipmID);
            track->SetTrackStatus(fStopAndKill);
          }
        } catch (const std::exception& e) {
          G4ExceptionDescription msg;
          msg << "Error processing SiPM ID: " << e.what();
          G4Exception("SteppingAction::UserSteppingAction",
                     "MyCode001", JustWarning, msg);
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
  if (!step) return false;
  
  // 광자 에너지에 따른 PDE 적용 (S13360-6075PE 기준)
  G4double photonEnergy = step->GetTrack()->GetTotalEnergy();
  
  // PDE 피크값: ~450nm에서 약 40%
  G4double peakPDE = 0.40;  // 40% at peak
  G4double peakWavelength = 450 * nanometer;
  G4double peakEnergy = (h_Planck * c_light) / peakWavelength;
  
  // 간단한 가우시안 형태로 PDE 계산
  G4double sigma = 0.5 * eV;  // PDE 커브의 폭
  G4double pde = peakPDE * std::exp(-std::pow((photonEnergy - peakEnergy)/(sigma), 2)/2.);
  
  // PDE에 따른 검출 여부 결정
  if (G4UniformRand() > pde) return false;
  
  // 기본적인 크로스토크 (3%)
  if (G4UniformRand() < 0.03) {
    fEventAction->AddCount_SiPM(GetSiPMID(step));
  }
  
  return true;
}

G4int SteppingAction::GetSiPMID(const G4Step* step) const
{
  // SiPM 볼륨의 이름에서 ID 추출
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();
  
  try {
    // "SiPM_XX" 형식에서 XX 부분을 정수로 변환
    return std::stoi(volumeName.substr(5));
  } catch (const std::exception& e) {
    G4ExceptionDescription msg;
    msg << "Invalid SiPM volume name format: " << volumeName;
    G4Exception("SteppingAction::GetSiPMID",
                "MyCode002", JustWarning, msg);
    return -1;
  }
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

void SteppingAction::ReportInvalidSiPMName(const G4String& volumeName) const
{
  G4ExceptionDescription msg;
  msg << "Invalid SiPM volume name: " << volumeName;
  G4Exception("SteppingAction::IsSiPMVolume",
              "MyCode003", JustWarning, msg);
}

void SteppingAction::RecordStartPosition(const G4Step* step)
{
  if (IsMuon(step) && step->GetTrack()->GetCurrentStepNumber() == 1) {
    G4ThreeVector startPos = step->GetTrack()->GetVertexPosition();
    fEventAction->AddStartPosition(startPos.x(), startPos.y(), startPos.z());
  }
}
