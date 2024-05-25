import matplotlib.pyplot as plt
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import GradientBoostingRegressor
from sklearn.feature_selection import SelectKBest, f_regression
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score

# Read data from the text file
# data = []
# with open('GomokuEngine/data_points_default_d6.txt', 'r') as file:
#     for line in file:
#         if line.strip():  # Ensure that the line is not empty
#             datas = line.split(',')
#             print(datas)
#             turn, x, y = int(datas[0]), int(datas[-2]), int(datas[-1])
#             if y > 1000000:
#                 y = 200000
#             if y < -1000000:
#                 y = -200000
#             data.append((x, y))

# # Separate the data into two lists: one for x values and one for y values
# x_values, y_values = zip(*data)

# # Create a scatter plot
# plt.figure(figsize=(10, 6))
# plt.scatter(x_values, y_values, color='blue', marker='o')

# # Add titles and labels
# plt.title('Data Plot from Text File')
# plt.xlabel('Initial score of the board')
# plt.ylabel('Score of the board after 6 moves')
# # plt.yscale('log')
# # plt.xscale('log')

# # Show the plot
# plt.grid(True)
# plt.show()




# Load the CSV file
file_path = './GomokuEngine/data_points_default_d6.csv'
data = pd.read_csv(file_path)

# Define the score ranges for clusters
score_bins = [-float('inf'), -500000, -50000, -15000, -5000, 5000, 15000, 50000, 500000, float('inf')]
score_labels = [
    '-inf to -500000',
    '-500000 to -50000',
    '-50000 to -15000',
    '-15000 to -5000',
    '-5000 to 5000',
    '5000 to 15000',
    '15000 to 50000',
    '50000 to 500000',
    '500000 to inf'
]

# Assign each score to a cluster based on the defined bins
data['Score Cluster'] = pd.cut(data['Score'], bins=score_bins, labels=score_labels)

# Analyzing the clusters based on the new score ranges
custom_cluster_analysis = data.groupby('Score Cluster').agg({
    'Score': ['mean', 'min', 'max', 'std', 'size'],
    'Capture': ['mean', 'min', 'max', 'std'],
    'Five or More': ['mean', 'min', 'max', 'std'],
    'Open one': ['mean', 'min', 'max', 'std'],
    'Half open one': ['mean', 'min', 'max', 'std'],
    'Open two': ['mean', 'min', 'max', 'std'],
    'Half open two': ['mean', 'min', 'max', 'std'],
    'Open three': ['mean', 'min', 'max', 'std'],
    'Half open three': ['mean', 'min', 'max', 'std'],
    'open four': ['mean', 'min', 'max', 'std'],
    'four': ['mean', 'min', 'max', 'std'],
    'Capture.1': ['mean', 'min', 'max', 'std'],
    'Five or More.1': ['mean', 'min', 'max', 'std'],
    'Open one.1': ['mean', 'min', 'max', 'std'],
    'Half open one.1': ['mean', 'min', 'max', 'std'],
    'Open two.1': ['mean', 'min', 'max', 'std'],
    'Half open two.1': ['mean', 'min', 'max', 'std'],
    'Open three.1': ['mean', 'min', 'max', 'std'],
    'Half open three.1': ['mean', 'min', 'max', 'std'],
    'open four.1': ['mean', 'min', 'max', 'std'],
    'four.1': ['mean', 'min', 'max', 'std']
})


print(custom_cluster_analysis)
# prind custom_cluster_analysis in a csv file
custom_cluster_analysis.to_csv('custom_cluster_analysis.csv')
