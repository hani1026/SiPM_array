import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# CSV 파일 읽기 (헤더 없이)
df = pd.read_csv('hani_nt_SiPM.csv', header=None)

# 처음 40개 컬럼만 선택 (SiPM 데이터)
sipm_data = df.iloc[:, :40]

# 각 SiPM의 평균값 계산
sipm_means = sipm_data.mean()

# 4x10 배열로 재구성
sipm_matrix = np.zeros((4, 10))
for i in range(40):
    row = i // 10
    col = i % 10
    sipm_matrix[row, col] = sipm_means[i]

# 시각화
plt.figure(figsize=(15, 6))
sns.heatmap(sipm_matrix, 
            cmap='viridis', 
            annot=True,  # 숫자 표시
            fmt='.1f',   # 소수점 1자리까지 표시
            cbar_kws={'label': 'Average Signal'})

plt.title('SiPM Signal Distribution (Average over 100 events)')
plt.xlabel('Column Index')
plt.ylabel('Row Index')

# y축 방향 반전 (위에서 아래로 0,1,2,3 되도록)
plt.gca().invert_yaxis()

plt.tight_layout()
plt.show()
