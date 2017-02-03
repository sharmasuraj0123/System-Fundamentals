#include "hw1.h"

int n = 320;
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


	if(StringCompare(*(argv+1),"-h")==0){
		ret = 0x80;
		printf("\n %d",ret);
		USAGE(ret);
		return ret;
	}

	//For file IO
	if(StringCompare(*(argv+3),"-")==0){
		*in = stdin;
	}
	else if(StringCompare(*(argv+4),"-")==0){
		*out = stdout;
	}
	else{
	*in = fopen(*(argv+3), "r");
	*out= fopen(*(argv+4), "w");
	}

	//FAILURE CASE FOR READING THE FILE
	if(!(*in)){
		USAGE(0);
		return 0;
	}


	if(StringCompare(*(argv+1),"-s")==0){

		/* code here */
		// FIX!!!!! it is only reading the first value of n.
		if(argc>5){

			n = stringToInt(*(argv+5));

		}

		ret = 0x40;
		char lastDigits = (char) n%length(Alphabet);
		//printf("%d \n",lastDigits);

		ret+= lastDigits;

		int shiftAmount = lastDigits;


		if(StringCompare(*(argv+2),"-d")==0){
			ret+= decode();

			substitutionCipher('d',in,out,shiftAmount);
		}
		else if(StringCompare(*(argv+2),"-e")==0){
			ret+= encode();
			substitutionCipher('e',in,out,shiftAmount);
		}
		else{}



	}
	if(StringCompare(*(argv+1),"-t")==0){

		if(StringCompare(*(argv+2),"-d")==0){
			ret+= decode();
		}
		else if(StringCompare(*(argv+2),"-e")==0){
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
		if(*(string1+i)!= *(string2+i)){
			return 1;
		}
	}
	return 0;
}

int length(char *array){
	int count =0;
	while(*(array+count)!= 0)
		count++;

	return count;
}

int encode(){
	return 0;
}

int decode(){
	return 0x20;
}

int stringToInt(char* string){
	int num =0;
	for(int i =0; i< length(string); i++){
		num *=10;
		char character = *(string +i);
		num += character - 48;
	}


	return num;

}

void substitutionCipher(char operation, FILE **in,
						FILE **out, int shiftAmount){


	if(operation == 'd')
		shiftAmount = -shiftAmount;


	char reader = 'a';
	do{
		reader= (char) fgetc(*in);

		if(reader >'a' && reader < 'z')
			reader-= 32;

		int position_in_array = presentInAlphabet(reader);


		if(position_in_array != -1){

			int new_position = (position_in_array +
								shiftAmount)%length(Alphabet);

			if(new_position<0)
				new_position += length(Alphabet);

			reader  = *(Alphabet + new_position);

		}

		fputc(reader,*out);
	}while(reader !=EOF);



}


int presentInAlphabet(char input){
int sizeOfArray = (int)length(Alphabet);
for(int i=0; i<sizeOfArray;i++){

	if(*(Alphabet+i)==input){
		return i;
	}
}
return -1;
}