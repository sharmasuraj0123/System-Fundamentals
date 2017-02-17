#include "hw2.h"

int main(int argc, char *argv[]){
    add_words = false;
    word_has_been_added = false;
    DEFAULT_INPUT = fopen("rsrc/sample.txt" ,"r");
    //DEFAULT_INPUT = stdin;
    DEFAULT_OUTPUT = stdout;
    DEFAULT_DICT ="rsrc/dictionary.txt";
    //create dictionary
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }

    if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word*))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
    m_list = NULL;
    number_of_new_words=0;
    struct Args args;
    // Set struct default values
    args.d = false;
    args.i = false;
    args.o = false;

    strcpy(args.dictFile, DEFAULT_DICT);

    // Make a loop index�
    int i;
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;

    char opt = '\0';
    for(i = 1; i< argc; i++)
    {
        char* currArg = argv[i];
        //there's a flag
        if(opt != '\0')
        {
            if(opt == 'd')
            {
                strcpy(args.dictFile, currArg);
                args.d = true;
            }
            if(opt == 'i')
            {
                strcpy(args.input, currArg);
                args.i = true;
                iFile = fopen(currArg, "r");
            }
            if(opt == 'o')
            {
                strcpy(args.output, currArg);
                args.o = true;
                oFile = fopen(currArg, "w");
            }
            opt = '\0';
        }
        else
        {
            if(strcmp(currArg, "-d") == 0)
                opt = 'd';
            else if(strcmp(currArg, "-i") == 0)
                opt = 'i';
            else if(strcmp(currArg, "-o") == 0)
                opt = 'o';
            else if(strcmp(currArg,"-h")==0){
                //USAGE(EXIT_SUCCESS);
            }
            else if(*(currArg +1)=='A' && *currArg== '-'){
                add_words=true;
                number_of_new_words = (int)(currArg[2]-48);
                if(number_of_new_words >5 ||number_of_new_words<0)
                    return EXIT_FAILURE;
            }

         }
    }
    dFile = fopen(args.dictFile, "r");

    if(iFile == NULL && args.i == true)
    {
        printf("Unable to open: %s.\n", args.input);
        return EXIT_FAILURE;
    }
    if(dFile == NULL)
    {
        printf("Unable to open: %s.\n", args.dictFile);
    }
    else
    {
        processDictionary(dFile);
    }

    strcpy(line,"\n--------INPUT FILE WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);


    while(!feof(iFile))
    {

        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];

        fgets(line, MAX_SIZE+1, iFile);

        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        //replaces spaces within a line with new lines
        char* character = line;


        while(*character != 0)
        {
            if(*character == ' ' || *character == '\n')
            {
                /*char* punct = wdPtr-1;
                    printf("char:%c",punct);
                while(!((*punct>='a' && *punct<='z') || (*punct>='A' && *punct<='Z')))
                {
                    punct--;
                }
                punct++;
                printf("%d", strlen(wdPtr)-strlen(punct));
                */

                *wdPtr = 0;
                wdPtr = word;
                processWord(wdPtr);

                if(*character=='\n')
                    strcat(wdPtr, "\n");
                else   strcat(wdPtr, " ");



                fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);
                //printf("%s\n",wdPtr);
            }
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }

        if(iFile == stdin)
            break;
    }

    if(add_words && word_has_been_added){
        //printf("%s\n","lol");
        // char * new_dict = NULL;
        // strcpy(new_dict , args.dictFile);

        // char* counter = new_dict;

        // while(*(counter+1) != '\0')
        //     counter++;

        // while(counter!= new_dict){
        //     if(*counter=='/'){
        //         strcat("new_",counter+1);
        //         break;
        //     }
        //     counter--;
        // }
        // if(*counter==*new_dict)
        //     strcat("new_",new_dict);

        FILE * new_dFile = fopen("rsrc/new_dict","w");

         updateDictionary(&new_dFile);


    }



    strcpy(line, "\n--------DICTIONARY WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);


    //printWords(dict->word_list , oFile);
    strcpy(line,"\n--------FREED WORDS--------\n");

    fwrite(line, strlen(line)+1, 1, oFile);
    //printMispelledList();

    //printDictionary();
    //freeWords(dict->word_list);

    //free dictionary
    free(dict);
    //free m_list
    free(m_list);

    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
        }

