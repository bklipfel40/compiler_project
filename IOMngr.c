#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IOMngr.h"

#define LISTINGMARGIN 6

FILE *sourceFile, *listingFile;
char buffer[MAXLINE];
int bufLen, nextPos;
int curLine;
bool needDisplay;

#define MAX_MESSAGES 26
char * indicatorLine;
char * messages[MAX_MESSAGES];
int messageCnt;

/*
* Ben Klipfel
* 10/03/2017
* Resources: https://www.cprogramming.com/tutorial/cfileio.html
*
*/
bool OpenFiles(const char * aSourceName,
               const char * aListingName){
    //make sure aSourceName exists
    if( aSourceName == NULL ){
      return false;
    }
    else{
      sourceFile = fopen(aSourceName, "r");
    }
    //if aListingName is NULL we print to stdout
    if( aListingName == NULL ){
      listingFile = stdout;
    }
    else{
      listingFile = fopen(aListingName, "w");
    }
    //we have successfully opened the file
    return true;

}

void CloseFiles() {
  //make sure all messages have been displayed
  //needDisplay = false;
  WriteMessage();
  //close the sourceFile
  if(sourceFile){
    fclose(sourceFile);
  }
  //close the listingFile
  if(listingFile){
    fclose(listingFile);
  }
  //we need to free everything we malloc
  if(indicatorLine){
    free(indicatorLine);
  }

}

char GetSourceChar() {
  //make sure we have completely read in the buffer
  if( nextPos >= bufLen ){
    WriteMessage();
    //read a line from the stream making sure it isn't longer than MAXLINE
    //check to see if there's a line left, store it in the buffer
    if( fgets(buffer, MAXLINE, sourceFile) ){
      needDisplay = true;
      bufLen = strlen(buffer);
      //DEBUG:: fprintf(stderr,"buflen %d\n",bufLen);
      //reset the position and advance the current line
      nextPos=0;
      curLine++;
    }
    else{
      //if we no longer read a line than we are at the end of our file
      //needDisplay = false;
      nextPos=0;
      curLine++;
      //indicatorLine = NULL;
      //messageCnt++;
      //clear the buffer
      buffer[0] = '\0';
      return EOF;
    }
  }
  //replace tabs with ' '
  if( buffer[nextPos] =='\t' ){
    buffer[nextPos] = ' ';
  }
  return buffer[nextPos++];
}

void WriteMessage() {
  //see if we need to dislay the message
  if(needDisplay){
    if(indicatorLine){
      //make the indicator line a null terminated ending
      indicatorLine[nextPos+1]='\0';
    }
    //print the messages if we have a message count
    if( messageCnt > 0 && buffer[0] != '\0'){
      fprintf(listingFile,"%4d: %s", curLine, buffer);
      fprintf(listingFile,"     %s\n", indicatorLine);
    }
    //handle the message if the listingfile is not to stdout
    else if( listingFile != stdout){
      fprintf(listingFile, "%4d: %s", curLine, buffer);
    }
    //show what we have found
    for( int i = 0; i < messageCnt; i++ ){
      fprintf(listingFile, "   -%c %s\n", 'A'+i,messages[i]);
      messages[i] = NULL;
    }
    //reset the variables and nullify indicatorLine
    messageCnt = 0;
    bufLen = 0;
    indicatorLine = NULL;
  }
}
void PostMessage(int aColumn, int aLength, const char * aMessage) {
  //silently ignore if the messageCnt exceeps max messages
  if( messageCnt > MAX_MESSAGES ){
    return;
  }
  if( !indicatorLine ){
    //allocate room for null character
    indicatorLine = malloc(MAXLINE+1);
    //make the string empty
    memset(indicatorLine, ' ', MAXLINE);
    indicatorLine[MAXLINE] = '\0';
  }
  //put aMessage in messages array at index messageCnt
  messages[messageCnt] = strdup( aMessage );
  //make sure aColumn isn't beyond MAXLINE
  if( aColumn < MAXLINE && aColumn >= 0){
    //get the correct A-Z indicator
    indicatorLine[aColumn]='A'+messageCnt;
    aColumn++;
    for( int i = 1; i < aLength; i++ ){
      indicatorLine[aColumn] = '-';
      aColumn++;
    }
  }
  messageCnt++;
}

int GetCurrentLine() {
  return curLine;
}

int GetCurrentColumn() {
  return nextPos;
}
