#include "symbolList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * function:    insertSymbol
 * description: create a new symbol and append to end of list
 * params:
 *      list    the symbolList to append to
 *      name    the symbolEntry name
 *      type    the symbolEntry type
 * returns:     the new list
 */
symbolList insertSymbol(symbolList list, char name[31], char type)
{
    // create new entry
    symbolEntry *new = (symbolEntry*) malloc(sizeof(symbolEntry));

    // exit on error
    if (new == 0)
    {
        perror("in symbolList - malloc unable to allocate space");
        exit(0);
    }

    // initialize new entry
    strcpy(new->name, name);
    new->type = type;
    new->next = END_OF_LIST;

    symbolEntry *cur = list;

    // special case when list is empty
    if (list == END_OF_LIST)
        list = new;
    else
    {
        // go to end of list
        while (cur->next != END_OF_LIST)
            cur = cur->next;
        cur->next = new;
    }

    return list;
}

/*
 * function:    updateSymbol
 * description: update a symbol in the list
 * params:
 *      list    the symbolList to update
 *      name    the symbolEntry name to look for
 *      type    the symbolEntry type to update to
 * returns:     void
 */
void updateSymbol(symbolList list, char name[31], char type)
{
    symbolEntry *cur = list;

    // traverse list
    while (cur != END_OF_LIST)
    {
        // found symbol?
        if (strcmp(cur->name, name) == 0)
        {
            // update entry and exit
            cur->type = type;
            return;
        }

        // symbol not found, keep going
        cur = cur->next;
    }
}

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
int findSymbol(symbolList list, char name[31], char *type)
{
    int count = 0;
    symbolEntry *cur = list;

    // traverse list
    while (cur != END_OF_LIST)
    {
        // symbol found?
        if (strcmp(cur->name, name) == 0)
        {
            // add to count and record found type
            count++;
            *type = cur->type;
        }
        cur = cur->next;
    }

    return count;
}

/*
 * function:    removeSymbol
 * description: remove a symbolEntry for a list
 * params:
 *      list    the symbolList to remove from
 *      name    the symbolEntry name to remove
 * returns:     the new list
 */
symbolList removeSymbol(symbolList list, char name[31])
{
    symbolEntry *cur = list;
    symbolEntry *next;

    // empty list?
    if (list == END_OF_LIST)
        return END_OF_LIST;

    // special case when removing from head
    if (strcmp(list->name, name) == 0)
    {
        free(cur);
        return list->next;
    }

    next = cur->next;

    // traverse list
    while (next != END_OF_LIST)
    {
        // symbol found?
        if (strcmp(next->name, name) == 0)
        {
            // remove from list and exit
            cur->next = next->next;
            free(next);
            break;
        }
        cur = next;
        next = cur->next;
    }

    return list;
}

/*
 * function:    printSymbols
 * description: print out all of the symbols in a list
 * params:
 *      list    the list to print from
 * returns:     void
 */
void printSymbols(symbolList list)
{
    symbolEntry *cur = list;

    // traverse list
    while (cur != END_OF_LIST)
    {
        printf("%-32s %c\n", cur->name, cur->type);
        cur = cur->next;
    }
}