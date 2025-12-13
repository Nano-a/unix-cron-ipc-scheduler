import struct
import tempfile
import os
import stat
import pwd
import tarfile
import subprocess
import signal
import time
import locale
import itertools
import traceback
import random
import select

verbose = False

if os.getenv("GRADE_PROJECT", "false") == "true":
  ENV_GRADE_PROJECT = True
else:
  ENV_GRADE_PROJECT = False

def testfail(report, msg):
  report['failures'] += 1
  report['text'] += f'Erreur : {msg}\n'

def testmsg(report, msg):
  report['text'] += f'{msg}\n'
  
def testsummary(report,jalon,test):
  report['nbtests'] += 1
  if report['failures'] == 0:
    report['text'] += f'Résultat : OK\n'
  else:
    report['text'] += f'Résultat : échec ({report["failures"]} erreurs)\n'
  report['text'] +=   f'============\n\n'

def jalonsummary(report, jalon):
  report['text'] += f'Jalon {jalon} : {report["totalok"]}/{report["totaltests"]} tests réussis, {report["totalfailures"]} échecs\n'

locale.setlocale(locale.LC_ALL)

class EmptyRead(Exception):
  pass

class PartialRead(Exception):
  pass

class TestFail(Exception):
  pass

class Stop(Exception):
  pass


def timing_deserialize(f):
  minutes_b = f.read(8)
  if (len(minutes_b) == 0):
    raise EmptyRead
  try:
    minutes = struct.unpack('>Q', minutes_b)[0]
    hours = struct.unpack('>I', f.read(4))[0]
    days = struct.unpack('>B', f.read(1))[0]
    return { 'minutes': minutes, 'hours': hours, 'days': days }
  except struct.error:
    raise PartialRead

def read_timing_file(run_dir, task_id):
  file_name = f"tasks/{task_id}/timing"
  with open(f"{run_dir}/{file_name}", mode='rb') as f:
    try:
      return timing_deserialize(f)
    except:
      raise TestFail('Fichier invalide : ' + file_name)

def read_tx_file(run_dir, task_id):
  tx = []
  file_name = "/tasks/" + str(task_id) + "/times-exitcodes"
  with open(run_dir + file_name, mode='rb') as f:
    while True:
      time_bytes = f.read(8)
      if len(time_bytes) == 0:
        break
      try:
        time = struct.unpack('>Q', time_bytes)[0]
        exit_code = struct.unpack('>H', f.read(2))[0]
      except struct.error:
        raise TestFail('File: ' + file_name + '\nEntrée incomplète dans le tableau times-exitcodes \
 (après ' + str(len(tx)) + ' entrées complètes)')
      tx.append({'time': time, 'exit_code': exit_code})
  return tx

def read_stdout_file(run_dir, task_id):
  file_name = f"tasks/{task_id}/stdout"
  try:
    with open(f"{run_dir}/{file_name}", mode='rb') as f:
      return f.read()
  except FileNotFoundError:
    return b'-'

def read_stderr_file(run_dir, task_id):
  file_name = f"tasks/{task_id}/stderr"
  try:
    with open(f"{run_dir}/{file_name}", mode='rb') as f:
      return f.read()
  except FileNotFoundError:
    return b'-'

