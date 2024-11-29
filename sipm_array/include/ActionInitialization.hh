// ActionInitialization.hh
#ifndef ActionInitialization_h
#define ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(G4String fileName);
  virtual ~ActionInitialization();

  virtual void BuildForMaster() const override;
  virtual void Build() const override;

private:
  G4String fFileName;
};

#endif
