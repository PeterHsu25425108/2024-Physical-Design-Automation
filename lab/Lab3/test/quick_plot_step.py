import os
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from tqdm import tqdm
from concurrent.futures import ThreadPoolExecutor


def read_file(filename):
    """
    Reads the .lg file and extracts die size and cells.
    """
    cells = []
    die_size = None
    placeRows = []
    moved_ffs = []

    with open(filename, "r") as file:
        for line in file:
            line = line.strip()
            if line.startswith("DieSize"):
                parts = line.split(" ")
                die_size = (
                    float(parts[1]),
                    float(parts[2]),
                    float(parts[3]),
                    float(parts[4]),
                )
            elif line.startswith("FF") or line.startswith("C"):
                parts = line.split(" ")
                name = parts[0]
                x, y = float(parts[1]), float(parts[2])
                width, height = float(parts[3]), float(parts[4])
                fixed = parts[5] == "FIX"
                cells.append((x, y, width, height, fixed, name))

    return die_size, cells, placeRows, moved_ffs


def plot_cells(ax, cells, moved_ffs, new_ff):
    """Plot all cells onto the given axis."""
    new_ff_patch = None
    for x, y, width, height, fixed, name in tqdm(cells):
        if fixed:
            color = "gray"
            alpha = 0.6
        elif name == new_ff:
            color = "green"
            alpha = 0.6
        elif name in moved_ffs:
            color = "red"
            alpha = 0.6
        elif name + "_remove" == new_ff:
            print("find new ff\n")
            print("x = ", x)
            print("y = ", y)
            color = "orange"
            alpha = 1
            new_ff_patch = patches.Rectangle(
                (x, y),
                width,
                height,
                edgecolor=color,
                facecolor=color,
                linewidth=1,
                alpha=alpha,
            )
            continue
        else:
            color = "blue"
            alpha = 0.6

        ax.add_patch(
            patches.Rectangle(
                (x, y),
                width,
                height,
                edgecolor=color,
                facecolor=color,
                linewidth=1,
                alpha=alpha,
            )
        )
    if new_ff_patch != None:
        ax.add_patch(new_ff_patch)


def plot_file(
    die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
):
    """
    Plots the .lg file contents, including only die size and cells.
    """
    fig, ax = plt.subplots(figsize=(10, 10))

    # Draw die boundary
    if die_size:
        ax.add_patch(
            patches.Rectangle(
                (die_size[0], die_size[1]),
                die_size[2] - die_size[0],
                die_size[3] - die_size[1],
                edgecolor="black",
                fill=False,
                linewidth=2,
                label="Die Boundary",
            )
        )

    # Parallelize cell plotting
    with ThreadPoolExecutor() as executor:
        executor.submit(plot_cells, ax, cells, moved_ffs, new_ff)
    print("done adding patches")

    # Formatting and saving
    ax.set_xlim(die_size[0], die_size[2])
    ax.set_ylim(die_size[1], die_size[3])
    if "remove" not in new_ff:
        ax.set_title("Legalized Cell " + new_ff)
    else:
        ax.set_title(new_ff)
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")
    ax.set_aspect("equal", adjustable="box")
    plt.tight_layout()

    # Ensure output directory exists
    os.makedirs(output_filename_base, exist_ok=True)

    # Save plot
    plt.savefig(f"{output_filename_base}/{new_ff}.{format}")
    plt.close()


if __name__ == "__main__":
    casename = sys.argv[1]
    new_ff = sys.argv[2]
    output_filename_base = f"../draw/plot/{casename}"
    lg_file = f"../draw/text/{new_ff}.txt"

    # Read and plot the .txt file
    die_size, cells, placeRows, moved_ffs = read_file(lg_file)
    print("num of cells =", len(cells))
    format = "png"
    plot_file(
        die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
    )

    print(f"Plotted {lg_file} to {output_filename_base}.")
