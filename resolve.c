/* 
 * Name: <put programmer name(s) here>
 * Description: This program performs the resolution task of a linker.
 *              It takes as input a list of .o files and archive files
 *              and keeps track of undefined and defined symbols.  All
 *              symbols in a .o file are added to one of the two lists.
 *              Archived object files are handled a little differently.
 *              If there is a symbol that is currently undefined, but defined
 *              in a object file within an archive, then all symbols in that
 *              archived object file are added to one of the two lists; 
 *              otherwise, no symbols from that archive member are added.  
 *              Members of an archive are visited repeatedly until there are 
 *              no changes in the lists of defined and undefined symbols.
 *
 *              If at some point there is an attempt to add a second strong
 *              symbol to the defined symbols list, then an error message is
 *              displayed indicating the multiple definition.
 *
 *              At the end of the process, the program looks to make sure that
 *              the main function is defined and if not, an error message is
 *              displayed.  Next, the program prints out the entries of the 
 *              undefined symbols list with an error message for each one.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolList.h"
#include "bool.h"

static symbolList u_list = END_OF_LIST;
static symbolList d_list = END_OF_LIST;

static bool isObjectFile(char *filename);
static bool isArchive(char *filename);
static void handleObjectFile(char *filename);
static void handleArchive(char *filename);
static bool processFile(char *filename, bool test_archive);
static void processSymbol(char name[31], char type);
static bool symbolCausesChange(char name[31], char type);
static void printUndefinedErrors();
static void printDefinedList();
static void displayErrorAndExit(char *message);
static void systemCommand(char *command);

int main(int argc, char *argv[])
{
    int i, istat; 
    struct stat stFileInfo;

    if (argc <= 1)
    {
       printf("resolve: no input files\n");
       exit(1);
    }
    for (i = 1; i < argc; i++)
    {
        istat = stat(argv[i], &stFileInfo);
        //if istat is 0 then file exists
        if (istat == 0)
        {
            if (!isObjectFile(argv[i]) && !isArchive(argv[i]))
            {
                printf("%s: file not recognized\n", argv[i]);
            } else {
                if (isArchive(argv[i])) handleArchive(argv[i]);
                if (isObjectFile(argv[i])) handleObjectFile(argv[i]);
            }
        } else {
            printf("%s: file not found\n", argv[i]);
        }
    }
    printUndefinedErrors();
    printDefinedList();
}

/*
 * function:    handleObjectFile
 * description: handles processing an object file
 * params:
 *      filename    the object file's relative path
 * returns:     void
 */
void handleObjectFile(char *filename)
{
    processFile(filename, false);
}

/*
 * function:    handleArchive
 * description: handles processing an archive file
 * params:
 *      filename: the archive file's relative path
 * returns:     void
 */
void handleArchive(char *filename)
{
    char command[150];
    char obj_name[80];
    char *buffer;
    bool changes = true;
    FILE *fp;

    // use .tmp/ prefix for extracted object file names
    strcpy(obj_name, ".tmp/");
    buffer = &obj_name[5];

    // remove .tmp/ if it exists and recreate it, ensuring that it is empty
    // then, copy in the archive file, extract it, and remove the copied file
    sprintf(command, "rm -r -f .tmp; mkdir .tmp; cp %s .tmp/__a; cd .tmp; ar -x __a; rm -f __a;", filename);
    systemCommand(command);

    // continued processing extracted object files until their symbols no
    // long cause changes
    while (changes)
    {
        changes = false;
        fp = popen("cd .tmp; ls -1", "r");
        if (fp == NULL) displayErrorAndExit("popen failed");

        while (fgets(buffer, sizeof(buffer), fp))
        {
            // if this object file will cause a change, process it like normal
            if (processFile(obj_name, true))
            {
                changes = true;
                processFile(obj_name, false);
            }
        }

        pclose(fp);
    }

    // cleanup extracted files
    systemCommand("rm -r -f .tmp");
}

/* 
 * function:    isObjectFile
 * description: This function takes as input a c-string and returns
 *              true if the c-string ends with a .o extension.
 * params:
 *      filename: the object file's relative path 
 * returns:     1 or 0
 */
bool isObjectFile(char *filename)
{
    int len = strlen(filename);
    if (len < 3) 
        return false;
    if (filename[len - 2] != '.')
        return false;
    if (filename[len-1] != 'o')
        return false;
    return true;
}

/* 
 * function:    isArchive
 * description: This function takes as input a c-string and returns
 *              true if the c-string ends with a .a extension.
 * params:
 *      filename: the archive file's relative path
 * returns:     1 or 0
 */
