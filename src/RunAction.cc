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
    fStartZ(0.0),
    fPhotonCount(0)
{
  InitializeAnalysisManager();
}

RunAction::~RunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
  delete analysisManager;
}

void RunAction::InitializeAnalysisManager()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  
  CreateNtuples(analysisManager);
  
  G4String fileNameFinal = fFileName + ".csv";
  analysisManager->OpenFile(fileNameFinal);
  G4cout << "Open File " << fileNameFinal << G4endl;
}

void RunAction::CreateNtuples(G4AnalysisManager* analysisManager)
{
  analysisManager->CreateNtuple("SiPM", "ntp");
  CreateBasicColumns(analysisManager);
  CreateSiPMColumns(analysisManager);
  CreatePositionColumns(analysisManager);
  analysisManager->FinishNtuple();
}

void RunAction::CreateBasicColumns(G4AnalysisManager* analysisManager)
{
  analysisManager->CreateNtupleIColumn("Event_ID");
}

void RunAction::CreateSiPMColumns(G4AnalysisManager* analysisManager)
{
  for(int i = 0; i < 40; ++i) {
    std::ostringstream columnName;
    columnName << "SiPM_" << i;
    analysisManager->CreateNtupleIColumn(columnName.str());
  }
}

void RunAction::CreatePositionColumns(G4AnalysisManager* analysisManager)
{
  analysisManager->CreateNtupleDColumn("X");
  analysisManager->CreateNtupleDColumn("Y");
  analysisManager->CreateNtupleDColumn("Z");
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  ResetRunVariables();
}

void RunAction::ResetRunVariables()
{
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonCount = 0.0;
  fPhotonPositions.clear();
  std::fill_n(f_SiPM_Count, 40, 0);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
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

void RunAction::SaveEventData()
{
  auto analysisManager = G4AnalysisManager::Instance();
  
  // 이벤트 ID 저장
  analysisManager->FillNtupleIColumn(0, fEvent);
  
  // SiPM 데이터 저장
  for(int i = 0; i < 40; ++i) {
    analysisManager->FillNtupleIColumn(i+1, f_SiPM_Count[i]);
  }
  
  // 위치 데이터 저장
  double avgStartX = (fPhotonCount > 0) ? (fStartX / fPhotonCount) : 0.0;
  double avgStartY = (fPhotonCount > 0) ? (fStartY / fPhotonCount) : 0.0;
  double avgStartZ = (fPhotonCount > 0) ? (fStartZ / fPhotonCount) : 0.0;

  analysisManager->FillNtupleDColumn(41, avgStartX);
  analysisManager->FillNtupleDColumn(42, avgStartY);
  analysisManager->FillNtupleDColumn(43, avgStartZ);
  
  analysisManager->AddNtupleRow();
  
  // 데이터 저장 후 변수들 초기화
  ResetRunVariables();
}

void RunAction::printEventproc()
{
  fEvent += 1;
  if(fEvent % 1000 == 0) {
    G4cout << " Events clear : " << fEvent << G4endl;
  }
}
