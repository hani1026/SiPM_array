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
  : G4UserRunAction(), 
    fFileName(fileName),
    fEvent(0),
    nofEvents(0),
    fStartX(0.0),
    fStartY(0.0),
    fStartZ(0.0)
{
  // Analysis manager 초기화
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(2);  // 더 자세한 디버그 출력
  
  // CSV 출력 설정
  analysisManager->SetNtupleDirectoryName(".");
  analysisManager->SetFirstNtupleId(0);
  
  // Ntuple 생성
  analysisManager->CreateNtuple("SiPM", "Detector Data");
  
  // 컬럼 생성
  analysisManager->CreateNtupleIColumn("Event_ID");  // column 0
  
  // SiPM 채널
  for(G4int i = 0; i < 40; i++) {
    G4String colName = "SiPM_" + std::to_string(i);
    G4String timeName = "SiPM_Time_" + std::to_string(i);
    analysisManager->CreateNtupleIColumn(colName);  // columns 1-40
    analysisManager->CreateNtupleDColumn(timeName);  // 시간 컬럼 추가
  }
  
  // 위치 정보
  analysisManager->CreateNtupleDColumn("X");  // column 41
  analysisManager->CreateNtupleDColumn("Y");  // column 42
  analysisManager->CreateNtupleDColumn("Z");  // column 43
  
  analysisManager->FinishNtuple();
  
  // 파일 열기
  G4String fileNameFull = fileName + "_nt_SiPM.csv";
  analysisManager->OpenFile(fileNameFull);
  G4cout << "Created and opened output file: " << fileNameFull << G4endl;
}

RunAction::~RunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
  delete analysisManager;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4cout << "Beginning of run action" << G4endl;
  fEvent = 0;  // 이벤트 카운터 초기화
  
  auto analysisManager = G4AnalysisManager::Instance();
  if (!analysisManager->IsOpenFile()) {
    G4String fileName = fFileName + "_nt_SiPM.csv";
    analysisManager->OpenFile(fileName);
    G4cout << "Reopened file: " << fileName << G4endl;
  }
}

void RunAction::ResetRunVariables()
{
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonPositions.clear();
  std::fill_n(f_SiPM_Count, 40, 0);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4cout << "End of run action" << G4endl;
  auto analysisManager = G4AnalysisManager::Instance();
  
  if (analysisManager->IsOpenFile()) {
    analysisManager->Write();
    G4cout << "Writing final data to file..." << G4endl;
    analysisManager->CloseFile(true);  // true: 파일을 강제로 닫음
    G4cout << "File closed." << G4endl;
  }
  
  ProcessEndOfRun(run);
  ResetRunVariables();
  printEventproc();
}

void RunAction::ProcessEndOfRun(const G4Run* run)
{
  nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  PrintRunSummary();
}

void RunAction::PrintRunSummary()
{
  if (IsMaster()) {
    G4cout << G4endl << "--------------------End of Global Run-----------------------";
  } else {
    G4cout << G4endl << "--------------------End of Local Run------------------------";
  }

  G4cout << G4endl
         << " The run consists of " << nofEvents << " events"
         << G4endl
         << "------------------------------------------------------------"
         << G4endl;
}

void RunAction::SaveEventData(const G4int sipmCounts[40], const G4double avgTimes[40],
                            G4double x, G4double y, G4double z)
{
  auto analysisManager = G4AnalysisManager::Instance();
  
  try {
    analysisManager->FillNtupleIColumn(0, fEvent);
    
    for(int i = 0; i < 40; ++i) {
      analysisManager->FillNtupleIColumn(2*i + 1, sipmCounts[i]);
      analysisManager->FillNtupleDColumn(2*i + 2, avgTimes[i]/ns);  // ns 단위로 저장
    }
    
    analysisManager->FillNtupleDColumn(81, x/mm);
    analysisManager->FillNtupleDColumn(82, y/mm);
    analysisManager->FillNtupleDColumn(83, z/mm);
    
    analysisManager->AddNtupleRow(0);
    analysisManager->Write();
    
    fEvent++;
    
  } catch (const std::exception& e) {
    G4ExceptionDescription msg;
    msg << "Error saving event data: " << e.what();
    G4Exception("RunAction::SaveEventData",
                "MyCode0001", FatalException, msg);
  }
}

void RunAction::printEventproc()
{
  fEvent += 1;
  if(fEvent % 1000 == 0) {
    G4cout << " Events clear : " << fEvent << G4endl;
  }
}
