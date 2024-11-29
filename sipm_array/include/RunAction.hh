// RunAction.hh
#ifndef RunAction_h
#define RunAction_h

#include "PrimaryGeneratorAction.hh"
#include "Construction.hh"

#include "G4UserRunAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"

#include "g4csv.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <vector>
#include "G4ThreeVector.hh"

class RunAction : public G4UserRunAction {
public:
  // 생성자 및 소멸자
  RunAction(G4String fileName);
  ~RunAction() override;

  // 런 시작 및 종료 액션
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;
  void printEventproc();

  // 데이터 집계를 위한 메서드
  void AddStartPosition(G4double x, G4double y, G4double z) {
    fStartX += x;
    fStartY += y;
    fStartZ += z;
    fPhotonCount += 1;
  }
  void AddPhotonPosition(const G4ThreeVector& pos) {
    fPhotonPositions.push_back(pos);
  }
  void AddSiPMCounts(const G4int counts[4]) {
    for(int i = 0; i < 40; ++i){
      f_SiPM_Count[i] += counts[i];
    }
  }

private:
  G4String fFileName;
  G4int fEvent = 0;
  G4int nofEvents = 0;

  // 집계 변수
  G4double fStartX = 0.0;
  G4double fStartY = 0.0;
  G4double fStartZ = 0.0;
  // 광자 수 카운트 변수
  G4int fPhotonCount = 0;

  std::vector<G4ThreeVector> fPhotonPositions;
  G4int f_SiPM_Count[40] = {0,};
};

#endif
