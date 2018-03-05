#include "includes.h"

/*
    Functions to generate and store/retrieve user defined chars.
    
*/

/**
 * @brief returns a defined character from container 
 * using id
 * 
 * @param id name of the character 
 * @return uint8_t* returns an array of character 
 */
uint8_t* getCustomCharById(const char* id);

/**
 * @brief add custom chars to a container for later retrieval
 * 
 * @param ch the character to add
 * @param id the name of character
 */
void addCustomChars(uint8_t ch[8], const char* id);

/**
 * @brief print the list of all elements in list
 * 
 */
void printList();

/**
 * @brief checks the given text whether there exists a code to be interpreted and trims
 * the given text - subtracts the code from text
 * 
 * @param text the text to examine
 * @return char* returns extracted the name/id of code in text
 */
char* checkForCode(char* text);

