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

		if(argc>5)
			n = stringToInt(*(argv+5));

		ret = 0x40;
		char lastDigits = (char) n%length(Alphabet);
		//printf("%d \n",lastDigits);
		ret+= lastDigits;
		int shiftAmount = lastDigits;

		if(StringCompare(*(argv+2),"-d")==0){
			ret+= decode();
			substitutionCipher('d',in, out,shiftAmount);
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
			tutneseDecryption(in, out);
		}
		else if(StringCompare(*(argv+2),"-e")==0){
			ret+= encode();
			tutneseEncryption(in ,out);
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

int presentInAlphabet(char input){
	int sizeOfArray = (int)length(Alphabet);
	for(int i=0; i<sizeOfArray;i++)
		if(*(Alphabet+i)==input)
			return i;

	return -1;
}


void substitutionCipher(char operation, FILE **in,
						FILE **out, int shiftAmount){

	if(operation == 'd')
		shiftAmount = -shiftAmount;
	char reader = 'a';
	do{
		reader= (char) fgetc(*in);

		if(reader >='a' && reader <= 'z')
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

int length_of_2D_array(char** array){
	int count =0;
	while(*(array+ count)!= NULL)
		count++;
	return count;
}

int isVowel(char a){
	if (a=='a'||a== 'e'|| a== 'i'|| a== 'o'|| a== 'u')
		return 0;
	 return 1;
}

char* presentInTutnese(char input){

	char *first_letter_in_array = *Tutnese;

	for(int i =0; i<length_of_2D_array(Tutnese);i++){

		if(*first_letter_in_array >= 'A' &&
								*first_letter_in_array<='Z')
			*first_letter_in_array = convertToLowerCase(*first_letter_in_array);

		if(*first_letter_in_array==input)
			return first_letter_in_array;

	while(*first_letter_in_array!='\0')
		first_letter_in_array++;

	if(first_letter_in_array!=NULL)
		first_letter_in_array++;
}

	return "not present";
}

int isLowerCase(char a){
	if(a >='a' && a<='z')
		return 0;
	return 1;
}
int  isUpperCase(char a){
	if(a >='A' && a<='Z')
		return 0;
	return 1;
}


char convertToLowerCase(char A){
	A +=32;
	return A;
}

char convertToUpperCase(char a){
	a -=32;
	return a;
}

void tutneseEncryption(FILE **in, FILE **out){
	char reader = 'a';

	do{
		// lower case then 0 ; uppercase then 1
		int case_of_reader = 0;
		int case_of_reader_2 =0;

		reader= (char) fgetc(*in);
		char reader_2 = (char)fgetc(*in);

		if(reader_2>= 'A' && reader_2<= 'Z'){
			case_of_reader_2 =1;
			reader_2 = convertToLowerCase(reader_2);
		}

		if(reader>= 'A' && reader<= 'Z'){
			case_of_reader =1;
			reader = convertToLowerCase(reader);
		}

		if(reader != reader_2){
			if(case_of_reader_2==1)
				reader_2 = convertToUpperCase(reader_2);

			ungetc(reader_2,*in);
			reader_2 =0;
			case_of_reader_2 =0;
		}

		char * encription = presentInTutnese(reader);

			if(reader==reader_2){
				if(case_of_reader==1){
					fputs("Squa", *out);
				}
				else fputs("squa",*out);

				if(isVowel(reader)==0)
					fputc('t', *out);

				case_of_reader= case_of_reader_2;
				case_of_reader_2=0;
				reader_2 =0;
				}

			if(StringCompare(encription,"not present")!=0&&
										isVowel(reader)==1){
			 if(case_of_reader==1){
			 		//printf("%c\n",*encription );
					fputc(convertToUpperCase(*encription), *out);
					fputs(encription+1,*out);
			 }
			 else fputs(encription,*out);
		}

		if(StringCompare(encription,"not present")==0 ||
									isVowel(reader)==0){
				if(case_of_reader==1){
					 reader = convertToUpperCase(reader);
				}
				fputc(reader,*out);
		}

	}while(reader != EOF);
}

void tutneseDecryption(FILE**in , FILE ** out){
	char reader ='a';

	do{

		 reader = (char)fgetc(*in);
		 int case_of_reader= 0;

		 if(isUpperCase(reader)==0){
		 	reader = convertToLowerCase(reader);
		 	case_of_reader =1;
		 }
		 printf("%c\n",reader );
		char * encription = presentInTutnese(reader);

		if(StringCompare(encription,"not present")!=0){

			 for (int i =1; i<length(encription);i++){
			 	printf("%s\n","lol" );
			 	fgetc(*in);
				}
			}
			if(case_of_reader==1)
				reader = convertToUpperCase(reader);

		 fputc(reader,*out);
	}while(reader != EOF);

}