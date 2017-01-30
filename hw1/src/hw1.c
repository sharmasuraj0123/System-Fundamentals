#include "hw1.h"


// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	//printf("%d %d",StringCompare(argv[1], argv[2]) , length(argv[1]));


if(argc<=1){
		USAGE(0);
		return EXIT_FAILURE;
}
	 //Test for what's in argc and argv.
	//printf("hello %d %s %p",argc ,*argv , argv);


	/* code here */
	if(StringCompare(argv[1],"-h")==0){
		ret = 0x80;
		printf("\n %d",ret);
		USAGE(ret);
		return ret;
	}

	if(StringCompare(argv[1],"-s")==0){
		ret = 0x40;
		//int LENGTH_OF_ALPHABET = sizeof(Alphabet)/sizeof(Alphabet[0]);
		//ret+= length;
	}
	if (StringCompare(argv[2],"-d")==0){
		ret+= 0x20;
	}

	//For file IO
	//*in = fopen(&argv[3], "r");
	//*out= fopen(&argv[4], "w");


	printf("\n %d",ret);

	return ret;
	}



int StringCompare(char* string1 , char* string2){

	if(length(string1)!=length(string2))
		return 1;

	for(int i = 0 ; i<length(string1); i++){
		if(string1[i]!= string2[i]){
			return 1;
		}
	}
	return 0;
}


int length(char *array){
	int count =0;
	while(array[count]!= 0)
		count++;

	return count;
}