#ifndef RunAction_h
#define RunAction_h 


#include "PrimaryGeneratorAction.hh"
#include "Construction.hh"


#include "G4UserRunAction.hh"


#include "G4RunManager.hh"
#include "G4Run.hh"
#include "g4root.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

class RunAction : public G4UserRunAction
{
public:

  RunAction(G4String fileName);
  ~RunAction() override;

  
  void BeginOfRunAction(const G4Run*) override;
  void   EndOfRunAction(const G4Run*) override;
  void printEventproc();


private:

  G4String fFileName;
  G4int fEvent =0;
  G4int nofEvents=0;  
};


#endif
