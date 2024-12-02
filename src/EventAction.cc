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
  // 버퍼링된 데이터 처리
  static const int BUFFER_SIZE = 1000;
  static std::vector<std::array<G4int, 40>> sipmBuffer;
  static std::vector<G4ThreeVector> posBuffer;
  
  // 현재 이벤트 데이터 추가
  sipmBuffer.push_back(std::array<G4int, 40>());
  std::copy(std::begin(f_SiPM_Count), std::end(f_SiPM_Count), 
            sipmBuffer.back().begin());
  
  posBuffer.push_back(G4ThreeVector(fStartX, fStartY, fStartZ));
  
  // 버퍼가 가득 차면 한번에 처리
  if (sipmBuffer.size() >= BUFFER_SIZE) {
    for (size_t i = 0; i < sipmBuffer.size(); ++i) {
      fRunAction->SaveEventData(sipmBuffer[i].data(), 
                              posBuffer[i].x(),
                              posBuffer[i].y(),
                              posBuffer[i].z());
    }
    sipmBuffer.clear();
    posBuffer.clear();
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


