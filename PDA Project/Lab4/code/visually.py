import numpy as np
import cv2
import sys

outdir = sys.argv[1]
print(outdir)

fo = open(outdir, "r")
lines = fo.read().splitlines()

# 建立一張 512x512 的 RGB 圖片（黑色）
img = np.zeros((1024, 2048, 3), np.uint8)
# 將圖片用淺灰色 (200, 200, 200) 填滿
img.fill(200)

curnet = ''
for line in lines:
    tok = line.split(' ')
    if tok[0] == '.begin':
        curnet = tok[1]

    if tok[0] == '.H':
        x0 = int(tok[1])*25 + 50
        y0 = 1024 - (int(tok[2])*25 + 50)
        x1 = int(tok[3])*25 + 50
        y1 = y0
        cv2.line(img, (x0, y0), (x1, y1), (0, 0, 255), 1)

    if tok[0] == '.V':
        x0 = int(tok[1])*25 + 50
        y0 = 1024 - (int(tok[2])*25 + 50)
        x1 = x0
        y1 = 1024 - (int(tok[3])*25 + 50)
        cv2.line(img, (x0, y0), (x1, y1), (255, 0, 0), 1)
        cv2.putText(img, curnet, (x0, y0), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 1, cv2.LINE_AA)

# img = cv2.flip(img, 0)
# 在圖片上畫一條紅色的對角線，寬度為 5 px
# cv2.line(img, (0, 0), (255, 255), (0, 0, 255), 5)
 
cv2.imwrite('output.jpg', img)