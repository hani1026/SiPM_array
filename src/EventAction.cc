// EventAction.cc
#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction(RunAction* runAction)
  : G4UserEventAction(), 
    fRunAction(runAction),
    fStartX(0.0),
    fStartY(0.0),
    fStartZ(0.0),
    fPhotonCount(0.0)
{
  ResetCounters();
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  ResetEventVariables();
}

void EventAction::ResetEventVariables()
{
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonCount = 0.0;
  ResetCounters();
  fPhotonPositions.clear();
}

void EventAction::ResetCounters()
{
  std::fill_n(f_SiPM_Count, 40, 0);
}

void EventAction::EndOfEventAction(const G4Event*)
{
  // RunAction에 데이터 직접 전달하여 저장
  fRunAction->SaveEventData(f_SiPM_Count, fStartX, fStartY, fStartZ);
  
  // 변수 초기화
  ResetEventVariables();
}

void EventAction::AddStartPosition(G4double x, G4double y, G4double z) {
  fStartX = x;
  fStartY = y;
  fStartZ = z;
}

void EventAction::UpdateRunAction()
{
  // RunAction에 데이터 전달
  fRunAction->AddStartPosition(fStartX, fStartY, fStartZ);
  fRunAction->AddSiPMCounts(f_SiPM_Count);

  for (const auto& pos : fPhotonPositions) {
    fRunAction->AddPhotonPosition(pos);
  }
}

void EventAction::PrintEventProgress()
{
  // 이벤트 진행 상황 출력
  fRunAction->printEventproc();
}

void EventAction::AddCount_SiPM(G4int sipmID)
{
  if (IsValidSiPMID(sipmID)) {
    f_SiPM_Count[sipmID]++;
  } else {
    ReportInvalidSiPMID(sipmID);
  }
}

bool EventAction::IsValidSiPMID(G4int sipmID)
{
  return (sipmID >= 0 && sipmID < 40);
}

void EventAction::ReportInvalidSiPMID(G4int sipmID)
{
  G4ExceptionDescription msg;
  msg << "Invalid SiPM ID: " << sipmID;
  G4Exception("EventAction::AddCount_SiPM",
              "MyCode003", JustWarning, msg);
}


