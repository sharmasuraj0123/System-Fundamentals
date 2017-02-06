#ifndef INFO_H
#define INFO_H
#include <stdlib.h>
#include <stdio.h>

#ifdef INFO
	#define info(alphabet) fprintf(stderr,"INFO : shifted alphabet: %s\n",alphabet)
	#define info2(shiftAmount) fprintf(stderr,"INFO: shift amount: %d\n",shiftAmount)
	#define info3(input) fprintf(stderr,"INFO: intput file: %s\n",input)
	#define info4(output) fprintf(stderr,"INFO: output file: %s\n",output)
	#define info5(operation) fprintf(stderr,"INFO: operation: %s\n",operation)
#else
	#define info(alphabet)
	#define info2(shiftAmount)
	#define info3(input)
	#define info4(output)
	#define info5(operation)
#endif
#endif
