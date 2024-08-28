#include "Construction.hh"       

MyDetectorConstruction::MyDetectorConstruction()  
{}

MyDetectorConstruction::~MyDetectorConstruction()  
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()   
{ 

  /////materials       
  
  G4NistManager * nist = G4NistManager::Instance();
  G4Material* Mat_Air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* PlasticSc = new G4Material("PLASTIC_SC", 1.023*g/cm3, 2);
  G4Material* Mat_ESR = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  
  //EJ-200 property
  G4Element* El_C = nist->FindOrBuildElement("C");
  G4Element* El_H = nist->FindOrBuildElement("H"); 
  
  PlasticSc->AddElement(El_H, 0.524340);
  PlasticSc->AddElement(El_C, 0.475659);
  
  G4bool checkoverlaps = true ;
  
  /////world

  G4double world_size = 3 * m ;

  G4Box * solid_World = new G4Box("solid_world",
                                  world_size,
                                  world_size,
                                  world_size);
                          
  G4LogicalVolume * logical_World = new G4LogicalVolume(solid_World,
                                                        Mat_Air,
                                                        "logical_world");

  G4VPhysicalVolume * physical_World = new G4PVPlacement(0,
							 G4ThreeVector(),
							 logical_World,
							 "physical_world",
							 0,
							 false,
							 0,
							 checkoverlaps);
  
  G4double air_size = 3 * m ;

  G4Box * solid_Air = new G4Box("solid_air",
				air_size,
				air_size,
				air_size);
                          
  G4LogicalVolume * logical_Air = new G4LogicalVolume(solid_Air,
						      Mat_Air,
						      "logical_air");

  G4VPhysicalVolume * physical_Air = new G4PVPlacement(0,
						       G4ThreeVector(),
						       logical_Air,
						       "physical_air",
						       logical_World,
						       false,
						       0,
						       checkoverlaps);

  ///// ESR Box

  G4double ESR_x = 200 * cm ;
  G4double ESR_y = 100 * cm ;
  G4double ESR_z = 20 * cm ;

  G4Box * solid_ESR = new G4Box("solid_esr",
                                0.5 * ESR_x,
                                0.5 * ESR_y,
                                0.5 * ESR_z);

  G4LogicalVolume * logical_ESR = new G4LogicalVolume(solid_ESR,
						      Mat_ESR,
						      "logical_esr");

  G4VPhysicalVolume * physical_ESR = new G4PVPlacement(0,
						       G4ThreeVector(),
						       logical_ESR,
						       "physical_esr",
						       logical_Air,
						       false,
						       0,
						       checkoverlaps);

  
  
  ///// PS Box

  G4double PS_x = 100 * cm ;
  G4double PS_y = 50 * cm ;
  G4double PS_z = 2 * cm ;

  G4Box * solid_PS = new G4Box("solid_ps",
			       0.5 * PS_x,
			       0.5 * PS_y,
			       0.5 * PS_z);

  G4LogicalVolume * logical_PS = new G4LogicalVolume(solid_PS,
						     PlasticSc,
						     "logical_ps");

  G4VPhysicalVolume * physical_PS = new G4PVPlacement(0,
						      G4ThreeVector(),
						      logical_PS,
						      "physical_ps",
						      logical_ESR,
						      false,
						      0,
						      checkoverlaps);

  ///// SiPM size
  G4double SD_width = 0.6 * cm;  // 6mm
  G4double SD_height = 0.01 * cm;  // 0.1mm
  
  G4int total_SiPM = 40;  
  G4int num_rows = 4;  // y axis
  G4double x_spacing = 10 * cm;  
  G4double y_spacing = 10 * cm;  

  G4Box* solid_SiPM = new G4Box("solid_sipm", 0.5 * SD_width, 0.5 * SD_width, 0.5 * SD_height);
  flogical_SiPM = new G4LogicalVolume(solid_SiPM, PlasticSc, "logical_sipm");


  // (1) 10cm spacing    
  G4int sipm_placed = 0;
  for (G4int row = 0; row < num_rows; row++) {
    for (G4int col = 0; col < total_SiPM / num_rows; col++) {
      G4double x_pos = (col - (total_SiPM / num_rows) / 2.0 + 0.5) * x_spacing;
      G4double y_pos = (row - (num_rows / 2.0) + 0.5) * y_spacing;

      new G4PVPlacement(0,
        G4ThreeVector(x_pos, y_pos, (0.5 * PS_z - 0.5 * SD_height)),
        flogical_SiPM,
        "SiPM_Phy",
        logical_PS,
        false,
        sipm_placed);
        
      sipm_placed++;

      // 만약 총 SiPM 수에 도달하면 배치 종료
      if (sipm_placed >= total_SiPM) break;
    }
    if (sipm_placed >= total_SiPM) break;
  }

  // (2) maximum spacing
  /*
  
  G4int sipm_placed = 0;
  G4double max_x = PS_x / 2.0 - SD_width / 2.0;
  G4double min_x = -max_x;
  G4double max_y = PS_y / 2.0 - SD_width / 2.0;
  G4double min_y = -max_y;

  G4double x_spacing_max = PS_x / (total_SiPM / num_rows);  // 최대 가로 간격 계산
  G4double y_spacing_max = PS_y / num_rows;  // 최대 세로 간격 계산
    
  for (G4int row = 0; row < num_rows; row++) {
    for (G4int col = 0; col < total_SiPM / num_rows; col++) {
      G4double x_pos = min_x + (col + 0.5) * x_spacing_max;
      G4double y_pos = min_y + (row + 0.5) * y_spacing_max;

      new G4PVPlacement(0,
        G4ThreeVector(x_pos, y_pos, (0.5 * PS_z - 0.5 * SD_height)),
        flogical_SiPM,
        "SiPM_Phy_MaxSpacing",
        logical_PS,
        false,
        sipm_placed);
        
      sipm_placed++;

      // 만약 총 SiPM 수에 도달하면 배치 종료
      if (sipm_placed >= total_SiPM) break;
    }
    if (sipm_placed >= total_SiPM) break;
  }
  */
  




  /////Surface 

  G4OpticalSurface* AirOpticalSurface = new G4OpticalSurface("AirSurface");

  AirOpticalSurface->SetModel(unified);
  AirOpticalSurface->SetType(dielectric_metal);
  AirOpticalSurface->SetFinish(ground);
  
  G4OpticalSurface* ESROpticalSurface = new G4OpticalSurface("ESRSurface");

  ESROpticalSurface->SetModel(unified); 
  ESROpticalSurface->SetType(dielectric_metal);
  ESROpticalSurface->SetFinish(ground);
  
  G4OpticalSurface* PSOpticalSurface = new G4OpticalSurface("PSSurface");

  PSOpticalSurface->SetModel(unified); 
  PSOpticalSurface->SetType(dielectric_metal);
  PSOpticalSurface->SetFinish(ground);
  

  G4LogicalBorderSurface* ESR_PS_Surface = new G4LogicalBorderSurface("AirSurface", physical_PS, physical_ESR, ESROpticalSurface);
  G4LogicalBorderSurface* PS_ESR_Surface = new G4LogicalBorderSurface("PSSurface", physical_ESR, physical_PS, PSOpticalSurface);

  
  G4MaterialPropertiesTable * PlasticSc_mt = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable * PlasticSc_sf = new G4MaterialPropertiesTable();

  G4MaterialPropertiesTable * Air_mt = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable * Air_sf = new G4MaterialPropertiesTable();

  G4MaterialPropertiesTable * ESR_mt = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable * ESR_sf = new G4MaterialPropertiesTable();

  
  const G4int NUMENTRIES = 12; 

  G4double PhotonEnergy[NUMENTRIES] = 
    { 2.08*eV, 2.38*eV, 2.58*eV, 2.7*eV, 2.76*eV,
      2.82*eV, 2.92*eV, 2.95*eV, 3.02*eV, 3.1*eV,
      3.26*eV, 3.44*eV };

  G4double RINDEX_PlasticSc[NUMENTRIES] =
    { 1.58, 1.58, 1.58, 1.58, 1.58,
      1.58, 1.58, 1.58, 1.58, 1.58,
      1.58, 1.58 };

    G4double ABSORPTION_PlasticSc[NUMENTRIES] =
    { 380*cm, 380*cm, 380*cm, 380*cm, 380*cm,
      380*cm, 380*cm, 380*cm, 380*cm, 380*cm,
      380*cm, 380*cm };
  
  G4double REFLECTIVITY_PlasticSc[NUMENTRIES] =
    { 1., 1., 1., 1., 1., 1., 
      1., 1., 1., 1., 1., 1.};


  PlasticSc_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  PlasticSc_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_PlasticSc, NUMENTRIES);
  
  PlasticSc_sf->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  PlasticSc_sf->AddProperty("REFLECTIVITY", PhotonEnergy, REFLECTIVITY_PlasticSc, NUMENTRIES);

  PlasticSc->SetMaterialPropertiesTable(PlasticSc_mt);
  PSOpticalSurface->SetMaterialPropertiesTable(PlasticSc_sf);




  ESR_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  ESR_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_PlasticSc, NUMENTRIES);
  
  ESR_sf->AddProperty("RINDEX", PhotonEnergy, RINDEX_PlasticSc, NUMENTRIES);
  ESR_sf->AddProperty("REFLECTIVITY", PhotonEnergy, REFLECTIVITY_PlasticSc, NUMENTRIES);

  Mat_ESR->SetMaterialPropertiesTable(ESR_mt);
  ESROpticalSurface->SetMaterialPropertiesTable(ESR_sf);





  
  G4double RINDEX_Air[NUMENTRIES] =
    { 1.0003, 1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003, 1.0003, 1.0003, 1.0003,
      1.0003, 1.0003 };

  G4double ABSORPTION_Air[NUMENTRIES] =
    { DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX,
      DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX,
      DBL_MAX, DBL_MAX };

  G4double REFLECTIVITY_Air[NUMENTRIES] =
    { 1., 1., 1., 1., 1., 1., 
      1., 1., 1., 1., 1., 1.};

  Air_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_Air, NUMENTRIES);
  Air_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_Air, NUMENTRIES);

  Air_sf->AddProperty("RINDEX", PhotonEnergy, RINDEX_Air, NUMENTRIES);
  Air_sf->AddProperty("REFLECTIVITY", PhotonEnergy, REFLECTIVITY_Air, NUMENTRIES);
  
  Mat_Air->SetMaterialPropertiesTable(Air_mt);
  AirOpticalSurface->SetMaterialPropertiesTable(Air_sf);
  
  
  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); // Red color
  worldVisAtt->SetVisibility(true);
  logical_World->SetVisAttributes(worldVisAtt);
  
  // Example visualization attributes for the sensitive detector (logical_ps)
  G4VisAttributes* psVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); // Blue color
  psVisAtt->SetVisibility(true);
  logical_PS->SetVisAttributes(psVisAtt);
  
  // Example visualization attributes for the SiPM volume (logical_sipm)
  G4VisAttributes* sipmVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Green color
  sipmVisAtt->SetVisibility(true);
  flogical_SiPM->SetVisAttributes(sipmVisAtt);
  
  return physical_World;
}
