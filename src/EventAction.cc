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
    fStartZ(0.0)
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
    f_SiPM_TimeSum[i] = 0.0;
  }
  fStartX = 0.0;
  fStartY = 0.0;
  fStartZ = 0.0;
  fDelayedCounts.clear();
}

void EventAction::EndOfEventAction(const G4Event*)
{
  G4double averageTimes[40];
  for(int i = 0; i < 40; i++) {
    averageTimes[i] = (f_SiPM_Count[i] > 0) ? 
                      f_SiPM_TimeSum[i] / f_SiPM_Count[i] : 0.0;
  }
  
  fRunAction->SaveEventData(f_SiPM_Count, averageTimes, fStartX, fStartY, fStartZ);
  ResetEventVariables();

  // 타이밍 정보 추가
  std::vector<G4double> hitTimes;
  for(const auto& delayedCount : fDelayedCounts) {
    hitTimes.push_back(delayedCount.time);
  }
}

void EventAction::AddStartPosition(G4double x, G4double y, G4double z)
{
  fStartX = x;
  fStartY = y;
  fStartZ = z;
}

void EventAction::UpdateRunAction()
{
  // RunAction에 데이터 전달
  fRunAction->AddStartPosition(fStartX, fStartY, fStartZ);
  fRunAction->AddSiPMCounts(f_SiPM_Count);

}

void EventAction::PrintEventProgress()
{
  // 이벤트 진행 상황 출력
  fRunAction->printEventproc();
}

void EventAction::AddCount_SiPM(G4int sipmID, G4double time) {
    // 배열 범위 체크를 비트 연산으로 최적화
    if (sipmID & ~0x3F) return;  // 40개 이상일 경우 빠른 반환
    
    f_SiPM_Count[sipmID]++;
    f_SiPM_TimeSum[sipmID] += time;
}

void EventAction::AddDelayedCount_SiPM(G4int sipmID, G4double delay)
{
  fDelayedCounts.push_back({sipmID, delay});
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
    AddCount_SiPM(count.sipmID, count.time);
  }
  fDelayedCounts.clear();
}


