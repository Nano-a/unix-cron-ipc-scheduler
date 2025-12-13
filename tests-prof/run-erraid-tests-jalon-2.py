import os
import itertools
from common import *

if not ENV_GRADE_PROJECT:
  print('Compiling...')
r = os.system("cd .. >/dev/null 2>&1 && make >/dev/null 2>&1 </dev/null")
if r != 0:
  print(f'FAIL: Compilation exited with non-zero exit code {r}. Aborting tests')
  exit(1)
if not os.path.isfile("../erraid"):
  print(f'FAIL: File ../erraid not present after compilation. Aborting tests')
  exit(1)

exec_path = f'{os.getcwd()}/../erraid'
recording_prefix = "erraid-tests/erraid-jalon-2-test-"

for i in itertools.count(start=1):
  path = f'{recording_prefix}{i+1}'
  if not os.path.isdir(path):
    nb_tests = i
    break
tests = [ (lambda i=i: lambda : daemon_replay(exec_path, f'{recording_prefix}{i}', (2,i),
                                 use_valgrind=os.path.exists(f'{path}/valgrind'))) ()
          # Ugly hack because python closures capture references, not values
          for i in range(1, nb_tests+1) ]
from multiprocessing.pool import ThreadPool
pool = ThreadPool(processes=len(tests))

#reports_async = [ pool.apply_async(test, ()) for test in tests ]
#reports = [ ar.get() for ar in reports_async ]

reports = [ f() for f in tests ]

report = {'totalok': 0, 'totaltests': 0, 'totalfailures':0, 'text': ''}
for r in reports:
  report['text'] += r['text']
  report['totalfailures'] += r['failures']
  report['totaltests'] += 1
  if r['failures'] == 0:
    report['totalok'] += 1

jalonsummary(report, jalon=2)

if ENV_GRADE_PROJECT:
  penalty = int(os.getenv("GRADE_PROJECT_PENALTY", "0")) / 100
  grade = report['totalok']*10/report['totaltests']-penalty
  if grade < 0:
    grade = 0
  if grade == int(grade):
    grade_str = f"{int(grade)}"
  else:
    grade_str = f"{grade:.1f}"
  report['text'] += f"\nNote : {grade_str} / 10\n"

print("\n"+report['text'])

