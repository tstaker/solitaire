#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    char* mode = "r";
    char* mode2 = "w";
    FILE* fp;

    int cardFlip = 0;
    int limitReset = 0;
    //2147483647 is max int
    int moves = 2147483647;

    //0 is no exchange format, 1 is exchange format
    int exchange = 0;
    FILE* output = stdout;

    int cmdCheck = 0;

    char filename[100];
    if(argc == 1){
        fp = stdin;
    }
    else{
        //Check to see if there are any cmd switches
        for(int i = 0; i < argc; i++){
            if(!strcmp(argv[i],"-m") || !strcmp(argv[i],"-x") || !strcmp(argv[i],"-o")){
                //Sets a specific amount of moves
                if(!strcmp(argv[i],"-m")){
		    cmdCheck = 1;
                    moves = atoi(argv[i+1]);
                    fp = fopen(argv[i+2], mode);
                }
                //Changes the output location of the game to a file
                else if(!strcmp(argv[i],"-o")){
                    //Output is set to file output instead of stdout
                    output = fopen(argv[i+1], mode2);
                }
                //Changes the style of output to exchange
                else if(!strcmp(argv[i],"-x")){
                    exchange = 1;
                }
            }
        }
        //If only a file to read from is provided
	for(int i = 0; i < argc; i++){
            if(cmdCheck == 0 && argv[i] != "-x" && argv[i-1] != "-o"){
                fp = fopen(argv[i], mode);
            }
	}
    }

    if(fp == NULL){
        fp = stdin;
    }

    char input[100];

    //------SCANNING FROM FILE------//
    //Signals end of file
    int fileEnd = 0;
    //Number of moves that have happened
    int numMoves = 1;
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

    //Prints the current state of the game
    void printState(){
        //Normal, "human-readable" form
        if(exchange == 0){
            //Foundations printing
            fprintf(output, "Foundations\n");
            int i = 0;
            while(i < 8){
                if(i < 6){
                    fprintf(output, "%c", foundations[i]);
                    i++;
                    fprintf(output, "%c ", foundations[i]);
                    i++;
                }
                else{
                    fprintf(output, "%c", foundations[i]);
                    i++;
                    fprintf(output, "%c\n", foundations[i]);
                    i++;
                }
            }

            //Tableau printing
            fprintf(output, "Tableau\n");
            int lines[7] = {0, 0, 0, 0, 0, 0, 0};
            //Index to cycle through for all columns
            int ind = 0;
            int done = 0;
            while(done == 0){
                done = 1;
                ind = ind + 2;
                for(int j = 0; j < 7; j++){
                    ind = ind - 2;
                    //Section for first character
                    //First, checks for a line at the current index
                    if(tableau[j][ind] == '|'){
                        lines[j] = 1;
                    }
                    //If we haven't found a line for this column yet, the card is covered.
                    //Print a hash to signify this.
                    if(lines[j] == 0){
                        fprintf(output, "#");
                    }
                    //If we have discovered the line, print the value for this card.
                    //(+1 to account for the line being in the array)
                    else{
                        fprintf(output, "%c", tableau[j][ind+1]);
                    }
                    ind++;

                    //Section for second character
                    if(tableau[j][ind] == '|'){
                        lines[j] = 1;
                    }
                    if(lines[j] == 0 && j != 6){
                        fprintf(output, "# ");
                        done = 0;
                    }
                    else if(lines[j] == 0){
                        fprintf(output, "#\n");
                        done = 0;
                    }
                    //Checks if the next char is a dot, signifying there are no cards left.
                    //Once this is true for all columns, it will stop printing so a row of
                    //all dots is never printed.
                    else if(j != 6){
                        fprintf(output, "%c ", tableau[j][ind+1]);
                        if(tableau[j][ind+2] != '.'){
                            done = 0;
                        }
                    }
                    else if(j == 6){
                        fprintf(output, "%c\n", tableau[j][ind+1]);
                        if(tableau[j][ind+2] != '.'){
                            done = 0;
                        }
                    }
                    ind++;
                }
            }

            //Waste printing
            fprintf(output, "Waste top\n");
            if(wIndex > 1){
                fprintf(output, "%c%c\n", waste[wIndex - 2], waste[wIndex - 1]);
            }
            else{
                fprintf(output, "(empty)\n");
            }
        }
        //Exchange, file form
        else if(exchange == 1){
            //Prints rules
            fprintf(output, "RULES:\n");
            fprintf(output, "   turn %d\n", cardFlip);
            if(limitReset == 2147483647){
                fprintf(output, "   unlimited\n");
            }
            else{
                fprintf(output, "   limit %d\n", limitReset);
            }

            //Prints foundations
            int k = 0;
            fprintf(output, "FOUNDATIONS:\n");
            while(k < 8){
                fprintf(output, "   %c", foundations[k]);
                k++;
                fprintf(output, "%c\n", foundations[k]);
                k++;
            }

            //Prints tableau
            fprintf(output, "TABLEAU:\n");
            for(int i = 6; i >= 0; i--){
                int ind = 0;
                if(i == 6){
                    ind = tIndex[6];
                }
                else if(i == 5){
                    ind = tIndex[5];
                }
                else if(i == 4){
                    ind = tIndex[4];
                }
                else if(i == 3){
                    ind = tIndex[3];
                }
                else if(i == 2){
                    ind = tIndex[2];
                }
                else if(i == 1){
                    ind = tIndex[1];
                }
                else if(i == 0){
                    ind = tIndex[0];
                }
                for(int j = 0; j < ind; j = j+2){
                    if(tableau[i][j] == '|'){
                        if(j+1 == ind){
                            fprintf(output, "%c\n", tableau[i][j]);
                            j--;
                        }
                        else{
                            fprintf(output, "%c ", tableau[i][j]);
                            j--;
                        }
                    }
                    else if(j+2 < ind){
                        fprintf(output, "%c", tableau[i][j]);
                        fprintf(output, "%c ", tableau[i][j+1]);
                    }
                    else{
                        fprintf(output, "%c", tableau[i][j]);
                        fprintf(output, "%c\n", tableau[i][j+1]);
                    }
                }
            }

            //Prints stock
            fprintf(output, "STOCK:\n");
            for(int i = 0; i < wIndex; i = i+2){
                fprintf(output, "%c", waste[i]);
                fprintf(output, "%c ", waste[i+1]);
            }
            fprintf(output, " | ");
            for(int i = 0; i < sIndex; i = i+2){
                fprintf(output, "%c", stock[i]);
                fprintf(output, "%c ", stock[i+1]);
            }
            fprintf(output, "\n");

            //Finally, prints moves
            fprintf(output, "MOVES:\n");
        }
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
    //Checks where the card goes from the waste
    void moveWaste(char string[], int col, int index, int suit, int lookingFor){
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
                        numMoves++;
                        error = 0;
                    }
                }
            }
            if(error == 1){
                fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                printState();
                exit(1);
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
                numMoves++;
            }
            else{
                fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                printState();
                exit(1);
            }
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(tableau[col][0] == '.'){
            fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
            printState();
            exit(1);
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
                numMoves++;
            }
            else{
                fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                printState();
                exit(1);
            }
        }
        else{
            fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
            printState();
            exit(1);
        }
    }

    void moveTableau(char string[], int col, int index, int suit, char fromChar, int fCol, int frmI, int indexToGet){
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
                        //Gives the tableau card to the foundation
                        foundations[i-1] = tableau[fCol][frmI - 2];
                        tableau[fCol][frmI - 2] = '.';
                        tableau[fCol][frmI - 1] = '.';
                        numMoves++;
                        error = 0;
                }
            }
            if(error == 1){
                fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                printState();
                exit(1);
            }
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
                numMoves++;
            }
            else{
                fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                printState();
                exit(1);
            }
        }
        //If the tableau being moved into is empty and shouldn't be
        else if(tableau[col][0] == '.'){
            fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
            printState();
            exit(1);
        }
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
                numMoves++;
            }
        }
        /*else{
            fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
            printState();
            exit(1);
        }*/

    }

    //Flips the top covered card of a column
    void flipCard(int col, int index){
        if(index > 1 && tableau[col][index - 1] == '|'){
            char temp1 = tableau[col][index - 3];
            char temp2 = tableau[col][index - 2];
            tableau[col][index - 3] = '|';
            tableau[col][index - 2] = temp1;
            tableau[col][index - 1] = temp2;
        }
    }

    //Skips comments, finds garbage, and activates moves
    int dMarker = 0;
    void skipGarbMoves(FILE* fileptr, char string[100]){
        int aMarker = 0;
        int bMarker = 0;
        int cMarker = 0;
        if(fscanf(fileptr, "%s", string) == EOF){
            fileEnd = 1;
            aMarker = 1;
        }
        string[100] = fgetc(fileptr);
        if(string[100] == EOF){
            fileEnd = 1;
            cMarker = 1;
        }
        if(aMarker == 0 && dMarker == 0){
            if(cMarker == 1){
                dMarker = 1;
            }
        //If input is a comment (or like this: example#comment)
        if(string[0] == '#' && string[100] != '\n'){
            while(fgetc(fileptr) != '\n');
            bMarker = 1;
        }
        else if(string[100] == ' '){
            char c[100];
            int i = 0;
            int check = 1;
            while(check == 1){
                c[i] = fgetc(fileptr);
                if(c[i] == '\n'){
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
        //Processes the possible moves with an arrow
        if(string[1] == '-' && string[2] == '>' && (string[100] == ' ' || dMarker == 1
           || string[100] == '\n' || string[100] == '\r' || string[100] == '\t') && bMarker == 0){
            //This includes moving from columns of TABLEAU
            if(string[0] == '1' || string[0] == '2' || string[0] == '3'
                || string[0] == '4' || string[0] == '5' || string[0] == '6'
                || string[0] == '7'){

                if(string[3] == '1' || string[3] == '2' || string[3] == '3'
                || string[3] == '4' || string[3] == '5' || string[3] == '6'
                || string[3] == '7' || string[3] == 'f'){
                //Move card from FROM to TO (check if FROM is empty and valid move)
                //Flip next card over on FROM
                int from = string[0] - '1';
                int fromI = 0;
                if(string[0] == '1'){
                    fromI = tIndex[0];
                }
                else if(string[0] == '2'){
                    fromI = tIndex[1];
                }
                else if(string[0] == '3'){
                    fromI = tIndex[2];
                }
                else if(string[0] == '4'){
                    fromI = tIndex[3];
                }
                else if(string[0] == '5'){
                    fromI = tIndex[4];
                }
                else if(string[0] == '6'){
                    fromI = tIndex[5];
                }
                else if(string[0] == '7'){
                    fromI = tIndex[6];
                }
                //Can't move from empty Tableau
                if(fromI == 0){
                    fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                    printState();
                    exit(1);
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
                }
                char fromChar = 'K';
                for(int i = 0; i < 14; i++){
                    //Nothing can go on an Ace
                    if(cardSyms[i] == 'A' && string[3] != 'f'){
                        fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                        printState();
                        exit(1);
                    }
                    //If the to tableau's last card matches the char in cardSyms, fromChar is the
                    //char that would need to be put on that one
                    //char cardSyms[] = "KQJT98765432A";
                    //char found[] = "KQJT98765432A_";
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
                    fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                    printState();
                    exit(1);
                }
                //Finds the suit of the card
                int suit = 0;
                if(tableau[from][fromI - indexToGet + 1] == 'd'){
                    suit = 1;
                }
                else if(tableau[from][fromI - indexToGet + 1] == 'h'){
                    suit = 2;
                }
                else if(tableau[from][fromI - indexToGet + 1] == 's'){
                    suit = 3;
                }
                //Checks all movement possibilities
                    if(string[3] == '1'){
                        moveTableau(string, 0, tIndex[0], suit, fromChar, from, fromI, indexToGet);
                        tIndex[0] = tIndex[0]+indexToGet;
                    }
                    if(string[3] == '2'){
                        moveTableau(string, 1, tIndex[1], suit, fromChar, from, fromI, indexToGet);
                        tIndex[1] = tIndex[1]+indexToGet;
                    }
                    if(string[3] == '3'){
                        moveTableau(string, 2, tIndex[2], suit, fromChar, from, fromI, indexToGet);
                        tIndex[2] = tIndex[2]+indexToGet;
                    }
                    if(string[3] == '4'){
                        moveTableau(string, 3, tIndex[3], suit, fromChar, from, fromI, indexToGet);
                        tIndex[3] = tIndex[3]+indexToGet;
                    }
                    if(string[3] == '5'){
                        moveTableau(string, 4, tIndex[4], suit, fromChar, from, fromI, indexToGet);
                        tIndex[4] = tIndex[4]+indexToGet;
                    }
                    if(string[3] == '6'){
                        moveTableau(string, 5, tIndex[5], suit, fromChar, from, fromI, indexToGet);
                        tIndex[5] = tIndex[5]+indexToGet;
                    }
                    if(string[3] == '7'){
                        moveTableau(string, 6, tIndex[6], suit, fromChar, from, fromI, indexToGet);
                        tIndex[6] = tIndex[6]+indexToGet;
                    }
                    if(string[3] == 'f'){
                        moveTableau(string, 8, tIndex[0], suit, fromChar, from, fromI, indexToGet);
                    }

                    //Properly adjusts the from column
                    if(string[0] == '1'){
                        tIndex[0] = tIndex[0] - indexToGet;
                        flipCard(0, tIndex[0]);
                    }
                    else if(string[0] == '2'){
                        tIndex[1] = tIndex[1] - indexToGet;
                        flipCard(1, tIndex[1]);
                    }
                    else if(string[0] == '3'){
                        tIndex[2] = tIndex[2] - indexToGet;
                        flipCard(2, tIndex[2]);
                    }
                    else if(string[0] == '4'){
                        tIndex[3] = tIndex[3] - indexToGet;
                        flipCard(3, tIndex[3]);
                    }
                    else if(string[0] == '5'){
                        tIndex[4] = tIndex[4] - indexToGet;
                        flipCard(4, tIndex[4]);
                    }
                    else if(string[0] == '6'){
                        tIndex[5] = tIndex[5] - indexToGet;
                        flipCard(5, tIndex[5]);
                    }
                    else if(string[0] == '7'){
                       tIndex[6] = tIndex[6] - indexToGet;
                       flipCard(6, tIndex[6]);
                    }
                }
            }
            //Or moving from the waste
            else if(string[0] == 'w'){
                if(string[3] == '1' || string[3] == '2' || string[3] == '3'
                || string[3] == '4' || string[3] == '5' || string[3] == '6'
                || string[3] == '7' || string[3] == 'f'){
                    //Can't move from empty waste
                    if(wIndex == 0){
                        fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                        printState();
                        exit(1);
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

                    //Checks all movement possibilities
                    if(string[3] == '1'){
                        moveWaste(string, 0, tIndex[0], suit, lookingForT);
                        tIndex[0] = tIndex[0]+2;
                    }
                    if(string[3] == '2'){
                        moveWaste(string, 1, tIndex[1], suit, lookingForT);
                        tIndex[1] = tIndex[1]+2;
                    }
                    if(string[3] == '3'){
                        moveWaste(string, 2, tIndex[2], suit, lookingForT);
                        tIndex[2] = tIndex[2]+2;
                    }
                    if(string[3] == '4'){
                        moveWaste(string, 3, tIndex[3], suit, lookingForT);
                        tIndex[3] = tIndex[3]+2;
                    }
                    if(string[3] == '5'){
                        moveWaste(string, 4, tIndex[4], suit, lookingForT);
                        tIndex[4] = tIndex[4]+2;
                    }
                    if(string[3] == '6'){
                        moveWaste(string, 5, tIndex[5], suit, lookingForT);
                        tIndex[5] = tIndex[5]+2;
                    }
                    if(string[3] == '7'){
                        moveWaste(string, 6, tIndex[6], suit, lookingForT);
                        tIndex[6] = tIndex[6]+2;
                    }
                    if(string[3] == 'f'){
                        moveWaste(string, 8, tIndex[0], suit, lookingForT);
                    }
                }
                else{
                    fprintf(stderr, "Formatting error on move %d!", numMoves);
                    exit(1);
                }
            }
            else{
                fprintf(stderr, "Formatting error on move %d!", numMoves);
                exit(1);
            }
        }
        else if(string[0] == '.' && bMarker == 0 && (string[100] == ' ' || dMarker == 1
           || string[100] == '\n' || string[100] == '\r' || string[100] == '\t')){
            for(int i = 0; i < cardFlip; i++){
                //Fails if the stock starts empty
                if(i == 0 && stock[0] == ' '){
                    fprintf(stderr, "Move %d is illegal: %c%c%c%c\n", numMoves, string[0], string[1], string[2], string[3]);
                    printState();
                    exit(1);
                }
                //Gives the top stock card to the waste
                //(the card next to the line on the left is the
                //top of the waste)
                //(the card next to the line on the right is the
                //top of the stock)
                if(stock[0] != ' '){
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
                    numMoves++;
                }
            }
        }
        //Resets the waste into the stock
        else if(string[0] == 'r' && bMarker == 0 && (string[100] == ' ' || dMarker == 1
           || string[100] == '\n' || string[100] == '\r' || string[100] == '\t')){
            if(stock[0] != ' ' || numLims >= limitReset){
                if(numLims >= limitReset){
                    fprintf(stderr, "Maximum number of resets reached. ");
                }
                fprintf(stderr, "Move %d is illegal: %c\n", numMoves, string[0]);
                printState();
                exit(1);
            }
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
            numMoves++;
            numLims++;
        }
        else if(bMarker == 0 && string[0] != '#'){
            fprintf(stderr, "Formatting error on move %d!", numMoves);
            exit(1);
        }
    }
    }

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
    /*if(!strcmp(argv[0],"./check")){
        //Printing necessary output for Part 1
        printf("\nInput file is valid\n");
        printf("%d covered cards\n", coveredCount);
        printf("%d stock cards\n", stockCount);
        printf("%d waste cards\n", wasteCount);
        exit(0);
    }*/
    //Stop when EOF or moves limit is met
    while(!fileEnd && numMoves <= moves){
        skipGarbMoves(fp, input);
    }
    fprintf(stdout, "Processed %d moves, all valid\n", numMoves-1);
    printState();
}
