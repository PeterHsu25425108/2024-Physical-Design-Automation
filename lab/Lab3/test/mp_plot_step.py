import os
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import multiprocessing


def read_file(filename):
    """
    Reads the .txt file and extracts die size, cells, placeRows, and moved_ffs.
    """
    cells = []
    placeRows = []
    moved_ffs = []
    die_size = None
    new_ff = None

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
                x, y = float(parts[1]), float(parts[2])
                width, height = float(parts[3]), float(parts[4])
                fixed = parts[5] == "FIX"
                cells.append((x, y, width, height, fixed))
            elif line.startswith("PlaceRow"):
                parts = line.split(" ")
                placeRows.append(
                    (float(parts[1]), float(parts[2]), float(parts[3]), float(parts[4]))
                )
            elif line.startswith("MovedFF"):
                parts = line.split(" ")
                moved_ffs.append(parts[1])
            elif line.startswith("NewFF"):
                parts = line.split(" ")
                new_ff = parts[1]

    return die_size, cells, placeRows, moved_ffs, new_ff


def plot_file(
    die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
):
    """
    Plots the .txt file contents, including die size, cells, placeRows, and moved_ffs.
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
    for x, y, width, height, fixed in cells:
        color = "gray" if fixed else "blue"
        ax.add_patch(
            patches.Rectangle(
                (x, y),
                width,
                height,
                edgecolor=color,
                facecolor=color,
                linewidth=1,
                alpha=0.6,
            )
        )
        # write the ff name at the center of the cell
        ax.text(
            x + width / 2,
            y + height / 2,
            new_ff,
            horizontalalignment="center",
            verticalalignment="center",
            fontsize=8,
            color="black",
        )

    # Formatting and saving
    ax.set_xlim(die_size[0], die_size[2])
    ax.set_ylim(die_size[1], die_size[3])
    ax.set_title("Legalized Cell Placement")
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True)
    plt.tight_layout()

    # if ../draw/plot/{output_filename_base} does not exist, create it
    if not os.path.exists(output_filename_base):
        os.makedirs(output_filename_base)

    # Save both .png and .svg
    plt.savefig(f"{output_filename_base}/{new_ff}.{format}")
    plt.savefig(f"{output_filename_base}/{new_ff}.svg")
    plt.close()


def process_file(args):
    """
    Worker function to process a single .txt file.
    """
    filename, casename = args
    new_ff = os.path.splitext(os.path.basename(filename))[0]
    output_filename_base = f"../draw/plot/{casename}"
    print(output_filename_base)
    print(new_ff)
    lg_file = f"../draw/text/{filename}"

    # Read and plot the .txt file
    die_size, cells, placeRows, moved_ffs, new_ff = read_file(lg_file)
    format = "png"
    plot_file(
        die_size, cells, placeRows, moved_ffs, output_filename_base, format, new_ff
    )

    print(f"Plotted {lg_file} to {output_filename_base}.")


if __name__ == "__main__":
    # the CASE_NAME
    casename = sys.argv[1]
    input_folder = "../draw/text"

    # Get a list of all .txt files in the input folder
    lg_files = [f for f in os.listdir(input_folder) if f.endswith(".txt")]

    # Create a pool of worker processes
    with multiprocessing.Pool() as pool:
        pool.map(process_file, [(f, casename) for f in lg_files])
