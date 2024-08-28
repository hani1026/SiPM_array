#ifndef EventAction_h
#define EventAction_h

#include "G4UserEventAction.hh"

#include "RunAction.hh"
#include "G4UserRunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "g4root.hh"

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction* runAction);
  ~EventAction() override;
  
  void BeginOfEventAction(const G4Event* event) override;
  void EndOfEventAction(const G4Event* event) override;
    

  void AddCount_WholePhoton(G4int Count1) { fCount1 += Count1; }

  void AddCount_WholeSiPM(G4int Count2) {fCount2 += Count2;}

  void AddCount_SiPM(G4int N[40], G4int P)
  {
    N[P] += 1;
  }

  void AddCount_length(G4int Count3) { fCount3 += Count3; }
  void AddXposition(G4int Count4) { xpos = Count4; }
  void AddYposition(G4int Count5) { ypos = Count5; }
  void AddZposition(G4int Count6) { zpos = Count6; }
  
  G4int f_SiPM_Count[40] = {0,};

private:
  RunAction* fRunAction = nullptr;
  G4int   fCount1 = 0;    
  G4int   fCount2 = 0;
  G4int   fCount3 = 0;
  G4int   xpos = 0;
  G4int   ypos = 0;
  G4int   zpos = 0;
  
  G4int   fEventID=0;
};

#endif
