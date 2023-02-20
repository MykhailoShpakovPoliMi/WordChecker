#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct TrieNode{
    char key;
    bool alive;
    struct TrieNode* nextSibling;
    struct TrieNode* child;
}TrieNode;

//the length of a string
int k;

//save chars that gave '+' in certain position
char* mandatory_chars;

//save whether the character was tried at certain position
//without success, gave '/' or '|'
//the rows are from 0 to k referring to each position in string
//the columns are referring to chars via the hash_char() func
//values: 1 - char is forbidden, 0 - otherwise
bool (*forbidden_chars)[64];

//save minimal and maximal number of char
//to process compatible words
//64 rows refer to each character
//1st column - #occurrences in r
//2nd column - #of maximal trials of this char in any p (
//3rd column - #of current trials of this char, during words filtering
unsigned int count_table[64][3];

//string used for +stampa_filtrate function
char* output_filtered;


short int hash_char(char c){
    if (c == '-')
        return 0;
    if ('0' <= c && c <= '9')
        return (short int) (c - 48 + 1);
    if ('A' <= c && c <= 'Z')
        return (short int) (c - 65 + 11);
    if (c == '_')
        return 37;
    if ('a' <= c && c <= 'z')
        return (short int) (c - 97 + 38);
    else
        return -1;
}

char read_command(){
    char command[17];
    char c;
    unsigned short int i = 0;

    c = (char) getchar();
    for(i = 0; c != '\n'; i++) {
        //no command can have more than 16 characters
        //or command contains not allowed character
        if(i>15 || hash_char(c) < 0){
            printf("Not allowed character");
            exit(0);
        }

        command[i] = c;
        c = (char) getchar();
    }

    //no command can have less than 12 characters
    //exit from for after reading newline \n
    if(i < 13){
        printf("Not allowed character");
        exit(0);
    }

    command[i] = '\0';

    if(strcmp(command,"nuova_partita") == 0)
        return '1';
    if(strcmp(command,"inserisci_inizio") == 0)
        return '2';
    if(strcmp(command,"inserisci_fine") == 0)
        return '3';
    if(strcmp(command,"stampa_filtrate") == 0)
        return '4';
    else{
        printf("Not allowed character");
        exit(0);
    }

}

//reads k characters from stdin and stores them in string passed as arg
//returns '0' if the word has been read correctly
//returns the code of the command in case a command was given via stdin
//+nuova_partita - '1'
//+inserisci_inizio - '2'
//+inserisci_fine - '3'
//+stampa_filtrate - '4'
//returns '5' in case unrecognized word was given in stdin
char read_string(char word[]){
    char c;
    for(int i = 0; i < k; i++){
        c = (char) getchar();

        //if the first char is +, then maybe it`s a command
        //check it
        if(c == '+' && i == 0)
            return read_command();

        //if we read not allowed character, abort the program
        if(hash_char(c)<0) {
            printf("Not allowed character");
            exit(0);
        }
        word[i] = c;
    }
    //the next char should be the line breaker
    if(getchar() == '\n') {
        word[k] = '\0';
        //return '0' if the word has been successfully read
        return '0';
    }
    else{
        printf("Not allowed character");
        exit(0);
    }
}

//creates node with all values 0, except bool alive
//since all newly created nodes must be alive ( acceptable for restrictions )
TrieNode* create_node(){
    TrieNode* node = (TrieNode*) calloc(1,sizeof(TrieNode));
    if(node == NULL){
        exit(0);
    }
    node->alive = true;
    return node;
}

