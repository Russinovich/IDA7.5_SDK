                                                 Version 6.3 from 30-04-2012

Revision history:
 1.0 - first release
 2.0 - a bug in the PE DLL parser is fixed
 3.0 - standard zip lib is used to compress ids files
       ar2idt utility is added
 5.1 - Exit keyword is added
 6.1 - Linux/Mac versions have been added
 6.3 - IDA can locate .idt files inside .zip files

----------------------------------------------------------------------------
        IDS UTILITIES
----------------------------------------------------------------------------

  This is a collection of utilities to create/maintain IDS files for IDA Pro.

  The IDS files are used automatically by IDA to assign meaningful names,
  comments, and additional information to the imported DLL functions.

  There are 3 utilties:

        DLL2IDT         creates an IDT file from a DLL file
                        The IDT file is a text file. Its format is
                        described below.

        AR2IDT          creates an IDT file from an AR library file

        ZIPIDS          compresses an IDT file and creates an IDS file.
                        (can be also used to uncompress an IDS file and get an IDT file)
                        This program displays lines on the screen like:

                File: <filename> {api's napi [ndr/nar/ncm]}     packed

                        where
                          filename - name of input file
                          napi     - the number of the entry points
                          ndr      - the number of the "Drops" keywords
                          nar      - the number of the "Typeinfo" or "Args"
                                        (if Args != Drops)
                          ncm      - the number of the "Comment" or "Rptcmt" keywords

  The IDS file should be placed in the proper
  subdirectory of the IDS directory. For the moment, the
  following subdirectories exist:

        EPOC    - Simbian EPOC
        EPOC6   - Simbian EPOC v6
        FLIRT   - FLIRT accompanying IDS files
        GEOS    - GeOS
        LINUX   - Linux file
        NETWARE - Novell Netware files
        OS2     - OS/2 files
        WIN     - MS Windows, Win95, Win NT files
        WINCE   - MS Windows CE

  Each of these subdirectories can have subdirectories named after the
  processor module files. For example, the WINCE directory has several
  subdirectories named ARM, MIPS, PC, SH3.

  If the IDS file is used by several operating systems
  then you can put it in the IDS directory itself.

  There is a file named "idsnames" in the IDS directory. It is used to
  map IDS file names if the DLL name doesn't conform to the MS DOS filename
  conventions.

----------------------------------------------------------------------------
        EXAMPLE
----------------------------------------------------------------------------

   Let's create an IDS file for FONTEXT.DLL file from Win'95.
   The following commands will suffice:

        dll2idt c:\windows\system\fontext.dll
        zipids fontext

   Now we need to copy it to the IDS\WIN directory:

        copy fontext.ids \ida\ids\win

   Please note that it is not necessary to create IDS files for the system DLLs
   because IDA can use the DLLs directly if it finds them on the disk.

   If you want to specify additional information about the entry points
   (like comments, information about arguments, etc), then you may
   edit the .IDT file before running the ZIPIDS utility.


----------------------------------------------------------------------------
        FORMAT OF IDT FILES
----------------------------------------------------------------------------

The IDT file may begin with auxillary lines. The auxillary lines
may be in any order. For the moment 2 auxillary lines are defined:

DECLARATION <string>
          <string> - any text information about the file. This string is
          not used by IDA. You may put here the operating system,
          the author, version, etc. Default value: empty string.

ALIGNMENT <num>
          num - 2 or 4. (default is 2)
          This value is used by ZIPIDS to check values of Args/Drops/Pascal
          keywords. All those values must be divisible by the alignment.

1.   All spaces and tabulations are ignored everywhere but in comments.
1.a) All tabulations are replaced by spaces. Tabulation size is 8 positions.
1.b) The leading whitespace is ignored even in comments.
1.c) The continuation lines are allowed. If a line is ended by '\' (backslash)
     then the next line is a continuation line.
1.d) The comment keyword should be the last keyword on the line. The comment
     is ended by the end of the line.
1.e) The length of string values should not exceed 255 bytes.
1.f) All characters with codes less than space (' ') are not permitted.
     (exception: '\t','\n','\r')
1.g) '\n' is ignored. '\r' denotes the end of a line.

2.   If the first non-space character in the line is ';' then this
     line is not used to build IDS file. (this line is a comment line)
2.a) The comment line cannot have continuation lines (by '\')

3.   All other lines should start with a decimal number - this is a number
     of exported entry.
3.a) If the line starts with number 0, then it is a module description line.
3.b) If the module description line is absent, then the module description
     is set to
     <idt-filename>.dll
3.c) Exported entry numbers are in range 1 - (2^32-1).
3.d) The numbers may be in any order, the sorting will be done automatically.

4.   After the exported entry number there are keywords. A line must have at
     least one keyword.
4.a) Keywords are separated by whitespaces.
4.b) A keyword has the following form:
        keyword=value
     where the keyword may be truncated up to one character.
     (whitespaces before and after '=' are allowed)
     The keyword is case-insensitive.
4.c) The comment keyword must be the last keyword on the line
4.d) The allowed range of keyword values is 0-0xFFFFFFFE
     NOTE: in the module description line (this line starts with number 0)
           the only allowed keywords are Name and Comment.

5.   The keywords:
5.a) Name     - name of entry point                                [string]
5.b) Args     - number of bytes occupied by entry point arguments  [number]
5.c) Drops    - number of bytes purged from the stack upon return  [number]
                NOTE: this value should be equal or less than Args.
                NOTE: it is not allowed to specify Args=0 if Drops
                      is not specified. In this case you should use
                      Pascal=0
5.d) Pascal   - the same as Args=Drops=                            [number]
                NOTE: Pascal cannot be used together with Args
                      or Drops.
5.e) Typeinfo - entry point function prototype (type of input/output
                arguments). Resevred for the future.               [string]
5.f) Comment  - a comment for the entry point                      [string]
5.g) Rptcmt   - use comment from the specified entry point         [number]
                NOTE: if both Comment and Rptcmt are used, then
                      IDA will display both comments: first the Comment
                      is displayed, a space and Rptcmt after.
                NOTE: Rptcmt cannot point to another Rptcmt.
                      Also, Rptcmt cannot refer to entry point
                      without a Comment.
5.h) Exit     - function does not return to the caller             [no value]

6. Formatting comments
6.a) In order to enable multiline comments '\n' is interpreted by IDA as
     the start of a new line
6.b) '\\' is replaced by a single backslash ('\')
6.c) The leading comments in the continuation lines are ignored.

7. Limitations
7.a)  numeric value -1 (0xFFFFFFFF) is not allowed
7.b)  empty string value is not allowed

============================================================================
