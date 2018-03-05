#include <inttypes.h>
#include "chGenerator.h"

// Linked list for custom chars
static struct node {
    char *id;
    uint8_t customChar[8];

    struct character *next;
};


struct node *head = NULL;
struct node *current = NULL;

uint8_t* getCustomCharById(const char* id)
{
    struct node* ptr = head;
    
    while(ptr != NULL)
    {
        int cmp = strcmp(ptr->id, id);
        //printf("\n%s | %s\n", ptr->id, id);
        if(cmp == 0) return ptr->customChar;
        ptr = ptr->next;
    }

    return;
}

/**
 * @brief print to the command line
 * 
 */
void printList()
{
    struct node* ptr = head;
    printf("Start\n");

    while(ptr != NULL)
    {
        printf("ID: %s.", ptr->id);
        ptr = ptr->next;
    }
    printf("\n");
}

void addCustomChars(uint8_t ch[8], const char* id)
{
    struct node *character = (struct node*) malloc(sizeof(struct node));

    character->id = (char*) malloc(strlen(id));
    strcpy(character->id, id);

    unsigned i = 0;
    for(i = 0; i < 8; i++) character->customChar[i] = ch[i];  

    character->next = head;
    head = character;
}

// Currently only one code per string is supported
char* checkForCode(char* text)
{
    // new array to keep the extracted string (without any code)
    char newStr[strlen(text)];

    //const char* text = "kodas {code:smiley} tekstas";

    // change this line for default code to check, example {cd: or {c:.
    // needs to start with { and end with :
    const char* code = "{code:";

    int from = 0;
    int to = 0;
    // Searches for first coded occurence
    char* found = strstr(text, code);

    // If there is no code, return NULL
    if(found == NULL) return text;

    // calculates 
    to = strlen(text) - strlen(found);
    
    // j - for iteration through new string
    unsigned i = 0, j = 0;
    for(i = 0; i < to; i++)
    {
        newStr[i] = text[i];
        j++;
    }
    
    // Search for : in code declaration
    char* found1 = strstr(text, ":");

    int k = 0;
    // Calculates the length of character code
    int codeEnd = 0;
    i = 0;
    for(i = 0; i < strlen(found1); i++)
    {
        if(found1[i] == '}')
        {
            codeEnd = 1;
            break;
        }
        k++;
    }

    if(codeEnd == 0) return NULL;
    // Copy the character code
    char* foundCode = (char *) malloc(k);
    strncpy(foundCode, found1, k);
    foundCode[k] = '\0';

    //printf("\nCode %s.\n", foundCode);

    // Eliminates the ":"" and " " to get pure code
    foundCode = strtok(foundCode, ":");
    foundCode = strtok(foundCode, " ");

    //printf("\nAfetr Code %s.\n", foundCode);

    // Calculates length from after the code to end of string
    from = strlen(foundCode) + 2;
    to = strlen(found1);

    // char for identifying the place for custom char
    newStr[j] = '`';
    j++;

    // Copies the text after the code
    for(i = from; i < to; i++)
    {
        //printf("%c", found1[i]);  
        newStr[j] = found1[i];
        j++;
    }
    // Need to add ending in new string
    newStr[j] = '\0';

    // Copy the extracted new string to the passed string
    strcpy(text, newStr);

    // Return the code for custom character
    return foundCode;
}