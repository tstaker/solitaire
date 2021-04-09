#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "termbox.h"
int main(int argc, char* argv[]){

    char* mode = "r";
    FILE* fp;
    int cardFlip = 1;
    int limitReset = 2147483647;
    //2147483647 is max int
    int seed = 1;
    unsigned long seedID = 25;

    char filename[30];
	for(int i = 0; i < 30; i++){
		filename[i] = ' ';
	}
    //CHANGE FILENAME TO THE SEED IF SEED IS TRIGGERED
    if(argc == 1){
        //fp = stdin;
        scanf("%s", filename);
        fp = fopen(filename, mode);
    }
    else{
        for(int i = 0; i < argc; i++){
                //Game is read from a file
                if(!strcmp(argv[i],"-f")){
                    fp = fopen(argv[i+1], mode);
                    seed = 0;
		    strcpy(filename, argv[i+1]);
		    i = argc;
                }
                //Game is "randomly" generated
                else if(!strcmp(argv[i],"-s")){
                    seedID = atoi(argv[i+1]);
		    strcpy(filename, argv[i+1]);
                }
                //Changes the style of output to exchange
                else if(!strcmp(argv[i],"-1")){
                    cardFlip = 1;
                }
                else if(!strcmp(argv[i],"-3")){
                    cardFlip = 3;
                }
                else if(!strcmp(argv[i],"-l")){
                    limitReset = atoi(argv[i+1]);
                }
        }
    }

    if(fp == NULL && seed == 0){
        fprintf(stderr, "File doesn't exist!\n");
        exit(1);
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
    int tIndex[7];
    for(int i = 0; i < 7; i++){
        tIndex[i] = 0;
    }
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

    //Helper method for printing strings on termbox
    void printString(int xcoord, int ycoord, char* string, struct tb_cell cell){
        for(int i = 0; i < strlen(string); i++){
            cell.ch = string[i];
            tb_put_cell(xcoord + i, ycoord, &cell);
        }
    }

    //Helper method for printing cards on termbox
    void printCard(int xcoord, int ycoord, char rank, char suit, struct tb_cell cell, int covered){
        /*if(suit == 'c'){
            cell.ch = '';
        }
        else if(suit == 'd'){
            cell.ch = '';
        }
        else if(suit == 'd'){
            cell.ch = '';
        }
        else{
            cell.ch = '';
        }*/
        //If foundation is empty or card is covered
        if(rank == '_' || covered == 1){
            if(rank == '_'){
                cell.ch = suit;
            }
            else{
                cell.ch = '#';
            }
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    tb_put_cell(xcoord + i, ycoord + j, &cell);
                }
            }
        }
        //Printing card normally
        else{
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                cell.bg = TB_WHITE;
                if(suit == 'd' || suit == 'h'){
                    cell.fg = TB_RED;
                }
                if((i == 0 && j == 0) || (i == 3 && j == 3)){
                    cell.ch = rank;
                }
                else if((i == 1 && j == 0) || (i == 2 && j == 3)){
                    cell.ch = suit;
                }
                else{
                    cell.ch = ' ';
                }
                tb_put_cell(xcoord + i, ycoord + j, &cell);
            }
        }
	struct tb_cell black;
	black.ch = ' ';
	black.fg = TB_BLACK;
	black.bg = TB_BLACK;
	int number = 4;
	while(number + ycoord < 25){
	    tb_put_cell(xcoord, ycoord + number, &black);
	    tb_put_cell(xcoord + 1, ycoord + number, &black);
	    tb_put_cell(xcoord + 2, ycoord + number, &black);
	    tb_put_cell(xcoord + 3, ycoord + number, &black);
	    number++;
	}
        }
    }

    //Prints the current state of the game
    //Text is blue, cards are white, covered cards are green
    void printState(){
        struct tb_cell display;
        display.ch = 'S';
        display.fg = TB_BLACK;
        display.bg = TB_CYAN;

        struct tb_cell card;
        card.ch = 'S';
        card.fg = TB_BLACK;
        card.bg = TB_WHITE;

        struct tb_cell covered;
        covered.ch = 'S';
        covered.fg = TB_BLACK;
        covered.bg = TB_GREEN;

        struct tb_cell block;
        block.ch = ' ';
        block.fg = TB_BLACK;
        block.bg = TB_BLACK;

        //Printing Foundations
        printString(0, 9, "Foundations", display);
        printCard(0, 11, foundations[0], foundations[1], covered, 0);
        printCard(5, 11, foundations[2], foundations[3], covered, 0);
        printCard(10, 11, foundations[4], foundations[5], covered, 0);
        printCard(15, 11, foundations[6], foundations[7], covered, 0);

        //Printing Waste and Stock
        printString(0, 16, "Waste", display);
        char str[10] = "Unlimited ";
        if(limitReset != 2147483647){
	    for(int i = 0; i < 10; i++){
                str[i] = ' ';
            }
            sprintf(str, "%d", limitReset - numLims);
        }
	//Prints stock with reset amount
        char newString[24];
        newString[0] = 'S';
        newString[1] = 't';
        newString[2] = 'o';
        newString[3] = 'c';
        newString[4] = 'k';
        newString[5] = ':';
        newString[6] = ' ';
        int k = 0;
        while(k < strlen(str)){
            newString[k+7] = str[k];
            k++;
        }
        newString[k+7] = ' ';
        newString[k+8] = 'r';
        newString[k+9] = 'e';
        newString[k+10] = 's';
        newString[k+11] = 'e';
        newString[k+12] = 't';
        newString[k+13] = 's';
        printString(7, 16, newString, display);
	printString(32, 16, "          ", block);

	//Prints waste and stock card
        if(wIndex <= 1){
            printCard(0, 18, ' ', ' ', card, 0);
        }
        else{
            printCard(0, 18, waste[wIndex-2], waste[wIndex-1], card, 0);
        }
        if(sIndex > 1){
            printCard(7, 18, 'A', 'A', covered, 1);
        }
	else{
	    tb_put_cell(7, 18, &block);
	    tb_put_cell(8, 18, &block);
	    tb_put_cell(9, 18, &block);
	    tb_put_cell(7, 19, &block);
	    tb_put_cell(8, 19, &block);
	    tb_put_cell(9, 19, &block);
	    tb_put_cell(7, 20, &block);
	    tb_put_cell(8, 20, &block);
	    tb_put_cell(9, 20, &block);
	    tb_put_cell(7, 21, &block);
	    tb_put_cell(8, 21, &block);
	    tb_put_cell(9, 21, &block);
	}

        //Printing Tableau
        printString(38, 0, "1", display);
        printString(43, 0, "2", display);
        printString(48, 0, "3", display);
        printString(53, 0, "4", display);
        printString(58, 0, "5", display);
        printString(63, 0, "6", display);
        printString(68, 0, "7", display);
        for(int i = 0; i < 7; i++){
            int hitLine = 1;
            for(int j = 0; j < tIndex[i]; j = j + 2){
                if(tableau[i][j] == '|'){
                    hitLine = 0;
                    j++;
                }
                if(tIndex[i] <= 1){
                    printCard(38 + (5 * i), (j / 2) + 1, ' ', ' ', covered, 0);
                }
                else if(hitLine == 0){
                    printCard(38 + (5 * i), (j / 2) + 1, tableau[i][j], tableau[i][j+1], card, hitLine);
                }
                else{
                    printCard(38 + (5 * i), (j / 2) + 1, tableau[i][j], tableau[i][j+1], covered, hitLine);
                }
            }
        }

        tb_present();

    }

    //Gets proper indexes and values for the tableau array
    void tableauIncrement(char arr[7][27], char string[], int c, int b){
            tableau[c-1][tIndex[c-1]] = string[0];
            tIndex[c-1]++;
            if(b == 1){
                tableau[c-1][tIndex[c-1]] = string[1];
                tIndex[c-1]++;
            }
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

    //If reading from a file, set up the game
    if(seed == 0){
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
    }
    //Function for seed
    else{
        int theDeck[52];
        char arrayCards[104] = "KcQcJcTc9c8c7c6c5c4c3c2cAcKdQdJdTd9d8d7d6d5d4d3d2dAdKhQhJhTh9h8h7h6h5h4h3h2hAhKsQsJsTs9s8s7s6s5s4s3s2sAs";

        double Random(){
            const unsigned long MODULUS = 2147483647;
            const unsigned long MULTIPLIER = 48271;
            const unsigned long Q = MODULUS / MULTIPLIER;
            const unsigned long R = MODULUS % MULTIPLIER;
            unsigned long t1 = MULTIPLIER * (seedID % Q);
            unsigned long t2 = R * (seedID / Q);
            if (t1 > t2){
                seedID = t1 - t2;
            }
            else{
                seedID = t1 + (MODULUS - t2);
            }
            return ((double) seedID / MODULUS);
        }

        long choose_randomly_between(long a, long b){
            return a + (long) ( (b-a+1) * Random() );
        }

        void shuffle(int* deck){
            int i;
            for(i=0; i<52; i++) deck[i] = i;
            for(i=0; i<51; i++){
                int j = choose_randomly_between(i, 51);
                if (i != j){
		    int temp = deck[i];
                    deck[i] = deck[j];
		    deck[j] = temp;
                }
            }
        }

        shuffle(theDeck);
        char shuffledDeck[104];
        for(int i = 0; i < 104; i = i+2){
            shuffledDeck[i] = arrayCards[theDeck[i/2] * 2];
            shuffledDeck[i+1] = arrayCards[(theDeck[i/2] * 2) + 1];
        }

        //Puts cards in the tableau
        int shuffledIndex = 0;
        for(int i = 0; i < 7; i++){
            for(int j = 6 - i; j < 7; j++){
                if(j + 1 == 7){
                    tableau[i][tIndex[i]] = '|';
                    tIndex[i]++;
                }
                tableau[i][tIndex[i]] = shuffledDeck[shuffledIndex];
                tIndex[i]++;
                shuffledIndex++;
                tableau[i][tIndex[i]] = shuffledDeck[shuffledIndex];
                tIndex[i]++;
                shuffledIndex++;
            }
        }
        //Puts the rest in the stock
	sIndex = 0;
        for(int i = 0; i < 24; i++){
            stock[sIndex] = shuffledDeck[shuffledIndex];
            sIndex++;
            shuffledIndex++;
            stock[sIndex] = shuffledDeck[shuffledIndex];
            sIndex++;
            shuffledIndex++;
        }

        //Initializes foundations
        foundations[0] = '_';
        foundations[1] = 'c';
        foundations[2] = '_';
        foundations[3] = 'd';
        foundations[4] = '_';
        foundations[5] = 'h';
        foundations[6] = '_';
        foundations[7] = 's';

    }

    int undoneIndex = 0;
    int cardFlipped = 0;
    char lastCardFrom = 'x';
    char lastCardTo = 'x';
    int undoneSuit = 0;
    int justUndone = 0;
    /*if(!strcmp(argv[0],"./check")){
        //Printing necessary output for Part 1
        printf("\nInput file is valid\n");
        printf("%d covered cards\n", coveredCount);
        printf("%d stock cards\n", stockCount);
        printf("%d waste cards\n", wasteCount);
        exit(0);
    }*/

    //Stop when EOF or moves limit is met
    /*while(!fileEnd && numMoves <= moves){
        skipGarbMoves(fp, input);
    }
    fprintf(stdout, "Processed %d moves, all valid\n", numMoves-1);*/

    int moveWaste(char string[], int col, int index, int suit, int lookingFor){
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
                if(foundations[i] == theSuit){
                    //If the rank of the foundation is one less than the waste card
                    //Uses looking for + 2 because looking for rank below, not above
                    if(foundations[i-1] == found[lookingFor+2]){
                        //Gives the waste card to the foundation
                        foundations[i-1] = waste[wIndex-2];
                        waste[wIndex-2] = ' ';
                        waste[wIndex-1] = ' ';
                        wIndex = wIndex - 2;
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
            if(tableau[col][0] == '|'){
                tableau[col][index] = waste[wIndex - 2];
                tableau[col][index+1] = waste[wIndex - 1];
                waste[wIndex - 2] = ' ';
                waste[wIndex - 1] = ' ';
                wIndex = wIndex - 2;
                return 1;
            }
            else{
                return 0;
            }
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(tableau[col][0] == '.'){
            return 0;
        }
        //If the top card of the tableau is what the waste card needs to move onto
        else if(cardSyms[lookingFor] == tableau[col][index - 2]){
            int suit2 = 0;
            if(tableau[col][index-1] == 'd'){
                suit2 = 1;
            }
            else if(tableau[col][index-1] == 'h'){
                suit2 = 2;
            }
            else if(tableau[col][index-1] == 's'){
                suit2 = 3;
            }
            if(((suit == 0 || suit == 3) && (suit2 == 1 || suit2 == 2)) || ((suit == 1 || suit == 2) && (suit2 == 0 || suit2 == 3))){
                tableau[col][index] = waste[wIndex - 2];
                tableau[col][index+1] = waste[wIndex - 1];
                waste[wIndex - 2] = ' ';
                waste[wIndex - 1] = ' ';
                wIndex = wIndex - 2;
                return 1;
            }
            else{
                return 0;
            }
        }
        return 0;
    }

    int moveTableau(char string[], int col, int index, int suit, char fromChar, int fCol, int frmI, int indexToGet){
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
                if(foundations[i] == theSuit){
                        //Gives the tableau card to the foundation
                        foundations[i-1] = tableau[fCol][frmI - 2];
                        tableau[fCol][frmI - 2] = '.';
                        tableau[fCol][frmI - 1] = '.';
                        return 1;
                }
            }
            return 0;
        }
        //If the tableau being moved into must be empty
        else if(fromChar == 'K'){
            //If empty
            if(tableau[col][0] == '|' && tableau[col][1] == '.'){
                while(indexToGet > 0){
                    tableau[col][index] = tableau[fCol][frmI - indexToGet];
                    tableau[fCol][frmI - indexToGet] = '.';
                    indexToGet--;
                    index++;
                }
                return 1;
            }
            return 0;
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(tableau[col][0] == '.'){
            return 0;
        }
        //char cardSyms[] = "KQJT98765432A";
        else{
            int suit2 = 0;
            if(tableau[col][index-1] == 'd'){
                suit2 = 1;
            }
            else if(tableau[col][index-1] == 'h'){
                suit2 = 2;
            }
            else if(tableau[col][index-1] == 's'){
                suit2 = 3;
            }
            if(((suit == 0 || suit == 3) && (suit2 == 1 || suit2 == 2)) || ((suit == 1 || suit == 2) && (suit2 == 0 || suit2 == 3))){
                while(indexToGet > 0){
                    tableau[col][index] = tableau[fCol][frmI - indexToGet];
                    tableau[fCol][frmI - indexToGet] = '.';
                    indexToGet--;
                    index++;
                }
                return 1;
            }
            return 0;
        }
    }

    void flipCard(int col, int index){
        char temp1 = tableau[col][index - 3];
        char temp2 = tableau[col][index - 2];
        tableau[col][index - 3] = '|';
        tableau[col][index - 2] = temp1;
        tableau[col][index - 1] = temp2;
    }

    int checkMove(char string[]){
	    justUndone = 0;
            lastCardTo = 'x';
	    lastCardFrom = 'x';
            //This includes moving from columns of TABLEAU
            if(string[0] == '1' || string[0] == '2' || string[0] == '3'
                || string[0] == '4' || string[0] == '5' || string[0] == '6'
                || string[0] == '7'){
		lastCardFrom = string[0];
                //Move card from FROM to TO (check if FROM is empty and valid move)
                //Flip next card over on FROM
                int from = string[0] - '1';
                int fromI = 0;
                for(int i = 0; i < 7; i++){
                    if(string[0] == i+'1'){
                        fromI = tIndex[i];
                    }
                }

                //Can't move from empty Tableau
                if(fromI == 0){
                    return 0;
                }
                //Gets the number of characters in front of the line
                int numChars = 0;
                int tempIndex = fromI - 1;
                int check = 1;
                //Kc|3d2cAh = 6
                while(check){
                    if(tableau[from][tempIndex] != '|'){
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
                    if(tableau[from][fromI - 1] == 'd'){
                        foundSuit = 1;
                    }
                    else if(tableau[from][fromI - 1] == 'h'){
                        foundSuit = 2;
                    }
                    else if(tableau[from][fromI - 1] == 's'){
                        foundSuit = 3;
                    }
                    undoneSuit = foundSuit;
                }

                char fromChar = 'K';
                for(int i = 0; i < 14; i++){
                    //Nothing can go on an Ace
                    if(cardSyms[i] == 'A' && string[3] != 'f'){
                        return 0;
                    }
                    //If the to tableau's last card matches the char in cardSyms, fromChar is the
                    //char that would need to be put on that one
                    //char cardSyms[] = "KQJT98765432A";
                    if(string[3] == 'f'){
                        if(foundations[foundSuit*2] == found[i]){
                            fromChar = found[i - 1];
                            i = 14;
                        }
                    }
                    else if(tIndex[string[3] - '1'] == 1){
                        i = 14;
                    }
                    else if(tableau[string[3] - '1'][tIndex[string[3] - '1'] - 2] == cardSyms[i]){
                        fromChar = cardSyms[i + 1];
                        i = 14;
                    }
                }

                //How far back the index for the from tableau needs to go in order to retrieve the
                //right card
                int indexToGet = 2;
                for(int i = 2; i <= numChars; i=i+2){
                    if(tableau[from][fromI - i] != fromChar){
                        indexToGet = indexToGet + 2;
                    }
                    else{
                        i = numChars;
                    }
                }
                //The card needed is not on the from column
                if(indexToGet > numChars){
                    return 0;
                }
                undoneIndex = indexToGet;

                //Finds the suit of the card
                int suit = 0;
                if(tableau[from][fromI - indexToGet + 1] == 'd'){
                    suit = 1;
                    undoneSuit = 1;
                }
                else if(tableau[from][fromI - indexToGet + 1] == 'h'){
                    suit = 2;
                    undoneSuit = 2;
                }
                else if(tableau[from][fromI - indexToGet + 1] == 's'){
                    suit = 3;
                    undoneSuit = 3;
                }
                //Checks all movement possibilities
                int go = 1;
                for(int i = 0; i < 7; i++){
                    if(string[3] == i + '1'){
                        go = moveTableau(string, i, tIndex[i], suit, fromChar, from, fromI, indexToGet);
                        if(go == 0){
                            return 0;
                        }
                        tIndex[i] = tIndex[i]+indexToGet;
                    }
                }
                if(string[3] == 'f'){
                    go = moveTableau(string, 8, tIndex[0], suit, fromChar, from, fromI, indexToGet);
                    if(go == 0){
                        return 0;
                    }
                }

                int wasCardFlipped = 0;
                for(int i = 0; i < 7; i++){
                    if(string[0] == i + '1'){
                        tIndex[i] = tIndex[i] - indexToGet;
                        if(tIndex[i] > 1 && tableau[i][tIndex[i] - 1] == '|'){
                            cardFlipped = 1;
                            wasCardFlipped = 1;
                            flipCard(i, tIndex[i]);
                        }
                    }
                }
                if(wasCardFlipped == 0){
                    cardFlipped = 0;
                }
		lastCardFrom = string[0];
                lastCardTo = string[3];
            }
            //Or moving from the waste
            else if(string[0] == 'w'){
		    lastCardFrom = 'w';
                    undoneIndex = 0;
                    //Can't move from empty waste
                    if(wIndex == 0){
                        return 0;
                    }
                    //Rank of the card the waste card must be placed on
                    int lookingForT = 0;
                    for(int i = 0; i < 13; i++){
                        if(waste[wIndex - 2] == cardSyms[i]){
                            lookingForT = i-1;
                        }
                    }
                    //Finds the suit of the waste card
                    int suit = 0;
                    if(waste[wIndex - 1] == 'd'){
                        suit = 1;
                    }
                    else if(waste[wIndex - 1] == 'h'){
                        suit = 2;
                    }
                    else if(waste[wIndex - 1] == 's'){
                        suit = 3;
                    }
                    undoneSuit = suit;
                    //Checks all movement possibilities
                    int go = 1;
                    for(int i = 0; i < 7; i++){
                        if(string[3] == i + '1'){
                            go = moveWaste(string, i, tIndex[i], suit, lookingForT);
                            if(go == 0){
                                return 0;
                            }
                            tIndex[i] = tIndex[i]+2;
                        }
                    }
                    if(string[3] == 'f'){
                        go = moveWaste(string, 8, tIndex[0], suit, lookingForT);
                    }
                    if(go == 0){
                        return 0;
                    }
		    lastCardFrom = string[0];
               	    lastCardTo = string[3];
                }
        else if(string[0] == '.'){
            undoneIndex = 0;
            lastCardFrom = '.';
            for(int i = 0; i < cardFlip; i++){
                //Fails if the stock starts empty
                if(i == 0 && stock[0] == ' '){
                    return 0;
                }
                //Gives the top stock card to the waste
                //(the card next to the line on the left is the
                //top of the waste)
                //(the card next to the line on the right is the
                //top of the stock)
                if(sIndex > 1){
                    waste[wIndex] = stock[0];
                    wIndex++;
                    waste[wIndex] = stock[1];
                    wIndex++;
                    for(int i = 0; i < sIndex - 2; i++){
                        stock[i] = stock[i+2];
                    }
                    stock[sIndex - 2] = ' ';
                    stock[sIndex - 1] = ' ';
                    sIndex = sIndex - 2;
                }
            }
	    lastCardFrom = '.';
        }
        //Resets the waste into the stock
        else if(string[0] == 'r'){
            undoneIndex = 0;
            if(stock[0] != ' ' || numLims >= limitReset){
                return 0;
            }
	    lastCardFrom = 'r';
            //First element of waste array is first element of stock
            int mark = wIndex;
            while(sIndex < mark){
                stock[sIndex] = waste[sIndex];
                sIndex++;
                stock[sIndex] = waste[sIndex];
                sIndex++;
                waste[sIndex - 2] = ' ';
                waste[sIndex - 1] = ' ';
                wIndex = wIndex - 2;
            }
            numLims++;
        }
        return 0;
    }

    void undo(){
	justUndone = 1;
        //Keep track of the last set of cards that were moved
        //Be able to move them back
        //Cover/uncover cards as needed
        //Ex: last move was 1->7
        //Now we must move those cards from 7->1
        //Th9c    KsQhJs   ->   Ac    KsQhJsTh9c
        //undoneIndex = 4;

        //Undoes next card from stock
        if(lastCardFrom == '.'){
	    for(int j = 0; j < cardFlip; j++){
                if(wIndex > 1){
                    sIndex = sIndex + 2;
                    for(int i = sIndex - 1; i >= 2; i--){
                        stock[i] = stock[i-2];
                    }
                    stock[0] = waste[wIndex - 2];
                    stock[1] = waste[wIndex - 1];
                    waste[wIndex - 2] = ' ';
                    waste[wIndex - 1] = ' ';
                    wIndex -= 2;
                }
            }
        }
        //Undoes a reset
        else if(lastCardFrom == 'r'){
	    int cheatCheck = 1;
            //Puts the stock into the waste
	    for(int i = 0; i < sIndex - 1; i += 2){
		cheatCheck = 0;
		waste[wIndex] = stock[i];
		waste[wIndex+1] = stock[i+1];
                wIndex += 2;
		stock[i] = ' ';
		stock[i+1] = ' ';
            }
	    if(cheatCheck == 0){
		numLims--;
	    }
            sIndex = 0;
        }
        //Undoes moves from waste
        int toIndex = lastCardTo - '1';
        int fromIndex = lastCardFrom - '1';
        if(lastCardFrom == 'w'){
            //To foundations
            //char found[] = "KQJT98765432A_";
            if(lastCardTo == 'f'){
		waste[wIndex] = foundations[undoneSuit * 2];
                waste[wIndex + 1] = foundations[(undoneSuit * 2) + 1];
                wIndex = wIndex + 2;
                //Takes card from foundations
                for(int i = 0; i < 14; i++){
                    if(foundations[undoneSuit * 2] == found[i]){
                        foundations[undoneSuit * 2] = found[i+1];
                        i = 14;
                    }
                }
            }
            //To tableau
            else{
                waste[wIndex] = tableau[toIndex][tIndex[toIndex] - 2];
                waste[wIndex + 1] = tableau[toIndex][tIndex[toIndex] - 1];
                tIndex[toIndex] -= 2;
                wIndex = wIndex + 2;
            }
        }
        //Undoes moves from Tableau
        else{
            //To foundations
            if(lastCardTo == 'f'){
		if(tableau[fromIndex][tIndex[fromIndex] - 3] == '|'){
                    //Changes the line to A
                    tableau[fromIndex][tIndex[fromIndex] - 3] = tableau[fromIndex][tIndex[fromIndex] - 2];
                    //Changes the duplicate A to c
                    tableau[fromIndex][tIndex[fromIndex] - 2] = tableau[fromIndex][tIndex[fromIndex] - 1];
                    //Covers the card
                    tableau[fromIndex][tIndex[fromIndex] - 1] = '|';
                }
		tableau[fromIndex][tIndex[fromIndex]] = foundations[undoneSuit * 2];
                tableau[fromIndex][tIndex[fromIndex] + 1] = foundations[(undoneSuit * 2) + 1];
                tIndex[fromIndex] += 2;
		//Takes card from foundations
                for(int i = 0; i < 14; i++){
                    if(foundations[undoneSuit * 2] == found[i]){
                        foundations[undoneSuit * 2] = found[i+1];
                        i = 14;
                    }
                }
            }
            //To tableau
            else if(lastCardFrom != 'x' && lastCardTo != 'x'){
                if(cardFlipped == 1){
                    //Changes the line to A
                    tableau[fromIndex][tIndex[fromIndex] - 3] = tableau[fromIndex][tIndex[fromIndex] - 2];
                    //Changes the duplicate A to c
                    tableau[fromIndex][tIndex[fromIndex] - 2] = tableau[fromIndex][tIndex[fromIndex] - 1];
                    //Covers the card
                    tableau[fromIndex][tIndex[fromIndex] - 1] = '|';
                }
                for(int i = 0; i < undoneIndex; i++){
                    tableau[fromIndex][tIndex[fromIndex]] = tableau[toIndex][tIndex[toIndex] - undoneIndex + i];
                    tableau[toIndex][tIndex[toIndex] - undoneIndex + i] = '.';
                    tIndex[fromIndex]++;
                }
                tIndex[toIndex] = tIndex[toIndex] - undoneIndex;
            }
        }
    }


    //Gameplay loop, runs while game is not yet won (foundations filled)
    tb_init();
    struct tb_cell display2;
    display2.ch = 'S';
    display2.fg = TB_BLACK;
    display2.bg = TB_CYAN;
    //Printing seed/file
        if(seed == 1){
            char* thisString = malloc(37);
            thisString[0] = 'S';
            thisString[1] = 'e';
            thisString[2] = 'e';
            thisString[3] = 'd';
            thisString[4] = ':';
            thisString[5] = ' ';
            int i = 0;
            while(filename[i] != ' '){
                thisString[i+6] = filename[i];
                i++;
            }
            printString(0, 0, thisString, display2);
        }
        else{
            char* thisString = malloc(37);
            thisString[0] = 'F';
            thisString[1] = 'i';
            thisString[2] = 'l';
            thisString[3] = 'e';
            thisString[4] = ':';
            thisString[5] = ' ';
            int i = 0;
            while(filename[i] != ' '){
                thisString[i+6] = filename[i];
                i++;
            }
            printString(0, 0, thisString, display2);
        }
        //Printing quit
        printString(0, 1, "Press Q to Quit", display2);
        //Printing undo
        printString(0, 4, "Press U to Undo (once per move)", display2);
        //Printing next card
        printString(0, 2, "Press N for Next card", display2);
        //Printing reset stock
        printString(0, 3, "Press R to Reset stock", display2);
        //Printing all other moves
        printString(0, 5, "To move, press the source and", display2);
        printString(0, 6, "then destination (one of W, F,", display2);
        printString(0, 7, "1, 2, 3, 4, 5, 6, 7)", display2);
    printState();
    int lookingForDest = 0;
    char source = '.';
    while(foundations[0] != 'K' || foundations[2] != 'K' || foundations[4] != 'K' || foundations[6] != 'K'){
        struct tb_event input1;
        tb_poll_event(&input1);
        //All possible tableau/waste moves
        if(input1.ch == 'f' || input1.ch == 'w' || input1.ch == '1' || input1.ch == '2' || input1.ch == '3' || input1.ch == '4' || input1.ch == '5' || input1.ch == '6' || input1.ch == '7'){
	    if(lookingForDest == 0 && input1.ch != 'f'){
                source = input1.ch;
                lookingForDest = 1;
            }
            else if(input1.ch != 'w'){
                lookingForDest = 0;
                char move[4];
                move[0] = source;
                move[1] = '-';
                move[2] = '>';
                move[3] = input1.ch;
                checkMove(move);
                printState();
            }
        }
        //All other viable button presses
        else if(input1.ch == 'Q' || input1.ch == 'q'){
            tb_shutdown();
            printf("\n\nGame successfully terminated.\n\n\n");
            exit(0);
        }
        else if(input1.ch == 'N' || input1.ch == 'n'){
            lookingForDest = 0;
            checkMove(".");
            printState();
        }
        else if(input1.ch == 'R' || input1.ch == 'r'){
            lookingForDest = 0;
            checkMove("r");
            printState();
        }
        else if(input1.ch == 'U' || input1.ch == 'u'){
	    if(justUndone == 0){
            	undo();
	    	printState();
	    }
        }
        //All other button presses do nothing
    }

    //Once game is won
    tb_shutdown();
    printf("\n\nYOU WON!\n\n");
    return 0;
}