import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.lines as mlines

DEBUG = 0


def plot_blocks_and_contours(file_path):
    with open(file_path, "r") as file:
        lines = file.readlines()

    # Read number of blocks
    num_blocks = int(lines[0].strip().split(": ")[1])

    # Read block information
    blocks = {}
    max_w = 0
    max_h = 0
    for i in range(1, num_blocks + 1):
        name, llx, lly, width, height, parent, left, right = lines[i].strip().split(" ")

        max_w = max(max_w, int(llx) + int(width))
        max_h = max(max_h, int(lly) + int(height))
        blocks[name] = {
            "llx": float(llx),
            "lly": float(lly),
            "width": float(width),
            "height": float(height),
            "parent": parent,
            "left": left,
            "right": right,
        }
        if DEBUG:
            print(f"Block {name}: {blocks[name]}")

    # Read number of contour lines
    num_contours = int(lines[num_blocks + 1].strip().split(": ")[1])

    # Read contour information
    contours = []
    for i in range(num_blocks + 2, num_blocks + 2 + num_contours):
        left_x, right_x, y = map(float, lines[i].strip().split(" "))
        contours.append((left_x, right_x, y))
        if DEBUG:
            print(f"Contour line from ({left_x}, {y}) to ({right_x}, {y})")

    # Read outline width and height
    outline_width, outline_height, boundaryWidth, boundaryHeight = map(
        int, lines[num_blocks + 2 + num_contours].strip().split(": ")[1].split(" ")
    )
    if DEBUG:
        print(f"Outline width: {outline_width}, height: {outline_height}")

    fig, ax = plt.subplots()

    # Plot blocks
    for name, block in blocks.items():

        if block["parent"] == "nullptr":
            color = "blue"
        else:
            color = "pink"

        rect = patches.Rectangle(
            (block["llx"], block["lly"]),
            block["width"],
            block["height"],
            linewidth=1,
            edgecolor="black",
            facecolor=color,
            fill=True,
            alpha=0.5,
        )
        ax.add_patch(rect)

        # Draw block name
        ax.text(
            block["llx"] + block["width"] / 2,
            block["lly"] + block["height"] / 2,
            name,
            ha="center",
            va="center",
            color="black",
            fontsize=8,
        )

        # Draw arrows for parent-child relationships
        if block["left"] != "nullptr":
            parent_block = block
            child_block = blocks[block["left"]]

            parent_center = (
                parent_block["llx"] + parent_block["width"] / 2,
                parent_block["lly"] + parent_block["height"] / 2,
            )
            child_center = (
                child_block["llx"] + child_block["width"] / 2,
                child_block["lly"] + child_block["height"] / 2,
            )

            # if the child is a left child, draw the arrow from the right side of the parent, and set the color to blue
            ax.annotate(
                "",
                xy=child_center,
                xytext=(parent_center[0], parent_center[1]),
                arrowprops=dict(
                    arrowstyle="->", color="blue", lw=1.5, shrinkA=5, shrinkB=5
                ),
            )

            if DEBUG:
                # print the name of the parent and child blocks
                print(f"Drawing arrow from {parent_block} to {child_block}")
        if block["right"] != "nullptr":
            parent_block = block
            child_block = blocks[block["right"]]

            parent_center = (
                parent_block["llx"] + parent_block["width"] / 2,
                parent_block["lly"] + parent_block["height"] / 2,
            )
            child_center = (
                child_block["llx"] + child_block["width"] / 2,
                child_block["lly"] + child_block["height"] / 2,
            )

            # if the child is a right child, draw the arrow from the left side of the parent, and set the color to red

            ax.annotate(
                "",
                xy=child_center,
                xytext=(parent_center[0], parent_center[1]),
                arrowprops=dict(
                    arrowstyle="->", color="green", lw=1.5, shrinkA=5, shrinkB=5
                ),
            )

            if DEBUG:
                print(f"Drawing arrow from {parent_block} to {child_block}")

    # Plot outline
    outline = patches.Rectangle(
        (0, 0),
        outline_width,
        outline_height,
        linewidth=1,
        edgecolor="purple",
        facecolor="none",
        linestyle="dashed",
    )
    ax.add_patch(outline)

    # plot boundary rectangle with green dashed line
    boundary = patches.Rectangle(
        (0, 0),
        boundaryWidth,
        boundaryHeight,
        linewidth=1,
        edgecolor="green",
        facecolor="none",
        linestyle="dashed",
    )
    ax.add_patch(boundary)

    # Plot contour lines
    for left_x, right_x, y in contours:
        line = mlines.Line2D([left_x, right_x], [y, y], color="red", dashes=[6, 2])
        ax.add_line(line)
        # plot gray dashed vertical lines from(x_left, 0) to (x_left, y) and from (x_right, 0) to (x_right, y)
        ax.plot([left_x, left_x], [0, y], color="gray", linestyle="dashed")
        ax.plot([right_x, right_x], [0, y], color="gray", linestyle="dashed")
        if DEBUG:
            print(f"Drawing contour line from ({left_x}, {y}) to ({right_x}, {y})")

    # Adjust axis limits based on the parsed data range
    max_w = max(max_w, outline_width)
    max_h = max(max_h, outline_height)
    ax.set_xlim(0, max_w * 1.1)
    ax.set_ylim(0, max_h * 1.1)

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("Blocks and Contours Plot")
    plt.grid(True)

    # Save and show plot
    format = "png"
    relative_path = str(file_path.split("text/")[0])
    # print(f"Relative path: {relative_path}")
    output_file = (
        relative_path
        + format
        + "/"
        + str(file_path.split("/")[-1].split(".")[0])
        + "."
        + format
    )
    print(f"Saving plot to {output_file}")
    plt.savefig(output_file, format=format)
    plt.show()


# Run if script is called directly
import sys

if len(sys.argv) != 2:
    print("Usage: python plot.py <file_path>")
    sys.exit(1)

filename = sys.argv[1]
plot_blocks_and_contours(filename)