bool isArchive(char *filename)
{
    int len = strlen(filename);
    if (len < 3) 
        return false;
    if (filename[len - 2] != '.')
        return false;
    if (filename[len-1] != 'a')
        return false;
    return true;
}

/*
 * function:    processFile
 * description: process the symbols in an object file and either update
 *              the U and D lists or determine if this file would cause
 *              changes to U or D
 * params:
 *      filename: the object file's relative path
 *      test_archive: if true then only test if this file causes changes
 *                    to U or D, otherwise process like normal
 * returns:     true if causes changes, false otherwise (or if test_archive
 *              is false)
 */
bool processFile(char *filename, bool test_archive)
{
    char command[80];
    char buffer[80];
    char name[31];
    char type;
    char found;
    bool caused_change = false;
    FILE *fp;

    // use nm to get this file's symbols
    snprintf(command, sizeof(command), "nm %s", filename);

    // open a pipe to proccess
    fp = popen(command, "r");
    if (fp == NULL) displayErrorAndExit("popen failed");

    // go line by line of nm output
    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (sscanf(buffer, "%*x %c %s\n", &type, name) == 0)
            sscanf(buffer, " %c %s\n", &type, name);

        if (!test_archive)
            processSymbol(name, type);
        else if (!caused_change)
            caused_change = symbolCausesChange(name, type);
    }
    pclose(fp);

    return test_archive && caused_change;
}

/*
 * function:    processSymbol
 * description: process a symbol and change U and/or D lists as needed
 * params:
 *      name: the symbol's name
 *      type: the symbol's type
 * returns:     void
 */
void processSymbol(char name[31], char type)
{
    char d_type = ' ';
    char u_type = ' ';
    char local_name[31];

    static int local_n = 0;

    int in_d = findSymbol(d_list, name, &d_type);
    int in_u = findSymbol(u_list, name, &u_type);

    switch (type)
    {
    case 'U':
        if (!in_d && !in_u)
            u_list = insertSymbol(u_list, name, type);
        break;
    case 'T':
    case 'D':
        if (in_d)
        {
            if (d_type == 'T' || d_type == 'D')
                printf(": multiple definition of %s\n", name);
            if (d_type == 'C')
                updateSymbol(d_list, name, type);
        }
        else if (in_u)
        {
            u_list = removeSymbol(u_list, name);
            d_list = insertSymbol(d_list, name, type);
        }
        else if (!in_d)
        {
            d_list = insertSymbol(d_list, name, type);
        }
        break;
    case 'C':
        if (!in_d)
        {
            d_list = insertSymbol(d_list, name, type);
        }
        if (in_u)
        {
            u_list = removeSymbol(u_list, name);
        }
        break;
    case 'b':
    case 'd':
        sprintf(local_name, "%s.%d", name, local_n);
        d_list = insertSymbol(d_list, local_name, type);
        local_n++;
        break;
    }
}

/*
 * function:    symbolCausesChanges
 * description: test if a symbol would change U or D lists
 * params:
 *      name: the symbol's name
 *      type: the symbol's name
 * returns:     true or false
 */
bool symbolCausesChange(char name[31], char type)
{
    char found;

    // strong globals can change undefined symbols of the same name
    if (findSymbol(u_list, name, &found))
        return type == 'D' || type == 'T';

    // any symbol can change COMMON symbols
    if (findSymbol(d_list, name, &found))
        return found == 'C';

    return false;
}

/*
 * function:    printUndefinedErrors
 * description: print an error for each symbol in U list
 * returns:     void
 */
void printUndefinedErrors()
{
    char c;
    if (!findSymbol(d_list, "main", &c))
        printf(": undefined reference to main\n");

    symbolEntry *cur = u_list;

    while (cur != END_OF_LIST)
    {
        printf(": undefined reference to %s\n", cur->name);
        cur = cur->next;
    }
}

/*
 * function:    printDefinedList
 * description: print all the symbols in D list
 * returns:     void
 */
void printDefinedList()
{
    printf("Defined Symbol Table\n");
    printf("-----------------------\n");
    printSymbols(d_list);
}

/*
 * function:    displayErrorAndExit
 * description: displays an error message and exits
 * returns:     void
 */
void displayErrorAndExit(char *message)
{
    printf("Error: %s\n", message);
    exit(0);
}

/*
 * function:    systemCommand
 * description: perform a system command and handle errors
 * params:
 *      command: the command to run
 * return:      void
 */
void systemCommand(char *command)
{
    if (system(command) == -1)
    {
        printf("Error: failed system call %s\n", command);
        exit(0);
    }
}