def unsafe_run_daemon_against_archive(report, exec_absolute_path, archive_path,
                                      duration_seconds, check_times=True, use_valgrind=False):
  prefix_dir = f'/tmp/{pwd.getpwuid(os.getuid()).pw_name}/erraid-tests'
  os.makedirs(prefix_dir, exist_ok = True)
  with tempfile.TemporaryDirectory(dir = prefix_dir) as tmp_dir:
    with tarfile.open(archive_path) as tar:
      tar.extractall(tmp_dir)
    run_dir = f'{tmp_dir}/tmp-username-erraid'
    if (time.localtime().tm_sec >= 11):
      time.sleep(random.randint(62,70)-time.localtime().tm_sec)
    elif (time.localtime().tm_sec < 1):
      time.sleep(random.randint(2,10))
    tasks_before = {}
    for task_DirEntry in os.scandir(f'{run_dir}/tasks'):
      if (task_DirEntry.name.isdigit() and task_DirEntry.is_dir(follow_symlinks=False)):
        task_id = int(task_DirEntry.name)
        tasks_before.update({task_id: {'stdout': read_stdout_file(run_dir, task_id),
                                'stderr': read_stdout_file(run_dir, task_id),
                                'times-exitcodes': read_tx_file(run_dir, task_id)}})
    start_time = int(time.time())
    argv = [exec_absolute_path, '-r', run_dir]
    if use_valgrind:
      argv = ['valgrind', '--error-exitcode=33', '--errors-for-leak-kinds=definite',
              '--leak-check=full', '--log-fd=1', '--quiet', '--'] + argv
    with subprocess.Popen(argv,
                          cwd = run_dir, start_new_session=True,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, stdin=subprocess.DEVNULL) as process:
      timed_out = False
      try:
        stdout, stderr = process.communicate(timeout=duration_seconds)
        dt = start_time + duration_seconds - time.time()
        if (dt > 0):
          time.sleep(dt)
        raise Stop
      except (subprocess.TimeoutExpired, Stop) as e:
        timed_out = True
        try:
          os.killpg(os.getpgid(process.pid), signal.SIGTERM)          
          try:
            stdout, stderr = process.communicate(timeout=5)
          except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(process.pid), signal.SIGKILL)
            stdout, stderr = process.communicate(timeout=1)
        except:
          pass
      exit_code = process.returncode
    #print(stdout.decode('utf-8')) # DEBUG
    #print(stderr.decode('utf-8')) # DEBUG
    if not timed_out:
      if exit_code >= 0:
        reason = f'a quitté prématurément avec le code de retour {exit_code}'
      else:
        reason = f'a été tué par le signal numéro {-exit_code}'
      testfail(report, f'Le démon {reason}\n'
               + f'stdout:\n{stdout.decode("utf-8")}'
               + f'stderr:\n{stderr.decode("utf-8")}')
    tasks = {}
    for task_DirEntry in os.scandir(f'{run_dir}/tasks'):
      if (task_DirEntry.name.isdigit() and task_DirEntry.is_dir(follow_symlinks=False)):
        task_id = int(task_DirEntry.name)
        tasks.update({task_id: {'stdout': read_stdout_file(run_dir, task_id),
                                'stderr': read_stderr_file(run_dir, task_id),
                                'times-exitcodes': read_tx_file(run_dir, task_id)}})
    if check_times:
      for task_id, task_info_before in tasks_before.items():
        check_task_times(report, run_dir, start_time, duration_seconds,
                         task_id, task_info_before['times-exitcodes'],
                         tasks[task_id]['times-exitcodes'])
    return (start_time, tasks)

def run_daemon_against_archive(report, exec_path, archive_path,
                               duration_seconds, check_times=True, use_valgrind=False):
  try:
    if not ENV_GRADE_PROJECT:
      print(f'Je lance {exec_path} sur l\'archive {archive_path} forpendant {duration_seconds} secondes...')
    return (unsafe_run_daemon_against_archive(report, os.getcwd() + "/" + exec_path, archive_path,
                                              duration_seconds, check_times, use_valgrind))
  except TestFail as e:
    testfail(report, str(e))
    return (int(time.time()), None)
  except Exception as e:
    nl = '\n'
    testfail(report, f'Exception inattendue :\n{nl.join(traceback.format_exception(e))}\n')
    return (int(time.time()), None)

def cmd_stop(buf, start):
  assert(start < len(buf))
  stop = start
  opcode = buf[stop:stop+2]
  stop += 2
  if opcode == b'SI':
    argc = int.from_bytes(buf[stop:stop+4], byteorder='big')
    stop += 4
    for i in range(argc):
      l = int.from_bytes(buf[stop:stop+4], byteorder='big')
      stop += 4+l
  else:
    n = int.from_bytes(buf[stop:stop+4], byteorder='big')
    stop += 4
    for i in range(n):
      stop = cmd_stop(buf, stop)
  return stop
  
