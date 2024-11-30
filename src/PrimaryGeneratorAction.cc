#include "PrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(),
    fParticleGun(nullptr)
{
  InitializeParticleGun();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  DeleteParticleGun();
}

void PrimaryGeneratorAction::InitializeParticleGun()
{
  fParticleGun = new G4GeneralParticleSource();
}

void PrimaryGeneratorAction::DeleteParticleGun()
{
  if (fParticleGun) {
    delete fParticleGun;
    fParticleGun = nullptr;
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (!fParticleGun) {
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries",
                "MyCode004", FatalException,
                "Particle gun is not initialized!");
    return;
  }
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