void insert_in_trie(TrieNode* currentNode, char word[]){

    bool childFound;
    TrieNode* currentChild;
    TrieNode* leftSibling;

    for (int i = 0; i < k ; i++) {

        if(currentNode->child == NULL){
            //create new child for the current currentNode
            currentNode->child = create_node();

            //assign current character as a key to a newly created currentNode
            currentNode->child->key = word[i];
            currentNode = currentNode->child;
        }
        else{
            //examine all siblings of currentNode to find this specific character
            //siblings are ordered in ASCII order
            currentChild = currentNode->child;
            childFound = false;
            leftSibling = NULL;

            while (!childFound){
                if(currentChild->key == word[i]){
                    //pass to this child containing requested character
                    currentNode = currentChild;
                    childFound = true;
                }
                else if(currentChild->key > word[i]){
                    //create child node on the left from currentChild
                    //then assign character to it
                    //and make appropriate connections based on whether
                    //left sibling exist,
                    if(leftSibling != NULL){
                        leftSibling -> nextSibling = create_node();
                        leftSibling -> nextSibling -> key = word[i];
                        leftSibling -> nextSibling -> nextSibling = currentChild;
                        currentNode = (leftSibling -> nextSibling);
                    }
                    else{
                        currentNode -> child = create_node();
                        currentNode -> child -> key = word[i];
                        currentNode -> child -> nextSibling = currentChild;
                        currentNode = currentNode -> child;
                    }

                    childFound = true;
                }
                else if(currentChild -> key < word[i]){
                    //move forward to examine further siblings of the same parent

                    if(currentChild->nextSibling != NULL){
                        leftSibling = currentChild;
                        currentChild = currentChild -> nextSibling;
                    }
                    else{
                        //create child on the right, rightmost child
                        currentChild->nextSibling = create_node();
                        currentChild->nextSibling->key = word[i];
                        currentNode = currentChild->nextSibling;
                        childFound = true;
                    }
                }
            }

        }
    }

}

//returns pointer to the leaf node if the word is contained in the dictionary
//returns NULL otherwise
TrieNode* check_in_trie(TrieNode* currentNode, char word[]){
    bool childFound;

    for (int i = 0; i < k; i++) {
        //examine all children of currentNode to find this specific character
        //siblings are ordered in ASCII order
        childFound = false;
        currentNode = currentNode->child;

        while (!childFound){
            //word has more characters than trie can contain ( k as the length )
            //no words have been yet inserted in the trie
            //or there`s no child with requested character
            if(currentNode == NULL){
                return NULL;
            }
            else if(currentNode -> key == word[i]){
                //there`s child with right character
                childFound = true;
            }
            else if(currentNode -> key > word[i]){
                //the desired character does not exist inside the list of children
                //since siblings are ordered in ASCII order
                //so the word does not exist in the trie
                return NULL;
            }
            else if(currentNode -> key < word[i]){
                //move forward to examine further siblings of the same parent
                //to find the desired character
                currentNode = currentNode -> nextSibling;
            }
        }
    }

    return currentNode;
}

//recursive function that prints compatible words if parameter print = true
//position goes from 0 to k
//returns number of compatible words
//currentNode must accept first child of the root
unsigned int print_filtered(TrieNode* currentNode, unsigned short int position, bool print){

    //if I arrived till the end without interruption, then print this word and return 1 to count it
    if(currentNode==NULL && position == k){
        if(print) {
            printf("%s\n", output_filtered);

        }

        return 1;
    }

    if(currentNode != NULL) {

        //increment current num of instances of this char in current word
        count_table[hash_char(currentNode->key)][2]++;

        //control for mandatory char
        if (currentNode->alive && mandatory_chars[position] != '\0') {
            //mandatory char does not coincide
            if (currentNode->key != mandatory_chars[position]) {
                currentNode->alive = false;
            }
        }
            //control for forbidden char
        else if (currentNode->alive && forbidden_chars[position][hash_char(currentNode->key)]) {
            currentNode->alive = false;
        }

        //control that exact num of symbols is not exceeded
        //control only the chars that have been tried at least once, so I know the exact num of this char,
        //includes also the case of character that does not exist in r but was tried once
        if(currentNode->alive){
            //if this char has ever been tried and user has discovered the exact num of chars
            if(count_table[hash_char(currentNode->key)][1] > count_table[hash_char(currentNode->key)][0])
            {
                //control that now it does not exceed the exact num of instances of this char
                if(count_table[hash_char(currentNode->key)][2] > count_table[hash_char(currentNode->key)][0]){
                    currentNode->alive = false;
                }
            }
        }

        //control min, max num of instances of all 64 chars
        //verify that there is enough space to satisfy all restrictions
        if (currentNode->alive) {
            unsigned int count_req = 0;

            for (unsigned short int i = 0; i < 64 && currentNode->alive; ++i) {

                //TODO if execution is slow, substitute i with hash_char(currentNode->key) and control only at the last node
                //know only min number of instances of this
                if (count_table[i][0] >= count_table[i][1]) {
                    //sum num of characters that must yet fit into remained portion of the string
                    //sum only those chars whose minimal requirements were not fully satisfied
                    if(count_table[i][1] > count_table[i][2])
                        count_req += (count_table[i][1] - count_table[i][2]);
                }
                    //know the exact num of instances in r
                else {
                    //sum num of exact symbols yet to satisfy
                    if(count_table[i][0] > count_table[i][2])
                        count_req += (count_table[i][0] - count_table[i][2]);
                }
                //count_req is the number of chars that must yet fit
                //into remained portion of the string to satisfy min, max requirements
                if (count_req > k - 1 - position) {
                    currentNode->alive = false;
                }
            }
        }

        unsigned int count_tmp = 0;

        //if the node is compatible after comparison of all restrictions
        //go to his child to search the words
        if(currentNode->alive) {
            if(print)
                output_filtered[position] = currentNode->key;

            //move on to the next node
            //and count num of words coming from the next node
            count_tmp = print_filtered(currentNode->child, position + 1, print);
        }

        //remove my contribution into count_table
        count_table[hash_char(currentNode->key)][2]--;

        //pass to the next sibling in order to print words in ASCII order
        //position in output_word does not change
        //sum number of all the words that come from siblings in order to
        //return it to your common father
        count_tmp += print_filtered(currentNode->nextSibling, position, print);

        return count_tmp;
    }

    //return 0 only if I reached the rightmost sibling and there is no siblings
    //to count words anymore
    return 0;
}

