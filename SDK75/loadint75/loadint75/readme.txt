	LOADINT utility for IDA
	-----------------------

The LOADINT utility creates/updates the predefined comments file IDA.INT
Usage:

	LOADINT COMMENT.CMT IDA.INT

If the output file doesn't exist, LOADINT will create it, otherwise LOADINT
will update it. You may compress IDA.INT afterwards (optionally):

	IDACOMP IDA.INT

Input files are:

ALLINS	 HPP	- enumeration of instructions for all processors.
		  don't modify this file
COMMENT  CMT	- main comments file
*        CMT    - instruction comments for each processor

You may modify *.CMT files. Brief description of these files follows:

        The comments database is one big 'switch' instruction in the
        C-language sense. When IDA needs to find a comment for an instruction,
        it 'executes' this big switch and retrieves a comment.
        So, we have the following construction:

                switcher ? value1 : comment
                           value2 : comment

        For example,

                cmd ? NN_mov : "comment for 'move' instruction"


More strictly, in the BNF form (terminals UPPERCASE), alternatives on different lines:

        switch  :=  switcher ? cases
        cases   :=  case cases
                    case
        case    :=  values : comment
        values  :=  value
                    value || values
        value   :=  NUMBER
                    IDENT
                    KEYWORD_other
        comment :=  STRING
                    STRING { switch }                        (*)
                    { switch }

        i.e. switches may be used recursively.
        When the rule marked with (*) is used, the STRINGs of several
        switches are concatenated and form one comment. For example:

          idp ? idp_M65 : "6502 - " {
            cmd ? M65_adc : "comment for ADC"
          }

        Comment for ADC instruction will be "6502 - comment for ADC"

        The possible switchers are:

        idp             IDP internal number (see comment.cmt)
        filetype        Input file format   (see comment.cmt)
        auxpref         Processor dependent value
        cmd             Instruction         (see ins.hpp)
        op1..op6        Instruction operands
        ah,al,ax,bh     Intel 80x86 registers
        bl,bp,bx,ch     Intel 80x86 registers
        cl,cs,cx,dh     Intel 80x86 registers
        di,dl,ds,dx     Intel 80x86 registers
        es,si,sp,ss     Intel 80x86 registers (sorry for other processors)

        The possible cases are:

        a number                (c-notation)
        an ident                (the ident should be defined in enumeration)
        keyword "other"         anything else (default)

        You may combine several cases using || operator:

        cmd ? NN_mov || NN_push || NN_pop : "a comment for move,push,pop instructions"

        You may use several string constants instead of one string:

                "This is a big comment\n"
                "On several lines\n"
                "Formatted nicely"


Enumerations:

        To ease use of LOADINT there are enumerations in the language.
        Formal definition:

          enum  := KEYWORD_enum IDENT { enum_list };

        i.e. absolutely the same as in C.

        Please don't modify enumerations.


Extern:

        For compatibility with C LOADINT simply skips "extern" instructions.
        Just FYI.


        For examples of using this 'language' please take a look
        at *.CMT files :)

Again just FYI: LOADINT uses IDA.HLP file from the IDA distributive.
It looks for IDA.HLP in directories from PATH and NLSPATH env. variables.
----------------------------------------------------------
Acknowledgements
----------------

386EX additions in portout.cmt are by Paul te Bokkel (Paul.te.Bokkel@invorm.nl)

----------------------------------------------------------

That's all about LOADINT. Any questions, unclear things etc - please
tell us.

Hex-Rays, <support@hex-rays.com>
18.08.96
10.12.97 (updated)
13.03.99 (updated)
15.01.00 (updated)
11.01.01 (updated)
23.03.01 (updated)
30.12.01 (updated)
01.10.03 (updated)
05.02.07 (updated)
28.01.09 (updated)
17.12.09 (updated, added necv850.cmt)
11.04.11 (updated, added unix versions)
16.09.11 (updated, added 65816.cmt)
30.04.12 (updated, add m16c.cmt)
10.01.13 (updated)
04.06.14 (updated)
29.12.14 (updated)
