#include "hw2.h"
/* Great filename. */

void processDictionary(FILE* f){

    dict = malloc(sizeof(struct dictionary));
    dict->num_words = 0;
    m_list = NULL;

    do
    {
        //initialize the current word.
        struct dict_word* currWord;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE];
        char* wdPtr = word;
        char line[MAX_SIZE];

        //char word_list[MAX_MISSPELLED_WORDS+1][MAX_SIZE];
        int counter = 0;
        int firstWord = 1;

        fgets(line, MAX_SIZE+1, f);

        //if there isn't a space at the end of the line, put one there
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        char *character = line;
        //printf("%c\n",*character );

        // This part is used to add one single element to dictionary.
        while(*character != 0)
        {
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            //if the character is a space, add the word in word_list and make word NULL.
            if(*character == ' ')
            {
                *wdPtr = 0;
                wdPtr = word;
                if(firstWord==1)
                {
                    addWord(currWord, wdPtr);
                    //printf("%s\n",currWord->word );

                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }
                    addMisspelledWord(&m_list,currMisspelling, currWord, wdPtr);
                }
            }
            else if(*character != '\n'){
                *(wdPtr++) = *character;
            }
            else{
                *character = ' ';
            }
            character++;
        }
    } while(!feof(f));

    //printMispelledList();
     //printDictionary();
    // printf("%s %d\n ", dict->word_list->next->next->word,dict->num_words);
}

void addWord(struct dict_word* dWord, char* word){
    struct dict_word **listForadding = &(dict->word_list);
    //setting up dWord field

    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;

    strcpy(dWord->word, word);

    dWord->next = *listForadding;
    *listForadding = dWord;
    dict->num_words++;
}

void addMisspelledWord(struct misspelled_word **tempList,struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;

    misspelledWord->next = *tempList;
    (correctWord->misspelled)[correctWord->num_misspellings++] = misspelledWord;
    *tempList = misspelledWord;
}

void freeWords(struct dict_word* currWord){
    if(currWord != NULL)
    {
        freeWords(currWord);
       // int i;
        //free word
        printf("FREED %s\n", currWord->word);
        free(currWord);
    }
}

void printWords(struct dict_word* currWord, FILE* f){
    if(currWord != NULL)
    {

        printWords(currWord->next, f);

        char line[MAX_SIZE];
        int i;

        sprintf(line, "%s\n", currWord->word);
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF TIMES WORD IS MISSPELLED: %d\n", currWord->misspelled_count); // puts string into buffer
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF MISSPELLINGS: %d\n", currWord->num_misspellings);
        fwrite(line, strlen(line)+1, 1, f);

        for(i = 0; i<currWord->num_misspellings; i++)
        {
            sprintf(line, "\tMISPELLED WORD #%d: %s\n", i,(currWord->misspelled[i])->word);
            fwrite(line, strlen(line)+1, 1, f);

            sprintf(line,"\t\tMISPELLED?: %d\n", (currWord->misspelled[i])->misspelled);
            fwrite(line, strlen(line)+1, 1, f);

            sprintf(line, "\t\tACTUAL WORD: %s\n", (currWord->misspelled[i])->word);
            fwrite(line, strlen(line)+1, 1, f);

            if((currWord->next)->word != NULL)
            {
                sprintf(line, "\t\tNEXT MISPELLED WORD: %s\n", ((currWord->misspelled)[i])->next->word);
                fwrite(line, strlen(line)+1, 1, f);
            }
        }

        if((currWord->next)->word != NULL)
        {
            sprintf(line,"\tNEXT WORD: %s\n", (currWord->next)->word);
            fwrite(line, strlen(line)+1, 1, f);
        }
    }
}

void processWord(char* inputWord){
    if(foundDictMatch(inputWord)){
        return;
    }
    else if(foundMisspelledMatch(inputWord)){
        return;
    }
    else
    {
        if(add_words){
            struct dict_word* newWord;
            //int counter = 0;
            if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
            {
                printf("ERROR: OUT OF MEMORY.\n");
                return;
            }

            newWord->num_misspellings = 0;
            newWord->misspelled_count = 0;
            addWord(newWord, inputWord);
            word_has_been_added = true;
            char ** mSpelledWordList;
            mSpelledWordList= gentypos(number_of_new_words, inputWord);


            for(int i =0; i<number_of_new_words;i++){
                struct misspelled_word* newMWord;

                if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }
                addMisspelledWord(&m_list,newMWord, newWord, mSpelledWordList[i]);
            }
        }
    }
}

bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}


void printDictWord(struct dict_word word){
    printf("word: %s\n", word.word);
    printf(" num_misspellings %d\n",word.num_misspellings);
    printf("misplled count %d\n",word.misspelled_count );
}

void printDictionary(){
    struct dict_word * cursor = dict->word_list;
    for(int i =0 ; i< dict->num_words;i++){
        printf("%s\n",cursor->word );
        cursor = cursor->next;
    }
}

void printMispelledList(){
    int count =0;
    struct misspelled_word * cursor = m_list;
    while (cursor != NULL){
        printf("%s\n", cursor->word);
        cursor = cursor->next;
        count++;
    }
    printf("%d\n",count);
}

void updateDictionary(FILE ** newFile){

    struct dict_word *counter = dict->word_list;

    for(int i =0; i<dict->num_words;i++){
        char line [MAX_SIZE] ="";
        //char* lnPtr = line;
        strcat(line,counter->word);
        for(int j =0; j<counter->num_misspellings;j++){
            struct misspelled_word* mCounter = counter->misspelled[j];
            strcat(line , " ");
            strcat(line, mCounter->word);

            mCounter= mCounter->next;
        }

        strcat(line , "\n");
        printf("%s\n",line );
        fputs(line, *newFile);
        counter= counter->next;
    }


}
