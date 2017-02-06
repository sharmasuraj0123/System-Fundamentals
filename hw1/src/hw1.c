#include "hw1.h"

int n = 320;
// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	if(argc<=1){
			USAGE(0);
			return 0;
	}
	 //Test for what's in argc and argv.
	if((StringCompare(*(argv+1),"-h")!=0)&&
						StringCompare(*(argv+1),"-s")==0&&
						StringCompare(*(argv+1),"-t")==0){
		USAGE(0);
		return 0;
	}

	if(StringCompare(*(argv+1),"-h")==0){
		ret = 0x80;
		USAGE(ret);
		return 0;
	}

	if((StringCompare(*(argv+2),"-d")==0)&&
						StringCompare(*(argv+2),"-e")==0){
		USAGE(0);
		return 0;
	}

	if(argc<=3){
		USAGE(0);
		return 0;
	}


	//For file IO
	if(StringCompare(*(argv+3),"-")==0){
		int r;
		*in = fopen("rsrc/input.txt", "w");
		while((r=(char)getc(stdin))!= EOF){
			fputc(r,*in);
		}
		fclose(*in);
		*(argv+3)= "rsrc/input.txt";
	}

	*in = fopen(*(argv+3), "r");

	if(StringCompare(*(argv+4),"-")==0){
		*out = stdout;
	}
	else *out= fopen(*(argv+4), "w");

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
			substitutionCipher('d',in, out,shiftAmount,argv);
		}
		else if(StringCompare(*(argv+2),"-e")==0){
			ret+= encode();
			substitutionCipher('e',in,out,shiftAmount, argv);
		}
		else{

		}

	}
	if(StringCompare(*(argv+1),"-t")==0){

		if(StringCompare(*(argv+2),"-d")==0){
			ret+= decode();
			tutneseDecryption(in, out);
		}
		else if(StringCompare(*(argv+2),"-e")==0){
			ret+= encode();
			tutneseEncryption(in ,out);
			ret+=1;
		}
		else{}
	}

	if(*out !=stdin)
		fclose(*out);
	fclose(*in);

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
						FILE **out, int shiftAmount ,char** argv){

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


	char *shiftedArray=Alphabet;

	for(int i =0; i<length(Alphabet);i++)
		*(shiftedArray+i) = *(Alphabet + (shiftAmount)%length(Alphabet));

	info(shiftedArray);
	info2(shiftAmount);
	info3(*(argv+3));
	info4(*(argv+4));
	if(operation=='d')
		info5("decryption");
	else info5("encription");



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

	while(reader != EOF){

		 reader = (char)fgetc(*in);
		 int case_of_reader= 0;

		 if(reader== 0 || reader == 255)
			break;

		 if(isUpperCase(reader)==0){
		 	reader = convertToLowerCase(reader);
		 	case_of_reader =1;
		 }

		 if(reader =='s'){
		 	*buffer =reader;
		 	for(int i =1; i<4;i++)
		 		*(buffer+i) = fgetc(*in);
		 	if(StringCompare(buffer,"squa")!=0){
		 		for(int i =3; i>=1;i--){
		 		 	ungetc(*(buffer+i),*in);
		 		}
			}
			else{
				int case_of_reader_2 =0;
				reader = fgetc(*in);
				if(isUpperCase(reader)==0){
		 			reader = convertToLowerCase(reader);
		 			case_of_reader_2 =1;
		 	}
		 		if (reader!='t'){
		 			if(case_of_reader==1)
		 				fputc(convertToUpperCase(reader),*out);
		 			else fputc(reader,*out);
		 				case_of_reader = case_of_reader_2;
		 				case_of_reader_2= 0;
		 		}
		 		else{
		 			char * encription = presentInTutnese(reader);
		 			for(int i=0; i<10;i++)
		 					*(buffer+i) =0;
		 			*buffer='t';

		 			if(case_of_reader_2==1){
		 				if(case_of_reader==1)
		 				fputc(convertToUpperCase(reader),*out);
		 				else fputc(reader,*out);
		 				case_of_reader = case_of_reader_2;
		 				case_of_reader_2= 0;
		 			}
					else{
		 				for(int i =1; i<length(encription);i++)
		 					*(buffer+i) = fgetc(*in);
		 				if(StringCompare(buffer,encription)!=0){
					 		for(int i =length(encription)-1; i>=1;i--)
					 		 	ungetc(*(buffer+i),*in);
					 		 reader = fgetc(*in);
					 		 if(isUpperCase(reader)==0){
					 			reader = convertToLowerCase(reader);
					 			case_of_reader_2 =1;
					 		}
					 		if(case_of_reader==1)
			 					fputc(convertToUpperCase(reader),*out);
			 				else fputc(reader,*out);
			 				case_of_reader = case_of_reader_2;
			 				case_of_reader_2= 0;
					 	}
					 	else{
					 		if(case_of_reader==1)
		 						fputc(convertToUpperCase(reader),*out);
		 					else fputc(reader,*out);
		 					case_of_reader = case_of_reader_2;
		 					case_of_reader_2= 0;

		 					for(int i =length(encription)-1; i>=1;i--)
					 		 	ungetc(*(buffer+i),*in);
					 		}
		 			}
			}
		 }
		}


		char * encription = presentInTutnese(reader);
		if(StringCompare(encription,"not present")!=0){
			 for (int i =1; i<length(encription);i++){
			 	fgetc(*in);
				}
			}
			if(case_of_reader==1)
				reader = convertToUpperCase(reader);
		 fputc(reader,*out);
	}
}