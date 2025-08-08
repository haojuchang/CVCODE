import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math
import time

def draw_block(ax,x,y,width,height,id, bl, lb, rt, tr):
    if id>0:
        color = "#ffcccc"
    else:
        color = "#bbbbbb"

    ax.text(x+width/2-1, y+height/2-1, id, fontsize=4)
    # ax.text(x, y+2, bl, fontsize=4)
    # ax.text(x+2, y, lb, fontsize=4)
    # ax.text(x+width-4, y+height-2, rt, fontsize=4)
    # ax.text(x+width-2, y+height-4, tr, fontsize=4)

    ax.add_patch(
        patches.Rectangle( 
            (x,y ),
            width,
            height,
            fill = True,
            linewidth=0.1,
            edgecolor = "#000000",
            facecolor = color,
            alpha = 1.0 #0.3 original
        )
    )


png_size = (16,12)
txt_name = sys.argv[1] 
png_name = sys.argv[2] 
fread = open(txt_name,'r')
f = fread.read().split("\n")


total_block_number = int(f[0])
window_width = int( f[1].split(" ")[0] )
window_height = int( f[1].split(" ")[1] )

fig = plt.figure(figsize=png_size)

ax = fig.add_subplot(111)
ax.set_xbound(0,window_width)
ax.set_ybound(0,window_height)


i = 2

for block in range(total_block_number ):
    ss =  f[i].split(" ")
    draw_block(ax,int(ss[1]),int(ss[2]),int(ss[3]),int(ss[4]),int(ss[0]),int(ss[5]),int(ss[6]),int(ss[7]),int(ss[8]))
    i+=1

#plt.savefig(str(sys.argv[1])[:-4]+".png")

plt.savefig(png_name)
# plt.savefig(png_name, dpi=600)
