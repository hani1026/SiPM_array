#include "PhysicsList.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  defaultCutValue = 1.0*mm;
  SetVerboseLevel(0);
  
  // 광학 파라미터 초기화
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetVerboseLevel(1);
  
  // 필수 물리 프로세스만 등록
  RegisterPhysics(new G4EmStandardPhysics_option1(0));  // EM 물리
  RegisterPhysics(new G4DecayPhysics(0));              // 붕괴 물리
  
  // 광학 물리 설정
  auto optical = new G4OpticalPhysics(0);
  optical->Configure(kScintillation, true);
  optical->Configure(kAbsorption, true);
  optical->Configure(kBoundary, true);
  
  // 섬광 프로세스 세부 설정
  optical->SetScintillationYieldFactor(1.0);
  optical->SetScintillationExcitationRatio(0.0);
  optical->SetTrackSecondariesFirst(kScintillation, true);
  optical->SetMaxNumPhotonsPerStep(50);
  optical->SetScintillationByParticleType(false);
  optical->SetTrackSecondariesFirst(kCerenkov, false);
  
  RegisterPhysics(optical);
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::SetCuts()
{
  SetCutsWithDefault();
}

void PhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  
  // 광학 프로세스 최적화
  auto params = G4OpticalParameters::Instance();
  params->SetScintTrackSecondariesFirst(true);
  params->SetScintEnhancedTimeConstants(true);
  params->SetBoundaryInvokeSD(true);
  
  // 섬광 수율 관련 설정
  params->SetScintYieldFactor(1.0);
  params->SetScintExcitationRatio(0.0);
}
