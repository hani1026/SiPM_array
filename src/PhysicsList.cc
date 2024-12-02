#include "PhysicsList.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4HadronicProcessStore.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmParameters.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  // 기본 컷값과 verbose 레벨 설정
  defaultCutValue = 0.1*mm;
  SetVerboseLevel(0);
  
  // 광학 파라미터 초기화 - 다른 프로세스보다 먼저
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetVerboseLevel(0);
  
  // 기본 물리 프로세스 등록
  RegisterPhysics(new G4EmStandardPhysics_option4(0));
  RegisterPhysics(new G4DecayPhysics(0));
  RegisterPhysics(new G4RadioactiveDecayPhysics(0));
  
  // 하드로닉 물리 등록
  RegisterPhysics(new G4HadronElasticPhysics(0));
  RegisterPhysics(new G4HadronPhysicsFTFP_BERT(0));
  RegisterPhysics(new G4StoppingPhysics(0));
  RegisterPhysics(new G4IonPhysics(0));
  
  // 광학 물리는 마지막에 등록
  auto optical = new G4OpticalPhysics(0);
  optical->Configure(kCerenkov, true);
  optical->Configure(kScintillation, true);
  optical->Configure(kAbsorption, true);
  optical->Configure(kRayleigh, true);
  optical->Configure(kMieHG, true);
  optical->Configure(kBoundary, true);
  optical->Configure(kWLS, true);
  optical->SetScintillationYieldFactor(1.0);
  optical->SetScintillationExcitationRatio(0.0);
  optical->SetTrackSecondariesFirst(kScintillation, true);
  optical->SetMaxNumPhotonsPerStep(100);
  optical->SetMaxBetaChangePerStep(10.0);
  optical->SetTrackSecondariesFirst(kCerenkov, true);
  RegisterPhysics(optical);
  
  // 하드로닉 프로세스 출력 레벨 설정
  G4HadronicProcessStore::Instance()->SetVerbose(0);
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::SetCuts()
{
  SetCutsWithDefault();
  
  // 입자별 컷 설정
  SetCutValue(defaultCutValue, "gamma");
  SetCutValue(defaultCutValue, "e-");
  SetCutValue(defaultCutValue, "e+");
  SetCutValue(defaultCutValue, "proton");
  SetCutValue(0.1*mm, "mu-");
  SetCutValue(0.1*mm, "mu+");
}

void PhysicsList::ConstructProcess()
{
  // 기본 프로세스 구성
  G4VModularPhysicsList::ConstructProcess();
  
  // EM 파라미터 설정
  G4EmParameters* emParams = G4EmParameters::Instance();
  emParams->SetMuHadLateralDisplacement(true);  // 뮤온 측면 변위 활성화
  emParams->SetFluo(true);                      // 형광 방출 활성화
  emParams->SetAuger(true);                     // Auger 전자 방출 활성화
  emParams->SetPixe(true);                      // PIXE 활성화
  
  // 다중 산란 파라미터 조정
  emParams->SetMscStepLimitType(fUseSafety);
  emParams->SetMscRangeFactor(0.2);
  emParams->SetMscGeomFactor(2.5);
  
  // 이온화 에너지 손실의 변동 모델링
  emParams->SetLossFluctuations(true);
  emParams->SetLinearLossLimit(0.01);           // 정확한 에너지 손실 계산
  
  // 제동 복사 및 쌍생성 임계값
  emParams->SetBremsstrahlungTh(0.1*GeV);      // 제동 복사 임계값
  emParams->SetMinEnergy(0.1*GeV);             // 쌍생성 임계값
  
  // 디버그 출력 활성화
  G4OpticalParameters::Instance()->SetVerboseLevel(0);
}
