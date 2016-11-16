#!/usr/bin/python3

import os
import sys

is_sort_num = lambda x: len(x) == 2 \
	and x[0] == '[sort_num]'

is_map_time = lambda x: len(x) == 4 \
	and x[0] == '[map]'

is_reduce_time = lambda x: len(x) == 4 \
	and x[0] == '[reduce]'

map_sort_num = lambda x: int(x[1])
map_map_time = lambda x: float(x[3])
map_reduce_time = lambda x: float(x[3])

if __name__ == '__main__':
	
	if len(sys.argv) != 2:
		sys.exit("./sort_bench_parse.py [input.txt]")

	INPUT_FILE = sys.argv[1]
	OUTPUT_FILE = sys.argv[1].split('.')[0]+'.csv'

	fin = open(INPUT_FILE, 'r')
	fout = open(OUTPUT_FILE, 'w')

	tokens = list()

	for line in fin:
		tokens.append(line.split())
	
	sort_num = list(map(map_sort_num, list(filter(is_sort_num, tokens))))
	map_time = list(map(map_map_time, list(filter(is_map_time, tokens))))
	reduce_time = list(map(map_reduce_time, list(filter(is_reduce_time, tokens))))

	if len(sort_num) != len(map_time) != reduce_time:
		print("error")
	
	#print(sort_num)
	#print(map_time)
	#print(reduce_time)
	#print(log_sort_num)
	#print(log_reduce_time)

	slop = [0.0]

	fout.write("#Id, sort_num, map_time(s), reduce_time(s)\n")

	for index, value in enumerate(sort_num):
		_sort_num = sort_num[index]
		_map_time = map_time[index]
		_reduce_time = reduce_time[index]

		fout.write('{}, {}, {:f}, {:f}\n'.format(index+1, _sort_num, _map_time, _reduce_time));

	fin.close()
	fout.close()
