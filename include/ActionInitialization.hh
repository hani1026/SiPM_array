#ifndef ActionInitialization_h
#define ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
public:

  ActionInitialization(G4String fileName);
  ~ActionInitialization() override;

  virtual void BuildForMaster() const; 
  virtual void Build() const;
  
private:
  
  G4String fFileName;

};

#endif