//cleans up the memory from leafs to the root
void free_trie(TrieNode* root){

    if(root != NULL){
        free_trie(root->child);
        free_trie(root->nextSibling);
    }
    else{
        return;
    }

    free(root);
}

//sets alive = true to all nodes that had alive = false
void restore_trie(TrieNode* root){

    if(root != NULL){
        restore_trie(root->child);
        restore_trie(root->nextSibling);
    }
    else{
        return;
    }
    if(! (root->alive) )
        root->alive = true;
}

void close_program(TrieNode* root){
    free_trie(root);
    free(output_filtered);
    free(forbidden_chars);
    free(mandatory_chars);
    exit(0);
}

int main(){

    //declare root of prefix trie
    TrieNode* root = create_node();

    //if memory allocation didn't go well
    if(root == NULL) {
        printf("The memory allocation did not go well. Bye!");
        exit(0);
    }

    TrieNode* r_leaf = NULL;
    TrieNode* p_leaf = NULL;

    //number of words to compare with r word
    int n;

    bool newRoundStarted;

    printf("\nHello\nWelcome to the game\nPlease insert the length of the words that you want to play with");

    if(scanf("%d", &k))
        getchar();

    if(k<=0){
        printf("\nSorry, the length can be negative. Good bye!");
        free_trie(root);
        free(forbidden_chars);
        free(mandatory_chars);
        exit(0);
    }

    char r[k+1];
    char p[k+1];

    //use this string for +stampa_filtrate
    output_filtered = (char*) calloc(1, sizeof(char)*(k+1));

    //initialize forbidden chars, mandatory chars vector and set to 0
    forbidden_chars = (bool (*)[64])calloc(k,sizeof *forbidden_chars);
    mandatory_chars = (char*) calloc(k,sizeof(char));

    if(forbidden_chars == NULL || mandatory_chars == NULL || output_filtered == NULL)
        close_program(root);

    output_filtered[k] = '\0';

    newRoundStarted = false;

    printf("\nNow insert the words that you want to add to your collection, each separated by line breaker\n"
           "If you want to start the round then type +nuova_partita");
    while (!newRoundStarted) {
        switch (read_string(p)) {
            case '0':
                //the word read successfully
                insert_in_trie(root, p);
                break;
            case '1':
                //start of the round was requested
                newRoundStarted = true;
                break;
            default:
                close_program(root);
        }
    }

    while(true) {
        printf("\nThe new round started !\n");

        //turn all nodes back to be alive as new round starts
        restore_trie(root);

        memset(count_table, 0, 64 * 3 * sizeof(unsigned int));
        memset(mandatory_chars, 0, k*sizeof(char));
        memset(forbidden_chars,0,k*64*sizeof(bool));

        printf("\nPlease, insert the guessed word among those you`ve inserted into your collection\n");
        //read reference word r
        if (read_string(r) != '0') {
            close_program(root);
        }
        //get the leaf corresponding to this word
        r_leaf = check_in_trie(root, r);

        //fill 1st column of occurrence table with characters from r
        for (int i = 0; i < k; ++i) {
            count_table[hash_char(r[i])][0]++;
        }

        printf("\nPlease, insert the maximal number of trials that player is allowed to take");
        //read n, max number of words to compare
        if(scanf("%d", &n))
            getchar();

        if (n <= 0) {
            printf("\nSorry, number of trials must be positive");
            close_program(root);
        }

        //start the game comparisons
        while (newRoundStarted) {

            printf("\nInsert trial word: \n");
            switch (read_string(p)) {
                case '0':
                    p_leaf = check_in_trie(root, p);
                    //word read successfully
                    if (p_leaf) {

                        //if the leafs are the same then r and p are identical
                        if(r_leaf == p_leaf){
                            //print ok without control and restrictions
                            putchar('o');
                            putchar('k');
                            putchar('\n');
                            newRoundStarted = false;
                            printf("\nYou won this round!\n");
                        }
                            //do the comparison and restrictions savings
                        else{

                            printf("\nThe result of letter by letter comparison is this\n");

                            //1st iteration, '+' update in count_table
                            for (int i = 0; i < k; ++i) {
                                if(r[i] == p[i]){
                                    mandatory_chars[i] = p[i];
                                    count_table[hash_char(p[i])][2]++;
                                }
                            }

                            //2nd iteration '/' and '|' update in count_table
                            for (int i = 0; i < k; ++i) {
                                if(r[i] != p[i]){
                                    //'|' instance of this character is on another position
                                    if(count_table[hash_char(p[i])][2] < count_table[hash_char(p[i])][0]){
                                        putchar('|');

                                    }
                                        //'/' num of instances of this character is run out
                                    else{
                                        putchar('/');

                                    }
                                    count_table[hash_char(p[i])][2]++;

                                    //update forbidden_chars as a result of '/' or '|'
                                    forbidden_chars[i][hash_char(p[i])] = true;
                                }
                                else{
                                    //characters coincide
                                    putchar('+');

                                }

                                //update max num of trials of this char if needed
                                if(count_table[hash_char(p[i])][2] > count_table[hash_char(p[i])][1]) {
                                    count_table[hash_char(p[i])][1] = count_table[hash_char(p[i])][2];
                                }
                            }

                            //reset current number of trials of chars
                            for (int i = 0; i < 64; ++i) {
                                count_table[i][2] = 0;
                            }

                            printf("\n%u\n", print_filtered(root->child, 0, false));

                        }

                        //count only the words that belong to the dictionary
                        n--;
                    }
                    else{
                        printf("not_exists\n");
                    }

                    break;
                case '2':
                    //do +inserisci_inizio ... +inserisci_fine
                    while (read_string(p) != '3') {
                        //read words to add to the dictionary trie
                        //the correctness of the word is guaranteed by read_string() func
                        insert_in_trie(root, p);
                    }

                    break;
                case '4':
                    //do +stampa_filtrate
                    printf("\nThe words that remained compatible according to the acquired restrictions are these:\n");
                    print_filtered(root->child,0, true);

                    break;
                default:
                    //error command
                    printf("\nThis command is not allowed in this phase! Goodbye!");
                    close_program(root);
            }

            //the user run out of trials and word was not guessed
            if(n <= 0 && newRoundStarted) {
                newRoundStarted = false;
                putchar('k');
                putchar('o');
                putchar('\n');

                printf("\nYou lost this round!");
            }
        }

        //insert new words and wait for +nuova_partita command
        while (!newRoundStarted) {
            printf("\nYou can start a new round by typing +nuova_partita or you can insert new words by "
                   "typing +inserisci_inizio and +inserisci_fine at the end\n");
            switch (read_string(p)) {
                case '1':
                    //start of the round was requested
                    newRoundStarted = true;
                    break;
                case '2':
                    while (read_string(p) != '3') {
                        //read words to add to the dictionary trie
                        //the correctness of the word is guaranteed by read_string() func
                        insert_in_trie(root, p);
                    }
                    break;
                default:
                    printf("\nThis command is not allowed in this phase! Goodbye!");
                    close_program(root);
            }
        }
    }

    return 0;
}