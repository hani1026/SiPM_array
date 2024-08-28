#include <iostream>
#include "G4RunManager.hh"

#include "G4RandomTools.hh"
#include "Randomize.hh"

#include "Construction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


#include <cstdlib>

int main(int argc, char** argv)
{

  G4String outputFileName = "Hawl.root";
  G4long seednum = 42;
  if (argc > 3){
    outputFileName = argv[2];
    seednum = std::stoi(argv[3]); 
  }


  G4UIExecutive *ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

  G4Random::setTheSeed(seednum);

  // Construct the default run manager
  //auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  G4RunManager * runManager = new G4RunManager();

  MyDetectorConstruction* detectorconstruction = new MyDetectorConstruction();
  runManager->SetUserInitialization(detectorconstruction);

  // Physics List

  G4VModularPhysicsList* physicsList = new MyPhysicsList();
  runManager->SetUserInitialization(physicsList);

  // User action initialization

  ActionInitialization* actioninit = new ActionInitialization(outputFileName);
  runManager->SetUserInitialization(actioninit);

  runManager->Initialize();

  auto visManager = new G4VisExecutive();
  visManager ->Initialize();
  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( ! ui ) 
    {
      // batch mode
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);    
    }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;

  return 0;
}
