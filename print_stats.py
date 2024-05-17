import math
import os
import pandas as pd
import matplotlib.pyplot as plt
import pdb


# Function to read CSV files and extract the data
def read_csv_files(directory):
    data = []
    # pdb.set_trace()
    for filename in os.listdir(directory):
        if filename.endswith(".csv"):
            filepath = os.path.join(directory, filename)
            df = pd.read_csv(filepath, header=None)
            data.append(df)
    return data


# Function to process data and calculate percentages
def process_data(data):
    depth_dict = {}

    for df in data:
        num_cols = df.shape[1]
        num_depths = num_cols // 4

        for depth in range(num_depths):
            col_start = depth * 4
            needed_evals = df.iloc[:, col_start].dropna()
            total_evals = df.iloc[:, col_start + 2].dropna()

            percentages = (needed_evals / total_evals) * 100
            if depth not in depth_dict:
                depth_dict[depth] = []
            depth_dict[depth].extend(percentages)

    return depth_dict


# Function to plot the data
def plot_data(depth_dict):
    num_depths = len(depth_dict)
    cols = 3  # Number of columns in the plot grid
    rows = math.ceil(num_depths / cols)  # Number of rows in the plot grid

    fig, axs = plt.subplots(rows, cols, figsize=(15, 5 * rows))

    for i, (depth, percentages) in enumerate(depth_dict.items()):
        row = i // cols
        col = i % cols
        ax = axs[row, col] if rows > 1 else axs[col]

        if depth > 0:
            ax.set_yscale("log")

        ax.hist(percentages, bins=range(0, 101, 5), edgecolor="black")
        ax.set_title(f"Distribution of Needed Evaluations for Depth {depth+1}")
        ax.set_ylabel("Number of Occurrences")
        ax.set_xticks(range(0, 101, 5))
        ax.grid(True)

    # Hide any empty subplots
    for j in range(i + 1, rows * cols):
        row = j // cols
        col = j % cols
        fig.delaxes(axs[row, col] if rows > 1 else axs[col])

    plt.tight_layout()
    plt.show()


def save_data(depth_dict, output_directory):
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    for depth, percentages in depth_dict.items():
        plt.figure()
        plt.hist(percentages, bins=range(0, 101, 5), edgecolor="black")
        plt.title(f"Distribution of Needed Evaluations for Depth {depth+1}")
        plt.xlabel("Percentage of Needed Evaluations")
        plt.ylabel("Number of Occurrences")
        plt.xticks(range(0, 101, 5))
        plt.grid(True)

        if depth > 0:
            plt.yscale("log")

        # Save the plot
        output_path = os.path.join(
            output_directory, f"depth_{depth+1}_distribution.png"
        )
        plt.savefig(output_path)
        plt.close()


# Main script
directory = "./GomokuBack/move_evaluation"  # Change to the directory where your CSV files are stored
output_directory = "./tmp"
data = read_csv_files(directory)
depth_dict = process_data(data)
# plot_data(depth_dict)
save_data(depth_dict, output_directory)
