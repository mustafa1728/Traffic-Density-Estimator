import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df_times = pd.read_csv("time_used.csv")

i = 0
method1_times = []
method1_parameters = []
method4_times = []
method4_parameters = []


method2_parameters= []
method2_times= []
method3_parameters= []
method3_times= []
method5_parameters= []
method5_times= []


def getAvgSorted(x_list, y_list):
	x_list, y_list = zip(*sorted(zip(x_list, y_list)))
	x_temp = []
	y_temp = []
	i = 0
	while(i<len(x_list)):
		j = i+1
		sum_err = y_list[i]
		count = 1
		while(j<len(x_list) and x_list[i] == x_list[j]):
			sum_err += y_list[j]
			count+=1
			j+=1
		y_temp.append(sum_err/count)
		x_temp.append(x_list[i])
		i = j
	return (x_temp, y_temp)

while(i<len(df_times.iloc[:, 0])):

	if(df_times.iloc[i, 0] == 1):
		method1_parameters.append(int(df_times.iloc[i, 1]))
		method1_times.append(df_times.iloc[i, 2])

	if(df_times.iloc[i, 0] == 2):
		method2_parameters.append(int(df_times.iloc[i, 1][1]) * int(df_times.iloc[i, 1][7]))
		method2_times.append(df_times.iloc[i, 2])

	if(df_times.iloc[i, 0] == 3):
		method3_parameters.append(int(df_times.iloc[i, 1][1]) * int(df_times.iloc[i, 1][7]))
		method3_times.append(df_times.iloc[i, 2])

	if(df_times.iloc[i, 0] == 4):
		method4_parameters.append(int(df_times.iloc[i, 1]))
		method4_times.append(df_times.iloc[i, 2])

	if(df_times.iloc[i, 0] == 5):
		method5_parameters.append(int(df_times.iloc[i, 1][1]) * int(df_times.iloc[i, 1][7]))
		method5_times.append(df_times.iloc[i, 2])
	


	i+=1
	
# Method1
# method1_parameters, method1_times = zip(*sorted(zip(method1_parameters, method1_times)))
method1_parameters, method1_times = getAvgSorted(method1_parameters, method1_times)
plt.plot(method1_parameters, method1_times, label='Method1')
plt.scatter(method1_parameters, method1_times)
plt.ylim(0, 100)


plt.xlabel("Number of Frames skipped")
plt.title("Runtime vs parameter for Method 1")
plt.ylabel('Runtime')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/runtime/method1_time.png", dpi=300)
plt.close()


# Method4
plt.ylim(50, 100)
method4_parameters, method4_times = getAvgSorted(method4_parameters, method4_times)
plt.plot(method4_parameters, method4_times, label='Method4')
plt.scatter(method4_parameters, method4_times, color = 'b')


plt.xlabel("Number of Threads")
plt.title("Runtime vs parameter for Method 4")
plt.ylabel('Runtime')

plt.scatter(method4_parameters, method4_times)
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/runtime/method4_time.png", dpi=300)
plt.close()


# Method2
plt.ylim(45, 75)
method2_parameters, method2_times = getAvgSorted(method2_parameters, method2_times)
plt.plot(method2_parameters, method2_times, label='Method2')
plt.scatter(method2_parameters, method2_times)

plt.xlabel("Reduction factor")
plt.title("Runtime vs parameter for Method 2")
plt.ylabel('Runtime')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/runtime/method2_time.png", dpi=300)
plt.close()


# Method3
plt.ylim(30, 50)
method3_parameters, method3_times = getAvgSorted(method3_parameters, method3_times)
plt.plot(method3_parameters, method3_times, label='Method3')
plt.scatter(method3_parameters, method3_times)

plt.xlabel("Number of threads")
plt.title("Runtime vs parameter for Method 3")
plt.ylabel('Runtime')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/runtime/method3_time.png", dpi=300)
plt.close()

# Method5
# plt.ylim(4, 50)
method5_parameters, method5_times = getAvgSorted(method5_parameters, method5_times)
plt.plot(method5_parameters, method5_times, label='Method5')
plt.scatter(method5_parameters, method5_times)

plt.xlabel("Number of threads")
plt.title("Runtime vs parameter for Method 5")
plt.ylabel('Runtime')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/runtime/method5_time.png", dpi=300)
plt.close()














df_errors = pd.read_csv("errors_used.csv")
# print(df_errors.head())

method1_errors = []
method1_parameters = []
method4_errors = []
method4_parameters = []


method2_parameters= []
method2_errors= []
method3_parameters= []
method3_errors= []
method5_parameters= []
method5_errors= []

i = 0

