/* Author:      Nick Williams
   Created:     02/22.19
   Resources:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "IOMngr.h"

int sourceFD;
char * source;
size_t sourceByteCnt;
char * sourceLastChar;
char * nextChar;        // address of next char to return
int curLine; //increment this each time new line symbol encountered
int nextMessageNumber; //increment upon each message added...
int nextEOFSpan;

int numChars;

struct Message {
  struct Span span;
  int seqNumber;
  char * message;
  struct Message * next;
};

struct Message * Messages;

//only called in this class
struct Message *
MakeMessage(struct Span span, int seq, const char * message) {
  struct Message * msg = malloc(sizeof(struct Message));
  msg->span = span;
  msg->seqNumber = seq;
  msg->message = strdup(message);
  msg->next = NULL;
  return msg;
}
//make insertMessage method that utilizes a
//check messageSpan method inside to see if the message is accpeted
//unless postMessage method does all of this..

/*
method used to freeing nodes that are not at the head of the list
*/
void
FreeMessageInList(struct Message * msgToFree) {
  struct Message * temp = Messages;
  while(temp->next != msgToFree) {
    temp = temp->next;
  }
  //temp will be the message before the one being freed
  temp->next = msgToFree->next; //relink...
  if(msgToFree->message != NULL) {
    free(msgToFree->message);
  }
  free(msgToFree);
}

void
FreeHeadMessage() {
  struct Message * headMessage = NULL;
  struct Message * next = NULL;
  if(Messages != NULL) {
    headMessage = Messages;
    if(headMessage->message != NULL) {
      headMessage->message = NULL;
      free(headMessage->message);
    }
    if(headMessage->next != NULL) { //need to re-link the head message
      //printf("DEBUG: starting to free head message -- error could happen here\n");
      //nextMessage = malloc(sizeof(struct Message));
      next = headMessage->next;
      Messages = next;//reset the head of messages
      headMessage->next = NULL;
      headMessage = NULL;
      free(headMessage);
      //Messages = next; //switch position to after free?
      //printf("DEBUG: succesfully (maybe?) deleted headMessage\n");
    } else { //deleting last message in list
      headMessage = NULL;
      free(headMessage);
      Messages = NULL;
    }
  }
}

bool
isLineBreak(char * c) {
  return *c == '\n' || *c == '\r' || c == sourceLastChar;
}

// ansi escape sequences for colors
char * colorSeqs[] = {"\033[91m","\033[92m","\033[93m","\033[94m","\033[95m","\033[96m"};

void
OutputMarkStart(struct Message * msg) {
  if(msg != NULL) {
    fprintf(stdout,"%s",colorSeqs[(msg->seqNumber % 6)]);
  } else {
    fprintf(stdout,"\033[0m");
  }

}

void
OutputMarkStop() {
  fprintf(stdout,"\033[0m");
}

void
OutputInterval(char * start, char * stop) {
  if (stop < start) return;
  fwrite(start,stop - start + 1,1,stdout);
}

void
AddMessageAfter(struct Message * curMsg) {
  struct Message * newMsg = MakeMessage(curMsg->span, curMsg->seqNumber, curMsg->message);
  struct Message * temp = curMsg->next;
  curMsg->next = newMsg;
  newMsg->next = temp;
}
/*
  Use this to print all the messages up until curMsg
  Only for messages that are being printed under the source messages
*/
void
OutputMessagesBefore(struct Message * curMsg) {
  struct Message * temp = Messages;
  while(temp != curMsg) {
    OutputMarkStart(temp);
    printf("\t** ");
    OutputMarkStop();
    printf("%s\n", temp->message);
    temp = temp->next;
    FreeHeadMessage();
  }
}

void
OutputSource() {

  int count =0; //
  int numLines=1;
  int sizeOfMessage = 0; //could be used for looping through the message...
  int spansToBePrinted = 0; //dont need
  char curChar = ' ';
  char * theMsg = NULL;

  struct Message * curMsg = Messages;
  struct Span curSpan;

  bool spanDone = false;
  bool endOfLine = false;
  //bool highlightOn = false;
  /*
  use the Message structs to print the lines after a new line is encountered
  //there may be some lines where there aren't any spans encountered
  use the spansToBePrinted var or combo of hte booleans to cehck?
  */
  /*while(curMsg!= NULL) {
    printf("curMsg is: %s\n", curMsg->message);
    curMsg = curMsg->next;
  }
  return;*/
  printf("%6d: ", numLines);
  while(count < numChars) { //loop through lines..
    curChar = source[count];
    if(curMsg != NULL) {
      curSpan = curMsg->span;
      theMsg = curMsg->message;
      sizeOfMessage = curSpan.last - curSpan.first; //shouldn't need?
    }
    if(curChar == '\n') { //new line... check if there are other spans to print
      //spans will either be done for this line or continue onto another?
      printf("%c",curChar);

      if(spansToBePrinted > 0) {
        OutputMessagesBefore(curMsg);
        spansToBePrinted = 0;
      }
      //need to check if span is done or goes to another line..
      if(count < curSpan.last && count >= curSpan.first) { //span continues to next line -- print ** after that line is finished
        spansToBePrinted++;
        OutputMarkStop();
        spanDone = false;
      }
      numLines++;
      endOfLine = true;
    }//else if -- this should work without else if, if logic correct

    if(count == curSpan.first) { //turn on high lighting..
      OutputMarkStart(curMsg);
    }

    if(endOfLine == true && count < numChars-1) {
      OutputMarkStop();
      printf("%6d: ", numLines);
      endOfLine = false;
    } else if (count < numChars-1){ //for last line/output being correct
      printf("%c",curChar);
    }
    if(spanDone == false && endOfLine == true) {//span continued to next line needs to keep being highlihghted
      OutputMarkStart(curMsg);
    }
    if(count == curSpan.last) { //turn off highlighting..
      OutputMarkStop();
      //and set curMsg to curMSg->next
      curMsg = NULL; //at last message in list.. could free?
      //used to be curMsg = curMsg->next;
      spansToBePrinted++;
      spanDone = true;
    }
    count++;
  }
  //loop through EOF
  if(Messages != NULL) {
    curMsg = Messages;
    while(curMsg != NULL) {
      OutputMarkStart(curMsg);
      printf("\t** ");
      OutputMarkStop();
      printf("%s\n", curMsg->message);
      curMsg = curMsg->next;
      FreeHeadMessage();
    }
  }
}

