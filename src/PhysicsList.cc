#include "PhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4Scintillation.hh"

MyPhysicsList::MyPhysicsList()
  : G4VModularPhysicsList()
{
  SetVerboseLevel(1);
  InitializePhysics();
}

MyPhysicsList::~MyPhysicsList()
{}

void MyPhysicsList::InitializePhysics()
{
  // 전자기 물리 프로세스 등록
  RegisterPhysics(new G4EmStandardPhysics());
  
  // 광학 물리 프로세스 설정
  ConstructOp();
}

void MyPhysicsList::SetCuts()
{
  // 기본 컷 값 설정
  SetCutsWithDefault();
  
  // 광자에 대한 특별한 컷 설정
  SetCutValue(0.1*nanometer, "gamma");
  SetCutValue(0.1*nanometer, "opticalphoton");
  
}

void MyPhysicsList::ConstructOp()
{
  G4OpticalParameters* params = G4OpticalParameters::Instance();
  
  // 체렌코프 효과 비활성화
  params->SetCerenkovStackPhotons(false);
  params->SetCerenkovMaxPhotonsPerStep(0);
  
  // 섬광 과정 설정
  params->SetScintStackPhotons(true);
  params->SetScintYieldFactor(1.0);
  params->SetScintExcitationRatio(0.0);
  params->SetScintTrackSecondariesFirst(true);
  
  // 광학 물리 프로세스 등록
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  RegisterPhysics(opticalPhysics);
}
