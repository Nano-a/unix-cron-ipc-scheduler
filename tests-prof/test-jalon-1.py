import os
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

def test_arborescence_1():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 1:\n'}
    
    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-1.tar.gz",
                                                     182)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = [(0, b"Feed me lasagna\n", b""),
                         (1, b"I hate mondays\n", b"")]
      check_task_output(report, tasks, expected_output)
      
    testsummary(report, jalon=1,test=1)
      
    return report

def test_arborescence_2():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 2:\n'}
    
    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-2.tar.gz",
                                                     182)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      task_id = 4
      try:
        run_time = time.localtime(tasks[task_id]['times-exitcodes'][-1]["time"])
        expected_stdout = (f'Nous sommes le {time.strftime("%d/%m/%Y", run_time)}\n'
                           + f'Il est {time.strftime("%H:%M:%S", run_time)}\n').encode("utf-8")
        expected_stderr = b''
        check_task_output(report, tasks, [(task_id, expected_stdout, expected_stderr)])
      except:
        testfail(report, f'Missing task {task_id} final run time')
        
    testsummary(report, jalon=1,test=2)
    
    return report

def test_arborescence_5():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 5:\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-5.tar.gz",
                                                     182)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=1)
      
      expected_output = [(15, b"Maman, les p'tits bateaux\n"
                          + b"Qui vont sur l'eau\n"
                          + b"Ont-ils des rames ?\n"
                          + b"Maman, les p'tits bateaux\n"
                          + b"Qui vont sur l'eau\n"
                          + b"Ont-ils des voiles ?\n"
                          + b"Maman, les p'tits bateaux\n"
                          + b"Qui vont sur l'eau\n"
                          + b"Ont-ils des jambes ?\n",
                          b"")]
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=5)

    return report

def test_arborescence_6():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 6 (fork):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-6-fork.tar.gz",
                                                     90, check_times=False)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = [(0, b"A\nX\nB\nY\n", b"")]
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=6)

    return report

def test_arborescence_7():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 7 (commandes longue durée):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-7-commandes-longues.tar.gz",
                                                     182, check_times=False)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = []
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=7)

    return report
    
def test_arborescence_8():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 8 (commande longue durée v2):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-8-commandes-longues-chevauchement.tar.gz",
                                                     182, check_times=False)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = []
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=8)

    return report
  
def test_arborescence_9():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 9 (commande longue durée avec écriture):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-9-commandes-longues-stdout.tar.gz",
                                                     160, check_times=False)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = [(0, b"AB\n", b"")]
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=9)

    return report
  
def test_arborescence_10():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 10 (commandes externes imprévisibles):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-10-commandes-externes.tar.gz",
                                                     62)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=56)
      
      expected_output = [(0, b"1+1=2\n", b"")]
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=10)

    return report
  
def test_arborescence_11():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 11 (arborescence profonde):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-11-arborescence-profonde.tar.gz",
                                                     62)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
      
      expected_output = [(30, b'AAAA\nAAAB\nAABA\nAABB\nABAA\nABAB\nABBA\nABBB\nBAAA\nBAAB\nBABA\nBABB\nBBAA\nBBAB\nBBBA\nBBBB\n', b"")]
      
      check_task_output(report, tasks, expected_output)
      
    testsummary(report,jalon=1,test=11)

    return report

def test_arborescence_11_valgrind():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 12 (valgrind):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-11-arborescence-profonde.tar.gz",
                                                     62, use_valgrind=True)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0, use_valgrind=True)
    
    testsummary(report,jalon=1,test=12)

    return report

def test_arborescence_13():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 13 (tâches simultanées):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-13-taches-simultanees.tar.gz",
                                                     168)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
    
    testsummary(report,jalon=1,test=13)

    return report

def test_arborescence_14():
    report = {'nbtests':0, 'failures':0, 'text':'JALON 1, TEST 14 (tâche jamais lancée):\n'}

    (start_time, tasks) = run_daemon_against_archive(report, "../erraid",
                                                     "exemples-arborescences/exemple-arborescence-14-tache-jamais-lancee.tar.gz",
                                                     122)

    if tasks is not None:
      check_task_exitcodes(report, tasks, expected_code=0)
    
    testsummary(report,jalon=1,test=14)

    return report

tests = [test_arborescence_1, test_arborescence_2, test_arborescence_5,
         test_arborescence_6, test_arborescence_7, test_arborescence_8,
         test_arborescence_9, test_arborescence_10, test_arborescence_11,
         test_arborescence_11_valgrind, test_arborescence_13,  test_arborescence_14]

from multiprocessing.pool import ThreadPool
pool = ThreadPool(processes=len(tests))

reports_async = [ pool.apply_async(test, ()) for test in tests ]
reports = [ ar.get() for ar in reports_async ]

report = {'totalok': 0, 'totaltests': 0, 'totalfailures':0, 'text': ''}
for r in reports:
  report['text'] += r['text']
  report['totalfailures'] += r['failures']
  report['totaltests'] += 1
  if r['failures'] == 0:
    report['totalok'] += 1

jalonsummary(report, jalon=1)

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

