import numpy as np
import os
import pandas as pd

baseline_file_path = "out.txt"
df_baseline = pd.read_csv(baseline_file_path)


output_folder_path = "./out_files"

file_list= [file for file in os.listdir(output_folder_path) if file.endswith('.txt')]


for file in file_list:
	df = pd.read_csv(os.path.join(output_folder_path, file))
	queue_error = ((df.iloc[1] - df_baseline.iloc[1]) ** 2).mean() ** .5
	dynamic_error = ((df.iloc[2] - df_baseline.iloc[2]) ** 2).mean() ** .5
	print("Errors of the file: ", file, " are ", queue_error, " and ", dynamic_error)