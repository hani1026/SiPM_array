#ifndef Construction_h
#define Construction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VisAttributes.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MyDetectorConstruction();
  virtual ~MyDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct() override;
  G4LogicalVolume* GetSiPMVolume() const { return flogical_SiPM; }

private:
  void DefineMaterials();
  void SetMaterialProperties();
  void SetAirProperties(G4double* energies, G4int nEntries);
  void SetESRProperties(G4double* energies, G4int nEntries);
  G4VPhysicalVolume* CreateWorldVolume();
  void CreateDetectorComponents(G4VPhysicalVolume* worldPhys);
  void CreateESRBox(G4VPhysicalVolume* worldPhys);
  void CreatePlasticScintillator();
  void CreateSiPMArray();
  void PlaceSiPMDetectors(G4double SD_width);
  void SetupOpticalSurfaces();
  void SetupVisualizationAttributes();

  // 멤버 변수
  G4LogicalVolume* flogical_SiPM;
  G4LogicalVolume* fLogicalESR;
  G4LogicalVolume* fLogicalPS;
  
  G4Material* fMat_Air;
  G4Material* fMat_ESR;
  G4Material* fPlasticSc;
  
  const G4bool fCheckOverlaps = true;
  G4LogicalBorderSurface* fESR_PS_Surface;
  G4LogicalBorderSurface* fPS_ESR_Surface;
  G4VPhysicalVolume* fPhysical_PS;
  G4VPhysicalVolume* fPhysical_ESR;
};

#endif