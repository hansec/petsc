
CFLAGS	        =
FFLAGS	        =
CPPFLAGS        =
FPPFLAGS        =
LOCDIR          = src/ts/examples/tests/
EXAMPLESC       = ex2.c ex3.c ex4.c ex5.c ex6.c ex7.c ex8.c ex9.c ex10.c ex25.c
EXAMPLESF       =
EXAMPLESFH      =
MANSEC          = TS

include ${PETSC_DIR}/conf/variables
include ${PETSC_DIR}/conf/rules

ex1: ex1.o  chkopts
	-${CLINKER} -o ex1 ex1.o  ${PETSC_TS_LIB}
	${RM} ex1.o

ex1f: ex1f.o  chkopts
	-${FLINKER} -o ex1f ex1f.o  ${PETSC_TS_LIB}
	${RM} ex1f.o

ex2: ex2.o  chkopts
	-${CLINKER} -o ex2 ex2.o ${PETSC_TS_LIB}
	${RM} ex2.o

ex3: ex3.o  chkopts
	-${CLINKER} -o ex3 ex3.o ${PETSC_TS_LIB}
	${RM} ex3.o

ex4: ex4.o  chkopts
	-${CLINKER} -o ex4 ex4.o ${PETSC_TS_LIB}
	${RM} ex4.o

ex5: ex5.o  chkopts
	-${CLINKER} -o ex5 ex5.o ${PETSC_TS_LIB}
	${RM} ex5.o

ex6: ex6.o  chkopts
	-${CLINKER} -o ex6 ex6.o ${PETSC_TS_LIB}
	${RM} ex6.o

ex7: ex7.o  chkopts
	-${CLINKER} -o ex7 ex7.o ${PETSC_TS_LIB}
	${RM} ex7.o

ex8: ex8.o  chkopts
	-${CLINKER} -o ex8 ex8.o ${PETSC_TS_LIB}
	${RM} ex8.o

ex9: ex9.o  chkopts
	-${CLINKER} -o ex9 ex9.o ${PETSC_TS_LIB}
	${RM} ex9.o

ex10: ex10.o  chkopts
	-${CLINKER} -o ex10 ex10.o ${PETSC_TS_LIB}
	${RM} ex10.o

ex25: ex25.o  chkopts
	-${CLINKER} -o ex25 ex25.o ${PETSC_TS_LIB}
	${RM} ex25.o

#----------------------------------------------------------------------------------
NPROCS    = 1  3

runex2_euler:
	-@${MPIEXEC} -n 1 ./ex2 -ts_type euler > ex2_euler.tmp 2>&1;   \
           ${DIFF} output/ex2_euler.out ex2_euler.tmp || echo  ${PWD} "\nPossible problem with ex2_euler, diffs above \n========================================="; \
           ${RM} -f ex2_euler.tmp

runex2_beuler:
	-@${MPIEXEC} -n 1 ./ex2 -ts_type beuler > ex2_beuler.tmp 2>&1;   \
           ${DIFF} output/ex2_beuler.out ex2_beuler.tmp || echo  ${PWD} "\nPossible problem with ex2_beuler, diffs above \n========================================="; \
           ${RM} -f ex2_beuler.tmp

runex2_sundials:
	-@${MPIEXEC} -n 1 ./ex2 -ts_type sundials -ts_sundials_monitor_steps false > ex2_sundials.tmp 2>&1;   \
           ${DIFF} output/ex2_sundials.out ex2_sundials.tmp || echo  ${PWD} "\nPossible problem with ex2_sundials, diffs above \n========================================="; \
           ${RM} -f ex2_sundials.tmp

runex3:
	-@${MPIEXEC} -n 1 ./ex3 > ex3.tmp 2>&1;	  \
	   ${DIFF} output/ex3.out ex3.tmp || echo  ${PWD} "\nPossible problem with ex3, diffs above \n========================================="; \
	   ${RM} -f ex3.tmp

runex3_2:
	-@${MPIEXEC} -n 1 ./ex3 -useAlhs > ex3.tmp 2>&1;	  \
	   ${DIFF} output/ex3.out ex3.tmp || echo  ${PWD} "\nPossible problem with ex3_2, diffs above \n========================================="; \
	   ${RM} -f ex3.tmp

runex4:
	-@${MPIEXEC} -n 1 ./ex4 -ts_fd -ts_type beuler> ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp
runex4_2:
	-@${MPIEXEC} -n 2 ./ex4 -ts_fd -ts_type beuler > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_2, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp
runex4_3:
	-@${MPIEXEC} -n 1 ./ex4 -ts_fd -ts_type cn > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4_3.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_3, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp
runex4_4:
	-@${MPIEXEC} -n 2 ./ex4 -ts_fd -ts_type cn > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4_3.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_4, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp
runex4_5:
	-@${MPIEXEC} -n 1 ./ex4 -ts_type beuler -ts_fd -fd_color -mat_coloring_type sl > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_5, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp
runex4_6:
	-@${MPIEXEC} -n 2 ./ex4 -ts_type beuler -ts_fd -fd_color -mat_coloring_type sl > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_6, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp

runex4_7:
	-@${MPIEXEC} -n 1 ./ex4 -ts_fd -ts_type beuler -test_PostStep > ex4.tmp 2>&1;	  \
	   ${DIFF} output/ex4_7.out ex4.tmp || echo  ${PWD} "\nPossible problem with ex4_7, diffs above \n========================================="; \
	   ${RM} -f ex4.tmp

runex5:
	-@${MPIEXEC} -n 1 ./ex5 -ts_max_steps 130 > ex5.tmp 2>&1;	  \
	   ${DIFF} output/ex5.out ex5.tmp || echo  ${PWD} "\nPossible problem with ex5_1, diffs above \n========================================="; \
	   ${RM} -f ex5.tmp

runex5_2:
	-@${MPIEXEC} -n 4 ./ex5 -ts_max_steps 130 > ex5.tmp 2>&1;	  \
	   ${DIFF} output/ex5.out ex5.tmp || echo  ${PWD} "\nPossible problem with ex5_2, diffs above \n========================================="; \
	   ${RM} -f ex5.tmp

runex25:
	-@${MPIEXEC} -n 1 ./ex25 -ts_exact_final_time INTERPOLATE > ex25_1.tmp 2>&1;	  \
	   ${DIFF} output/ex25_1.out ex25_1.tmp || printf "${PWD}\nPossible problem with ex25_1, diffs above \n=========================================\n"; \
	   ${RM} -f ex25_1.tmp

runex25_2:
	-@${MPIEXEC} -n 2 ./ex25 -ts_exact_final_time INTERPOLATE > ex25_2.tmp 2>&1;	  \
	   ${DIFF} output/ex25_2.out ex25_2.tmp || printf "${PWD}\nPossible problem with ex25_2, diffs above \n=========================================\n"; \
	   ${RM} -f ex25_2.tmp

TESTEXAMPLES_C		  = ex4.PETSc runex4 runex4_2 runex4_3 runex4_4 runex4_5 runex4_6 \
                            runex4_7 ex4.rm ex25.PETSc runex25 runex25_2 ex25.rm

testexamples_C_NoComplex  = ex3.PETSc runex3 runex3_2 ex3.rm ex5.PETSc runex5 runex5_2 ex5.rm
TESTEXAMPLES_C_X	  =
TESTEXAMPLES_FORTRAN	  =
TESTEXAMPLES_C_X_MPIUNI =
TESTEXAMPLES_13		  = ex2.PETSc ex2.rm ex4.PETSc ex4.rm

include ${PETSC_DIR}/conf/test
