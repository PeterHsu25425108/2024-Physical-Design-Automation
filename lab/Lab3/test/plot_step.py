import os
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches


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
            # Remove leading/trailing whitespaces
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
            elif line.startswith("moved ffs"):
                # read a consective seq of ff names seperated with space
                parts = line.split(" ")
                for i in range(2, len(parts)):
                    name = parts[i]
                    moved_ffs.append(name)
            elif line.startswith("PlacementRows"):
                parts = line.split(" ")
                startY = float(parts[1])
                height = float(parts[2])
                free_sites = []
                for i in range(3, len(parts)):
                    x = parts[i].split(",")[0]
                    width = parts[i].split(",")[1]
                    free_sites.append((float(x), float(width)))
                placeRows.append((startY, height, free_sites))

    return die_size, cells, placeRows, moved_ffs


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

    # Plot cells
    for x, y, width, height, fixed, name in cells:
        color = None
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
            color = "orange"
            alpha = 0.3
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
        # write the ff name at the center of the cell
        ax.text(
            x + width / 2,
            y + height / 2,
            name,
            horizontalalignment="center",
            verticalalignment="center",
            fontsize=8,
            color="black",
        )

    # plot the free sites in placement rows
    # each with lower left corner at (x, startY) and width, plotted as rectangles with purple dashed lines as edges
    for startY, height, free_sites in placeRows:
        for x, width in free_sites:
            ax.add_patch(
                patches.Rectangle(
                    (x, startY),
                    width,
                    height,
                    edgecolor="purple",
                    facecolor="none",
                    linewidth=1,
                    linestyle="--",
                )
            )

    # Formatting and saving
    ax.set_xlim(die_size[0], die_size[2])
    ax.set_ylim(die_size[1], die_size[3])
    if new_ff.find("remove") == -1:
        ax.set_title("Legalized Cell " + new_ff)
    else:
        ax.set_title(new_ff)
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True)
    plt.tight_layout()

    # if ../draw/plot/{output_filename_base} does not exist, create it
    if not os.path.exists(f"{output_filename_base}"):
        os.makedirs(f"{output_filename_base}")

    # Save both .png and .svg
    plt.savefig(f"{output_filename_base}" + "/" + new_ff + "." + format)
    plt.close()


if __name__ == "__main__":
    # the CASE_NAME
    # output_filename_base = sys.argv[1]
    # plot ../draw/text/{CASE_NAME}/{FF_NAME}.txt
    casename = sys.argv[1]
    # new_ff = sys.argv[2]
    # output_filename_base = "../draw/plot/" + casename
    # lg_file = "../draw/text/" + "/" + new_ff + ".txt"

    # # Read and plot the .txt file
    # die_size, cells, placeRows, moved_ffs = read_file(lg_file)
    # print("num of cells = ", len(cells))
    # format = "png"
    # plot_file(
    #     die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
    # )

    # print(f"Plotted {lg_file} to {output_filename_base}.")

    output_filename_base = sys.argv[1]
    for filename in os.listdir("../draw/text/"):
        new_ff = filename.split(".")[0]
        output_filename_base = "../draw/plot/" + casename
        print(output_filename_base)
        print(new_ff)
        lg_file = "../draw/text/" + filename

        # Read and plot the .txt file
        die_size, cells, placeRows, moved_ffs = read_file(lg_file)
        format = "png"
        plot_file(
            die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
        )

        print(f"Plotted {lg_file} to {output_filename_base}.")
