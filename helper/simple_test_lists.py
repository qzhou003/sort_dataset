#!/usr/bin/env python

import glob, os, sys
dir="/home/admobilize/Qi/distracted_driver/imgs/test/"
os.chdir(dir)

for dirpath, dirnames, files in os.walk('.'):
	if files:
		print "There are %d files" % len(files)
		for each in files:
			to_write = each + "\n"
			with open("/home/admobilize/Qi/git/sort_HK_dataset/helper/test.txt","a") as text_file:
				text_file.write(to_write)

