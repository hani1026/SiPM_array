// ActionInitialization.cc
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SiPMSensitiveDetector.hh"
#include "G4SDManager.hh"
#include "Construction.hh"

ActionInitialization::ActionInitialization(G4String fileName)
  : G4VUserActionInitialization(), fFileName(fileName)
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const {
  SetupRunAction();
}

void ActionInitialization::Build() const {
  // 모든 액션 클래스 설정
  SetupGeneratorAction();
  RunAction* runAction = SetupRunAction();
  EventAction* eventAction = SetupEventAction(runAction);
  SetupSteppingAction(eventAction);
  SetupSensitiveDetector(eventAction);
}

void ActionInitialization::SetupGeneratorAction() const {
  SetUserAction(new PrimaryGeneratorAction());
}

RunAction* ActionInitialization::SetupRunAction() const {
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);
  return runAction;
}

EventAction* ActionInitialization::SetupEventAction(RunAction* runAction) const {
  EventAction* eventAction = new EventAction(runAction);
  SetUserAction(eventAction);
  return eventAction;
}

void ActionInitialization::SetupSteppingAction(EventAction* eventAction) const {
  SetUserAction(new SteppingAction(eventAction));
}

void ActionInitialization::SetupSensitiveDetector(EventAction* eventAction) const {
  // 민감 검출기 등록
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  SiPMSensitiveDetector* sipmSD = new SiPMSensitiveDetector("SiPMSensitiveDetector", eventAction);
  sdManager->AddNewDetector(sipmSD);

  // SiPM 논리 볼륨에 민감 검출기 할당
  AssignSensitiveDetector(sipmSD);
}

void ActionInitialization::AssignSensitiveDetector(SiPMSensitiveDetector* sipmSD) const {
  auto detector = dynamic_cast<const MyDetectorConstruction*>(
    G4RunManager::GetRunManager()->GetUserDetectorConstruction()
  );
    
  if (detector) {
    G4String sipmLogicalName = "logical_sipm";
    G4LogicalVolume* sipmLogical = 
      G4LogicalVolumeStore::GetInstance()->GetVolume(sipmLogicalName, false);
    if (sipmLogical) {
      sipmLogical->SetSensitiveDetector(sipmSD);
    }
  }
}
