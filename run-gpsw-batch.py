#!/usr/bin/python3
import io
import os
import time
from subprocess import Popen, PIPE
import signal
from contextlib import contextmanager
import re
from shutil import copyfile
from random import randint
from numpy import arange
logfile = 'log.txt'
#logfile = '/dev/null'

def debug(x, also_print=True):
	with open(logfile, 'a') as f:
		f.write(x)
		f.write('\n')
	if also_print:
		print(x)


@contextmanager
def output(*cmd):
	debug(f'output({cmd})')
	try:
		proc = Popen(cmd, stdout=PIPE, stderr=PIPE)
		yield map(str.strip, io.TextIOWrapper(proc.stdout, encoding="utf-8"))
	finally:
		proc.send_signal(signal.SIGTERM)
		proc.wait()

def exec(*cmd):
	debug(f'exec({cmd})')
	proc = Popen(cmd, stdout=PIPE, stderr=PIPE)
	print(cmd)
	proc.wait()
	debug('\n'.join(map(str.strip, io.TextIOWrapper(proc.stdout, encoding="utf-8"))), False)
	return proc.returncode


def random_policy(attributes):
	policy=""
	nodes=arange(0,attributes).tolist()
	while(len(nodes)>1):
		childrens=randint(2,len(nodes))
		n=childrens
		node=[]
		while(childrens>0):
			idx=randint(0,len(nodes)-1)
			ch=nodes.pop(idx)
			node.append(ch)
			if(not isinstance(ch,list)):
				policy+=(str(ch)+"_")
			childrens-=1
			k=randint(2,n)
		policy+=(str(k)+"of"+str(n)+"_")
		nodes.append(node)
	return policy

def worst_case_policy(attributes):
	policy="_".join(arange(0,attributes).astype(str))
	policy+="_"+str(attributes)+"of"+str(attributes)
	return policy
basepath = '/home/user/contiki-ng/'
b_folder='benchmarksGPSW_N_W'
power_monitor_cmd='/home/user/contiki-ng/power_meter.py'
try:
    os.mkdir(b_folder)
except OSError:
    pass
#CURVES = 'B12-P381 B12-P446 BN-P446'.split()
curve = "BN-P158"
REPETITIONS=1
def stacktime():
	assert exec('make', 'relic_clean') == 0
	bench_type='BENCH_SPACE'
	for ATTRIBUTES in [5 ,10 ,20]:
		filename = f'{curve}_{ATTRIBUTES}_{bench_type[6:]}.csv'

		# use previous results
		with output('ls', b_folder) as out:
			if any(filename in line for line in out):
				continue
		for _ in range(0,REPETITIONS):		
			relic_cmake_script = f'relic-cmake-{curve}.sh'
			try:
				with output('make',
					f'librelic',
					f'FUNCTIONS=GPSW',
					f'{bench_type}=1',
					f'GPSW_SIM=1',
					f'GPSW_N_ATTR={ATTRIBUTES}',	 
					f"GPSW_POLICY_STRING='\"{worst_case_policy(ATTRIBUTES)}\"'",
				  	f'RELIC_CMAKE_SCRIPT={basepath}/presets/{relic_cmake_script}',
					f'main.upload', f'login'
				) as out:
					csv = 'function,metric\n'
					if bench_type=='BENCH_TIME':
						csv_e = csv
					for line in out:
						debug(line, False)

						if line == '-- END':
							break

						if line == 'hardfault':
							raise Exception(line)
						
						if bench_type=='BENCH_TIME':
							if re.match(r'function started$', line):
								power_monitor=Popen(power_monitor_cmd, stdout=PIPE, universal_newlines=True)
								continue
						m = re.match(r'^(?:TIME|STACK): (\w+)\s+= ([\d\s]+)$', line)
						if not m:
							continue
						func, quant = m.groups()
						debug(str((func, quant)))
						csv += f'{func},{",".join(quant.split())}\n'
						if bench_type=='BENCH_TIME':
							power_monitor.terminate()
							power_monitor.wait()
							lines=power_monitor.stdout.readlines()
							energy=float(lines[0])
							last_p=float(lines[1])
							stime=float(quant)
							energy+=((stime%20)*last_p)/3600
							csv_e+=f'{func},{energy}\n'
				
					with open(f'{b_folder}/{filename}', 'a') as f:
						f.write(csv)
					if bench_type=='BENCH_TIME':
						with open(f'{b_folder}/{curve}_{ATTRIBUTES}_ENERGY.csv', 'a') as f:
							f.write(csv_e)
				copyfile('target/include/relic_conf.h',f'{b_folder}/{curve}.h')
			except Exception as e:
				debug(f'''{"="*20}\nerror\n{e}\nwhile processing\n{filename}\n{"="*20}''')
				debug(str(e))

if __name__ == '__main__':
	exit(stacktime())
