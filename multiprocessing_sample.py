from multiprocessing import Process
import time
import random
import os

def f(name):
	info()
	t=random.randint(1,10)
	time.sleep(t)
	print 'hello', name, t

def info():
    print 'module name:', __name__
    if hasattr(os, 'getppid'):  # only available on Unix
        print 'parent process:', os.getppid(), 'process id:', os.getpid()

if __name__ == '__main__':

	p = list()

	for i in range(100):
		p.append(Process(target=f, args=(i,)))
		p[i].start()
		
	for i in range(100):
		p[i].join()

	print "Fim da execucao"
