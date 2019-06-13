/* ScannerDriver.c

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ScanTokens.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"

#define MESSAGE_SIZE 256
#define ACTION_MESSAGE_SIZE 32

struct Attributes {
  struct Span span;
  int cnt;
};
enum AttrKinds { IGNORE_KIND };

// used with qsort to sort list of symbol table entries
int entryCmp(const void * A, const void * B) {
  // A is pointer to element of array which contains a pointer to a struct SymEntry
  const char * strA = GetName(*(struct SymEntry **)A);
  const char * strB = GetName(*(struct SymEntry **)B);
  return strcmp(strA,strB);
}

int main(int argc, char **argv) {
  int Token;
  char message[MESSAGE_SIZE];
  char actionMessage[ACTION_MESSAGE_SIZE];
  struct SymTab * table = NULL;

  bool ret = OpenSource("ScannerSource");
  if (!ret) {
    printf("Could not open source and listing files\n");
    exit(1);
  }

  int intSum = 0;
  float floatSum = 0;

  //yytext is the string of that token
  //yyleng is the length of that token
  while ((Token = yylex()) != 0) {
    snprintf(actionMessage,ACTION_MESSAGE_SIZE," ");
    switch(Token) {
      case INIT_TOK: {
        // create a symbol table
        if (!table) table = CreateSymTab(20,"main",NULL);
      } break;
      case IDENT_TOK: {
        // place the identifier in the table (if it exists), if new then create and init
        // attributes structure, if already in table then update attributes cnt field, in
        // each case set actionMessage with one of
        // sprintf(actionMessage," -- No SymbolTable");
        // sprintf(actionMessage," -- occurrence %d",attr->cnt);
         sprintf(actionMessage," -- new ident");
        if (!table) {
          snprintf(actionMessage,ACTION_MESSAGE_SIZE," -- No SymbolTable");
          break;
        } else {
          struct SymEntry * entry = LookupName(table,yytext);
          if(entry != NULL) { //was found in table..update attributes
            struct Attributes * attr = GetAttr(entry);
            attr->cnt++;
            sprintf(actionMessage," -- occurrence %d",attr->cnt);
          } else { //create a new entry and add to table...
            entry = EnterName(table, yytext);
            struct Attributes * attr = malloc(sizeof(struct Attributes));
            attr->span.first = GetLastPosition() - yyleng;
            attr->span.last = GetLastPosition();
            attr->cnt = 1;
            //OR
            SetAttr(entry,IGNORE_KIND,attr);
          }
        }
        //
        // code
        //not sure what this does here.... but need to fill in
        //
      } break;
      case INT_TOK: {
        // code
        //sum up
        intSum+= atoi(yytext);
      } break;
      case FLOAT_TOK: {
        // code
        //sum up
        floatSum+= atof(yytext);
      } break;
      case DUMP_TOK: {
        printf("---------\n");
        printf("intSum = %d\n",intSum);
        printf("floatSum = %f\n",floatSum);
        // get table statistics, alloc an array to hold entry pointers
        struct Stats * stats = Statistics(table);
        struct SymEntry ** entries = malloc(stats->entryCnt * sizeof(struct SymEntry *));
        memcpy(entries,GetEntries(table,false,NULL),stats->entryCnt * sizeof(struct SymEntry *));

        // sort the entries
        qsort(entries,stats->entryCnt,sizeof(struct SymEntry *),entryCmp);

        // list the contents of the table in sorted order
        printf("\nContents of Symbol Table\n");
        printf("                    name  strt  stop   cnt \n");
        for (int i = 0; i < stats->entryCnt; i++) {
          printf("%3d %20s %5d %5d %5d\n", i,
                 GetName(entries[i]),
                 ((struct Attributes *) GetAttr(entries[i]))->span.first,
                 ((struct Attributes *) GetAttr(entries[i]))->span.last,
                 ((struct Attributes *) GetAttr(entries[i]))->cnt);
        }
        free(stats);
      } break;
    }
    sprintf(message,"Token#=%d, Length=%lu, Text=\"%s\"%*c%s",Token,yyleng,yytext,(int)(15-yyleng),' ',actionMessage);
    //fprintf(stderr,"msg: %s\n",message);
    PostMessage(MakeSpanFromLength(tokenStartPosition,yyleng),message);
  }
  PostMessage(MakeEOFSpan(),"EOF");
  CloseSource();
  DestroySymTab(table);
}
/*

*/
/*my notes 2/18/19
no regular expression can handle a nested comment -->FSA's cannot handle nesting
need to make something that tells if you are inside of a comment or not yet
cannot make a regular expression to handle This
going to have regular expressions for counting
have counters for comments to increment and decrement
check if they are 0 and you will know if you are in a comment or not
NEED TO DO: define IN_COMMENT -- some expresison or condition that tells if you want to return
for // just need to look for end of line character (boolean) - new line makes it 0
can be nested inside of // -- check for that! CORNER CASE
2 properties: counters and boolean for checking end of line for // comments
only operators you need are concatenation by consecutive things or vertical bar and star and plus
other operator that might be useful is the ? (shows that token is optional)
need to use offset to see if the last char read was included in the token
1 means it was included, 0 means it was included
used to see where exactly that token is located*/
/* for any char in scanner.l
if (lineComment == 0 && blockComment == 0) RETURN(OTHER_TOK,1);
*/
