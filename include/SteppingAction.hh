// SteppingAction.hh
#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "EventAction.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  virtual ~SteppingAction();

  virtual void UserSteppingAction(const G4Step*) override;

private:
  // SiPM 응답 시뮬레이션 관련 함수들
  G4bool SimulateSiPMResponse(const G4Step* step);
  G4int GetSiPMID(const G4Step* step) const;
  
  // 입자 식별 함수들
  bool IsOpticalPhoton(const G4Step* step) const;
  bool IsMuon(const G4Step* step) const;
  
  // 볼륨 관련 함수들
  bool IsSiPMVolume(const G4String& volumeName) const;
  
  // 위치 기록 함수
  void RecordStartPosition(const G4Step* step);

  EventAction* fEventAction;
};

#endif