bool
OpenSource(const char * aFilename) {
  sourceFD = open(aFilename,O_RDONLY);
  if (sourceFD < 0) return false;
  struct stat buf;
  if (fstat(sourceFD,&buf)) return false;
  sourceByteCnt = buf.st_size;
  source = mmap(NULL,sourceByteCnt,PROT_READ,MAP_PRIVATE,sourceFD,0);
  //
  //
  //
  numChars = 0;
  nextEOFSpan = sourceByteCnt; //init to 0? not sure if need...
  nextMessageNumber = 0;

  nextChar = source;
  sourceLastChar = source + sourceByteCnt -1;
  return true;
}

//close source when done (from outputsource function)
void
CloseSource() {
  //call outputSource from here
  //read the rest of file if closing on unkown token
  while(numChars < sourceByteCnt ) {
    numChars++;
  }
  int retVal = 0;
  OutputSource();

  retVal = munmap(source, sourceByteCnt);

  if(retVal != 0) {
    //throw an error...
    //printf("DEBUG: could not unMap\n");
  }
  retVal = close(sourceFD);
  if(retVal != 0) {
    //throw an error...
    //printf("DEBUG: Could not close sourceFD\n");
  }
  if(sourceLastChar != NULL) {
    //free(sourceLastChar);
  }
  if(nextChar != NULL) {
    //free(nextChar);
  }
  if(Messages != NULL) {//messages are freed in freeHeadMessage
    //free(Messages);
  }

  //printf("DEBUG: Sucessfully closed file\n");
}

//returns chars
int
GetSourceChar() {
  if(nextChar > sourceLastChar) {
    //printf("DEBUG: ran out of source at %d of %zu == EOF\n", numChars, sourceByteCnt);
    return EOF;
  }
  char temp = source[numChars];
  numChars++;
  nextChar = &source[numChars];

  return temp;
}

bool
PostMessage(struct Span span, const char * aMessage) {
   bool wasAccepted = true;

   if(nextEOFSpan==sourceByteCnt  && ValidMessageSpan(span) == false) {
     //printf("NOT A VALID MESSAGE SPAN FOR: %d to %d\n",span.first, span.last);
     //printf("FOR THE MSG: %s\n", aMessage);
     return false;
   } else {
     //create the new Message struct and add it to the appropriate place in the list
     struct Message * msg = MakeMessage(span, nextMessageNumber, aMessage);
     struct Message * temp = Messages;
     if(nextMessageNumber == 0) { //first message being inserted
       Messages = msg; //set the head of the list
       //printf("first message created with message %s\n", aMessage);
     } else {
       //adds to end of list to be printed --
       while(temp->next != NULL) { //loop until you can add it to the end
         temp = temp->next;
       }
       //temp->next is null so add it here
       temp->next = msg;
       //printf("Message# %d created with message: %s\n", nextMessageNumber, aMessage);
       //printf("this message spans from %d to %d\n", span.first, span.last);
     }
     nextMessageNumber++;
   }
   return wasAccepted;
}

bool
ValidMessageSpan(struct Span span) {
  bool isGood = true;
  int begin,end = 0;
  begin = span.first;
  end = span.last;

  struct Message * curMsg = Messages;
  /*if(nextEOFSpan > 0) { //adding EOF's so need to return true
    return true;
  }*/
  while(curMsg != NULL){//check the span does not overlap other messages
    if(curMsg->span.first <= begin && curMsg->span.last >= begin) {
      isGood = false;//curMsg starts before the span and
      //ends somewhere after or equal to the beginning of the span
    } else if (curMsg->span.first <= end && curMsg->span.last >= end) {
      isGood = false;//curMsg starts before the end of the span and
      //ends after or equal to the end of the span
    } else if (curMsg->span.first >= begin && curMsg->span.last <= end) {
      isGood = false;
    }
    if(isGood == false) {
      //printf("the message conflicts with: %s from %d to %d\n", curMsg->message,curMsg->span.first, curMsg->span.last);
      return false;
    }
    curMsg = curMsg->next;
  }

  return isGood;
}

int
GetLastPosition() {
  return nextChar - 1 - source;
}

struct Span
MakeSpan(int first, int last) {
  struct Span span = {first, last};
  return span;
}

struct Span
MakeSpanFromLength(int start, int length) {
  struct Span span = {start,start + length - 1};
  return span;
}

struct Span
MakeEOFSpan() {
  //printf("adding eof\n");
  struct Span span = {nextEOFSpan,nextEOFSpan};
  nextEOFSpan++;
  return span;
}
