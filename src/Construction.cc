#include "Construction.hh"
#include "G4PhysicalVolumeStore.hh"

MyDetectorConstruction::MyDetectorConstruction()
  : G4VUserDetectorConstruction(), flogical_SiPM(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction()
{
  for (auto visAtt : fVisAttributes) {
    delete visAtt;
  }
}

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
  if (!nist) {
    G4Exception("MyDetectorConstruction::DefineMaterials",
                "MyCode001", FatalException,
                "Failed to get NIST manager instance");
    return;
  }
  
  fMat_Air = nullptr;
  fMat_ESR = nullptr;
  fPlasticSc = nullptr;
  fSiPM_Mat = nullptr;
  
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
  
  // SiPM 물질 정의 (실리콘 기반)
  fSiPM_Mat = nist->FindOrBuildMaterial("G4_Si");
  
  SetMaterialProperties();
}

void MyDetectorConstruction::SetMaterialProperties()
{
  
  const G4int NUMENTRIES = 2;
  G4double PhotonEnergy[NUMENTRIES] = {2.0*eV, 3.5*eV};
  
  // 플라스틱 섬광체
  G4double RINDEX_PlasticSc[NUMENTRIES] = {1.58, 1.58};
  
  // ESR
  G4double RINDEX_ESR[NUMENTRIES] = {1.5, 1.5};
  
  // Air
  G4double RINDEX_AIR[NUMENTRIES] = {1.0, 1.0};
  
  // 플라스틱 섬광체의 굴절률
  G4double ABSORPTION_PlasticSc[NUMENTRIES];
  std::fill_n(ABSORPTION_PlasticSc, NUMENTRIES, 380*cm);
  
  // EJ-200의 발광 스펙트럼 정의
  const G4int NUMENTRIES_SCINT = 9;
  G4double SCINT_Energy[NUMENTRIES_SCINT] = {
    2.27*eV, 2.48*eV, 2.68*eV, 2.88*eV, 3.10*eV,
    3.26*eV, 3.44*eV, 3.64*eV, 3.75*eV
  };
  
  G4double SCINT_FAST[NUMENTRIES_SCINT] = {
    0.1, 0.3, 0.6, 0.9, 1.0,
    0.8, 0.5, 0.2, 0.1
  };

  G4double SCINT_SLOW[NUMENTRIES_SCINT] = {
    0.01, 0.03, 0.10, 0.21, 0.24,
    0.16, 0.08, 0.02, 0.00
  };

  // EJ-200 특성
  G4double temperature = 293.15*kelvin;  // 실험 환경 온도
  G4double referenceTemp = 298.15*kelvin;
  G4double tempCoeff = -0.0023;  // EJ-200의 온도 계수 (-0.23%/°C)
  G4double tempCorrection = 1.0 + tempCoeff * (temperature - referenceTemp);

  auto PlasticSc_mt = new G4MaterialPropertiesTable();
  
  // 광학 특성
  PlasticSc_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  PlasticSc_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_PlasticSc, NUMENTRIES);
  
  // 섬광 특성
  PlasticSc_mt->AddProperty("FASTCOMPONENT", SCINT_Energy, SCINT_FAST, NUMENTRIES_SCINT);
  PlasticSc_mt->AddProperty("SLOWCOMPONENT", SCINT_Energy, SCINT_SLOW, NUMENTRIES_SCINT);
  
  // EJ-200 기준 값들
  PlasticSc_mt->AddConstProperty("SCINTILLATIONYIELD", 10000.*tempCorrection/MeV);  // 10,000 ph/MeV
  PlasticSc_mt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  PlasticSc_mt->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);    // EJ-200 decay time
  PlasticSc_mt->AddConstProperty("SLOWTIMECONSTANT", 13.8*ns);   // EJ-200 slow component
  PlasticSc_mt->AddConstProperty("YIELDRATIO", 0.91);            // fast component 비율
  
  // EJ-200의 Birks 상수
  PlasticSc_mt->AddConstProperty("BIRKS_CONSTANT", 0.126*mm/MeV);

  fPlasticSc->SetMaterialPropertiesTable(PlasticSc_mt);
  
  // Air 속성 설정
  SetAirProperties(PhotonEnergy, NUMENTRIES);
  
  // ESR 속성 설정
  G4double absorption[2] = {100*m, 100*m};   // ESR의 흡수 길이
  
  G4MaterialPropertiesTable* esrProperties = new G4MaterialPropertiesTable();
  esrProperties->AddProperty("RINDEX", PhotonEnergy, RINDEX_ESR, NUMENTRIES);
  esrProperties->AddProperty("ABSLENGTH", PhotonEnergy, absorption, NUMENTRIES);
  
  fMat_ESR->SetMaterialPropertiesTable(esrProperties);
  
  // SiPM 물질의 광학 속성 설정
  G4double RINDEX_SiPM[NUMENTRIES];
  std::fill_n(RINDEX_SiPM, NUMENTRIES, 1.55);  // SiPM의 굴절률 설정
  
  G4MaterialPropertiesTable* SiPM_mt = new G4MaterialPropertiesTable();
  SiPM_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_SiPM, NUMENTRIES);
  fSiPM_Mat->SetMaterialPropertiesTable(SiPM_mt);  // fSiPM_Mat는 SiPM 물질

  // SiPM 광학 특성 설정 (파장에 따른 PDE 반영)
  const G4int NUMENTRIES_PDE = 9;
  G4double PDE_Energy[NUMENTRIES_PDE] = {
      1.77*eV, 2.13*eV, 2.58*eV, 2.95*eV, 3.26*eV,
      3.54*eV, 3.87*eV, 4.27*eV, 4.59*eV
  };

  G4double PDE_Efficiency[NUMENTRIES_PDE] = {
      0.08, 0.19, 0.35, 0.40, 0.37,
      0.30, 0.15, 0.05, 0.02
  };

  G4MaterialPropertiesTable* sipmProperties = new G4MaterialPropertiesTable();
  sipmProperties->AddProperty("EFFICIENCY", PDE_Energy, PDE_Efficiency, NUMENTRIES_PDE);
  fSiPM_Mat->SetMaterialPropertiesTable(sipmProperties);
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
  flogical_SiPM = new G4LogicalVolume(solidSiPM, fSiPM_Mat, "logical_sipm");
  
  PlaceSiPMDetectors(SD_width);
}

