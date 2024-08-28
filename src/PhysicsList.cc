#include "PhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"

MyPhysicsList::MyPhysicsList():G4VModularPhysicsList()
{
  // optical physics
  RegisterPhysics(new G4OpticalPhysics());
  RegisterPhysics(new G4EmStandardPhysics());
}

MyPhysicsList::~MyPhysicsList()
{
}
