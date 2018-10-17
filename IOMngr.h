/* IOMngr.h

   Implements a simple interface for reading characters from
   the source file while producing a listing file. Functionality
   includes

   - opening source and listing files by name
   - closing source and listing files
   - getting a single character from the source file
   - post message to be included after a source line
   - get current line number
   - get current column number

   The implementation has two modes. If a listing file name is supplied to
   the open call then all source lines and any associated messages are written
   to the listing file. If the listing file name is NULL then stdout is used as
   the listing file and only lines that have associated messages are written.
*/
#ifndef IOMNGR_H
#define IOMNGR_H

#include <stdbool.h>

#define MAXLINE 1024

/* OpenFiles      Opens the two files given by the filenames. aSourceName is
                  required but aListingName may be NULL. If aListingName is
                  NULL output will be directed to stdout (the system output
                  stream). If the opens are successful this function returns
                  true, otherwise it returns false with neither file open.

   CloseFiles     Closes the two files and insures all posted messages have
                  been displayed.
*/
bool OpenFiles(const char * aSourceName, const char * aListingName);
void CloseFiles();

/* GetSourceChar  Returns the next character in the source file reading a line
                  from the input file if none are available. This function must
                  also arrange for source file lines to be echoed to the
                  listing file with a prepended line number. If aListingName
                  was not null then all source lines are written to the listing.
                  If aListingName was null then only lines that have posted
                  messages are written to the listing. As a consequence, source
                  lines must be read in their entirety and buffered. It is
                  only when the characters of a line have been exhausted, and
                  a new line must be read, that the line is considered for
                  printing to the listing file along with associated messages.
                  Any tab characters ('\t')found in the input buffer are
                  replaced by a single ' '. The EOF character is returned if
                  the source file is at the end of file when this function is
                  called.
*/
char GetSourceChar();

void WriteMessage();

/* PostMessage    Registers a message to be associated with the current line
                  and indicated by a marker in the indicated column. The
                  letters 'A' ... 'Z' are used as the markers, consequently,
                  a maximum of 26 message lines are allowed for each source
                  line. Calls to PostMessage beyond this limit are silently
                  ignored as are calls with aColumn outside the range of the
                  current line. When aLength is greater than 1, the marker
                  is followed by a sequence of '-' characters to fill out
                  the length. When multiple posts are associated with the
                  same column the letter mark for the last post is displayed
                  in that column.
*/
void PostMessage(int aColumn, int aLength, const char * aMessage);

/* GetCurrentLine Returns the current line number in the source file.

   GetCurrentColumn Returns the column number of the most recently returned
                  character.
*/
int GetCurrentLine();
int GetCurrentColumn();

#endif
