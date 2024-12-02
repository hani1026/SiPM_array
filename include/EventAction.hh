// EventAction.hh
#ifndef EventAction_h
#define EventAction_h

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include <vector>

class RunAction;

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction* runAction);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event*) override;
  virtual void EndOfEventAction(const G4Event*) override;

  // 데이터 추가 메서드
  void AddCount_SiPM(G4int sipmID);
  void AddStartPosition(G4double x, G4double y, G4double z);
  void AddPhotonPosition(const G4ThreeVector& pos) {
    fPhotonPositions.push_back(pos);
  }
  void AddDelayedCount_SiPM(G4int sipmID, G4double delay);
  void ProcessDelayedCounts();

private:
  // 초기화 메서드
  void ResetEventVariables();
  void ResetCounters();

  // 이벤트 처리 메서드
  void CalculateAveragePositions();
  void UpdateRunAction();
  void PrintEventProgress();

  // 유효성 검사 메서드
  bool IsValidSiPMID(G4int sipmID);
  void ReportInvalidSiPMID(G4int sipmID);

  // 멤버 변수
  RunAction* fRunAction;
  std::vector<G4ThreeVector> fPhotonPositions;
  G4double fStartX;
  G4double fStartY;
  G4double fStartZ;
  G4double fPhotonCount;
  G4int f_SiPM_Count[40];
  struct DelayedCount {
    G4int sipmID;
    G4double time;
  };
  std::vector<DelayedCount> fDelayedCounts;
  G4double fDarkNoiseRate;  // Hz/mm^2
};

#endif
