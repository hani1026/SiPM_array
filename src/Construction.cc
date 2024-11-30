#include "Construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
  : G4VUserDetectorConstruction(), flogical_SiPM(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
  DefineMaterials();
  G4VPhysicalVolume* worldPhys = CreateWorldVolume();
  CreateDetectorComponents(worldPhys);
  SetupOpticalSurfaces();
  SetupVisualizationAttributes();
  return worldPhys;
}

void MyDetectorConstruction::DefineMaterials()
{
  G4NistManager* nist = G4NistManager::Instance();
  
  // Air 정의
  fMat_Air = nist->FindOrBuildMaterial("G4_AIR");
  
  // ESR 정의
  fMat_ESR = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  
  // Plastic Scintillator 정의
  fPlasticSc = new G4Material("PLASTIC_SC", 1.023*g/cm3, 2);
  G4Element* El_C = nist->FindOrBuildElement("C");
  G4Element* El_H = nist->FindOrBuildElement("H");
  fPlasticSc->AddElement(El_H, 0.524340);
  fPlasticSc->AddElement(El_C, 0.475659);
  
  SetMaterialProperties();
}

void MyDetectorConstruction::SetMaterialProperties()
{
  const G4int NUMENTRIES = 12;
  G4double PhotonEnergy[NUMENTRIES] = {
    2.08*eV, 2.38*eV, 2.58*eV, 2.7*eV, 2.76*eV,
    2.82*eV, 2.92*eV, 2.95*eV, 3.02*eV, 3.1*eV,
    3.26*eV, 3.44*eV
  };
  
  // 플라스틱 섬광체 속성
  G4double RINDEX_PlasticSc[NUMENTRIES];
  G4double ABSORPTION_PlasticSc[NUMENTRIES];
  std::fill_n(RINDEX_PlasticSc, NUMENTRIES, 1.58);
  std::fill_n(ABSORPTION_PlasticSc, NUMENTRIES, 380*cm);
  
  auto PlasticSc_mt = new G4MaterialPropertiesTable();
  PlasticSc_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  PlasticSc_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_PlasticSc, NUMENTRIES);
  fPlasticSc->SetMaterialPropertiesTable(PlasticSc_mt);
  
  // Air 속성 설정
  SetAirProperties(PhotonEnergy, NUMENTRIES);
  
  // ESR 속성 설정
  SetESRProperties(PhotonEnergy, NUMENTRIES);
}

G4VPhysicalVolume* MyDetectorConstruction::CreateWorldVolume()
{
  G4double world_size = 3.0 * m;
  
  G4Box* solidWorld = new G4Box("World", world_size, world_size, world_size);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, fMat_Air, "World");
  
  G4VPhysicalVolume* physical_Air = new G4PVPlacement(0,
                                                    G4ThreeVector(),
                                                    logicWorld,
                                                    "World",
                                                    0,
                                                    false,
                                                    0,
                                                    fCheckOverlaps);
  
  return physical_Air;
}

void MyDetectorConstruction::CreateDetectorComponents(G4VPhysicalVolume* worldPhys)
{
  // ESR Box 생성
  CreateESRBox(worldPhys);
  
  // Plastic Scintillator 생성
  CreatePlasticScintillator();
  
  // SiPM 배열 생성
  CreateSiPMArray();
}

void MyDetectorConstruction::CreateESRBox(G4VPhysicalVolume* worldPhys)
{
  G4double ESR_x = 200 * cm;
  G4double ESR_y = 100 * cm;
  G4double ESR_z = 20 * cm;
  
  G4Box* solidESR = new G4Box("ESR", 0.5 * ESR_x, 0.5 * ESR_y, 0.5 * ESR_z);
  fLogicalESR = new G4LogicalVolume(solidESR, fMat_ESR, "ESR");
  
  fPhysical_ESR = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicalESR,
                                   "ESR",
                                   worldPhys->GetLogicalVolume(),
                                   false,
                                   0,
                                   fCheckOverlaps);
}

void MyDetectorConstruction::CreatePlasticScintillator()
{
  G4double PS_x = 100 * cm;
  G4double PS_y = 50 * cm;
  G4double PS_z = 2 * cm;
  
  G4Box* solidPS = new G4Box("PS", 0.5 * PS_x, 0.5 * PS_y, 0.5 * PS_z);
  fLogicalPS = new G4LogicalVolume(solidPS, fPlasticSc, "PS");
  
  fPhysical_PS = new G4PVPlacement(0,
                                  G4ThreeVector(),
                                  fLogicalPS,
                                  "PS",
                                  fLogicalESR,
                                  false,
                                  0,
                                  fCheckOverlaps);
}

void MyDetectorConstruction::CreateSiPMArray()
{
  G4double SD_width = 0.6 * cm;
  G4double SD_height = 0.01 * cm;
  
  G4Box* solidSiPM = new G4Box("SiPM", 0.5 * SD_width, 0.5 * SD_width, 0.5 * SD_height);
  flogical_SiPM = new G4LogicalVolume(solidSiPM, fPlasticSc, "logical_sipm");
  
  PlaceSiPMDetectors(SD_width);
}

