#include "hw1.h"

int main(int argc, char **argv) {

    FILE* in;
    FILE* out;


    /* Note: create a variable to assign the result of validargs */
    char success = validargs(argc, argv, &in, &out);
    printf("\n %c\n", success);
	if(success==0)
		return EXIT_FAILURE;

	else return EXIT_SUCCESS;
}
