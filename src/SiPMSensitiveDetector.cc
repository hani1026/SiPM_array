// SiPMSensitiveDetector.cc
#include "SiPMSensitiveDetector.hh"
#include "EventAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"

SiPMSensitiveDetector::SiPMSensitiveDetector(const G4String& name, EventAction* eventAction)
  : G4VSensitiveDetector(name), fEventAction(eventAction) {}

SiPMSensitiveDetector::~SiPMSensitiveDetector() {}

G4bool SiPMSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if (!IsOpticalPhoton(step)) return false;
  return ProcessSiPMHit(step);
}

bool SiPMSensitiveDetector::IsOpticalPhoton(G4Step* step) {
  G4Track* track = step->GetTrack();
  return (track->GetDefinition()->GetParticleName() == "opticalphoton");
}

G4bool SiPMSensitiveDetector::ProcessSiPMHit(G4Step* step) {
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  if (volumeName.substr(0,4) != "SiPM") return false;

  try {
    G4int sipmID = ExtractSiPMID(volumeName);
    if (IsValidSiPMID(sipmID)) {
      fEventAction->AddCount_SiPM(sipmID);
      return true;
    }
  } catch (const std::exception& e) {
    ReportInvalidSiPMName(volumeName);
  }

  return false;
}

G4int SiPMSensitiveDetector::ExtractSiPMID(const G4String& volumeName) {
  return std::stoi(volumeName.substr(5));
}

bool SiPMSensitiveDetector::IsValidSiPMID(G4int sipmID) {
  return (sipmID >= 0 && sipmID < 40);
}

void SiPMSensitiveDetector::ReportInvalidSiPMName(const G4String& volumeName) {
  G4ExceptionDescription msg;
  msg << "Invalid SiPM ID in volume name: " << volumeName;
  G4Exception("SiPMSensitiveDetector::ProcessSiPMHit",
              "MyCode002", JustWarning, msg);
}

