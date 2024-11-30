#include "PhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4SystemOfUnits.hh"

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
  // 광학 물리 프로세스 등록
  RegisterPhysics(new G4OpticalPhysics());
  
  // 전자기 물리 프로세스 등록
  RegisterPhysics(new G4EmStandardPhysics());
}

void MyPhysicsList::SetCuts()
{
  // 기본 컷 값 설정
  SetCutsWithDefault();
  
  // 광자에 대한 특별한 컷 설정
  SetCutValue(1*nanometer, "gamma");
}
