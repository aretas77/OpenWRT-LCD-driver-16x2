#include "getter.h"


// Looks up a value according to the given config
// works like 'uci get example.example.enable' 
static void uci_lookup(const char* config, char* ip);

static void errorUCI(const char* error, char* text);

void getText(const char* config, char *text)
{
    uci_lookup(config, text);
}

void uci_lookup(const char* config, char* ip)
{
    char* tekstas = (char *) malloc(strlen(config));
    strcpy(tekstas, config);

    struct uci_ptr ptr;
    struct uci_context *c = uci_alloc_context();

    if (!c)
        printf("Couldn't create uci_context\n");

    /* 
		 * uci_lookup_ptr suranda config faile esancia 
		 * reiksme pagal path - jei randa, grazina UCI_OK
		 * ptr.o - uci option
		 * ptr.o->v - option reiksme
	*/
    if ((uci_lookup_ptr(c, &ptr, tekstas, true) != UCI_OK) 
        || (ptr.o == NULL) || (ptr.o->v.string == NULL))
    {
        printf("Can't find values. In %s\n", config);
        errorUCI("VALUE NULL", ip);
        uci_free_context(c);
        return;
    }
    
    if (ptr.flags & UCI_LOOKUP_COMPLETE)
        strcpy(ip, ptr.o->v.string);


    uci_free_context(c);
}

void errorUCI(const char* error, char* text)
{
    strcpy(text, error);
}

