#ifndef SYMBOLLIST_H
#define SYMBOLLIST_H
#define END_OF_LIST 0

typedef struct symbolEntry
{
    char type;
    char name[31];
    struct symbolEntry *next;
} symbolEntry;

/*
 * typedef for symbol lists, should be initialized
 * to END_OF_LIST.
 */
typedef symbolEntry* symbolList;

/*
 * function:    insertSymbol
 * description: create a new symbol and append to end of list
 * params:
 *      list    the symbolList to append to
 *      name    the symbolEntry name
 *      type    the symbolEntry type
 * returns:     the new list
 */
symbolList insertSymbol(symbolList list, char name[31], char type);

/*
 * function:    updateSymbol
 * description: update a symbol in the list
 * params:
 *      list    the symbolList to update
 *      name    the symbolEntry name to look for
 *      type    the symbolEntry type to update to
 * returns:     void
 */
void updateSymbol(symbolList list, char name[31], char type);

/*
 * function:    findSymbol
 * description: count the search matches in a list
 * params:
 *      list    the symbolList to search in
 *      name    the symbolEntry name to search for
 *      type    the symbolEntry type to return, this will be set to the last
 *              match found in the list
 * returns:     count of matches
 */
int findSymbol(symbolList list, char name[31], char *type);

/*
 * function:    removeSymbol
 * description: remove a symbolEntry for a list
 * params:
 *      list    the symbolList to remove from
 *      name    the symbolEntry name to remove
 * returns:     the new list
 */
 symbolList removeSymbol(symbolList list, char name[31]);

/*
 * function:    printSymbols
 * description: print out all of the symbols in a list
 * params:
 *      list    the list to print from
 * returns:     void
 */
void printSymbols(symbolList list);

#endif