while(i<len(df_errors.iloc[:, 0])):

	if(int(df_errors.iloc[i, 0]) == 1):
		method1_parameters.append(int(df_errors.iloc[i, 1]))
		method1_errors.append(df_errors.iloc[i, 2])

	if(int(df_errors.iloc[i, 0]) == 2):
		method2_parameters.append(int(df_errors.iloc[i, 1][1]) * int(df_errors.iloc[i, 1][4]))
		method2_errors.append(df_errors.iloc[i, 2])

	if(int(df_errors.iloc[i, 0]) == 3):
		method3_parameters.append(int(df_errors.iloc[i, 1][1]) * int(df_errors.iloc[i, 1][4]))
		method3_errors.append(df_errors.iloc[i, 2])

	if(int(df_errors.iloc[i, 0]) == 4):
		method4_parameters.append(int(df_errors.iloc[i, 1]))
		method4_errors.append(df_errors.iloc[i, 2])
	
	if(int(df_errors.iloc[i, 0]) == 5):
		method5_parameters.append(int(df_errors.iloc[i, 1][1]) * int(df_errors.iloc[i, 1][4]))
		method5_errors.append(df_errors.iloc[i, 2])


	i+=1
	
# Method1

method1_parameters, method1_errors = getAvgSorted(method1_parameters, method1_errors)
plt.plot(method1_parameters, method1_errors, label='Method1')
plt.scatter(method1_parameters, method1_errors)
# plt.ylim(0, 100)


plt.xlabel("Number of Frames skipped")
plt.title("Error vs parameter for Method 1")
plt.ylabel('Error')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/utility/method1_error.png", dpi=300)
plt.close()


# Method4
# plt.ylim(50, 100)
method4_parameters, method4_errors = getAvgSorted(method4_parameters, method4_errors)
plt.plot(method4_parameters, method4_errors, label='Method4')
plt.scatter(method4_parameters, method4_errors, color = 'b')


plt.xlabel("Number of Threads")
plt.title("Error vs parameter for Method 4")
plt.ylabel('Error')

plt.scatter(method4_parameters, method4_errors)
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/utility/method4_error.png", dpi=300)
plt.close()


# Method2
# plt.ylim(45, 75)


method2_parameters, method2_errors = getAvgSorted(method2_parameters, method2_errors)
# method2_parameters, method2_errors = zip(*sorted(zip(method2_parameters, method2_errors)))

plt.plot(method2_parameters, method2_errors, label='Method2')
plt.scatter(method2_parameters, method2_errors)

plt.xlabel("Reduction factor")
plt.title("Error vs parameter for Method 2")
plt.ylabel('Error')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/utility/method2_error.png", dpi=300)
plt.close()


# Method3
# plt.ylim(30, 0)
method3_parameters, method3_errors = getAvgSorted(method3_parameters, method3_errors)
plt.plot(method3_parameters, method3_errors, label='Method3')
plt.scatter(method3_parameters, method3_errors)

plt.xlabel("Number of threads")
plt.title("Error vs parameter for Method 3")
plt.ylabel('Error')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/utility/method3_error.png", dpi=300)
plt.close()


# Method5
# plt.ylim(30, 50)
method5_parameters, method5_errors = getAvgSorted(method5_parameters, method5_errors)
plt.plot(method5_parameters, method5_errors, label='Method5')
plt.scatter(method5_parameters, method5_errors)

plt.xlabel("Number of threads")
plt.title("Error vs parameter for Method 5")
plt.ylabel('Error')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/utility/method5_error.png", dpi=300)
plt.close()







# Runtime vs error graphs
utilities =[1 - i for i in method1_errors]
plt.scatter(method1_times, utilities)

plt.xlabel("Runtime")
plt.title("Utility vs Runtime for Method 1")
plt.ylabel('Utility')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/tradeoff/method1_tradeoff.png", dpi=300)
plt.close()




utilities =[1 - i for i in method2_errors]
plt.scatter(method2_times, utilities)

plt.xlabel("Runtime")
plt.title("Utility vs Runtime for method 2")
plt.ylabel('Utility')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/tradeoff/method2_tradeoff.png", dpi=300)
plt.close()




utilities =[1 - i for i in method3_errors]
plt.scatter(method3_times, utilities)

plt.xlabel("Runtime")
plt.title("Utility vs Runtime for method 3")
plt.ylabel('Utility')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/tradeoff/method3_tradeoff.png", dpi=300)
plt.close()



utilities =[1 - i for i in method4_errors]
plt.scatter(method4_times, utilities)

plt.xlabel("Runtime")
plt.title("Utility vs Runtime for method 4")
plt.ylabel('Utility')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/tradeoff/method4_tradeoff.png", dpi=300)
plt.close()


utilities =[1 - i for i in method5_errors]
plt.scatter(method5_times, utilities)

plt.xlabel("Runtime")
plt.title("Utility vs Runtime for method 5")
plt.ylabel('Utility')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/tradeoff/method5_tradeoff.png", dpi=300)
plt.close()






# CPU utilizations
df_cpu = pd.read_csv("CPU_utilisation.csv")

method1_cpu_usage = []
method1_parameters = []
method4_cpu_usage = []
method4_parameters = []


method2_parameters= []
method2_cpu_usage= []
method3_parameters= []
method3_cpu_usage= []
method5_parameters= []
method5_cpu_usage= []


i = 0

