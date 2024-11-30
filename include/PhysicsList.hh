#ifndef PHYSICS_h
#define PHYSICS_h

#include "G4VModularPhysicsList.hh"

class MyPhysicsList : public G4VModularPhysicsList
{
public:
  MyPhysicsList();
  virtual ~MyPhysicsList();
  void ConstructOp();

private:
  void InitializePhysics();
  virtual void SetCuts() override;
};

#endif
