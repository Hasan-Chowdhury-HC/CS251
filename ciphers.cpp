#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!
char rot(char c, int amount);
string rot(const string& line, int amount);
void caesarEncryptCommand();

void rot(vector<string>& strings, int amount);
string clean(const string& s);
vector<string> splitBySpaces(const string& s);
string joinWithSpaces(const vector<string>& words);
int numWordsIn(const vector<string>& words, const vector<string>& dict);
void caesarDecryptCommand(const vector<string>& dict);

string applySubstCipher(const vector<char>& cipher, const string& s);
void applyRandSubstCipherCommand();

/**

 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  vector<string> dictionary;
  ifstream dictionaryFile("dictionary.txt");

  string word;
  while (getline(dictionaryFile, word)) {
    dictionary.push_back(word);
  }

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      caesarEncryptCommand();
    } else if (command == "D" || command == "d") {
      caesarDecryptCommand(dictionary);
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  int index = ALPHABET.find(c);  // Finds index of c
  int updatedIndex = (index + amount) %
                     ALPHABET.size();  // Updates according to rotation amount
                                       // while accounting for wrapping
  return ALPHABET[updatedIndex];       // Returns updated character
}

string rot(const string& line, int amount) {
  string result;

  for (char ch : line) {  // For every char in a line
    if (isalpha(ch)) {    // Check if its a letter
      char upperCh =
          toupper(ch);  // If yes, change to uppercase and append to result
      result += rot(upperCh, amount);
    } else if (isspace(ch)) {  // Append to result if it is a space/whitespace
      result += ch;
    }
  }
  return result;  // Returns result
}

void caesarEncryptCommand() {
  cout << "Enter the text to encrypt:";
  string userInputText;
  getline(cin, userInputText);  // Gets all of user inputted text

  cout << "Enter the number of characters to rotate by:";
  string userEncryptKey;
  getline(cin, userEncryptKey);  // Gets all of user inputted text

  int rotation = stoi(
      userEncryptKey);  // Assigns an integer after changing from string to int

  string encryptedText =
      rot(userInputText,
          rotation);  // Uses the rot function to encrypt the text using the key

  cout << "Encrypted Text: " << encryptedText
       << endl;  // Outputs encrypted text from function
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (string& word : strings) {  // Cycle through every word in the string
    string rotatedWord;
    for (char ch : word) {  // Cycle through every char in the word
      if (isalpha(ch)) {
        rotatedWord += rot(ch, amount);  // Append to rotatedWord after running
                                         // through the rot function
      } else {
        rotatedWord += ch;  // Appends non-characters/whitepsaces
      }
    }
    word = rotatedWord;  // Sets word to the rotated word
  }
}

string clean(const string& s) {
  string cleanedString;
  for (char ch : s) {  // For every character in the string, sets it to the
                       // uppercase version
    if (isalpha(ch)) {
      cleanedString += toupper(ch);
    }
  }
  return cleanedString;  // Returns fully upercased string
}

vector<string> splitBySpaces(const string& s) {
  vector<string> words;
  string currentSpace;

  for (char ch : s) {   // Cycle through every character in string
    if (isspace(ch)) {  // Checks if whitespace
      if (!currentSpace
               .empty()) {  // If not whitespace, append to words, then clear
        words.push_back(currentSpace);
        currentSpace.clear();
      }
    } else {
      currentSpace += ch;  // Append if whitespace
    }
  }

  if (!currentSpace.empty()) {  // Extra check for final whitespace or character
    words.push_back(currentSpace);
  }

  return words;  // Returns string vector of all words separated by spaces
}

string joinWithSpaces(const vector<string>& words) {
  string result;
  // Cycles through every word and adds a whitespace, except for the final word
  for (int i = 0; i < words.size(); i++) {
    if (i == words.size() - 1) {
      result += words[i];
    } else {
      result += words[i];
      result += " ";
    }
  }
  return result;  // Returns the new string that includes whitespaces
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int wordCount = 0;
  // Cycles through every word in dict and compares it to every word in user
  // string Increments for every matching word
  for (const string& word : words) {
    for (const string& dWord : dict) {
      if (word == dWord) {
        wordCount++;
        break;
      }
    }
  }
  return wordCount;  // Returns number of words that match with those in dict
}

void caesarDecryptCommand(const vector<string>& dict) {
  cout << "Enter the text to decrypt:";
  string textToDecypher;
  getline(cin, textToDecypher);  // Gets user text to decypher

  vector<string> words =
      splitBySpaces(textToDecypher);  // Splits user input into a vector to
                                      // remove all whitespaces

  for (string& word : words) {
    word = clean(word);  // Cleans all words, aka, makes them all fully
                         // uppercase
  }

  bool decrypted = false;  // initialize bool

  for (int i = 0; i < 26; i++) {  // Runs through all 26 different possible keys
    vector<string> attempts = words;
    rot(attempts, i);  // Rotates words according to the key (i)

    int numWords = numWordsIn(attempts, dict);  // Runs numWordsIn function
    if (numWords >
        (attempts.size() / 2)) {  // Checks if more than half the words are
                                  // actual words in dictionary
      decrypted =
          true;  // if true, set bool to true, then outputs decrypted line
      cout << joinWithSpaces(attempts) << endl;
    }
  }

  if (!decrypted) {  // If no good decryptions are found (none are greater than
                     // attempts.size()/1) output the following line
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;
  for (char ch : s) {              // For every character in s
    if (isalpha(ch)) {             // Checks if it is a character
      char upperCh = toupper(ch);  // Sets all characters to uppercase
      int index =
          upperCh -
          'A';  // Subtracting allows us to gain the index in the alphabet
      result += cipher[index];  // Append the ciphered character
    } else {
      result += ch;  // Append the non-character/whitespace
    }
  }
  return result;  // Returns the result with the substitutions
}

void applyRandSubstCipherCommand() {
  vector<char> rnd =
      genRandomSubstCipher();  // Gives us our randomized substitution

  cout << "Enter text: ";
  string userInputText;
  getline(cin, userInputText);  // Gets user input

  string encryptedText = applySubstCipher(
      rnd, userInputText);  // Encrypt the user text using the randomized
                            // substitution and user input

  cout << encryptedText << endl;  // Outputs encrypted text
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student
  return 0.0;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student
}

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  // TODO: student
  return vector<char>{};
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // TODO: student
  return vector<char>{};
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student
}

#pragma endregion SubstDec