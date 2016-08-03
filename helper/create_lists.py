#!/usr/bin/env python

import glob, os, sys
dir="/home/admobilize/Qi/distracted_driver/imgs/test/"
# os.chdir(dir)

files = os.listdir(dir)

# from pprint import pprint
# pprint(files)

for sub_dir in files :
	if sub_dir == ".DS_Store":
		continue
	full_sub_dir = dir + sub_dir
	# print sub_dir
	os.chdir(full_sub_dir)
	for dirpath, dirnames, files in os.walk('.'):
		if files:
			for each in files :
				# the following line is for distracted driver dataset
				class_label = sub_dir[-1:]
				to_write = sub_dir + "/" + each + " " + class_label + "\n"
				with open("/home/admobilize/Qi/git/sort_HK_dataset/helper/test.txt","a") as text_file:
					text_file.write(to_write)
