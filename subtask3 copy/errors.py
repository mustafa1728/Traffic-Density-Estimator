import numpy as np
import os
import pandas as pd

baseline_file_path = "out_baseline.csv"
df_baseline = pd.read_csv(baseline_file_path)


output_folder_path = "./out_files"

file_list= [file for file in os.listdir(output_folder_path) if file.endswith('.csv')]


for file in file_list:
	# file_path = "method1_1_out.csv"
	# if(file!=file_path):
	# 	continue
	df = pd.read_csv(os.path.join(output_folder_path, file))
	i = 0
	j = 0
	queue_total_square_diff = 0
	count = 0
	while(i<len(df_baseline.iloc[:, 0]) and j<len(df.iloc[:, 0])):
		if(df_baseline.iloc[i, 0] == df.iloc[i, 0]):
			queue_total_square_diff += (df_baseline.iloc[i, 1] - df.iloc[i, 1])**2
			count+=1
			i+=1
			j+=1
		elif(df_baseline.iloc[i, 0] < df.iloc[i, 0]):
			i+=1
		else:
			j+=1
	queue_rmse = np.sqrt(queue_total_square_diff/count)

	print("Error in Queue Denisty of the file: ", file, " is ", queue_rmse)