def canon_reply(reply, request):
  if request[:2] == b'LS' and reply[:2] == b'OK':
    n = int.from_bytes(reply[2:6], byteorder='big')
    r = []
    start = 2+4
    stop = start
    for i in range(n):
      oldstop = stop
      stop = cmd_stop(reply, oldstop + 8+8+4+1)
      r.append(reply[oldstop:stop])
    r.sort()
    return b''.join([request[:start]] + r)
  else:
    return reply
    
def daemon_replay(exec_absolute_path, recording_path, test_id, use_valgrind=False):
  report = {'nbtests':0, 'failures':0, 'text':f'Jalon {test_id[0]} (démon), test {test_id[1]} :\n'}
  prefix_dir = f'/tmp/{pwd.getpwuid(os.getuid()).pw_name}/erraid-tests'
  archive_path = f'{recording_path}/arborescence.tar.gz'
  os.makedirs(prefix_dir, exist_ok = True)
  with tempfile.TemporaryDirectory(dir = prefix_dir) as tmp_dir:
    with tarfile.open(archive_path) as tar:
      tar.extractall(tmp_dir)
    run_dir = f'{tmp_dir}/run'
    if (time.localtime().tm_sec >= 50):
      time.sleep(61-time.localtime().tm_sec)
    elif (time.localtime().tm_sec < 1):
      time.sleep(1)
    argv = [exec_absolute_path, '-r', run_dir]
    if use_valgrind:
      argv = ['valgrind', '--error-exitcode=33', '--errors-for-leak-kinds=definite',
              '--leak-check=full', '--log-fd=1', '--quiet', '--'] + argv
    start_time = time.time()
    stop_time = start_time + 5
    with subprocess.Popen(argv,
                          cwd = run_dir, start_new_session=True,
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL) as process:
      try:
        dt = 0.01
        request_pipe_path = f'{run_dir}/pipes/erraid-request-pipe'
        reply_pipe_path = f'{run_dir}/pipes/erraid-reply-pipe'
        while not (os.path.exists(request_pipe_path)
                   and os.path.exists(reply_pipe_path)):
          time.sleep(dt)
          dt *= 2
          if (dt > 1):
            testfail(report, 'Le démon n\'a pas créé les fichiers pipes/erraid-request-pipe et pipes/erraid-reply-pipe')
            return report
        if not (stat.S_ISFIFO(os.stat(request_pipe_path).st_mode)
                and stat.S_ISFIFO(os.stat(reply_pipe_path).st_mode)):
          testfail(report, 'Les fichiers pipes/erraid-request-pipe et pipes/erraid-reply-pipe ne sont aps des tubes nommés')
          return report
        with open(f'{recording_path}/request', 'rb') as request_file:
          request = request_file.read()
        with open(f'{recording_path}/reply', 'rb') as reply_file:
          expected_reply = reply_file.read()
        bytes_written = 0
        actual_reply = b''
        reply_pipe = None
        with open(f'{run_dir}/pipes/erraid-request-pipe', 'wb') as request_pipe:
          os.set_blocking(request_pipe.fileno(), False)
          wsel = [ request_pipe ]
          rsel = [ ]
          stop = False
          timed_out = False
          while not stop:
            revents, wevents, xevents = select.select(rsel, wsel, [], stop_time - time.time())
            if revents == [] and wevents == []:
              stop = True
              timed_out = True
            if revents != []:
              r = reply_pipe.read1()
              if r is not None:
                actual_reply += r
                if len(r) == 0:
                  stop = True
            if wevents != []:
              bytes_written += request_pipe.write(request[bytes_written:])
              if bytes_written >= len(request):
                wsel = [ ]
                request_pipe.close()
                if reply_pipe is None: # hopefully the request fits in the buffer
                  reply_pipe = open(f'{run_dir}/pipes/erraid-reply-pipe', 'rb')
                  os.set_blocking(reply_pipe.fileno(), False)
                  rsel = [ reply_pipe ]
        if reply_pipe is not None:
          reply_pipe.close()

        if timed_out:
          testfail(report, 'Le démon n\'a pas fermé le tube de réponse')
        
        if canon_reply(actual_reply, request) != canon_reply(expected_reply, request) :
          testfail(report, f'le démon a répondu :\n{actual_reply}\nau lieu de :\n{expected_reply}')
      except Exception as e:
        nl = '\n'
        testfail(report, f'Exception inattendue :\n{nl.join(traceback.format_exception(e))}')
      
      os.killpg(os.getpgid(process.pid), signal.SIGTERM)
      try:
        process.wait(timeout=1)
      except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(process.pid), signal.SIGKILL)
        process.wait()

      
      testsummary(report, jalon=test_id[0],test=test_id[1])

      return report

