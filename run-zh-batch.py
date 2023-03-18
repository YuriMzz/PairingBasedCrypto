#!/usr/bin/python3
import io
import os
import time
from subprocess import Popen, PIPE
import signal
from contextlib import contextmanager
import re
from shutil import copyfile
from random import randint, seed
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
	user=""
	for _ in range(0,attributes):
		r=randint(0,2)
		if(r==0):
			policy+="p"
			user+="p"
		if(r==1):
			policy+="n"
			user+="n"
		if(r==2):
			policy+="w"
			r=randint(0,1)
			if(r==1):
				user+="p"
			else:
				user+="n"
	return policy, user

basepath = '/home/user/contiki-ng/'
b_folder='benchmarksZH'
power_monitor_cmd='/home/user/contiki-ng/power_meter.py'
try:
    os.mkdir(b_folder)
except OSError:
    pass
#CURVES = 'B12-P381 B12-P446 BN-P446'.split()
curve = "BN-P158"
REPETITIONS=30
tic=0
def stacktime():
	assert exec('make', 'relic_clean') == 0
	bench_type='BENCH_SPACE'
	for ATTRIBUTES in (5, 10, 20, 40):
		filename = f'{curve}_{str(ATTRIBUTES).zfill(2)}_{bench_type[6:]}.csv'

		# use previous results
		with output('ls', b_folder) as out:
			if any(filename in line for line in out):
				continue
		for _ in range(0,REPETITIONS):		
			relic_cmake_script = f'relic-cmake-{curve}.sh'
			try:
				pol, attr=random_policy(ATTRIBUTES)
				with output('make', 
					f'librelic',
					f'{bench_type}=1',
					f'FUNCTIONS=ZH ',
					f'ZH_N_ATTR={ATTRIBUTES}',	 
					f"ZH_POLICY_STRING='\"{pol}\"'",
					f"ZH_USER_STRING='\"{attr}\"'",
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
								tic=time.perf_counter()
								power_monitor=Popen(power_monitor_cmd, stdout=PIPE, universal_newlines=True)
								continue
						m = re.match(r'^(?:TIME|STACK): (\w+)\s+= ([\d\s]+)$', line)
						if not m:
							continue
						toc = time.perf_counter()
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
							py_time=(toc-tic)
							#csv+=f'{func}_py,{py_time}\n'
							debug(str(('py', py_time)))
				
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
	seed(10)
	""" pol, attr=random_policy(50)
	print(pol)
	print(attr)
	exit(1) """
	
	exit(stacktime())