def convertToTime(string_time):
	minutes = ""
	seconds = ""
	minutes_encountered = False 
	for s in string_time:
		if(s=='m'):
			minutes_encountered = True
			continue
		if(s=='s'):
			break
		if(minutes_encountered):
			seconds += s
		else:
			minutes+=s

	return 60*int(minutes) + float(seconds)




while(i<len(df_cpu.iloc[:, 0])):
	if(int(df_cpu.iloc[i, 0]) == 1):
		method1_parameters.append(int(df_cpu.iloc[i, 1]))
		method1_cpu_usage.append((convertToTime(df_cpu.iloc[i, 5]) + convertToTime(df_cpu.iloc[i, 4])) / convertToTime(df_cpu.iloc[i, 3]))

	if(int(df_cpu.iloc[i, 0]) == 2):
		method2_parameters.append(int(df_cpu.iloc[i, 1]) * int(df_cpu.iloc[i, 2]))
		method2_cpu_usage.append((convertToTime(df_cpu.iloc[i, 5]) + convertToTime(df_cpu.iloc[i, 4])) / convertToTime(df_cpu.iloc[i, 3]))

	if(int(df_cpu.iloc[i, 0]) == 3):
		method3_parameters.append(int(df_cpu.iloc[i, 1]) * int(df_cpu.iloc[i, 2]))
		method3_cpu_usage.append((convertToTime(df_cpu.iloc[i, 5]) + convertToTime(df_cpu.iloc[i, 4])) / convertToTime(df_cpu.iloc[i, 3]))

	if(int(df_cpu.iloc[i, 0]) == 4):
		method4_parameters.append(int(df_cpu.iloc[i, 1]))
		method4_cpu_usage.append((convertToTime(df_cpu.iloc[i, 5]) + convertToTime(df_cpu.iloc[i, 4])) / convertToTime(df_cpu.iloc[i, 3]))
	
	if(int(df_cpu.iloc[i, 0]) == 5):
		method5_parameters.append(int(df_cpu.iloc[i, 1]) * int(df_cpu.iloc[i, 2]))
		method5_cpu_usage.append((convertToTime(df_cpu.iloc[i, 5]) + convertToTime(df_cpu.iloc[i, 4])) / convertToTime(df_cpu.iloc[i, 3]))


	i+=1


# Method1

method1_parameters, method1_cpu_usage = getAvgSorted(method1_parameters, method1_cpu_usage)
plt.plot(method1_parameters, method1_cpu_usage, label='Method1')
plt.scatter(method1_parameters, method1_cpu_usage)
# plt.ylim(0, 100)


plt.xlabel("Number of Frames skipped")
plt.title("CPU Usage vs parameter for Method 1")
plt.ylabel('CPU Usage')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/cpu/method1_cpu.png", dpi=300)
plt.close()


# Method4
# plt.ylim(50, 100)
method4_parameters, method4_cpu_usage = getAvgSorted(method4_parameters, method4_cpu_usage)
plt.plot(method4_parameters, method4_cpu_usage, label='Method4')
plt.scatter(method4_parameters, method4_cpu_usage, color = 'b')


plt.xlabel("Number of Threads")
plt.title("CPU Usage vs parameter for Method 4")
plt.ylabel('CPU Usage')

plt.scatter(method4_parameters, method4_cpu_usage)
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/cpu/method4_cpu.png", dpi=300)
plt.close()


# Method2
# plt.ylim(45, 75)


method2_parameters, method2_cpu_usage = getAvgSorted(method2_parameters, method2_cpu_usage)
# method2_parameters, method2_usage = zip(*sorted(zip(method2_parameters, method2_usage)))

plt.plot(method2_parameters, method2_cpu_usage, label='Method2')
plt.scatter(method2_parameters, method2_cpu_usage)

plt.xlabel("Reduction factor")
plt.title("CPU Usage vs parameter for Method 2")
plt.ylabel('CPU Usage')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/cpu/method2_cpu.png", dpi=300)
plt.close()


# Method3
# plt.ylim(30, 0)
method3_parameters, method3_cpu_usage = getAvgSorted(method3_parameters, method3_cpu_usage)
plt.plot(method3_parameters, method3_cpu_usage, label='Method3')
plt.scatter(method3_parameters, method3_cpu_usage)

plt.xlabel("Number of threads")
plt.title("CPU Usage vs parameter for Method 3")
plt.ylabel('CPU Usage')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/cpu/method3_cpu.png", dpi=300)
plt.close()


# Method5
# plt.ylim(30, 50)
method5_parameters, method5_cpu_usage = getAvgSorted(method5_parameters, method5_cpu_usage)
plt.plot(method5_parameters, method5_cpu_usage, label='Method5')
plt.scatter(method5_parameters, method5_cpu_usage)

plt.xlabel("Number of threads")
plt.title("CPU Usage vs parameter for Method 5")
plt.ylabel('CPU Usage')
figure = plt.gcf()

figure.set_size_inches(7, 5)
plt.savefig("plots/cpu/method5_cpu.png", dpi=300)
plt.close()
