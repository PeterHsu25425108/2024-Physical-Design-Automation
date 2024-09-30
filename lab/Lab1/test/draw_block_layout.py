import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches


def draw_block(ax, x, y, width, height, id, LL_Left, LL_Bottom, UR_Top, UR_Right,block_id=None):
    if id > 0:
        # if id == block_id, set the color green
        if block_id == None:
            color = "#FCC"
        elif id == 19:#int(block_id):
            color = "#CFC"
        else:
            color = "#FCC"
        
    else:
        color = "#BBB"


    ax.add_patch(
        patches.Rectangle(
            (x, y),
            width,
            height,
            fill=True,
            edgecolor="#000",
            facecolor=color,
            alpha=1.0,  # 0.3 original
        )
    )

    # Calculate the center of the block
    center_x = x + width / 2
    center_y = y + height / 2

    # Calculate the font size
    font_size = 3
    small_font_size = 2

    # Add the id text at the center of the block
    ax.text(center_x, center_y, str(id), ha='center', va='center', fontsize=font_size, color='black')

    # # Add the LL_Left text at the bottom left of the block
    # ax.text(x+0.1*width, center_y, LL_Left, ha='center', va='center', fontsize = small_font_size, color='black')

    # # Add the LL_Bottom text at the bottom left of the block
    # ax.text(x+0.15*width, y+0.1*height, LL_Bottom, ha='center', va='center', fontsize = small_font_size, color='black')

    # # Add the UR_Top text at the top right of the block
    # ax.text(x + 0.9*width, y + 0.9*height, UR_Top, ha='center', va='center', fontsize = small_font_size, color='black')

    # # Add the UR_Right text at the top right of the block
    # ax.text(x + 0.9*width, center_y, UR_Right, ha='center', va='center', fontsize = small_font_size, color='black')


png_size = (16, 12)
txt_name = sys.argv[1]
digit = False

# if there is any digit in txt_name
if any(char.isdigit() for char in txt_name):
# txt_name format: "Insert_[id].txt", parse the id from the txt_name
    block_id = txt_name.split("_")[1][:-4]
    digit = True

png_name = sys.argv[2]
fread = open(txt_name, "r")
f = fread.read().split("\n")


total_block_number = int(f[0])
window_width = int(f[1].split(" ")[0])
window_height = int(f[1].split(" ")[1])

fig = plt.figure(figsize=png_size)

ax = fig.add_subplot(111)
ax.set_xbound(0, window_width)
ax.set_ybound(0, window_height)


i = 2

for block in range(total_block_number):
    ss = f[i].split(" ")
    if digit:
        draw_block(ax, int(ss[1]), int(ss[2]), int(ss[3]), int(ss[4]), int(ss[0]), str(ss[5]), str(ss[6]), str(ss[7]), str(ss[8]), block_id)
    else:
        draw_block(ax, int(ss[1]), int(ss[2]), int(ss[3]), int(ss[4]), int(ss[0]), str(ss[5]), str(ss[6]), str(ss[7]), str(ss[8]))
    i += 1

# plt.savefig(str(sys.argv[1])[:-4]+".png")
##plt.show()
plt.savefig(png_name)
