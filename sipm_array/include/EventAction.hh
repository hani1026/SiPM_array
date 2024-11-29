// EventAction.hh
#ifndef EventAction_h
#define EventAction_h

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "RunAction.hh"
#include <vector>

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction* runAction);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event*) override;
  virtual void EndOfEventAction(const G4Event*) override;

  // SiPM 카운트를 증가시키는 메서드
  void AddCount_SiPM(G4int sipmID) {
    if(sipmID >= 0 && sipmID < 40) {
      f_SiPM_Count[sipmID] += 1;
    }
  }

  // 기타 카운트 메서드들
  void AddStartPosition(G4double x, G4double y, G4double z) {
    fStartX += x;
    fStartY += y;
    fStartZ += z;
  }

private:
  RunAction* fRunAction = nullptr;
  std::vector<G4ThreeVector> fPhotonPositions;
  G4double fStartX = 0.0;
  G4double fStartY = 0.0;
  G4double fStartZ = 0.0;
  G4double fPhotonCount = 0.0;
  G4int f_SiPM_Count[40] = {0,};
};

#endif
