#include "RunAction.hh"

RunAction::RunAction(G4String fileName):G4UserRunAction(),fFileName(fileName)
{

  auto analysisManager = G4AnalysisManager::Instance();     
  analysisManager->SetVerboseLevel(1);  
  analysisManager->CreateNtuple("HAWL_tree", "Event_by_Event");
  analysisManager->CreateNtupleDColumn("Event_ID");    // 0
  analysisManager->CreateNtupleDColumn("0_SiPM");      // 1
  analysisManager->CreateNtupleDColumn("1_SiPM");      // 2
  analysisManager->CreateNtupleDColumn("2_SiPM");      // 3
  analysisManager->CreateNtupleDColumn("3_SiPM");      // 4
  analysisManager->CreateNtupleDColumn("4_SiPM");      // 5
  analysisManager->CreateNtupleDColumn("5_SiPM");      // 6
  analysisManager->CreateNtupleDColumn("6_SiPM");      // 7
  analysisManager->CreateNtupleDColumn("7_SiPM");      // 8
  analysisManager->CreateNtupleDColumn("8_SiPM");      // 9
  analysisManager->CreateNtupleDColumn("9_SiPM");      // 10
  analysisManager->CreateNtupleDColumn("10_SiPM");      // 11
  analysisManager->CreateNtupleDColumn("11_SiPM");      // 12
  analysisManager->CreateNtupleDColumn("12_SiPM");      // 13
  analysisManager->CreateNtupleDColumn("13_SiPM");      // 14
  analysisManager->CreateNtupleDColumn("14_SiPM");      // 15
  analysisManager->CreateNtupleDColumn("15_SiPM");      // 16
  analysisManager->CreateNtupleDColumn("16_SiPM");      // 17
  analysisManager->CreateNtupleDColumn("17_SiPM");      // 18
  analysisManager->CreateNtupleDColumn("18_SiPM");      // 19
  analysisManager->CreateNtupleDColumn("19_SiPM");      // 20    
  analysisManager->CreateNtupleDColumn("20_SiPM");      // 21
  analysisManager->CreateNtupleDColumn("21_SiPM");      // 22
  analysisManager->CreateNtupleDColumn("22_SiPM");      // 23
  analysisManager->CreateNtupleDColumn("23_SiPM");      // 24
  analysisManager->CreateNtupleDColumn("24_SiPM");      // 25
  analysisManager->CreateNtupleDColumn("25_SiPM");      // 26
  analysisManager->CreateNtupleDColumn("26_SiPM");      // 27
  analysisManager->CreateNtupleDColumn("27_SiPM");      // 28
  analysisManager->CreateNtupleDColumn("28_SiPM");      // 29
  analysisManager->CreateNtupleDColumn("29_SiPM");      // 30
  analysisManager->CreateNtupleDColumn("30_SiPM");      // 31
  analysisManager->CreateNtupleDColumn("31_SiPM");      // 32
  analysisManager->CreateNtupleDColumn("32_SiPM");      // 33
  analysisManager->CreateNtupleDColumn("33_SiPM");      // 34
  analysisManager->CreateNtupleDColumn("34_SiPM");      // 35
  analysisManager->CreateNtupleDColumn("35_SiPM");      // 36
  analysisManager->CreateNtupleDColumn("36_SiPM");      // 37
  analysisManager->CreateNtupleDColumn("37_SiPM");      // 38
  analysisManager->CreateNtupleDColumn("38_SiPM");      // 39
  analysisManager->CreateNtupleDColumn("39_SiPM");      // 40
  analysisManager->CreateNtupleDColumn("WholePhoton");  // 41
  analysisManager->CreateNtupleDColumn("WholeSiPM");    // 42
  analysisManager->CreateNtupleDColumn("AverageLength");// 43
  analysisManager->CreateNtupleDColumn("startX");       // 44
  analysisManager->CreateNtupleDColumn("startY");       // 45
  analysisManager->CreateNtupleDColumn("startZ");       // 46
    
  analysisManager->FinishNtuple();
}

RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* )
{
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4String fileName = fFileName;
  
  analysisManager->OpenFile(fileName);
  G4cout << "Open File " << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

 
  const MyDetectorConstruction* detConstruction
   = static_cast<const MyDetectorConstruction*>
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  
  // Run conditions

  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  
  const PrimaryGeneratorAction* generatorAction
   = static_cast<const PrimaryGeneratorAction*>
    (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  
  if (IsMaster()) {
    G4cout
      << G4endl
      << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
      << G4endl
      << "--------------------End of Local Run------------------------";
  }

   G4cout
     << G4endl
     << " The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << "------------------------------------------------------------"
     << G4endl;

   // save histograms & ntuple
   analysisManager->Write();
   analysisManager->CloseFile();
}

void RunAction::printEventproc()
{

  fEvent += 1;
  if(fEvent % 10 == 0)
    {
      G4cout<<" Events clear : "<<fEvent <<G4endl;
    }
  
}
