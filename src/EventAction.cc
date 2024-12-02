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
    fPhotonCount(0.0),
    fDarkNoiseRate(100.0)  // 100 kHz/mm² @ 25°C
{
  ResetEventVariables();
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  G4cout << "\nBegin of Event Action" << G4endl;
  ResetEventVariables();
}

void EventAction::ResetEventVariables()
{
  G4cout << "Resetting event variables" << G4endl;
  for(int i = 0; i < 40; i++) {
    f_SiPM_Count[i] = 0;
  }
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fPhotonCount = 0;
  fDelayedCounts.clear();
}

void EventAction::EndOfEventAction(const G4Event*)
{
  G4int totalCounts = 0;
  for(int i = 0; i < 40; i++) {
    totalCounts += f_SiPM_Count[i];
  }
  
  fRunAction->SaveEventData(f_SiPM_Count, fStartX, fStartY, fStartZ);
  ResetEventVariables();

  // 타이밍 정보 추가
  std::vector<G4double> hitTimes;
  for(const auto& delayedCount : fDelayedCounts) {
    hitTimes.push_back(delayedCount.time);
  }
  
  // 에너지 분해능 시뮬레이션
  G4double totalEnergy = 0;
  for(int i = 0; i < 40; i++) {
    G4double resolution = 0.1;  // 10% @ 1MeV
    G4double sigma = resolution * std::sqrt(f_SiPM_Count[i]);
    f_SiPM_Count[i] = G4int(G4RandGauss::shoot(f_SiPM_Count[i], sigma));
  }
}

void EventAction::AddStartPosition(G4double x, G4double y, G4double z)
{
  fStartX = x;
  fStartY = y;
  fStartZ = z;
  G4cout << "Added start position: (" << x << ", " << y << ", " << z << ")" << G4endl;
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

void EventAction::AddDelayedCount_SiPM(G4int sipmID, G4double delay)
{
  if(IsValidSiPMID(sipmID)) {
    fDelayedCounts.push_back({sipmID, delay});
  } else {
    ReportInvalidSiPMID(sipmID);
  }
}

void EventAction::ProcessDelayedCounts()
{
  if (fDelayedCounts.empty()) return;
  
  // 지연 시간 순으로 정렬
  std::sort(fDelayedCounts.begin(), fDelayedCounts.end(),
            [](const DelayedCount& a, const DelayedCount& b) {
              return a.time < b.time;
            });
            
  // 지연된 카운트 처리
  for(const auto& count : fDelayedCounts) {
    AddCount_SiPM(count.sipmID);
  }
  fDelayedCounts.clear();
}


