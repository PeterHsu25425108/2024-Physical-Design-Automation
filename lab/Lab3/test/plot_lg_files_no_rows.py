import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def read_lg_file(filename):
    """
    Reads the .lg file and extracts die size and cells.
    """
    cells = []
    die_size = None

    with open(filename, "r") as file:
        for line in file:
            line = line.strip()
            if line.startswith("DieSize"):
                parts = line.split(" ")
                die_size = (float(parts[1]), float(parts[2]), float(parts[3]), float(parts[4]))
            elif line.startswith("FF") or line.startswith("C") or line.startswith("Gate"):
                parts = line.split(" ")
                x, y = float(parts[1]), float(parts[2])
                width, height = float(parts[3]), float(parts[4])
                fixed = parts[5] == "FIX"
                cells.append((x, y, width, height, fixed))

    return die_size, cells

def plot_lg_file(die_size, cells, output_filename_base):
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
                label="Die Boundary"
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
                alpha=0.6
            )
        )

    # Formatting and saving
    ax.set_xlim(die_size[0], die_size[2])
    ax.set_ylim(die_size[1], die_size[3])
    ax.set_title("Initial Cell Placement")
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True)
    plt.tight_layout()

    # Save both .png and .svg
    plt.savefig(f"{output_filename_base}.png")
    plt.savefig(f"{output_filename_base}.svg")
    plt.close()

if __name__ == "__main__":
    # Set the folder paths
    testcase_folder = "./testcase"
    output_folder = "./"

    # Process all .lg files in the testcase folder
    for filename in os.listdir(testcase_folder):
        if filename.endswith(".lg"):
            lg_path = os.path.join(testcase_folder, filename)
            output_filename_base = os.path.join(output_folder, f"{os.path.splitext(filename)[0]}_plot")

            # Read and plot the .lg file
            die_size, cells = read_lg_file(lg_path)
            plot_lg_file(die_size, cells, output_filename_base)

            print(f"Plotted {filename} to {output_filename_base}.png and {output_filename_base}.svg")
