#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    char* mode = "r";
    FILE* fp;

    int cardFlip = 0;
    int limitReset = 0;
    //Amount of moves being checked by checkWinnable (default 1000)
    int amtMovesToCheck = 1000;
    //Boolean value to signify checkWinnable being in verbose mode or not
    int verbose = 0;
    //Signifies if foundation moves should be forced
    int forcedFoundations = 0;

    char filename[100];
    if(argc <= 1){
        fp = stdin;
    }
    else{
        for(int i = 1; i < argc; i++){
            if(!strcmp(argv[i],"-m") || !strcmp(argv[i],"-v") || !strcmp(argv[i],"-f")){
                //Search for a specified amount of moves only
                if(!strcmp(argv[i],"-m")){
                    amtMovesToCheck = atoi(argv[i+1]);
                }
                //Outputs diagnostic information
                else if(!strcmp(argv[i],"-v")){
                    verbose = 1;
                }
                else{
                    forcedFoundations = 1;
                }
            }
	    else if(argv[i][0] != '-' && strcmp(argv[i-1],"-m")){
		fp = fopen(argv[i], mode);
	    }
        }
    }

    if(fp == NULL){
	fp = stdin;
    }

    char input[100];

    //------SCANNING FROM FILE------//
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
    //Number of limits used by the user
    int numLims = 0;
    int amt = amtMovesToCheck;
    if(amtMovesToCheck > 600){
        amtMovesToCheck = 600;
    }
    //Initializing said arrays
    for(int i = 0; i < 999; i++){
        allCards[i] = '0';
        duplicates[i] = '0';
    }

    //Arrays for all parts of the game
    char tableau[7][27]; //27 because 13*2 for the max amt of possible cards and line
    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 27; j++){
            tableau[i][j] = '.';
        }
    }
    int tIndex = 0;
    int tIndex2 = 0;
    int tIndex3 = 0;
    int tIndex4 = 0;
    int tIndex5 = 0;
    int tIndex6 = 0;
    int tIndex7 = 0;
    char foundations[8];
    int fIndex = 0;
    char waste[105];
    char stock[105];
    for(int i = 0; i < 105; i++){
        stock[i] = ' ';
        waste[i] = ' ';
    }
    int wIndex = 0;
    int sIndex = 0;

    //Gets proper indexes and values for the tableau array
    void tableauIncrement(char arr[7][27], char string[], int c, int b){
            if(c == 1){
                tableau[c-1][tIndex] = string[0];
                tIndex++;
                if(b == 1){
                    tableau[c-1][tIndex] = string[1];
                    tIndex++;
                }
            }
            if(c == 2){
                tableau[c-1][tIndex2] = string[0];
                tIndex2++;
                if(b == 1){
                    tableau[c-1][tIndex2] = string[1];
                    tIndex2++;
                }
            }
            if(c == 3){
                tableau[c-1][tIndex3] = string[0];
                tIndex3++;
                if(b == 1){
                    tableau[c-1][tIndex3] = string[1];
                    tIndex3++;
                }
            }
            if(c == 4){
                tableau[c-1][tIndex4] = string[0];
                tIndex4++;
                if(b == 1){
                    tableau[c-1][tIndex4] = string[1];
                    tIndex4++;
                }
            }
            if(c == 5){
                tableau[c-1][tIndex5] = string[0];
                tIndex5++;
                if(b == 1){
                    tableau[c-1][tIndex5] = string[1];
                    tIndex5++;
                }
            }
            if(c == 6){
                tableau[c-1][tIndex6] = string[0];
                tIndex6++;
                if(b == 1){
                    tableau[c-1][tIndex6] = string[1];
                    tIndex6++;
                }
            }
            if(c == 7){
                tableau[c-1][tIndex7] = string[0];
                tIndex7++;
                if(b == 1){
                    tableau[c-1][tIndex7] = string[1];
                    tIndex7++;
                }
            }
    }

    //Checks if the searched-for string is found
    int stringCheck(char string[100], char compTo[100], int size){
        for(int i = 0; i < size; i++){
            if(string[i] != compTo[i]){
                return 0;
            }
        }
        if(string[100] != '\n' && string[100] != ' ' && string[100] != '\r' && string[100] != '\t' && string[100] != '#' && string[100] != EOF){
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

    char cardSyms[] = "KQJT98765432A";
    char found[] = "KQJT98765432A_";

    //Function to add all cards in a foundation to the array of all cards
    void addFoundations(char string[100], char suit){
        int initFind = 1;
        if(string[0] == '_'){
            foundations[fIndex] = '_';
            fIndex++;
            foundations[fIndex] = suit;
            fIndex++;
            initFind = 0;
        }
        for(int i = 0; i < 13; i++){
            if(string[0] == found[i]){
                allCards[amtCards] = found[i];
                amtCards++;
                allCards[amtCards] = suit;
                amtCards++;
                string[0] = found[i+1];
                if(initFind == 1){
                    foundations[fIndex] = found[i];
                    fIndex++;
                    foundations[fIndex] = suit;
                    fIndex++;
                    initFind = 0;
                }
            }
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
        //Keeps track of if the current column is uncovered
        uncovered = 0;

        //Keeps track of when one of the column lines has ended
        lineEnd = 0;

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
            tableauIncrement(tableau, input, numCols, 1);
        }
        //If this char was a line, marks uncovered to true
        else if(input[0] != '#'){
            tableauIncrement(tableau, input, numCols, 0);
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
                tableauIncrement(tableau, input, numCols, 1);
            }
            if(uncoveredStart == 1){
                uncoveredStart++;
            }
            if(input[0] != '#' && input[0] == '|'){
                uncovered = 1;
                uncoveredStart = 1;
                tableauIncrement(tableau, input, numCols, 0);
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
            if(uncovered == 0){
                waste[wIndex] = input[0];
                wIndex++;
                waste[wIndex] = input[1];
                wIndex++;
            }
        }
        if(uncovered == 1 && input[0] != '#' && input[0] != 'M'){
            stockCount++;
            stock[sIndex] = input[0];
            sIndex++;
            stock[sIndex] = input[1];
            sIndex++;
        }
        if(input[0] == '|' && input[0] != '#'){
            uncovered = 1;
        }
        if(uncovered == 0){
            wasteCount++;
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
    char missingCards[999];
    //These represent all of the different cards
    char arrayCards[104] = "KdQdJdTd9d8d7d6d5d4d3d2dAdKcQcJcTc9c8c7c6c5c4c3c2cAcKhQhJhTh9h8h7h6h5h4h3h2hAhKsQsJsTs9s8s7s6s5s4s3s2sAs";
    char copyArrayCards[104] = "KdQdJdTd9d8d7d6d5d4d3d2dAdKcQcJcTc9c8c7c6c5c4c3c2cAcKhQhJhTh9h8h7h6h5h4h3h2hAhKsQsJsTs9s8s7s6s5s4s3s2sAs";
    for(int i = 0; i < 999; i++){
        missingCards[i] = allCards[i];
    }
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

    // Part1
    /*if(!strcmp(argv[0],"./check")){
        //Printing necessary output for Part 1
        printf("\nInput file is valid\n");
        printf("%d covered cards\n", coveredCount);
        printf("%d stock cards\n", stockCount);
        printf("%d waste cards\n", wasteCount);
        exit(0);
    }*/

    // Part 2
    //Stop when EOF or moves limit is met
    /*while(!fileEnd && numMoves <= moves){
        skipGarbMoves(fp, input);
    }
    fprintf(stdout, "Processed %d moves, all valid\n", numMoves-1);
    printState();*/


    // Part 3

    //TODO
    //Make checkMove

    struct gamestate{
        char tableau[7][27];
        char waste[105];
        char stock[105];
        char foundations[8];
        int cardFlip;
        int limitReset;
        int wIndex;
        int sIndex;
        int tIndex[7];
        int lost;
        char* movesToGetHere;
        int numCompletedMoves;
        //int forbidden[7][7];
    };

    struct gamestate newg;

    //Checks where the card goes from the waste
    int moveWaste(char string[], int col, int index, int suit, int lookingFor, struct gamestate* g){
        //If the card is moving to the foundations
        if(col == 8){
            char theSuit = 's';
            if(suit == 0){
                theSuit = 'c';
            }
            else if(suit == 1){
                theSuit = 'd';
            }
            else if(suit == 2){
                theSuit = 'h';
            }
            int error = 1;
            for(int i = 1; i < 8; i = i+2){
                //If the suits are the same
                if(g->foundations[i] == theSuit){
                    //If the rank of the foundation is one less than the waste card
                    //Uses looking for + 2 because looking for rank below, not above
                    if(g->foundations[i-1] == found[lookingFor+2]){
                        //Gives the waste card to the foundation
                        g->foundations[i-1] = g->waste[g->wIndex-2];
                        g->waste[g->wIndex-2] = ' ';
                        g->waste[g->wIndex-1] = ' ';
                        g->wIndex = g->wIndex - 2;
                        error = 0;
                        return 1;
                    }
                }
            }
            if(error == 1){
                return 0;
            }
        }
        //If the tableau being moved into must be empty
        else if(lookingFor == -1){
            //If empty
            if(g->tableau[col][0] == '|'){
                g->tableau[col][index] = g->waste[g->wIndex - 2];
                g->tableau[col][index+1] = g->waste[g->wIndex - 1];
                g->waste[g->wIndex - 2] = ' ';
                g->waste[g->wIndex - 1] = ' ';
                g->wIndex = g->wIndex - 2;
                return 1;
            }
            else{
                return 0;
            }
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(g->tableau[col][0] == '.'){
            return 0;
        }
        //If the top card of the tableau is what the waste card needs to move onto
        else if(cardSyms[lookingFor] == g->tableau[col][index - 2]){
            int suit2 = 0;
            if(g->tableau[col][index-1] == 'd'){
                suit2 = 1;
            }
            else if(g->tableau[col][index-1] == 'h'){
                suit2 = 2;
            }
            else if(g->tableau[col][index-1] == 's'){
                suit2 = 3;
            }
            if(((suit == 0 || suit == 3) && (suit2 == 1 || suit2 == 2)) || ((suit == 1 || suit == 2) && (suit2 == 0 || suit2 == 3))){
                g->tableau[col][index] = g->waste[g->wIndex - 2];
                g->tableau[col][index+1] = g->waste[g->wIndex - 1];
                g->waste[g->wIndex - 2] = ' ';
                g->waste[g->wIndex - 1] = ' ';
                g->wIndex = g->wIndex - 2;
                return 1;
            }
            else{
                return 0;
            }
        }
        return 0;
    }

    int moveTableau(char string[], int col, int index, int suit, char fromChar, int fCol, int frmI, int indexToGet, struct gamestate* g){
        //If the card is moving to the foundations
        if(col == 8){
            char theSuit = 's';
            if(suit == 0){
                theSuit = 'c';
            }
            else if(suit == 1){
                theSuit = 'd';
            }
            else if(suit == 2){
                theSuit = 'h';
            }
            for(int i = 1; i < 8; i = i+2){
                //If the suits are the same
                if(g->foundations[i] == theSuit){
                        //Gives the tableau card to the foundation
                        g->foundations[i-1] = g->tableau[fCol][frmI - 2];
                        g->tableau[fCol][frmI - 2] = '.';
                        g->tableau[fCol][frmI - 1] = '.';
                        return 1;
                }
            }
            return 0;
        }
        //If the tableau being moved into must be empty
        else if(fromChar == 'K'){
            //If empty
            if(g->tableau[col][0] == '|' && g->tableau[col][1] == '.'){
                while(indexToGet > 0){
                    g->tableau[col][index] = g->tableau[fCol][frmI - indexToGet];
                    g->tableau[fCol][frmI - indexToGet] = '.';
                    indexToGet--;
                    index++;
                }
                return 1;
            }
            return 0;
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(g->tableau[col][0] == '.'){
            return 0;
        }
        //char cardSyms[] = "KQJT98765432A";
        else{
            int suit2 = 0;
            if(g->tableau[col][index-1] == 'd'){
                suit2 = 1;
            }
            else if(g->tableau[col][index-1] == 'h'){
                suit2 = 2;
            }
            else if(g->tableau[col][index-1] == 's'){
                suit2 = 3;
            }
            if(((suit == 0 || suit == 3) && (suit2 == 1 || suit2 == 2)) || ((suit == 1 || suit == 2) && (suit2 == 0 || suit2 == 3))){
                while(indexToGet > 0){
                    g->tableau[col][index] = g->tableau[fCol][frmI - indexToGet];
                    g->tableau[fCol][frmI - indexToGet] = '.';
                    indexToGet--;
                    index++;
                }
                return 1;
            }
            return 0;
        }
    }

    //Flips the top covered card of a column
    void flipCard(int col, int index, struct gamestate* g){
        if(index > 1 && g->tableau[col][index - 1] == '|'){
            char temp1 = g->tableau[col][index - 3];
            char temp2 = g->tableau[col][index - 2];
            g->tableau[col][index - 3] = '|';
            g->tableau[col][index - 2] = temp1;
            g->tableau[col][index - 1] = temp2;
        }
    }

    //Initializes the base gamestate
    struct gamestate base;
    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 27; j++){
            base.tableau[i][j] = tableau[i][j];
        }
    }
    for(int i = 0; i < 105; i++){
        base.waste[i] = waste[i];
        base.stock[i] = stock[i];
    }
    for(int i = 0; i < 8; i++){
        base.foundations[i] = foundations[i];
    }
    base.cardFlip = cardFlip;
    base.limitReset = limitReset;
    base.wIndex = wIndex;
    base.sIndex = sIndex;
    base.tIndex[0] = tIndex;
    base.tIndex[1] = tIndex2;
    base.tIndex[2] = tIndex3;
    base.tIndex[3] = tIndex4;
    base.tIndex[4] = tIndex5;
    base.tIndex[5] = tIndex6;
    base.tIndex[6] = tIndex7;
    base.lost = 0;
    base.movesToGetHere = malloc(9999);
    base.numCompletedMoves = 0;
    /*for(int i = 0; i < 7; i++){
        for(int j = 0; j < 7; j++){
            base.forbidden[i][j] = 0;
        }
    }*/

    //Checks if the given move works, and appends it to a list of moves that
    //have been made to get to this gamestate if it works.
    struct gamestate checkMove(struct gamestate g, char string[]){
            int numStringChars = 4;
            //This includes moving from columns of TABLEAU
            if(string[0] == '1' || string[0] == '2' || string[0] == '3'
                || string[0] == '4' || string[0] == '5' || string[0] == '6'
                || string[0] == '7'){
                //Move card from FROM to TO (check if FROM is empty and valid move)
                //Flip next card over on FROM
                int from = string[0] - '1';
                int fromI = 0;
                for(int i = 0; i < 7; i++){
                    if(string[0] == i+'1'){
                        fromI = g.tIndex[i];
                    }
                }

                //Can't move from empty Tableau
                if(fromI == 0){
                    g.lost = 1;
                    newg = g;
                    return newg;
                }
                //Gets the number of characters in front of the line
                int numChars = 0;
                int tempIndex = fromI - 1;
                int check = 1;
                //Kc|3d2cAh = 6
                while(check){
                    if(g.tableau[from][tempIndex] != '|'){
                        tempIndex--;
                        numChars++;
                    }
                    else{
                        check = 0;
                    }
                }

                //Almost useless, only necessary for foundation checking
                int foundSuit = 0;
                if(string[3] == 'f'){
                    if(g.tableau[from][fromI - 1] == 'd'){
                        foundSuit = 1;
                    }
                    else if(g.tableau[from][fromI - 1] == 'h'){
                        foundSuit = 2;
                    }
                    else if(g.tableau[from][fromI - 1] == 's'){
                        foundSuit = 3;
                    }
                }

                char fromChar = 'K';
                for(int i = 0; i < 14; i++){
                    //Nothing can go on an Ace
                    if(cardSyms[i] == 'A' && string[3] != 'f'){
                        g.lost = 1;
                        newg = g;
                        return newg;
                    }
                    //If the to tableau's last card matches the char in cardSyms, fromChar is the
                    //char that would need to be put on that one
                    //char cardSyms[] = "KQJT98765432A";
                    if(string[3] == 'f'){
                        if(g.foundations[foundSuit*2] == found[i]){
                            fromChar = found[i - 1];
                            i = 14;
                        }
                    }
                    else if(g.tIndex[string[3] - '1'] == 1){
                        i = 14;
                    }
                    else if(g.tableau[string[3] - '1'][g.tIndex[string[3] - '1'] - 2] == cardSyms[i]){
                        fromChar = cardSyms[i + 1];
                        i = 14;
                    }
                }
                if(tableau[from][0] == '|' && fromChar == 'K'){
                    g.lost = 1;
                    newg = g;
                    return newg;
                }

                //How far back the index for the from tableau needs to go in order to retrieve the
                //right card
                int indexToGet = 2;
                for(int i = 2; i <= numChars; i=i+2){
                    if(g.tableau[from][fromI - i] != fromChar){
                        indexToGet = indexToGet + 2;
                    }
                    else{
                        i = numChars;
                    }
                }
                //The card needed is not on the from column
                if(indexToGet > numChars){
                    g.lost = 1;
                    newg = g;
                    return newg;
                }

                //Finds the suit of the card
                int suit = 0;
                if(g.tableau[from][fromI - indexToGet + 1] == 'd'){
                    suit = 1;
                }
                else if(g.tableau[from][fromI - indexToGet + 1] == 'h'){
                    suit = 2;
                }
                else if(g.tableau[from][fromI - indexToGet + 1] == 's'){
                    suit = 3;
                }
                //Checks all movement possibilities
                int go = 1;
                for(int i = 0; i < 7; i++){
                    if(string[3] == i + '1'){
                        go = moveTableau(string, i, g.tIndex[i], suit, fromChar, from, fromI, indexToGet, &g);
                        g.tIndex[i] = g.tIndex[i]+indexToGet;
                        if(go == 0){
                            g.lost = 1;
                            newg = g;
                            return newg;
                        }
                    }
                }
                if(string[3] == 'f'){
                    go = moveTableau(string, 8, g.tIndex[0], suit, fromChar, from, fromI, indexToGet, &g);
                    if(go == 0){
                        g.lost = 1;
                        newg = g;
                        return newg;
                    }
                }

                for(int i = 0; i < 7; i++){
                    if(string[0] == i + '1'){
                        g.tIndex[i] = g.tIndex[i] - indexToGet;
                        flipCard(i, g.tIndex[i], &g);
                    }
                }
            }
            //Or moving from the waste
            else if(string[0] == 'w'){
                    //Can't move from empty waste
                    if(g.wIndex == 0){
                        g.lost = 1;
                        newg = g;
                        return newg;
                    }
                    //Rank of the card the waste card must be placed on
                    int lookingForT = 0;
                    for(int i = 0; i < 13; i++){
                        if(g.waste[g.wIndex - 2] == cardSyms[i]){
                            lookingForT = i-1;
                        }
                    }
                    //Finds the suit of the waste card
                    int suit = 0;
                    if(g.waste[g.wIndex - 1] == 'd'){
                        suit = 1;
                    }
                    else if(g.waste[g.wIndex - 1] == 'h'){
                        suit = 2;
                    }
                    else if(g.waste[g.wIndex - 1] == 's'){
                        suit = 3;
                    }
                    //Checks all movement possibilities
                    int go = 1;
                    for(int i = 0; i < 7; i++){
                        if(string[3] == i + '1'){
                            go = moveWaste(string, i, g.tIndex[i], suit, lookingForT, &g);
                            g.tIndex[i] = g.tIndex[i]+2;
                        }
                        if(go == 0){
                            g.lost = 1;
                            newg = g;
                            return newg;
                        }
                    }
                    if(string[3] == 'f'){
                        go = moveWaste(string, 8, g.tIndex[0], suit, lookingForT, &g);
                    }
                    if(go == 0){
                        g.lost = 1;
                        newg = g;
                        return newg;
                    }
                }
        else if(string[0] == '.'){
            numStringChars = 1;
            for(int i = 0; i < cardFlip; i++){
                //Fails if the stock starts empty
                if(i == 0 && g.stock[0] == ' '){
                    g.lost = 1;
                    newg = g;
                    return newg;
                }
                //Gives the top stock card to the waste
                //(the card next to the line on the left is the
                //top of the waste)
                //(the card next to the line on the right is the
                //top of the stock)
                if(g.stock[0] != ' '){
                    g.waste[g.wIndex] = g.stock[0];
                    g.wIndex++;
                    g.waste[g.wIndex] = g.stock[1];
                    g.wIndex++;
                    for(int i = 0; i < g.sIndex - 2; i++){
                        g.stock[i] = g.stock[i+2];
                    }
                    g.stock[g.sIndex - 2] = ' ';
                    g.stock[g.sIndex - 1] = ' ';
                    g.sIndex = g.sIndex - 2;
                }
            }
        }
        //Resets the waste into the stock
        else if(string[0] == 'r'){
            numStringChars = 1;
            if(g.stock[0] != ' ' || numLims >= g.limitReset){
                g.lost = 1;
                newg = g;
                return newg;
            }
            //First element of waste array is first element of stock
            int mark = g.wIndex;
            while(g.sIndex < mark){
                g.stock[g.sIndex] = g.waste[g.sIndex];
                g.sIndex++;
                g.stock[g.sIndex] = g.waste[g.sIndex];
                g.sIndex++;
                g.waste[g.sIndex - 2] = ' ';
                g.waste[g.sIndex - 1] = ' ';
                g.wIndex = g.wIndex - 2;
            }
            numLims++;
        }
        //Adds the move to the list of completed moves
        for(int i = 0; i < numStringChars; i++){
            g.movesToGetHere[g.numCompletedMoves] = string[i];
            g.numCompletedMoves++;
        }
        g.movesToGetHere[g.numCompletedMoves] = '\n';
        g.numCompletedMoves++;
        newg = g;
        return newg;
    }

    //Function that recursively checks if the game can be won
    //Winnable game: All tableau array columns start with |, stockCount = 0
    //and wasteCount <= 1
    int lostGame = 1;
    int numIter = 0;

    //Prints moves in exchange format
    void printMoves(int totalMoves, struct gamestate g){
        if(verbose == 1){
            fprintf(stderr, "%d configurations checked.\n", numIter - 1);
        }
        if(totalMoves >= 600){
            totalMoves = amt;
        }
        fprintf(stdout, "#Game is winnable in %d moves\n", totalMoves);

        int i = 0;
        while(g.movesToGetHere[i] != '!'){
            fprintf(stdout, "%c", g.movesToGetHere[i]);
            i++;
        }
    }

    //59 possible moves (tableau cannot move to itself
    void checkWinnable(struct gamestate g, int soFarChecked){
        if(g.lost == 1){
            return;
        }
        if(soFarChecked > amtMovesToCheck){
            return;
        }
        else{
            soFarChecked++;
        }
        if(verbose == 1){
            if(numIter == 0){
                fprintf(stderr, "Using DFS search\n");
            }
            if(numIter % 250000 == 0 && numIter != 0){
                fprintf(stderr, "%d configurations checked so far\n", numIter);
            }
            numIter++;
        }

        //printMoves(soFarChecked-1, g);

        //Checks if winnable conditions are met
        int won = 1;
        for(int i = 0; i < 7; i++){
            if(g.tableau[i][0] != '|'){
                won = 0;
            }
        }
        if(g.stock[0] != ' ' || g.waste[2] != ' '){
            won = 0;
        }
        if(won == 1){
            lostGame = 0;
            g.movesToGetHere[g.numCompletedMoves] = '!';
            printMoves(soFarChecked-1, g);
            exit(0);
        }

        //If winnable conditions aren't met yet, makes all possible moves

        char input[4] = "w->f";
        //Loop through every possible waste move
        checkWinnable(checkMove(g, input), soFarChecked);
        for(int i = 0; i < 7; i++){
            input[3] = i+'1';
            checkWinnable(checkMove(g, input), soFarChecked);
        }

        //Loop through every possible tableau move
        input[3] = 'f';
        for(int i = 0; i < 7; i++){
            input[0] = i+'1';
            checkWinnable(checkMove(g, input), soFarChecked);
        }
        for(int i = 0; i < 7; i++){
            input[0] = i+'1';
            for(int j = 0; j < 7; j++){
                input[3] = j+'1';
                /*if(j != i && g.forbidden[i][j] != 1){
                    //Every move affected by the one just made is now allowed again
                    for(int k = 0; k < 7; k++){
                        g.forbidden[i][k] = 0;
                        g.forbidden[j][k] = 0;
                    }
                    //The mirror move is now forbidden
                    g.forbidden[j][i] = 1;
                    checkWinnable(checkMove(g, input), soFarChecked);
                }*/
                /*if(j != i && (input[0] != g.movesToGetHere[g.numCompletedMoves-1] &&
                    g.movesToGetHere[g.numCompletedMoves-5] != input[3])){
                    checkWinnable(checkMove(g, input), soFarChecked);
                }*/
                if(j != i){
                    checkWinnable(checkMove(g, input), soFarChecked);
                }
            }
        }

        //Do the remaining moves: ., r
        checkWinnable(checkMove(g, "r"), soFarChecked);
        checkWinnable(checkMove(g, "."), soFarChecked);
    }

    void checkWinnableForcedFoundations(struct gamestate g, int soFarChecked){
        if(soFarChecked > amtMovesToCheck){
            return;
        }
        else{
            soFarChecked++;
        }
        if(g.lost == 1){
            return;
        }
        if(verbose == 1){
            numIter++;
            if(numIter == 1){
                fprintf(stderr, "Using DFS search\n");
            }
            if((numIter - 1) % 250000 == 0 && numIter != 1){
                fprintf(stderr, "%d configurations checked so far\n", numIter - 1);
            }
        }

        //Checks if winnable conditions are met
        int won = 1;
        for(int i = 0; i < 7; i++){
            if(g.tableau[i][0] != '|'){
                won = 0;
            }
        }
        if(g.stock[0] != ' ' || g.waste[2] != ' '){
            won = 0;
        }
        if(won == 1){
            lostGame = 0;
            g.movesToGetHere[g.numCompletedMoves] = '!';
            printMoves(soFarChecked-1, g);
            exit(0);
        }

        //If winnable conditions aren't met yet, makes all possible moves

        //Forces SAFE moves to the foundations
        char input[4] = "1->f";
        int canFoundationBeForced = 0;
        //From Tableau
        for(int i = 0; i < 8; i++){
            char needed = '.';
            int cRank = 0;
            int dRank = 0;
            int hRank = 0;
            int sRank = 0;
            for(int j = 0; j < 13; j++){
                if(i != 7){
                    if(cardSyms[j] == g.tableau[i][g.tIndex[i] - 2]){
                        needed = cardSyms[j+1];
                    }
                }
                else{
                    if(cardSyms[j] == g.waste[wIndex - 2]){
                        needed = cardSyms[j+1];
                    }
                }
                if(cardSyms[j] == g.foundations[0]){
                    cRank = -j;
                }
                if(cardSyms[j] == g.foundations[2]){
                    dRank = -j;
                }
                if(cardSyms[j] == g.foundations[4]){
                    hRank = -j;
                }
                if(cardSyms[j] == g.foundations[6]){
                    sRank = -j;
                }
            }
            if(i != 7){
                input[0] = i + '1';
                //If the card being moved is clubs and it's rank is one greater than the foundations
                if(g.tableau[i][g.tIndex[i] - 1] == 'c' && needed == g.foundations[0]){
                //If the red foundations are >= the clubs one
                    if(dRank >= cRank && hRank >= cRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                //Checks diamonds
                else if(g.tableau[i][g.tIndex[i] - 1] == 'd' && needed == g.foundations[2]){
                    if(cRank >= dRank && sRank >= dRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                //Hearts
                else if(g.tableau[i][g.tIndex[i] - 1] == 'h' && needed == g.foundations[4]){
                    if(cRank >= hRank && sRank >= hRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                //Spades
                else if(g.tableau[i][g.tIndex[i] - 1] == 's' && needed == g.foundations[6]){
                    if(dRank >= sRank && hRank >= sRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
            }
            else{
                input[0] = 'w';
                if(g.waste[wIndex - 1] == 'c' && needed == g.foundations[0]){
                    if(dRank >= cRank && hRank >= cRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                if(g.waste[wIndex - 1] == 'd' && needed == g.foundations[2]){
                    if(cRank >= dRank && sRank >= dRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                if(g.waste[wIndex - 1] == 'h' && needed == g.foundations[4]){
                    if(cRank >= hRank && sRank >= hRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
                if(g.waste[wIndex - 1] == 's' && needed == g.foundations[6]){
                    if(dRank >= sRank && hRank >= sRank){
                        checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                        canFoundationBeForced = 1;
                    }
                }
            }
        }

        //Makes it so all foundation moves must be forced before
        //anything else happens
        if(canFoundationBeForced == 1){
            return;
        }
        //Loop through every possible waste move
        for(int i = 0; i < 7; i++){
            input[3] = i+'1';
            checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
        }

        //Tableau moves
        for(int i = 0; i < 7; i++){
            input[0] = i+'1';
            for(int j = 0; j < 7; j++){
                input[3] = j+'1';
                if(j != i){
                    checkWinnableForcedFoundations(checkMove(g, input), soFarChecked);
                }
            }
        }

        //Do the remaining moves: ., r
        checkWinnableForcedFoundations(checkMove(g, "r"), soFarChecked);
        checkWinnableForcedFoundations(checkMove(g, "."), soFarChecked);
    }

    if(forcedFoundations == 0){
        checkWinnable(base, 0);
    }
    else{
        checkWinnableForcedFoundations(base, 0);
    }

    if(lostGame == 1){
        if(verbose == 1){
            fprintf(stderr, "%d configurations checked.\n", numIter);
        }
        fprintf(stdout, "# Game is not winnable within %d moves\n", amt);
        exit(0);
    }
}
