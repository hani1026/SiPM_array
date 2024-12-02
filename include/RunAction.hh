// RunAction.hh
#ifndef RunAction_h
#define RunAction_h

#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "g4csv.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <vector>

class RunAction : public G4UserRunAction {
public:
  RunAction(G4String fileName);
  virtual ~RunAction() override;

  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void EndOfRunAction(const G4Run*) override;
  
  void printEventproc();
  
  // 이벤트 데이터 저장을 위한 새로운 메서드
  void SaveEventData(const G4int sipmCounts[40], G4double x, G4double y, G4double z);

  // 데이터 추가 메서드
  void AddStartPosition(G4double x, G4double y, G4double z) {
    fStartX += x;
    fStartY += y;
    fStartZ += z;
    fPhotonCount += 1;
  }
  
  void AddPhotonPosition(const G4ThreeVector& pos) {
    fPhotonPositions.push_back(pos);
  }
  
  void AddSiPMCounts(const G4int counts[40]) {
    for(int i = 0; i < 40; ++i) {
      f_SiPM_Count[i] += counts[i];
    }
  }

private:
  // 초기화 관련 메서드
  void InitializeAnalysisManager();
  void CreateNtuples(G4AnalysisManager* analysisManager);
  void CreateBasicColumns(G4AnalysisManager* analysisManager);
  void CreateSiPMColumns(G4AnalysisManager* analysisManager);
  void CreatePositionColumns(G4AnalysisManager* analysisManager);
  
  // 실행 관련 메서드
  void ResetRunVariables();
  void ProcessEndOfRun(const G4Run* run);
  void PrintRunSummary();

  // 멤버 변수
  G4String fFileName;
  G4int fEvent;
  G4int nofEvents;
  
  G4double fStartX;
  G4double fStartY;
  G4double fStartZ;
  G4double fPhotonCount;
  
  std::vector<G4ThreeVector> fPhotonPositions;
  G4int f_SiPM_Count[40];
};

#endif
