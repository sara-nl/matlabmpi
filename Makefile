MFILES=library/*.m matlabmain.m f1.m
HELPER=library/helper.o
MEXFILES=library/bcastvar.mexa64 library/receivevar.mexa64 library/sendvar.mexa64

VERBOSITY=-v
TIMINGS=
#VERBOSITY=


all: matlabprog

library/helper.o:
	gcc $(TIMINGS) -fPIC -c -o library/helper.o library/helper.c

library/%.mexa64: library/%.c $(HELPER)
	mex $(TIMINGS) -lmpi $(VERBOSITY) -output $@ $< library/helper.o

libmmpi.so: $(MFILES) $(MEXFILES)
	mcc -B csharedlib:libmmpi $(MFILES) $(MEXFILES) $(VERBOSITY)

matlabprog: libmmpi.so
	LD_RUN_PATH=$(PWD) mbuild -output matlabprog library/matlabmpi.c -L. -I. -lmmpi -lmpi $(VERBOSITY)

clean:
	rm -f matlabprog libmmpi.* mccExcludedFiles.log requiredMCRProducts.txt readme.txt library/*.mexa64 library/helper.o
