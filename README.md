
## Introduction

This is an advanced version of the popular game named [<b>Wordle</b>](https://en.wikipedia.org/wiki/Wordle).
It controls the correspondence between 2 words: one guessed by user and another one supplied in an attempt to guess it.

<img width="40%" height="auto" align="left" alt="Screen-Shot-2022-01-30-at-10 05 09-PM" src="https://user-images.githubusercontent.com/100211796/220171060-f7dbb4d4-ed9a-4ec5-9b75-e167d25be0c0.png">

The goal is to insert the correct word without exceeding maximal number of trials.

The aim of the project was to choose the proper data structures that would be optimal in terms of **space complexity** and to design an algorithm that would be optimal in terms of **time complexity** to guarantee fast processing while dealing with large amounts of data.

## Gameplay
The length of a word as well as the collection of allowed words is set by user at the beginning of the game.

As soon as user types `+nuova_partita` the new round gets started.

* Firstly user picks one word from the collection of allowed words. Such word will be the guessed word.
* Then user sets the maximal number of trials that he/she should fit into while guessing the word.
* After that user inserts the sequence of words trying to guess the guessed word. After each word **letter by letter** comparison takes place.

As an outcome of letter by letter comparison, program indicates which letter of an attempt word stands on the <i>correct position</i> printing the symbol `+`, which one is contained inside the guessed word but stands on <i>another position</i>, printing the symbol `|`, and which one is <i>not contained</i> inside the guessed word printing the symbol `/`. Notice that <b>multiple instances</b> of the same character are considered as <b>separate characters</b> when producing the result.

Along with letter by letter comparison, program also provides the number of allowed words that are still compatible with the constraints acquired up to this moment of the game.

After each comparison user can decide whether to
* insert the next word in an attempt to match the guessed word
* insert `+stampa_filtrate` to check all the words that remained compatible with the restrictions acquired up until this moment of the game
* insert `+inserisci_inizio` ... `+inserisci_fine` that encloses the set of new words to add to the collection of allowed words

If the user inserts the word that does not belong to the collection of allowed word, program prints `+not_exists`

If the user exceedes the maximal number of guesses, the round finishes. User can add new words with `+inserisci_inizio` ... `+inserisci_fine` or start a new round by typing `+nuova_partita`.

## Project implementation

As a data structure I opted for **prefix trie** since it allows to conserve space for the words that have common prefixes. Moreover, choosing prefix trie guarantees that search time complexity will not depend on the number of words but only on the length of the word which results in more scalability while dealing with large amount of data and faster execution. As new words get added to the collection, new child nodes can eventually appear at any of the existing node. Child nodes are sorted in ASCII order to guarantee faster letter search when checking the word in the trie.

<img src="https://user-images.githubusercontent.com/100211796/220166902-d575f7c8-9363-4d6a-895d-a4d0db27d974.png" width="50%" height="auto" align="right" />

Dynamic matrixes and vectors were used to save restrictions derived from each comparison. These data objects occupy memory according to the linear function of the word lenght. 


## How to laucnh the game ( example for windows )

1.Install gcc on your computer

Download it on (https://gcc.gnu.org/) and install it following the instructions.

Then you can check if the installation was done by typing this command into command prompt
```bash
gcc -v
```

2.Download the source code 

Download file _word_checker.c_ from the main branch of this repository

3.Compile it

Go to the directory on your computer where the source file is located.
Then type into command prompt.
```bash
gcc -o word_checker word_checker.c
```

4.Execute

Now the c code is compiled and you can execute it by typing
```bash
word_checker
```
Make sure you are in the same directory as the compiled code file.

Follow the instructions on the command prompt and enjoy the game !

## To be continued

Further updates will include graphic interface. Thank you for reading !
Stay tuned!

