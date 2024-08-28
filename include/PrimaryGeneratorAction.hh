#ifndef PRIMARYGENERATORACTION_h
#define PRIMARYGENERATORACTION_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

class G4GeneralParicleSource;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction() override;

  virtual void GeneratePrimaries(G4Event*) override;

private:
  G4GeneralParticleSource *fParticleGun;
};

#endif
