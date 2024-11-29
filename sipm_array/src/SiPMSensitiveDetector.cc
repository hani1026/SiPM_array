// SiPMSensitiveDetector.cc
#include "SiPMSensitiveDetector.hh"
#include "EventAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>

SiPMSensitiveDetector::SiPMSensitiveDetector(const G4String& name, EventAction* eventAction)
  : G4VSensitiveDetector(name), fEventAction(eventAction) {}

SiPMSensitiveDetector::~SiPMSensitiveDetector() {}

G4bool SiPMSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  // 광자만 처리
  G4Track* track = step->GetTrack();
  if (track->GetDefinition()->GetParticleName() != "opticalphoton") return false;

  // 현재 볼륨 이름 가져오기 (물리적 볼륨 이름 사용)
  G4VPhysicalVolume* physVol = step->GetPreStepPoint()->GetTouchable()->GetVolume();
  G4String volumeName = physVol->GetName();

  // SiPM ID 추출 (언더스코어 건너뛰기)
  if (volumeName.substr(0,4) == "SiPM") {
    try {
      G4int sipmID = std::stoi(volumeName.substr(5)); // 'SiPM_' 다음부터 숫자 추출
      if (sipmID >= 0 && sipmID < 40) {
        // SiPM 카운트 증가
        fEventAction->AddCount_SiPM(sipmID);

        // 디버그 출력
	//std::cout << "Photon reached SiPM ID: " << sipmID << std::endl;
      }
    } catch (const std::invalid_argument& e) {
      G4cerr << "Invalid SiPM ID in volume name: " << volumeName << G4endl;
    } catch (const std::out_of_range& e) {
      G4cerr << "SiPM ID out of range in volume name: " << volumeName << G4endl;
    }
  }

  return true;
}

