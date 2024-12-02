#include <iostream>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

#include "Construction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
  // 명령행 인자 처리
  if (argc < 2) {
    G4cerr << "Usage: " << argv[0] << " <macro> [output_name] [seed]" << G4endl;
    return 1;
  }

  // 기본값 설정
  G4String outputFileName = "SiPM";
  G4long seedNum = 42;

  // 선택적 인자 처리
  if (argc > 2) outputFileName = argv[2];
  if (argc > 3) seedNum = std::stol(argv[3]);

  // UI 매니저 초기화
  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // 난수 시드 설정
  G4Random::setTheSeed(seedNum);

  // Run 매니저 초기화 전에 예외 처리 추가
  auto* runManager = G4RunManager::GetRunManager();
  if (!runManager) {
    runManager = new G4RunManager();
  }
  
  try {
    // 검출기 구성
    auto* detConstruction = new MyDetectorConstruction();
    runManager->SetUserInitialization(detConstruction);

    // 물리 프로세스 설정
    auto* physicsList = new PhysicsList();
    runManager->SetUserInitialization(physicsList);

    // 액션 초기화
    runManager->SetUserInitialization(new ActionInitialization(outputFileName));
    
    runManager->Initialize();
    
  } catch (const std::exception& e) {
    G4cerr << "Exception caught: " << e.what() << G4endl;
    delete runManager;
    return 1;
  }

  // 시각화 매니저 설정
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  // UI 매니저 설정
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // 매크로 파일 실행 또는 대화형 모드 시작
  if (!ui) {
    // 배치 모드
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + argv[1]);
  } else {
    // 대화형 모드
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // 정리
  delete visManager;
  delete runManager;

  return 0;
}
