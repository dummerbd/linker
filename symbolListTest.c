#include "symbolList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* test data: */
char *NAME_1 = "first";
char TYPE_1 = '1';
char *NAME_2 = "second";
char TYPE_2 = '2';
char *NAME_3 = "third";
char TYPE_3 = '3';

void assertTrue(int cond, char *msg)
{
    if (!cond)
    {
        printf("error: %s\n", msg);
        exit(0);
    }
}

void testInsertIntoEmptyList()
{
    printf("test insert into empty list...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);

    assertTrue(list != END_OF_LIST,
        "list should not be null after insert");

    assertTrue(list->next == END_OF_LIST,
        "end of list should be null");

    assertTrue(strcmp(list->name, NAME_1) == 0,
        "name at index 0 does not match inserted name");

    assertTrue(list->type == TYPE_1,
        "type at index 0 does not match inserted type");

    printf("passed\n");
}

void testInsertIntoNonEmptyList()
{
    printf("test insert into non-empty list...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_2, TYPE_2);

    assertTrue(list != END_OF_LIST,
        "list should not be null after insert");

    assertTrue(list->next->next == END_OF_LIST,
        "end of list should be null");

    assertTrue(strcmp(list->next->name, NAME_2) == 0,
        "name at index 1 does not match inserted name");

    assertTrue(list->next->type == TYPE_2,
        "type at index 1 does not match inserted type");

    printf("passed\n");
}

void testRemoveFromEmptyList()
{
    printf("test remove from empty list...\n");

    symbolList list = END_OF_LIST;

    // shouldn't cause seg fault
    list = removeSymbol(list, "");

    assertTrue(list == END_OF_LIST,
        "list should be empty");

    printf("passed\n");
}

void testRemoveFromSingleItemList()
{
    printf("test remove from single item list...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);

    list = removeSymbol(list, NAME_1);

    assertTrue(list == END_OF_LIST,
        "list should be empty");

    printf("passed\n");
}

void testRemoveFromHeadOfList()
{
    printf("test remove from head of list...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_2, TYPE_2);

    list = removeSymbol(list, NAME_1);

    assertTrue(list != END_OF_LIST,
        "list should not be null");

    assertTrue(list->next == END_OF_LIST,
        "end of list should be null");

    assertTrue(strcmp(list->name, NAME_2) == 0,
        "name at index 0 does not match inserted name");

    assertTrue(list->type == TYPE_2,
        "type at index 0 does not match inserted type");

    printf("passed\n");
}

void testRemoveFromMiddleOfList()
{
    printf("test remove from middle of list...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_2, TYPE_2);
    list = insertSymbol(list, NAME_3, TYPE_3);

    list = removeSymbol(list, NAME_2);

    assertTrue(list != END_OF_LIST,
        "list should not be null");

    assertTrue(list->next->next == END_OF_LIST,
        "end of list should be null");

    assertTrue(strcmp(list->name, NAME_1) == 0,
        "name at index 0 does not match inserted name");

    assertTrue(list->type == TYPE_1,
        "type at index 0 does not match inserted type");

    assertTrue(strcmp(list->next->name, NAME_3) == 0,
        "name at index 1 does not match inserted name");

    assertTrue(list->next->type == TYPE_3,
        "type at index 1 does not match inserted type");

    printf("passed\n");
}

void testUpdateSymbol()
{
    printf("test update symbol...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_2, TYPE_2);

    updateSymbol(list, NAME_2, TYPE_3);

    assertTrue(list->type == TYPE_1,
        "type at index 0 should not have changed");

    assertTrue(list->next->type == TYPE_3,
        "type at index 1 should have updated");

    printf("passed\n");
}

void testFindSymbol()
{
    printf("test find symbol...\n");

    symbolList list = END_OF_LIST;

    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_2, TYPE_2);
    list = insertSymbol(list, NAME_1, TYPE_1);
    list = insertSymbol(list, NAME_3, TYPE_3);
    list = insertSymbol(list, NAME_1, TYPE_2);

    char type = ' ';
    int count = findSymbol(list, NAME_1, &type);

    assertTrue(count > 0,
        "find should have found a match");

    assertTrue(count == 3,
        "find should have found multiple matches");

    assertTrue(type == TYPE_2,
        "find should have returned last type found");

    printf("passed\n");
}

int main(int argc, char *argv)
{
    printf("Starting symbol list tests...\n");

    testInsertIntoEmptyList();
    testInsertIntoNonEmptyList();

    testRemoveFromEmptyList();
    testRemoveFromSingleItemList();
    testRemoveFromHeadOfList();
    testRemoveFromMiddleOfList();

    testUpdateSymbol();
    testFindSymbol();
}