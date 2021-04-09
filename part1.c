#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

    char* mode = "r";
    FILE* fp;

    int cardFlip = 0;
    int limitReset = 0;

    char filename[100];
    if(argc == 1){
        //fp = stdin;
        scanf("%s", filename);
        fp = fopen(filename, mode);
    }
    else if(argc == 2){
        fp = fopen(argv[1], mode);
    }

    if(fp == NULL){
        fprintf(stderr, "File doesn't exist!\n");
        exit(1);
    }

    char input[100];

    //------SCANNING FROM FILE------//
    //Boolean value to mark if the scan is currently on a comment or not
    //int comment = 0;
    //Boolean value to mark if the scan is currently on a line or not
    int line = 0;
    //Boolean value to mark if the scan is at the end of a line in the tableau
    int lineEnd = 0;
    //Number of columns remaining to be scanned in the tableau
    int numCols = 7;
    //Amount of strings representing cards that have been scanned
    int amtCards = 0;
    //Boolean value to mark if the scan is in the stock
    int isStock = 0;
    //Number of duplicate cards
    int numDupes = 0;
    //Array of characters representing all of the cards that will be scanned in
    char allCards[999];
    //Array of characters that will contain any duplicate cards
    char duplicates[999];
    //Initializing said arrays
    for(int i = 0; i < 999; i++){
        allCards[i] = '0';
        duplicates[i] = '0';
    }

    //Checks if the searched-for string is found
    int stringCheck(char string[100], char compTo[100], int size){
        for(int i = 0; i < size; i++){
            if(string[i] != compTo[i]){
                return 0;
            }
        }
        if(string[100] != '\n' && string[100] != ' ' && string[100] != '\r' && string[100] != '\t' && string[100] != '#'){
            return 0;
        }
        return 1;
    }
    //Checks if the searched-for string representing a card is found, one for a specific suit and one not
    int stringSuitCheck(char string[100], char suit){
        char cards[14] = {"_A23456789TJQK"};
        //Used as a true/false check to return
        int count = 0;
        for(int i = 0; i < 14; i++){
            if(string[0] == cards[i]){
                count = 1;
            }
        }
        if(string[1] != suit){
            count = 0;
        }
        if(string[100] != '\n' && string[100] != ' ' && string[100] != '\r' && string[100] != '\t' && string[100] != '#'){
            count = 0;
        }
        return count;
    }
    int stringCardCheck(char string[100]){
        char cards[14] = {"_A23456789TJQK"};
        int noSuit = 0;
        //Used as a true/false check to return
        int count = 0;
        if(string[0] == '|'){
            count = 1;
            noSuit = 1;
        }
        for(int i = 0; i < 14; i++){
            if(string[0] == cards[i]){
                count = 1;
            }
        }
        if(string[1] != 'c' && string[1] != 'd' && string[1] != 'h' && string[1] != 's' && noSuit == 0){
            count = 0;
        }
        if(string[100] != '\n' && string[100] != ' ' && string[100] != '\r' && string[100] != '\t' && string[100] != '#'){
            count = 0;
        }
        if(stringCheck(string, "MOVES:", 6) && isStock == 1){
            count = 1;
        }
        return count;
    }
    //Skips comments and throws an error for uncommented text
    void skipGarbage(FILE* fileptr, char string[100], char compTo[100], int size, char errMsg[100]){
        fscanf(fileptr, "%s", string);
        string[100] = fgetc(fileptr);
        if(string[0] == '#' && (string[100] != '\n' && string[100] != '\r')){
            char c = fgetc(fileptr);
            while(c != '\n' && c != '\r'){
                c = fgetc(fileptr);
            }
        }
        //Checks for end of line
        else if(string[100] == ' '){
            char c[100];
            int i = 0;
            int check = 1;
            while(check == 1){
                c[i] = fgetc(fileptr);
                if(c[i] == '\n' || c[i] == '\r'){
                    check = 0;
                }
                i++;
                if(c[i-1] != ' ' && c[i-1] != '\n' && c[i-1] != '\t' && c[i-1] != '\r'){
                    check = 0;
                    while(i > 0){
                        i--;
                        ungetc(c[i], fp);
                    }
                }
            }
        }
        else if(string[0] != '#' && !(stringCheck(string, compTo, size))){
            fprintf(stderr, errMsg);
            exit(1);
        }

    }
    //skipGarbage, but card version
    void skipGarbCards(FILE* fileptr, char string[100], char suit, char errMsg[100]){
        fscanf(fileptr, "%s", string);
        string[100] = fgetc(fileptr);
        //Checks for end of line
        if(string[0] == '#' && (string[100] != '\n' && string[100] != '\r')){
            char c = fgetc(fileptr);
            while(c != '\n' && c != '\r'){
                c = fgetc(fileptr);
            }
        }
        else if(string[100] == ' '){
            char c[100];
            int i = 0;
            int check = 1;
            while(check == 1){
                c[i] = fgetc(fileptr);
                if(c[i] == '\n' || c[i] == '\r'){
                    check = 0;
                }
                i++;
                if(c[i-1] != ' ' && c[i-1] != '\n' && c[i-1] != '\t' && c[i-1] != '\r'){
                    check = 0;
                    while(i > 0){
                        i--;
                        ungetc(c[i], fp);
                    }
                }
            }
        }
        else if(string[0] != '#' && !(stringSuitCheck(string, suit))){
            fprintf(stderr, errMsg);
            exit(1);
        }
    }
    //skipGarbage, but for the Tableau/Stock
    void skipGarbTableau(FILE* fileptr, char string[100], char errMsg[100]){
        fscanf(fileptr, "%s", string);
        string[100] = fgetc(fileptr);
        //Checks for end of line
        //If a comment has been found, ignore the rest of the line
        if(string[0] == '#' && (string[100] != '\n' && string[100] != '\r')){
            char c = fgetc(fileptr);
            while(c != '\n' && c != '\r'){
                c = fgetc(fileptr);
            }
            //The line has ended, mark it so
            lineEnd = 1;
        }
        else if(string[100] == ' '){
            char c[100];
            int i = 0;
            int check = 1;
            while(check == 1){
                c[i] = fgetc(fileptr);
                if(c[i] == '\n' || c[i] == '\r'){
                    check = 0;
                    lineEnd = 1;
                }
                i++;
                if(c[i-1] != ' ' && c[i-1] != '\n' && c[i-1] != '\t' && c[i-1] != '\r'){
                    check = 0;
                    while(i > 0){
                        i--;
                        ungetc(c[i], fp);
                    }
                }
            }
        }
        else if(string[0] != '#' && !(stringCardCheck(string))){
            fprintf(stderr, errMsg);
            exit(1);
        }
        if(string[0] == '|'){
            line++;
        }
        if(string[100] == '\n' || string[100] == '\r'){
            lineEnd = 1;
        }
    }

    //Function to add all cards in a foundation to the array of all cards
    char cardSyms[] = "KQJT98765432A";
    void addFoundations(char string[100], char suit){
        if(string[0] == 'K'){
            allCards[amtCards] = 'K';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = 'Q';
        }
        if(string[0] == 'Q'){
            allCards[amtCards] = 'Q';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = 'J';
        }
        if(string[0] == 'J'){
            allCards[amtCards] = 'J';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = 'T';
        }
        if(string[0] == 'T'){
            allCards[amtCards] = 'T';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '9';
        }
        if(string[0] == '9'){
            allCards[amtCards] = '9';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '8';
        }
        if(string[0] == '8'){
            allCards[amtCards] = '8';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '7';
        }
        if(string[0] == '7'){
            allCards[amtCards] = '7';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '6';
        }
        if(string[0] == '6'){
            allCards[amtCards] = '6';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '5';
        }
        if(string[0] == '5'){
            allCards[amtCards] = '5';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '4';
        }
        if(string[0] == '4'){
            allCards[amtCards] = '4';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '3';
        }
        if(string[0] == '3'){
            allCards[amtCards] = '3';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '2';
        }
        if(string[0] == '2'){
            allCards[amtCards] = '2';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = 'A';
        }
        if(string[0] == 'A'){
            allCards[amtCards] = 'A';
            amtCards++;
            allCards[amtCards] = suit;
            amtCards++;
            string[0] = '_';
        }
    }

    //Skipping all text until RULES:
    while(!(stringCheck(input, "RULES:", 6))){
        skipGarbage(fp, input, "RULES:", 6, "Invalid format before RULES!");
    }
    //Skipping all text until turn
    while(!(stringCheck(input, "turn", 4))){
        skipGarbage(fp, input, "turn", 4, "Invalid format before turn requirement");
    }
    //Getting number of cards to turn as per rules
    fscanf(fp, "%d", &cardFlip);
    if(cardFlip != 1 && cardFlip != 3){
        fprintf(stderr, "Invalid rules!\n");
        exit(1);
    }
    //Skipping all text until unlimited or limit
    //Because of two checks cannot use skipGarbage function
    while(!((stringCheck(input, "unlimited", 9)) || stringCheck(input, "limit", 5))){
        fscanf(fp, "%s", input);
        input[100] = fgetc(fp);
        if(input[0] == '#'){
            while(fgetc(fp) != '\n');
        }
        if(input[0] != '#' && !((stringCheck(input, "unlimited", 9)) || (stringCheck(input, "limit", 5)))){
            fprintf(stderr, "Invalid format before stock specification!");
            exit(1);
        }
    }
    //Getting number of waste resets as per rules
    if(input[0] == 'l' && input[1] == 'i' && input[2] == 'm'
       && input[3] == 'i' && input[4] == 't' ){
        fscanf(fp, "%d", &limitReset);
        if(limitReset < 0){
            fprintf(stderr, "Invalid limit!\n");
            exit(1);
        }
    }
    else{
        //Max int value, virtually unlimited resets
        limitReset = 2147483647;
    }

    //Skipping all text until FOUNDATIONS:
    while(!(stringCheck(input, "FOUNDATIONS:", 12))){
        skipGarbage(fp, input, "FOUNDATIONS:", 12, "Invalid format before FOUNDATIONS!");
    }
    //Skipping all text until the clubs foundation
    while(!(stringSuitCheck(input, 'c'))){
        skipGarbCards(fp, input, 'c', "Invalid format before clubs foundation!");
    }
    addFoundations(input, 'c');
    //Skipping all text until the diamonds foundation
    while(!(stringSuitCheck(input, 'd'))){
        skipGarbCards(fp, input, 'd', "Invalid format before diamonds foundation!");
    }
    addFoundations(input, 'd');
    //Skipping all text until the hearts foundation
    while(!(stringSuitCheck(input, 'h'))){
        skipGarbCards(fp, input, 'h', "Invalid format before hearts foundation!");
    }
    addFoundations(input, 'h');
    //Skipping all text until the spades foundation
    while(!(stringSuitCheck(input, 's'))){
        skipGarbCards(fp, input, 's', "Invalid format before spades foundation!");
    }
    addFoundations(input, 's');
    //Skipping all text until TABLEAU:
    while(!(stringCheck(input, "TABLEAU:", 8))){
        skipGarbage(fp, input, "TABLEAU:", 8, "Invalid format before TABLEAU!");
    }

    //Does all of the column work
    int coveredCount = 0;
    int stockCount = 0;
    int wasteCount = 0;
    int uncovered = 0;
    int marker = 0;
    while(numCols > 0){
        //Keeps track of the possibility of a duplicate line error
        line = 0;
        //Makes sure the parser parses the beginning of each line
        marker = 0;
        //Keeps track of when one of the column lines has ended
        lineEnd = 0;
        //Keeps track of if the current column is uncovered
        uncovered = 0;

        //Skips garbage in the tableau until a column is found and gets the first card in column
        while(!(stringCardCheck(input)) || marker == 0){
            marker = 1;
            skipGarbTableau(fp, input, "Invalid format in TABLEAU columns!");
        }
        //Catches the only possible other errors: 2 lines in a single column or dup. cards
        if(line > 1){
            fprintf(stderr, "Too many lines in TABLEAU column!");
            exit(1);
        }
        //Finds and adds duplicate cards to the array keeping track
        for(int i = 0; i < amtCards; i = i+2){
            if(allCards[i] == input[0] && allCards[i+1] == input[1]){
                duplicates[numDupes] = allCards[i];
                numDupes++;
                duplicates[numDupes] = allCards[i+1];
                numDupes++;
                i = amtCards;
            }
        }
        //Adds the card to the array of all cards
        if(input[0] != '|' && input[0] != '#'){
            allCards[amtCards] = input[0];
            amtCards++;
            allCards[amtCards] = input[1];
            amtCards++;
        }
        //If this char was a line, marks uncovered to true
        else if(input[0] != '#'){
            uncovered = 1;
        }
        //If covered, increases the covered counter
        if(uncovered == 0){
            coveredCount++;
        }
        int uncoveredStart = 0;
        //Gets the cards for the rest of the line
        int isColumn = 0;
        if(lineEnd == 1){
            numCols--;
        }
        while(lineEnd != 1){
            isColumn = 1;
            skipGarbTableau(fp, input, "Invalid format in TABLEAU columns!");
            if(line > 1){
                fprintf(stderr, "Too many lines in TABLEAU column!");
                exit(1);
            }
            //Checks the legality of the tableau for red cards
            if((allCards[amtCards - 1] == 'd' || allCards[amtCards - 1] == 'h') && uncoveredStart == 2){
                if(input[1] == 'd' || input[1] == 'h'){
                    fprintf(stderr, "Illegal TABLEAU setup in column %d!", numCols);
                    exit(1);
                }
            }
            //Checks the legality for black cards
            else if((allCards[amtCards - 1] == 'c' || allCards[amtCards - 1] == 's') && uncoveredStart == 2){
                if(input[1] == 'c' || input[1] == 's'){
                    fprintf(stderr, "Illegal TABLEAU setup in column %d!", numCols);
                    exit(1);
                }
            }
            //Checks the legality of the cards being in the correct numerical order
            else if(input[0] != 'K' && uncoveredStart == 2){
                char currSym = input[0];
                int symSpot = 0;
                for(int i = 0; i < 13; i++){
                    if(cardSyms[i] == currSym){
                        symSpot = i;
                        i = 13;
                    }
                }
                if(allCards[amtCards - 2] != cardSyms[symSpot - 1]){
                    fprintf(stderr, "Illegal TABLEAU setup in column %d!", numCols);
                    exit(1);
                }
            }
            //Makes sure cards that are covered don't interfere with the truth checking above
            //int uncoveredStart = 0;
            //Finds and adds duplicate cards to the array keeping track
            for(int i = 0; i < amtCards; i = i+2){
                if(allCards[i] == input[0] && allCards[i+1] == input[1]){
                    duplicates[numDupes] = allCards[i];
                    numDupes++;
                    duplicates[numDupes] = allCards[i+1];
                    numDupes++;
                    i = amtCards;
                }
            }
            if(input[0] != '|' && input[0] != '#'){
                allCards[amtCards] = input[0];
                amtCards++;
                allCards[amtCards] = input[1];
                amtCards++;
            }
            //Makes sure the tableau doesn't mark as illegal if the first uncovered card wouldn't be legal with
            //with the last covered card
            if(uncoveredStart == 1){
                uncoveredStart++;
            }
            if(input[0] != '#' && input[0] == '|'){
                uncovered = 1;
                uncoveredStart = 1;
            }
            if(uncovered == 0){
                coveredCount++;
            }
        }
        //Skips lines in the TABLEAU that are comments so they aren't counted here
        if(isColumn == 1){
            numCols--;
        }
    }
    //Skipping all text until STOCK:
    while(!(stringCheck(input, "STOCK:", 6))){
        skipGarbage(fp, input, "STOCK:", 6, "Invalid format before STOCK!");
    }
    isStock = 1;
    //Adding cards from stock to the array of all cards and checking for errors in stock
    line = 0;
    uncovered = 0;
    while(!(stringCheck(input, "MOVES:", 6))){
        skipGarbTableau(fp, input, "Invalid format in STOCK!");
        if(line > 1){
            fprintf(stderr, "Too many lines in STOCK!");
            exit(1);
        }
        //Finds and adds duplicate cards to the array keeping track
        for(int i = 0; i < amtCards; i = i+2){
            if(allCards[i] == input[0] && allCards[i+1] == input[1]){
                duplicates[numDupes] = allCards[i];
                numDupes++;
                duplicates[numDupes] = allCards[i+1];
                numDupes++;
                i = amtCards;
            }
        }
        if(input[0] != '|' && input[0] != '#' && !(stringCheck(input, "MOVES:", 6))){
            allCards[amtCards] = input[0];
            amtCards++;
            allCards[amtCards] = input[1];
            amtCards++;
        }
        if(uncovered == 1 && input[0] != '#'){
           stockCount++;
        }
        if(uncovered == 0){
            wasteCount++;
        }
        if(input[0] == '|' && input[0] != '#'){
            uncovered = 1;
        }
    }

    //One final check just in case the last card added is a duplicate
    for(int i = 0; i < amtCards; i = i+2){
        if(allCards[i] == input[0] && allCards[i+1] == input[1]){
            duplicates[numDupes] = allCards[i];
            numDupes++;
            duplicates[numDupes] = allCards[i+1];
            numDupes++;
            i = amtCards;
        }
    }

    //Skipping all text until MOVES:
    while(!(stringCheck(input, "MOVES:", 6))){
        skipGarbage(fp, input, "MOVES:", 6, "Invalid format before MOVES!");
    }

    //Character arrays to check for missing cards
    //char copyAllCards[999];
    char missingCards[999];
    //These represent all of the different cards
    char arrayCards[104] = "KdQdJdTd9d8d7d6d5d4d3d2dAdKcQcJcTc9c8c7c6c5c4c3c2cAcKhQhJhTh9h8h7h6h5h4h3h2hAhKsQsJsTs9s8s7s6s5s4s3s2sAs";
    char copyArrayCards[104] = "KdQdJdTd9d8d7d6d5d4d3d2dAdKcQcJcTc9c8c7c6c5c4c3c2cAcKhQhJhTh9h8h7h6h5h4h3h2hAhKsQsJsTs9s8s7s6s5s4s3s2sAs";
    for(int i = 0; i < 999; i++){
        //copyAllCards[i] = allCards[i];
        missingCards[i] = allCards[i];
    }
    //j
    for(int i = 0; i < 104; i = i+2){
        for(int j = 0; j < 999; j = j+2){
            if(allCards[j] == arrayCards[i] && allCards[j+1] == arrayCards[i+1]){
                copyArrayCards[i] = 0;
                copyArrayCards[i+1] = 0;
            }
        }
    }

    int numMissing = 0;
    for(int i = 0; i < 104; i = i+2){
        if(copyArrayCards[i] != 0){
            missingCards[numMissing] = copyArrayCards[i];
            numMissing++;
            missingCards[numMissing] = copyArrayCards[i+1];
            numMissing++;
        }
    }

    //Utilizes all of the previous duplicate card checks in the above statements to print any duplicate cards
    if(numDupes > 0){
        fprintf(stderr, "Error, duplicate cards: ");
        for(int i = 0; i < numDupes; i = i+2){
            fprintf(stderr, "%c%c ", duplicates[i], duplicates[i+1]);
        }
        fprintf(stderr, "\n");
        if(numMissing == 0){
            exit(1);
        }
    }
    //Does the same as the duplicate cards, but for missing cards
    if(numMissing > 0){
        fprintf(stderr, "Error, missing cards: ");
        for(int i = 0; i < numMissing; i = i+2){
            fprintf(stderr, "%c%c ", missingCards[i], missingCards[i+1]);
        }
        fprintf(stderr, "\n");
        exit(1);

    }
    stockCount -= 1;
    wasteCount -= 1;

    //Printing necessary output for Part 1
    printf("\nInput file is valid\n");
    printf("%d covered cards\n", coveredCount);
    printf("%d stock cards\n", stockCount);
    printf("%d waste cards\n", wasteCount);
}