void MyDetectorConstruction::PlaceSiPMDetectors(G4double SD_width)
{
  G4double border_gap = 2 * cm;
  G4double max_x = (100 * cm) / 2.0 - border_gap - SD_width / 2.0;
  G4double min_x = -max_x;
  G4double max_y = (50 * cm) / 2.0 - border_gap - SD_width / 2.0;
  G4double min_y = -max_y;
  
  G4double x_spacing = (2 * max_x) / 9.0;  // 10개의 SiPM을 위한 간격
  G4double y_spacing = (2 * max_y) / 3.0;  // 4행을 위한 간격
  
  for (G4int row = 0; row < 4; row++) {
    for (G4int col = 0; col < 10; col++) {
      G4double x_pos = min_x + col * x_spacing;
      G4double y_pos = min_y + row * y_spacing;
      
      std::ostringstream name;
      name << "SiPM_" << (row * 10 + col);
      
      new G4PVPlacement(0,
                       G4ThreeVector(x_pos, y_pos, 1.0 * cm),
                       flogical_SiPM,
                       name.str(),
                       fLogicalPS,
                       false,
                       row * 10 + col,
                       fCheckOverlaps);
    }
  }
}

void MyDetectorConstruction::SetupOpticalSurfaces()
{
  // ESR 광학 표면 속성 설정
  G4OpticalSurface* ESROpticalSurface = new G4OpticalSurface("ESRSurface");
  ESROpticalSurface->SetType(dielectric_metal);
  ESROpticalSurface->SetFinish(polished);
  ESROpticalSurface->SetModel(unified);
  
  // ESR 반사율 설정
  const G4int NUM = 2;
  G4double pp[NUM] = {2.0*eV, 3.5*eV};
  G4double reflectivity[NUM] = {0.98, 0.98};
  G4MaterialPropertiesTable* ESROpticalProperties = new G4MaterialPropertiesTable();
  ESROpticalProperties->AddProperty("REFLECTIVITY", pp, reflectivity, NUM);
  ESROpticalSurface->SetMaterialPropertiesTable(ESROpticalProperties);

  // PS 광학 표면 속성 설정
  G4OpticalSurface* PSOpticalSurface = new G4OpticalSurface("PSSurface");
  PSOpticalSurface->SetType(dielectric_dielectric);
  PSOpticalSurface->SetFinish(polished);
  PSOpticalSurface->SetModel(unified);
  
  // PS 표면 속성 설정
  G4MaterialPropertiesTable* PSOpticalProperties = new G4MaterialPropertiesTable();
  G4double specularlobe[NUM] = {0.3, 0.3};
  G4double specularspike[NUM] = {0.2, 0.2};
  G4double backscatter[NUM] = {0.1, 0.1};
  PSOpticalProperties->AddProperty("SPECULARLOBECONSTANT", pp, specularlobe, NUM);
  PSOpticalProperties->AddProperty("SPECULARSPIKECONSTANT", pp, specularspike, NUM);
  PSOpticalProperties->AddProperty("BACKSCATTERCONSTANT", pp, backscatter, NUM);
  PSOpticalSurface->SetMaterialPropertiesTable(PSOpticalProperties);

  // 경계면 설정
  fESR_PS_Surface = new G4LogicalBorderSurface("AirSurface", fPhysical_PS, fPhysical_ESR, ESROpticalSurface);
  fPS_ESR_Surface = new G4LogicalBorderSurface("PSSurface", fPhysical_ESR, fPhysical_PS, PSOpticalSurface);
}

void MyDetectorConstruction::SetAirProperties(G4double* energies, G4int nEntries)
{
  // Air의 광학 속성 설정
  G4double refractiveIndex[2] = {1.0, 1.0};  // Air의 굴절률
  
  G4MaterialPropertiesTable* airProperties = new G4MaterialPropertiesTable();
  airProperties->AddProperty("RINDEX", energies, refractiveIndex, nEntries);
  
  fMat_Air->SetMaterialPropertiesTable(airProperties);
}

void MyDetectorConstruction::SetESRProperties(G4double* energies, G4int nEntries)
{
  // ESR의 광학 속성 설정
  G4double refractiveIndex[2] = {1.5, 1.5};  // ESR의 굴절률
  G4double absorption[2] = {100*m, 100*m};   // ESR의 흡수 길이
  
  G4MaterialPropertiesTable* esrProperties = new G4MaterialPropertiesTable();
  esrProperties->AddProperty("RINDEX", energies, refractiveIndex, nEntries);
  esrProperties->AddProperty("ABSLENGTH", energies, absorption, nEntries);
  
  fMat_ESR->SetMaterialPropertiesTable(esrProperties);
}

void MyDetectorConstruction::SetupVisualizationAttributes()
{
  // 시각화 속성 설정
  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.1));
  worldVisAtt->SetVisibility(true);
  
  G4VisAttributes* esrVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
  esrVisAtt->SetVisibility(true);
  
  G4VisAttributes* psVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  psVisAtt->SetVisibility(true);
  
  G4VisAttributes* sipmVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  sipmVisAtt->SetVisibility(true);
  
  // 논리 볼륨에 시각화 속성 할당
  if (fLogicalESR) fLogicalESR->SetVisAttributes(esrVisAtt);
  if (fLogicalPS) fLogicalPS->SetVisAttributes(psVisAtt);
  if (flogical_SiPM) flogical_SiPM->SetVisAttributes(sipmVisAtt);
}
