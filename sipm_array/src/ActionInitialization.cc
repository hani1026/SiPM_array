// ActionInitialization.cc
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SiPMSensitiveDetector.hh"
#include "G4SDManager.hh"

ActionInitialization::ActionInitialization(G4String fileName)
  : G4VUserActionInitialization(), fFileName(fileName)
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const {
  // Master thread에서는 RunAction만 설정
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);
}

void ActionInitialization::Build() const {
  // PrimaryGeneratorAction 생성 및 설정
  PrimaryGeneratorAction* primaryGenerator = new PrimaryGeneratorAction();
  SetUserAction(primaryGenerator);

  // RunAction 생성 및 설정
  RunAction* runAction = new RunAction(fFileName);
  SetUserAction(runAction);

  // EventAction 생성 및 RunAction 전달
  EventAction* eventAction = new EventAction(runAction);
  SetUserAction(eventAction);

  // SteppingAction 생성 및 EventAction 전달
  SteppingAction* steppingAction = new SteppingAction(eventAction);
  SetUserAction(steppingAction);

  // 민감 검출기 등록
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  SiPMSensitiveDetector* sipmSD = new SiPMSensitiveDetector("SiPMSensitiveDetector", eventAction);
  sdManager->AddNewDetector(sipmSD);

  // SiPM 논리 볼륨에 민감 검출기 할당
  // 여기서는 DetectorConstruction 클래스에서 논리 SiPM 볼륨을 "logical_sipm"으로 정의했다고 가정
  const MyDetectorConstruction* detector = static_cast<const MyDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if (detector) {
    for(int i = 0; i < 40; ++i){
      G4String sipmLogicalName = "logical_sipm";
      G4LogicalVolume* sipmLogical = G4LogicalVolumeStore::GetInstance()->GetVolume(sipmLogicalName, false);
      if (sipmLogical) {
	sipmLogical->SetSensitiveDetector(sipmSD);
      }
    }
  }
}
