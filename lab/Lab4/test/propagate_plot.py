import numpy as np
import matplotlib.pyplot as plt
import sys
import os

font = 6

# Load the data
filename = sys.argv[1]
# Extract the net name from the filename
net_name = os.path.basename(filename).replace('.txt', '')
print("Net name: ", net_name)

# Extract the grid parameters and indices from the file
with open(filename, 'r') as f:
    first_line = f.readline().strip().split()
    GMP_LLX = float(first_line[1])
    GMP_LLY = float(first_line[3])
    GRID_W = float(first_line[5])
    GRID_H = float(first_line[7])
    GMP_WIDTH = float(first_line[9])
    GMP_HEIGHT = float(first_line[11])

    # Read the source and target indices
    src_line = f.readline().strip().split()
    tar_line = f.readline().strip().split()
    src_idx = (int(src_line[2]), int(src_line[3]))
    tar_idx = (int(tar_line[2]), int(tar_line[3]))

    # Read the rest of the data points
    data = np.loadtxt(f)

# Function to convert grid index to actual position
def idx_to_pos(idx, llx, lly):
    x = llx + idx[1] * GRID_W 
    y = lly + idx[0] * GRID_H 
    return (x, y)

# Plot the grid
fig, ax = plt.subplots()
ax.set_xlim(GMP_LLX - GRID_W, GMP_LLX + GRID_W + GMP_WIDTH)
ax.set_ylim(GMP_LLY - GRID_H, GMP_LLY + GRID_H + GMP_HEIGHT)

# Plot the source and target points
src_pos = idx_to_pos(src_idx, GMP_LLX, GMP_LLY)
tar_pos = idx_to_pos(tar_idx, GMP_LLX, GMP_LLY)

# Plot the source and target points with text
ax.text(src_pos[0], src_pos[1], 'Src', fontsize=font, color='blue')
ax.text(tar_pos[0], tar_pos[1], 'Tar', fontsize=font, color='green')
# ax.scatter(src_pos[0], src_pos[1], c='blue', marker='o', label='Source')
# ax.scatter(tar_pos[0], tar_pos[1], c='green', marker='o', label='Target')

# Draw vertical grid lines
for x in range(int(GMP_LLX), int(GMP_LLX + GMP_WIDTH), int(GRID_W)):
    ax.axvline(x, color='orange', linestyle='--', linewidth=0.5)

# Draw horizontal grid lines
for y in range(int(GMP_LLY), int(GMP_LLY + GMP_HEIGHT), int(GRID_H)):
    ax.axhline(y, color='orange', linestyle='--', linewidth=0.5)

# Plot the path points
if data.size > 0:
    x = []
    y = []
    count = 0
    for point in data:
        pos = idx_to_pos((int(point[0]), int(point[1])), GMP_LLX, GMP_LLY)
        # x.append(pos[0])
        # y.append(pos[1])
        x = pos[0]
        y = pos[1]
        
        ax.text(x, y, str(count), fontsize=font, color='red')
        count += 1

# Add grid lines
ax.grid(True)

# Set labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_title('Grid Plot')

# # Add legend
# ax.legend()

# Save the plot
plt.savefig(net_name +"_plot.png")