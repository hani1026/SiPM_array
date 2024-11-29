// SiPMSensitiveDetector.hh
#ifndef SiPMSensitiveDetector_h
#define SiPMSensitiveDetector_h

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

class EventAction;

class SiPMSensitiveDetector : public G4VSensitiveDetector {
public:
  SiPMSensitiveDetector(const G4String& name, EventAction* eventAction);
  virtual ~SiPMSensitiveDetector();

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

private:
  EventAction* fEventAction;
};

#endif
