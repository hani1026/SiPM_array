#ifndef Construction_h
#define Construction_h

#include "G4VUserDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4UnionSolid.hh"

#include "G4MaterialTable.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4VisAttributes.hh" 

#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MyDetectorConstruction();
  virtual ~MyDetectorConstruction() override;
  
  virtual G4VPhysicalVolume* Construct() override;

  virtual G4LogicalVolume* GetSiPMVolume() const { return flogical_SiPM; }

protected:

  G4LogicalVolume* flogical_SiPM = nullptr;

private:

};
#endif
