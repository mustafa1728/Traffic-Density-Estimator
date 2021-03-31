import numpy as np
import os
import pandas as pd
import math

baseline_file_path = "out_baseline.csv"
df_baseline = pd.read_csv(baseline_file_path)


output_folder_path = "./out_files"

file_list= [file for file in os.listdir(output_folder_path) if file.endswith('.csv')]

errors = {"method": [], "parameters": [], "error": []}
df_errors = pd.DataFrame(data=errors)

for file in file_list:
	# file_path = "out.csv"
	# # if(file!=file_path):
	# # 	continue
	# file = file_path
	# df = pd.read_csv("out.csv")
	try:
		df = pd.read_csv(os.path.join(output_folder_path, file))
		# print(df.head())
	except:
		print("not read ", file)
		continue
	i = 0
	j = 0
	queue_total_square_diff = 0
	count = 0
	if df.empty:
		print("this file was empty!")
		continue
	while(df_baseline.iloc[i, 0]=="The video is over!"):
		i+=1
	while(df.iloc[j, 0]=="The video is over!"):
		j+=1

	while(i<len(df_baseline.iloc[:, 1]) and j<len(df.iloc[:, 1])):
		temp = (float(df_baseline.iloc[i, 1]) - float(df.iloc[i, 1]))**2
		if(not math.isnan(temp)):
			queue_total_square_diff += temp
		# print(queue_total_square_diff, '\r')
		count+=1
		i+=1
		j+=1
	queue_rmse = 0
	if(count!=0):
		queue_rmse = np.sqrt(queue_total_square_diff/count)

	method = int(file[6])
	print("method: ", method)
	parameters = []
	if(method == 1 or method == 4):
		parameters = file[8]
	elif(method == 2 or method == 3 or method == 5):
		parameters = (int(file[8]), int(file[10]))

	errors["method"].append(method)
	errors["parameters"].append(parameters)
	errors["error"].append(queue_rmse)
	print("Error in Queue Denisty of the file: ", file, " is ", queue_rmse)

df_errors = pd.DataFrame(data=errors)
df_errors.to_csv('errors.csv', index=False)
print(df_errors.head())


