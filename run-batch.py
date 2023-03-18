#!/usr/bin/python3
import io
import os
import time
from subprocess import Popen, PIPE
import signal
from contextlib import contextmanager
import re
from shutil import copyfile
#logfile = 'benchmarks/log.txt'
logfile = '/dev/null'

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

basepath = '/home/user/contiki-ng/'
b_folder='benchmarksIBEnew'
power_monitor_cmd='/home/user/contiki-ng/power_meter.py'
try:
    os.mkdir(b_folder)
except OSError:
    pass
#CURVES = 'B12-P381 B12-P446 BN-P446'.split()
CURVES = 'B24-P315 B12-P377'.split()
def stacktime():

	# for curve in ('B12-P446','BN-P446'):
	# for curve in ('BN-P254','BN-P256'):
	for curve in CURVES:
		for bench_type in ('BENCH_SPACE', 'BENCH_TIME'):
			filename = f'{curve}_{bench_type[6:]}.csv'

			# use previous results
			with output('ls', b_folder) as out:
				if any(filename in line for line in out):
					continue
				
			relic_cmake_script = f'relic-cmake-{curve}.sh'
			try:
				assert exec('make', 'relic_clean') == 0
				assert exec('make', f'{bench_type}=1',
					f'FUNCTION=IBE',
				  f'RELIC_CMAKE_SCRIPT={basepath}/presets/{relic_cmake_script}') == 0
				copyfile('target/include/relic_conf.h',f'{b_folder}/{curve}.h')
				with output('make', f'{bench_type}=1',  'main.upload', 'login') as out:
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
						# STACK: g1_rand    = 5400 
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
				
				with open(f'{b_folder}/{filename}', 'w') as f:
					f.write(csv)
				if bench_type=='BENCH_TIME':
					with open(f'{b_folder}/{curve}_ENERGY.csv', 'w') as f:
						f.write(csv_e)
			except Exception as e:
				debug(f'''{"="*20}\nerror\n{e}\nwhile processing\n{filename}\n{"="*20}''')
				debug(str(e))
bss = 0x00221b
text = 0x009f69
data = 0x000210
def textbss():
	filename = f'textbss.csv'
	with open(f'{b_folder}/{filename}', 'w') as f:
		f.write('curve, .text, .bss + .data\n')

		for curve in CURVES:
			print(curve)
			relic_cmake_script = f'relic-cmake-{curve}.sh'

			assert exec('make', 'relic_clean') == 0
			assert exec('make', f'RELIC_CMAKE_SCRIPT={basepath}/presets/{relic_cmake_script}') == 0

			with output('readelf', '-S', 'main.zoul') as out:
				bb = None
				tt = None
				dd = None
				for line in out:
					b = re.search(r'\.bss\s+NOBITS\s+[0-9a-f]+\s[0-9a-f]+\s([0-9a-f]+)', line)
					t = re.search(r'\.text\s+PROGBITS\s+[0-9a-f]+\s[0-9a-f]+\s([0-9a-f]+)', line)
					d = re.search(r'\.data\s+PROGBITS\s+[0-9a-f]+\s[0-9a-f]+\s([0-9a-f]+)', line)

					if b:
						bb = int(b.groups()[0], 16)
					elif t:
						tt = int(t.groups()[0], 16)
					elif d:
						dd = int(d.groups()[0], 16)

			assert bb and tt and dd
			print(f'{hex(bb)} {hex(tt)} {hex(dd)}')

			f.write(f'{curve}, {tt - text}, {( bb - bss ) + (dd - data)}\n')



if __name__ == '__main__':
	exit(stacktime())
