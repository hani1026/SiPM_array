// EventAction.cc
#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include <iostream> // 디버그 출력을 위해 추가

EventAction::EventAction(RunAction* runAction)
  : G4UserEventAction(), fRunAction(runAction), fPhotonPositions()
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonCount = 0.0;
  for(int i = 0; i < 40; i++){
    f_SiPM_Count[i] = 0;
  }
  fPhotonPositions.clear();
}

void EventAction::EndOfEventAction(const G4Event*)
{
  // 집계된 데이터를 RunAction에 추가
  fRunAction->AddStartPosition(fStartX, fStartY, fStartZ);
  fRunAction->AddSiPMCounts(f_SiPM_Count);

  for (const auto& pos : fPhotonPositions) {
    fRunAction->AddPhotonPosition(pos);
  }

  // SiPM 카운트 출력
  //for(int i = 0; i < 400; ++i){
  //  if(f_SiPM_Count[i] > 0){
  //    std::cout << "Event: SiPM_" << i << " count = " << f_SiPM_Count[i] << std::endl;
  //  }
  //}

  // 이벤트 진행 상황 출력
  fRunAction->printEventproc();
}


