
 FLAIR -- Fast Library Acquisition for Identification and Recognition
 ====================================================================

FLAIR utilities allow you to create your own signature files from
OBJECT or LIBRARY files for IDA Pro v3.8 or higher.

FLAIR consists of the following executables:

plb	        parselib  processes OMF  libraries and creates PAT file
pcf             parsecoff processes COFF libraries and creates PAT file
pelf            parseelf  processes ELF  libraries and creates PAT file
ppsx            parsepsx  processes PSX  libraries and creates PAT file (Sony Playstation)
ptmobj          parsetobj processes Trimedia libraries .... .... ....
sigmake         sigmake takes PAT files as input and creates SIG file
zipsig          zipsig compresses and uncompresses SIG files
dumpsig         dumpsig dumps contents of SIG file in a text form.

Typical scenario of a signature creation is:
	- run a parser and create pattern (PAT) files
	- run sigmake and get EXC file with collisions
	- edit EXC file and resolve collisions
	- run sigmake again and get SIG file
	- repeat the above 2 steps till no collisions exist
	- run zipsig and get compressed SIG file

A SIMPLE EXAMPLE
================

Suppose we have got a library named SAMPLE.LIB and want to make a
signature from it. If SAMPLE.LIB is an OMF library, the following will
do the job.

Only two commands:

        >PLB     SAMPLE.LIB     SAMPLE.PAT
        >SIGMAKE SAMPLE.PAT     SAMPLE.SIG

Yes, that's all!

After these two commands we get either a signature file or a
collision file. If we get a signature file - great, that's what we
wanted.  Otherwise we need to deal with collisions. The collision
file will be named SAMPLE.EXC. If we do not want to examine
collisions then the quickest method is to delete the comments at the
start of the collisions file and run sigmake again. After the second
run of sigmake we will get a signature file. We can compress the
resulting signature file with zipsig to save the disk space.

If SAMPLE.LIB is an AR/COFF library, then we need to run PCF instead
of PLB.  If you are not sure about the format of your library, just
try to run both utilities (plb/pcf). If the input library has a wrong
format, they will clearly indicate it.

Of course this method of resolving collisions is not the best method.
If you want to get a truly good signature file, you need to go
through the collisions file and examine each collision closely,
deciding what to do with it. More about collisions is in SIGMAKE.TXT
file.

HOW TO USE THE CREATED SIGNATURE
================================

First of all, copy your signature file into SIG subdirectory of IDA.
If your signature is for a processor different from IBM PC, then create
a special subdirectory for your signature. The name of the subdirectory
should be equal to the name of the processor module file. For example,
all signature files for the C166 processor should be in SIG\C166.
Launch IDA.
In IDA, open the signatures window and press Insert. Select your
signature from the list and press Enter. IDA will eventually apply
your signature to the input file.


ADDING COMMENTS TO FUNCTIONS
============================

If you want to add comments to library functions, you can do that.
All you need is to create a special file with the comments to
the functions. This file will have an IDS format. So you will need
to download the utilities to work with IDS files.
Just put the IDS file into IDS\FLIRT subdirectory of IDA and IDA
will automatically use it.


STARTUP SIGNATURES
==================

If you want your signature to be applied automatically then you need to
create a startup signature.
Creation of startup signature files is slightly different.
You need to have all pattern files for all compilers in order to create
startup signature files. I've put all files needed to create startup
signatures in the STARTUP directory.

To make your signature apply automatically you need to create startup
patterns, then copy them to the STARTUP directory and run startup.bat.
Please note the naming convention of startup patterns: EXE file patterns
have EXE*.PAT names etc.


PASCAL AND DELPHI SUPPORT
=========================

Nick Pisanov courteously provided us with the utilities to create
signatures from TPU files. See PASCAL subdirectory for the details.

MISC
====

About ZIPSIG utility: this utility allows you to compress the signature
files. The compressed signature files occupy less disk space but it takes
more time to load them into IDA. This utility understands wildcards in
the input file names.

I've put some examples of command files and EXC files in EXAMPLE directory.

For information about utilities please read files
        PLB.TXT
        PCF.TXT
        SIGMAKE.TXT

For questions: <support@hex-rays.com>