def timing_in(timing, timestamp):
  local_time = time.localtime(timestamp)
  return ((timing['minutes'] & (1 << local_time.tm_min)) != 0
          and (timing['hours'] & (1 << local_time.tm_hour)) != 0
          and (timing['days'] & (1 << ((local_time.tm_wday+1) % 7))) != 0)

def generate_expected_run_times(tx_before, timing, start_time, duration_seconds):
  next_minute = start_time + 59 - ((start_time+59) % 60)
  for run in tx_before:
    yield run['time']
  for time in range(next_minute, start_time+duration_seconds+1, 60):
    if timing_in(timing, time):
      yield time

# Assumes each task was completed in the same minute as it was started
def check_task_times(report, run_dir, start_time, duration_seconds, task_id, tx_before, tx):
  timing = read_timing_file(run_dir, task_id)
  for (expected_time, run) in itertools.zip_longest(generate_expected_run_times(tx_before, timing,
                                                                                start_time, duration_seconds),
                                                    tx, fillvalue = None):
    if run is None:
      testfail(report, f'Task {task_id} was supposed to run on '
               + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(expected_time))
               + ' but did not')
    elif expected_time is None:
      testfail(report, f'Task {task_id} ran on '
               + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time']))
               + ' but was not supposed to')
    elif (expected_time // 60) != (run['time'] // 60):
      testfail(report, f'Task {task_id} was supposed to run on '
               + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(expected_time))
               + f' but ran on {time.strftime("%a %Y-%m-%d at %H:%M:%S", time.localtime(run["time"]))} instead')
    elif verbose:
      testmsg(report, f'Task {task_id} ran on '
              + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time'])))

def check_task_exitcodes(report, tasks, expected_code, use_valgrind=False):
  for (task_id, task_data) in tasks.items():
    for run in task_data['times-exitcodes']:
      if run['exit_code'] != expected_code:
        if run['exit_code'] == 0xFFFF:
          testfail(report, f'Task {task_id} was killed by a signal on '
                   + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time'])))
        elif use_valgrind and run['exit_code'] == 33:
          testfail(report, f'Task {task_id} exited with code {run["exit_code"]} on '
                   + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time']))
                   + '\nThis most likely means that `valgrind` detected a memory error in your program.')
        else:
          testfail(report, f'Task {task_id} exited with code {run["exit_code"]} instead of {expected_code} on '
                   + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time'])))
      elif verbose:
        testmsg(report, f'Task {task_id} exited with code {run["exit_code"]} on '
                + time.strftime('%a %Y-%m-%d at %H:%M:%S', time.localtime(run['time'])))
          
## Verifie les sorties des dernieres executions de chaque tache
## (si elle a été exécutée au moins une fois, ce qui n'est pas certain)
          
def check_task_output(report, tasks, expected_output):
  for (task_id, expected_stdout, expected_stderr) in expected_output:
    if task_id not in tasks:
      testfail(report, f'Missing task {task_id}')
    else:
      for last_run in tasks[task_id]['times-exitcodes'][-1:]:
        if tasks[task_id]['stdout'] != expected_stdout:
          testfail(report, f'Task {task_id} had incorrect output on stdout:\n{tasks[task_id]["stdout"].decode("utf-8")}Expected:\n{expected_stdout.decode("utf-8")}')
        elif verbose:
          testmsg(report, f'Task {task_id} had correct output on stdout:\n{tasks[task_id]["stdout"].decode("utf-8")}')
        if tasks[task_id]['stderr'] != expected_stderr:
          testfail(report, f'Task {task_id} had incorrect output on stderr:\n{tasks[task_id]["stderr"].decode("utf-8")}Expected:\n{expected_stderr.decode("utf-8")}')
        elif verbose:
          testmsg(report, f'Task {task_id} had correct output on stderr:\n{tasks[task_id]["stderr"].decode("utf-8")}')
