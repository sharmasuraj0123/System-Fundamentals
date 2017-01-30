#include "hw1.h"


// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;


if(argc<=1){
		USAGE(0);
		return EXIT_FAILURE;
}
	 //Test for what's in argc and argv.
	//printf("hello %d %s %p",argc ,*argv , argv);


	/* code here */
	if(*argv[1]== 'h'){
		ret = 0x80;
		printf("\n %d",ret);

		USAGE(ret);

		return ret;
	}

	if(*argv[1]=='s'){
		ret = 0x40;
		//int LENGTH_OF_ALPHABET = sizeof(Alphabet)/sizeof(Alphabet[0]);
		//ret+= length;
	}
	if (*argv[2]=='d'){
		ret+= 0x20;
	}

	//For file IO
	*in = fopen(&argv[3], "r");
	*out= fopen(&argv[4], "w");


	printf("\n %d",ret);

	return ret;
	}

