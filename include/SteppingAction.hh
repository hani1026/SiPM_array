// SteppingAction.hh
#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "EventAction.hh"

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  virtual ~SteppingAction();

  virtual void UserSteppingAction(const G4Step*) override;

private:
  bool IsOpticalPhoton(const G4Step* step);
  void ProcessSiPMHit(const G4Step* step);
  bool IsValidSiPMID(G4int sipmID);
  void ReportInvalidSiPMName(const G4String& volumeName);
  void RecordStartPosition(const G4Step* step);
  bool IsMuon(const G4Step* step);

  EventAction* fEventAction;
};

#endif
