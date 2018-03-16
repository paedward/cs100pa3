#!/usr/bin/env python3
'''
Grading script for PA3 (Graphs and Networks)
Balasubramaniam Srinivasan 2018
'''
import argparse,signal
from subprocess import check_output,CalledProcessError,DEVNULL,TimeoutExpired, PIPE, Popen
from os.path import isfile
from random import sample,randint
from time import time
import sys

# global constants
LINK_HEADER = "Actors"
KCORE_HEADER = "Actors"
TIME_RANGE = 2 # how many times longer student soln can take than ref soln
MIN_TIME_LINK =  30 # minimum amount of time to allow no matter how fast reference goes
MIN_TIME_CORE = 20
LARGE_N = 20 # large number of queries
OUTPUT_LP1 = "+4 for correct output"
OUTPUT_LP2 = "+5 for correct output"
OUTPUT_LF = "+0 for incorrect output"
OUTPUT_KC = "+9 for correct output"
OUTPUT_KP = "+4 for partially correct output"
OUTPUT_KF = "+0 for incorrect output"
MEMLEAK_P = "+4 for no memory leaks"
MEMLEAK_F = "+0 for memory leaks"
MAX_RETRIES = 2

# definitions for killing function after certain amount of time
class TimeoutException(Exception):
	pass
def timeout_handler(signum, frame):
	raise TimeoutException
signal.signal(signal.SIGALRM, timeout_handler)


def check_prediction(ref_prediction,student_prediction):
	ref_values = ref_prediction.split("\t")
	ref_values = [val for val in ref_values if val is not '']
	ref_values = [val for val in ref_values if val is not 'NULL']
	student_values = student_prediction.split("\t")
	student_values =  [val for val in student_values if val is not '']
	student_values =  [val for val in student_values if val is not 'NULL']
	if len(ref_values)!=len(student_values) :
		return False
	for i in range(len(ref_values)):
		if ref_values[i]!=student_values[i]:
			return False
	return True
	

def checkInvitees(ref_list,student_list):
	ref_list = [val for val in ref_list if val is not '']
	total_ref = len(ref_list)
	student_list = [val for val in student_list if val is not '']
	ref = set(ref_list)
	student = set(student_list)
	mistakes = len(list(ref.symmetric_difference(student))) 
	if ref == student :
		return 2
	if mistakes < int(total_ref/2.0) :
		return 1
	else :
		return 0
	
	

def runCommand(command, timeout):
		proc = None
		e = None
		try:
				proc = Popen(command.split(), stdout = PIPE, stderr = PIPE)
				stdout, stderr = proc.communicate(timeout = timeout)
				return (None, proc.returncode, stdout, stderr)
		#catch all exceptions, including keyboard interrupt
		except TimeoutExpired as timeoutError:
				return (timeoutError, None, None, None)
		except:
				#I'm not sure this is necessary, but here's an attempt to handle keyboard interrupts and still kill the subprocess
				#Need to catch all exceptions so that we can catch keyboard interrupts.
				#If we don't catch a keyboard interrupt, then not even the finally block seems to run before the program dies
				#Its possible that another keyboard interrupr is sent after we enter this block before the process is killed.
				#I can't figure out any sane way to handle that
				e = sys.exc_info()[1] #stroe the error for future use
		finally:
				if proc is not None:
						proc.kill()
				if e is not None:
						#e wasn't an error we're expecting, so we shouldn't really have caught it,
						#we just needed to kill the supbrocess. Now that that's done, reraise it
						raise e



def runCommandForTest(command, timeout, possiblePoints, retryNum = 1):
		print("Command: %s" % command)

		start = time()
		timeoutError, status, stdout, stderr = runCommand(command, timeout)
		if timeoutError:
				if retryNum < MAX_RETRIES:
						print("Your code timed out. Reattempting")
						return runCommandForTest(command, timeout, possiblePoints, retryNum+1)
				else:
						print("Test failed. In each of the %i tries, your program took longer than the %i seconds allocated for this test." % (MAX_RETRIES, timeout))
						print("0/%i points awarded for this test\n" % possiblePoints)
						return None

		end = time()
		print("Elapsed time %.10f" % (end - start))
		stdout = stdout.decode('latin-1')
		stderr = stderr.decode('latin-1')

		if status != 0:
				print("Your program crashed. 0/%i points awarded for this test" % possiblePoints)
				print("Program return value (negative values correspond to unix signals):", status)
				print("Stdout from your program command:", stdout)
				print("Stderr from your program command:", stderr)
				print("")
				return None

		return stdout



