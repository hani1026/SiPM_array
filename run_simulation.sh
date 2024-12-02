#!/bin/bash

# 시작 시간 기록
start_time=$(date +%s.%N)

# Geant4 시뮬레이션 실행
./SiPM gun.mac hani 42

# 종료 시간 기록
end_time=$(date +%s.%N)

# 실행 시간 계산 (초 단위)
execution_time=$(echo "$end_time - $start_time" | bc)

# 결과 출력
echo "실행 시간: $execution_time 초" 