// RunAction.cc
#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "PrimaryGeneratorAction.hh"
#include "Construction.hh"

#include "G4SystemOfUnits.hh"

#include "G4ThreeVector.hh"

#include <sstream>
#include <iostream>

RunAction::RunAction(G4String fileName)
  : G4UserRunAction(), fFileName(fileName)
{
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->SetVerboseLevel(1);

  // 런 단위 ntuple 생성
  analysisManager->CreateNtuple("SiPM", "ntp");
  analysisManager->CreateNtupleIColumn("Run_ID");            // 0
  analysisManager->CreateNtupleIColumn("TotalEvents");       // 1

  // SiPM 총합을 위한 컬럼 생성
  for(int i = 0; i < 40; ++i){
    std::ostringstream columnName;
    columnName << "SiPM_" << i;
    analysisManager->CreateNtupleIColumn(columnName.str().c_str());
  }

  analysisManager->CreateNtupleDColumn("X");          // 8
  analysisManager->CreateNtupleDColumn("Y");          // 9
  analysisManager->CreateNtupleDColumn("Z");          // 10

  analysisManager->FinishNtuple();

  // 파일 이름 설정 부분 수정
  G4String fileNameFinal = fFileName + ".csv";  // "_nt_SIPM" 부분 제거
  analysisManager->OpenFile(fileNameFinal);
  G4cout << "Open File " << fileNameFinal << G4endl;

}

RunAction::~RunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
  delete analysisManager;
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  // 랜덤 넘버 시드 저장 비활성화
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // 분석 매니저 인스턴스 가져오기
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // 집계 변수 초기화
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonCount = 0.0;
  fPhotonPositions.clear();
  for(int i = 0; i < 40; ++i){
    f_SiPM_Count[i] = 0;
  }
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4String runCondition;

  if (IsMaster()) {
    G4cout << G4endl << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout << G4endl << "--------------------End of Local Run------------------------";
  }

    G4cout
      << G4endl
      << " The run consists of " << nofEvents << " " << runCondition
      << G4endl
      << "------------------------------------------------------------"
      << G4endl;

    // 런 단위 데이터 기록
    analysisManager->FillNtupleIColumn(0, run->GetRunID());
    analysisManager->FillNtupleIColumn(1, nofEvents);

    // SiPM 총합 기록
    for(int i = 0; i < 40; ++i){
      analysisManager->FillNtupleIColumn(i+2, f_SiPM_Count[i]);
    }

    // 평균 시작 위치 계산

    double avgStartX = (fPhotonCount > 0) ? (fStartX / fPhotonCount) : 0.0;
    double avgStartY = (fPhotonCount > 0) ? (fStartY / fPhotonCount) : 0.0;
    double avgStartZ = (fPhotonCount > 0) ? (fStartZ / fPhotonCount) : 0.0;

    analysisManager->FillNtupleDColumn(42, avgStartX);
    analysisManager->FillNtupleDColumn(43, avgStartY);
    analysisManager->FillNtupleDColumn(44, avgStartZ);

    analysisManager->AddNtupleRow();

    // ROOT 파일 저장 및 닫기
    //analysisManager->Write();
    //analysisManager->CloseFile();

    // 집계 변수 초기화
    fStartX = 0.0;
    fStartY = 0.0;
    fStartZ = 0.0;
    fPhotonCount = 0;
    fPhotonPositions.clear();
    for(int i = 0; i < 40; ++i){
      f_SiPM_Count[i] = 0;
    }

    printEventproc();
}

void RunAction::printEventproc()
{
  fEvent += 1;
  if(fEvent % 1000 == 0)
    {
      G4cout << " Events clear : " << fEvent << G4endl;
    }
}