#Don't check for correct outputs, just check for memory leak
#Due to point scoring system, memLeakTest must be last
def memLeakTest(timeout,command):
		print("-----Running memory leak test-----")

		start = time()

		#timeout * 15 for reps, * 2 for valgrind slowdown
		output = runCommandForTest(command, timeout *5* 3 * 2, 4)
		check_output("rm -f valgrindout".split())
		if output is None:
				return False
	
		if "no leaks are possible" in output or ("definitely lost: 0 bytes" in output and
												"indirectly lost: 0 bytes" in output):
				print("No memory leaks detected. Memory leak test succeeded.")
				#print("4/4 points awarded for the memory leak test\n")
				return True
		else:
				print("Memory leaks found. Memory leak test failed.")
				#print("0/4 points awarded for the memory leak test\n")
				return False


# grade PA3
def grade(movie_casts,reflinkpredictor,refawardsceremony): # parameters are full paths to movie_casts.tsv file and reflinkpredictor and refawardsceremony executables
	score = 0 # total possible = 20
	pos = 0
	skip_linkpredictor = False
	skip_awardsceremony = False
	actors = set()
	for a,t,y in [l.strip().split('\t') for l in open(movie_casts,encoding='latin-1').read().splitlines()][1:]: # ignore header line
		actors.add(a)
	# check if code can compile
	print("Compiling linkpredictor") #print("Checking if code compiles:")
	#pos += 1
	try:
		check_output("make clean".split())
		check_output("rm -f linkpredictor".split())
		check_output("make linkpredictor".split())
		if not isfile("linkpredictor"):
			print("Failed to compile linkpredictor using 'make' command. 0 points")
			skip_linkpredictor = True
			pos+=10
		#print("+1 for code compiling"); score += 1
	except CalledProcessError:
		print("Failed to compile using 'make' command. 0 points")
		skip_linkpredictor = True
		pos+=10
	print()
	
	if skip_linkpredictor == False:
			print("Running Link Predictor:")
			for _ in range(1):
				pos += 9
				create_actors = [sample(actors,1) for __ in range(LARGE_N)]
				input_actors = []
				for i in create_actors :
					input_actors.append(i[0])
				f = open('refInput','w',encoding='latin-1')
				f.write('\n'.join([LINK_HEADER] + input_actors))
				f.close()
				T0 = time()
				check_output([reflinkpredictor,movie_casts,'refInput','refExisting','refNew'], stderr=DEVNULL)
				TIME = max(MIN_TIME_LINK,TIME_RANGE*int(time()-T0))
				oe_ref = [l.strip() for l in open('refExisting',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
				on_ref = [l.strip() for l in open('refNew',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
				check_output('rm -f refExisting refNew'.split(), stderr=DEVNULL)
				try:
					signal.alarm(TIME)
					check_output(['./linkpredictor',movie_casts,'refInput','studentExisting','studentNew'], stderr=DEVNULL)
					signal.alarm(0)
					oe_sol = [l.strip() for l in open('studentExisting',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
					on_sol = [l.strip() for l in open('studentNew',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
					failedExisting = False
					failedNew = False
					print("\nChecking Existing Connections\n")
					if len(oe_ref) == len(oe_sol):
						for i in range(len(oe_ref)):
							if check_prediction(oe_ref[i],oe_sol[i]) == False:
								failedExisting=True
					else:
						failedExisting = True
					if not failedExisting:
						print(OUTPUT_LP1); score += 5
					else:
						print(OUTPUT_LF)

					print("\nChecking New Connections\n")
					if len(on_ref) == len(on_sol):
						for i in range(len(on_ref)):
							if check_prediction(on_ref[i],on_sol[i]) == False:
								failedNew=True
					else:
						failedNew = True
					if not failedNew:
						print(OUTPUT_LP2); score += 4
					else:
						print(OUTPUT_LF)

				except CalledProcessError:
					print("Your program crashed when running the linkpredictor. 0 points")
				except TimeoutException:
					print("Your program took longer than %d seconds when running the linkpredictor. 0 points" % TIME)
				check_output("rm -f refInput refExisting refNew studentExisting studentNew".split())
			print()
	
			command = "valgrind --log-fd=1 --leak-check=yes ./linkpredictor /home/linux/ieng6/cs100w/public/pa3/tsv/movie_casts.tsv /home/linux/ieng6/cs100w/public/pa3/sampleInputs/sampleInputPart2 valgrindout valgringout"

			if memLeakTest(TIME,command):
				score += 1
			pos+=1	

	# check if code can compile
	print("\n\nCompiling Awards Ceremony") #print("Checking if code compiles:")
	#pos += 1
	try:
			check_output("make clean".split())
			check_output("rm -f awardsceremony".split())
			check_output("make awardsceremony".split())
			if not isfile("awardsceremony"):
					print("Failed to compile awardsceremony using 'make' command. 0 points")
					skip_awardsceremony = True
					pos+=10
			#print("+1 for code compiling"); score += 1
	except CalledProcessError:
			print("Failed to compile using 'make' command. 0 points")
			skip_awardsceremony = True
			pos+=10
	print()

	if skip_awardsceremony == False:
					print("Running Awards Ceremony:")
					for _ in range(1):
							pos += 9
							T0 = time()
							kvalue = str(randint(200,300))
							check_output([refawardsceremony,movie_casts,kvalue,'refOutput'], stderr=DEVNULL)
							TIME = max(MIN_TIME_LINK,TIME_RANGE*int(time()-T0))
							o_ref = [l.strip() for l in open('refOutput',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
							check_output('rm -f refOutput'.split(), stderr=DEVNULL)
							try:
									signal.alarm(TIME)
									check_output(['./awardsceremony',movie_casts,kvalue,'studentOutput'], stderr=DEVNULL)
									signal.alarm(0)
									o_sol = [l.strip() for l in open('studentOutput',encoding='latin-1').read().strip().splitlines()][1:] # ignore header line
									failed = False
									complete = True
									print("\nChecking Invitees\n")
									if len(o_ref) <= 2*len(o_sol):
											checker = checkInvitees(o_ref,o_sol)
											if checker == 1:
												complete = False
											elif checker == 0 :
												complete = False
												failed = False	
									else:
											failed = True

									if not failed:
											if complete :
													print(OUTPUT_KC); score += 9
											else :
												print(OUTPUT_KP); score += 4
									else:
											print(OUTPUT_KF)

							except CalledProcessError:
									print("Your program crashed when running the awardsceremony. 0 points")
							except TimeoutException:
									print("Your program took longer than %d seconds when running the awardsceremony. 0 points" % TIME)
							check_output("rm -f refInput refExisting refNew studentExisting studentNew".split())
					print()

					command = "valgrind --log-fd=1 --leak-check=yes ./awardsceremony /home/linux/ieng6/cs100w/public/pa3/tsv/movie_casts.tsv 100 valgringout"

					if memLeakTest(TIME,command):
							score += 1
					pos+=1


	# clean up at the end
	print("Total Score: %d/%d" % (score,pos))
	check_output("rm -f refInput refExisting refNew studentExisting studentNew refOutput studentOutput".split())
	check_output("make clean".split())



# main function: call grader
if __name__ == "__main__":
	parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument('-mc', '--movie_casts', required=False, type=str, default="/home/linux/ieng6/cs100w/public/pa3/tsv/movie_casts.tsv", help="Full path to movie_casts.tsv file")
	parser.add_argument('-rp', '--reflinkpredictor', required=False, type=str, default="/home/linux/ieng6/cs100w/public/pa3/reference_binaries/reflinkpredictor", help="Full path to reflinkpredictor")
	parser.add_argument('-ra', '--refawardsceremony', required=False, type=str, default="/home/linux/ieng6/cs100w/public/pa3/reference_binaries/refawardsceremony", help="Full path to refawardsceremony")
	args = parser.parse_args()
	assert isfile(args.movie_casts), "movie_casts.tsv file not found: %s" % args.movie_casts
	assert isfile(args.reflinkpredictor), "reflinkpredictor executable not found: %s" % args.reflinkpredictor
	assert isfile(args.refawardsceremony), "refawardsceremony executable not found: %s" % args.refawardsceremony
	grade(args.movie_casts,args.reflinkpredictor,args.refawardsceremony)