void MyDetectorConstruction::PlaceSiPMDetectors(G4double SD_width)
{
  // SiPM 위치 조정
  G4double border_gap = 2.0 * cm;
  G4double z_offset = 1.0 * cm;  // 섬광체 표면에 정확히 위치하도록 조정

  G4double max_x = (100 * cm) / 2.0 - border_gap - SD_width / 2.0;
  G4double min_x = -max_x;
  G4double max_y = (50 * cm) / 2.0 - border_gap - SD_width / 2.0;
  G4double min_y = -max_y;
  
  G4double x_spacing = (2 * max_x) / 9.0;
  G4double y_spacing = (2 * max_y) / 3.0;
  
  for (G4int row = 0; row < 4; row++) {
    for (G4int col = 0; col < 10; col++) {
      G4double x_pos = min_x + col * x_spacing;
      G4double y_pos = min_y + row * y_spacing;
      
      std::ostringstream name;
      name << "SiPM_" << (row * 10 + col);
      
      fPhysical_SiPM = new G4PVPlacement(0,
                                        G4ThreeVector(x_pos, y_pos, z_offset),
                                        flogical_SiPM,
                                        name.str(),
                                        fLogicalPS,
                                        false,
                                        row * 10 + col,
                                        false);
    }
  }
}

void MyDetectorConstruction::SetupOpticalSurfaces()
{
  // 광학 표면 속성 캐싱
  static G4OpticalSurface* PSOpticalSurface = nullptr;
  if (!PSOpticalSurface) {
    PSOpticalSurface = new G4OpticalSurface("PSOpticalSurface");
    PSOpticalSurface->SetType(dielectric_dielectric);
    PSOpticalSurface->SetModel(unified);
    PSOpticalSurface->SetFinish(groundfrontpainted);
    PSOpticalSurface->SetSigmaAlpha(0.1);
  }
  
  // ESR 반사체 표면
  auto ESROpticalSurface = new G4OpticalSurface("ESROpticalSurface");
  ESROpticalSurface->SetType(dielectric_metal);
  ESROpticalSurface->SetModel(unified);
  ESROpticalSurface->SetFinish(polished);  // ESR은 매우 매끄러운 표면
  
  // 파장에 따른 ESR 반사율 (실제 3M ESR 데이터 기반)
  const G4int NUM_REFL = 9;
  G4double REFL_energy[NUM_REFL] = {
    2.27*eV, 2.48*eV, 2.68*eV, 2.88*eV, 3.10*eV,
    3.26*eV, 3.44*eV, 3.64*eV, 3.75*eV
  };
  
  G4double ESR_reflectivity[NUM_REFL] = {
    0.988, 0.989, 0.989, 0.989, 0.988,
    0.987, 0.986, 0.984, 0.983
  };  // ESR의 파장별 반사율
  
  G4double ESR_specularlobe[NUM_REFL];
  G4double ESR_specularspike[NUM_REFL];
  G4double ESR_backscatter[NUM_REFL];
  
  for(G4int i = 0; i < NUM_REFL; i++) {
    ESR_specularlobe[i] = 0.05;    // 약간의 lobe 반사
    ESR_specularspike[i] = 0.94;   // 대부분 정반사
    ESR_backscatter[i] = 0.01;     // 미세한 후방 산란
  }
  
  auto ESRProperties = new G4MaterialPropertiesTable();
  ESRProperties->AddProperty("REFLECTIVITY", REFL_energy, ESR_reflectivity, NUM_REFL);
  ESRProperties->AddProperty("SPECULARLOBECONSTANT", REFL_energy, ESR_specularlobe, NUM_REFL);
  ESRProperties->AddProperty("SPECULARSPIKECONSTANT", REFL_energy, ESR_specularspike, NUM_REFL);
  ESRProperties->AddProperty("BACKSCATTERCONSTANT", REFL_energy, ESR_backscatter, NUM_REFL);
  
  ESROpticalSurface->SetMaterialPropertiesTable(ESRProperties);
  
  // SiPM 표면
  auto SiPMOpticalSurface = new G4OpticalSurface("SiPMOpticalSurface");
  SiPMOpticalSurface->SetType(dielectric_metal);
  SiPMOpticalSurface->SetModel(unified);
  SiPMOpticalSurface->SetFinish(polished);
  
  // 에폭시 레이어와 실리콘 경계면의 특성 반영
  G4double SiPM_reflectivity[NUM_REFL];
  G4double SiPM_specularlobe[NUM_REFL];
  G4double SiPM_specularspike[NUM_REFL];
  
  for(G4int i = 0; i < NUM_REFL; i++) {
    SiPM_reflectivity[i] = 0.20;     // 약 20% 반사
    SiPM_specularlobe[i] = 0.80;     // 대부분 lobe 반사
    SiPM_specularspike[i] = 0.19;    // 일부 정반사
  }
  
  auto SiPMProperties = new G4MaterialPropertiesTable();
  SiPMProperties->AddProperty("REFLECTIVITY", REFL_energy, SiPM_reflectivity, NUM_REFL);
  SiPMProperties->AddProperty("SPECULARLOBECONSTANT", REFL_energy, SiPM_specularlobe, NUM_REFL);
  SiPMProperties->AddProperty("SPECULARSPIKECONSTANT", REFL_energy, SiPM_specularspike, NUM_REFL);
  
  SiPMOpticalSurface->SetMaterialPropertiesTable(SiPMProperties);
  
  // 표면 속성을 볼륨에 적용
  new G4LogicalBorderSurface("ESR_PS_Surface", fPhysical_ESR, fPhysical_PS, ESROpticalSurface);
  new G4LogicalBorderSurface("PS_ESR_Surface", fPhysical_PS, fPhysical_ESR, ESROpticalSurface);
  new G4LogicalBorderSurface("SiPM_Surface", fPhysical_PS, fPhysical_SiPM, SiPMOpticalSurface);
  
  // SiPM 표면 속성 디버그 출력
  G4MaterialPropertiesTable* sipmProps = 
    SiPMOpticalSurface->GetMaterialPropertiesTable();
  
  G4cout << "SiPM Optical Properties:" << G4endl;
  if(sipmProps) {
    G4MaterialPropertyVector* effVec = 
      sipmProps->GetProperty("EFFICIENCY");
    if(effVec) {
      for(size_t i = 0; i < effVec->GetVectorLength(); i++) {
        G4cout << "Energy = " << effVec->Energy(i)/eV 
               << " eV, Efficiency = " 
               << (*effVec)[i] << G4endl;
      }
    }
  }
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
  
  fVisAttributes.push_back(worldVisAtt);
  fVisAttributes.push_back(esrVisAtt);
  fVisAttributes.push_back(psVisAtt);
  fVisAttributes.push_back(sipmVisAtt);
}
