#include "hw1.h"

int n = 320;
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

	//For file IO
	if(StringCompare(argv[3],"-")==0){
		*in = stdin;
	}
	else if(StringCompare(argv[4],"-")==0){
		*out = stdout;
	}
	else{
	*in = fopen(argv[3], "r");
	*out= fopen(argv[4], "w");
	}

	//FAILURE CASE FOR READING THE FILE
	if(!(*in))
		return 0;



	if(StringCompare(argv[1],"-h")==0){
		ret = 0x80;
		printf("\n %d",ret);
		USAGE(ret);
		return ret;
	}


	if(StringCompare(argv[1],"-s")==0){

		/* code here */
		// FIX!!!!! it is only reading the first value of n.
		if(argc>5){
			printf("%d %c ", *argv[3] , *argv[3]);
			n = *argv[3];
			printf("%d ",n);
		}

		ret = 0x40;
		char lastDigits = n%length(Alphabet);
		ret+= lastDigits;

		if(StringCompare(argv[2],"-d")==0){
			ret+= decode();
		}
		else if(StringCompare(argv[2],"-e")==0){
			ret+= encode();
		}
		else{}


	}
	if(StringCompare(argv[1],"-t")==0){

		if(StringCompare(argv[2],"-d")==0){
			ret+= decode();
		}
		else if(StringCompare(argv[2],"-e")==0){
			ret+= encode();
		}
		else{}
	}




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

int encode(){
	return 0;
}

int decode(){
	return 0x20;
}

void substitutionCipher(char operation, FILE **in,
						FILE **out, int shiftAmount){

}


int presentInAlphabet(char input){
int sizeOfArray = (int)length(Alphabet);
for(int i=0; i<sizeOfArray;i++)
	if(Alphabet[i]==input)
		return i;
return 0;
}