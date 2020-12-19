#!/bin/python3

import sys, os, requests

session = input("please enter your session cookie: ")

MAX_DAY = 19

os.makedirs("./inp", exist_ok=True)

for day in range(1, MAX_DAY + 1):
	print(f"downloading day {day}...")
	link = f"https://adventofcode.com/2020/day/{day}/input"
	resp = requests.get(link, cookies={ "session": session })
	if resp.status_code != 200:
		print(f"Failed to get input, got status {resp.status_code}!")
		exit(1)
	with open(f"./inp/{day}.txt", "w") as fp:
		fp.write(resp.text)
	
