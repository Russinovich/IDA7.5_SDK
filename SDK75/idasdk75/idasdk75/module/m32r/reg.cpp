
#include "m32r.hpp"
#include <ieee.h>
static int data_id;

// m32r register names
static const char *const RegNames[] =
{
  "R0", "R1", "R2", "R3", "R4",
  "R5", "R6", "R7", "R8", "R9",
  "R10", "R11", "R12", "R13", "R14", "R15",
  "CR0", "CR1", "CR2", "CR3", "CR6",
  "PC",
  "A0", "A1",
  "CR4", "CR5", "CR7", "CR8", "CR9",
  "CR10", "CR11", "CR12", "CR13", "CR14", "CR15",
  "cs", "ds" // required by IDA kernel
};
static const char *const RegNames_alias[] =
{
  "R0", "R1", "R2", "R3", "R4",
  "R5", "R6", "R7", "R8", "R9",
  "R10", "R11", "R12", "fp", "lr", "sp",
  "psw", "cbr", "spi", "spu", "bpc",
  "PC",
  "A0", "A1",
  "CR4", "CR5", "fpsr", "CR8", "CR9",
  "CR10", "CR11", "CR12", "CR13", "CR14", "CR15",
  "cs", "ds" // required by IDA kernel
};

static char const cfgname[] = "m32r.cfg";

void m32r_iohandler_t::get_cfg_filename(char *buf, size_t bufsize)
{
  qstrncpy(buf, cfgname, bufsize);
}

//----------------------------------------------------------------------------
static int idaapi choose_device(int, form_actions_t &fa)
{
  m32r_t &pm = *(m32r_t *)fa.get_ud();
  if ( choose_ioport_device(&pm.ioh.device, cfgname) )
    pm.ioh.set_device_name(pm.ioh.device.c_str(), IORESP_NONE);
  return 0;
}

//----------------------------------------------------------------------------
// create the netnode helper and fetch idpflags value
uint32 m32r_t::refresh_idpflags(void)
{
  idpflags = (uint32)helper.altval(-1);
  return idpflags;
}

//----------------------------------------------------------------------------
// patch the RegNames[] array according to the use_reg_aliases parameter.
void m32r_t::patch_regnames(void)
{
  if ( use_reg_aliases() )
    ph.reg_names = RegNames_alias;
  else
    ph.reg_names = RegNames;
}

//----------------------------------------------------------------------------
// This function (called when opening the module related configuration in
// the general options) will create a dialog box asking the end-user if he
// wants to use synthetic instructions and register aliases.
const char *m32r_t::set_idp_options(
        const char *keyword,
        int /*value_type*/,
        const void * /*value*/,
        bool idb_loaded)
{
  short opt_subs = 0;

  if ( keyword != NULL )
    return IDPOPT_BADKEY;

  if ( use_synthetic_insn() )
    opt_subs |= 1;
  if ( use_reg_aliases() )
    opt_subs |= 2;

  static const char form[] =
    "HELP\n"
    "Mitsubishi 32-Bit (m32r) related options :\n"
    "\n"
    " Use synthetic instructions\n"
    "\n"
    "       If this option is on, IDA will simplify instructions and replace\n"
    "       them by synthetic pseudo-instructions.\n"
    "\n"
    "       For example,\n"
    "\n"
    "           bc     label1            ; 8 bits offset    \n"
    "           bc     label2            ; 24 bits offset   \n"
    "           ldi    r1, #0xF              \n"
    "           ldi    r2, #0x123456         \n"
    "           st     r3, @-sp                             \n"
    "           ld     r4, @sp+                             \n"
    "\n"
    "       will be replaced by\n"
    "\n"
    "           bc.s   label1             \n"
    "           bc.l   label2             \n"
    "           ldi8   r1, #0xF           \n"
    "           ldi24  r2, #0x123456      \n"
    "           push   r3                 \n"
    "           pop    r4                 \n"
    "\n"
    " Use registers aliases\n"
    "\n"
    "       If checked, IDA will use aliases names for the following registers :\n"
    "\n"
    "           r13     -> fp          \n"
    "           r14     -> lr          \n"
    "           r15     -> sp          \n"
    "           cr0     -> psw         \n"
    "           cr1     -> cbr         \n"
    "           cr2     -> spi         \n"
    "           cr3     -> spu         \n"
    "           cr6     -> bpc         \n"
    "\n"
    "ENDHELP\n"
    "m32r related options\n"
    "%*\n"
    "<##Substitutions"
    "#For example, use bc.s instead of 8-Bit bc instructions#Use ~s~ynthetic instructions:C>"
    "<#For example, use fp instead or r14#Use registers ~a~liases:C>>\n\n\n\n"
    "<~C~hoose device name:B:0::>"
    "\n\n\n";

  ask_form(form, this, &opt_subs, choose_device);

  idpflags = 0;    // reset the configuration
  if ( opt_subs & 1 )
    idpflags |= NETNODE_USE_INSN_SYNTHETIC;
  if ( opt_subs & 2 )
    idpflags |= NETNODE_USE_REG_ALIASES;

  if ( idb_loaded )
    save_idpflags();
  patch_regnames();
  return IDPOPT_OK;
}

// returns a pointer to a ioport_t object if address was found in the config file.
// otherwise, returns NULL.
const ioport_t *m32r_t::find_sym(ea_t address)
{
  return find_ioport(ioh.ports, address);
}

// GNU Assembler description
static const asm_t gnu_asm =
{
  AS_COLON
 |ASH_HEXF3     // hex 0x123 format
 |ASB_BINF3     // bin 0b010 format
  // don't display the final 0 in string declarations
 |AS_ASCIIZ | AS_ASCIIC | AS_1TEXT,
  0,
  "m32r GNU Assembler",
  0,
  NULL,         // no headers
  NULL,
  NULL,

  ";",          // comment string
  '"',          // string delimiter
  '\'',         // char delimiter
  "\\\"'",      // special symbols in char and string constants

  ".string",    // ascii string directive
  ".byte",      // byte directive
  ".short",     // word directive
  ".word",      // dword  (4 bytes)
  NULL,         // qword  (8 bytes)
  NULL,         // oword  (16 bytes)

  //  Although the M32R/X/D has no hardware floating point,
  //  the '.float' and '.double' directives generate IEEE-format
  //  floating-point values for compatibility with other development tools.

  ".float",     // float  (4 bytes)
  ".double",    // double (8 bytes)
  NULL,         // tbyte  (10/12 bytes)
  NULL,         // packed decimal real
  NULL,         // arrays (#h,#d,#v,#s(...)
  "dfs %s",     // uninited arrays
  "equ",        // Equ
  NULL,         // seg prefix
  "$",          // current IP (instruction pointer) symbol in assembler
  NULL,         // func_header
  NULL,         // func_footer
  ".global",    // public
  NULL,         // weak
  NULL,         // extrn
  NULL,         // comm
  NULL,         // get_type_name
  NULL,         // align
  '(', ')',     // lbrace, rbrace
  "%",          // mod
  "&",          // and
  "|",          // or
  "^",          // xor
  "!",          // not
  "<<",         // shl
  ">>",         // shr
  NULL,         // sizeof
  0,            // flag2 ???
  NULL,         // comment close string
  NULL,         // low8 op
  NULL,         // high8 op
  "LOW(%s)",    // low16 op
  "HIGH(%s)"    // high16 op
};

// As this time, we only support the GNU assembler.
static const asm_t *const asms[] = { &gnu_asm, NULL };

// Short and long names for our module
#define FAMILY "Mitsubishi 32-BIT family:"
static const char *const shnames[] =
{
  "m32r",
  "m32rx",
  NULL
};
static const char *const lnames[] =
{
  FAMILY"Mitsubishi 32-BIT family",
  "Mitsubishi 32-BIT family (extended)",
  NULL
};

// Opcodes of "return" instructions. This information will be used in 2 ways:
//      - if an instruction has the "return" opcode, its autogenerated label
//        will be "locret" rather than "loc".
//      - IDA will use the first "return" opcode to create empty subroutines.

static const uchar retcode_1[] = { 0x1F, 0xCE };        // jmp lr
static const uchar retcode_2[] = { 0x10, 0xD6 };        // rte

static const bytes_t retcodes[] =
{
  { sizeof(retcode_1), retcode_1 },
  { sizeof(retcode_2), retcode_2 },
  { 0, NULL }                            // NULL terminated array
};

//----------------------------------------------------------------------------
ssize_t idaapi idb_listener_t::on_event(ssize_t code, va_list)
{
  switch ( code )
  {
    case idb_event::savebase:
    case idb_event::closebase:
      // synchronize the database long variable with the current configuration settings
      pm.save_idpflags();
      pm.helper.supset(-1, pm.ioh.device.c_str());
      break;
  }
  return 0;
}

//----------------------------------------------------------------------
// This old-style callback only returns the processor module object.
static ssize_t idaapi notify(void *, int msgid, va_list)
{
  if ( msgid == processor_t::ev_get_procmod )
    return size_t(SET_MODULE_DATA(m32r_t));
  return 0;
}

//----------------------------------------------------------------------------
ssize_t idaapi m32r_t::on_event(ssize_t msgid, va_list va)
{
  int code = 0;
  switch ( msgid )
  {
    case processor_t::ev_init:
      hook_event_listener(HT_IDB, &idb_listener, &LPH);
      // this processor is big endian
      inf_set_be(true);
      helper.create("$ m32r");
      break;

    case processor_t::ev_term:
      ioh.ports.clear();
      unhook_event_listener(HT_IDB, &idb_listener);
      clr_module_data(data_id);
      break;

    case processor_t::ev_newfile:
      if ( choose_ioport_device(&ioh.device, cfgname) )
        ioh.set_device_name(ioh.device.c_str(), IORESP_ALL);
      // default configuration
      idpflags = NETNODE_USE_INSN_SYNTHETIC
               | NETNODE_USE_REG_ALIASES;
      // save flags to idb
      save_idpflags();

      // patch register names according to idpflags
      patch_regnames();
      break;

    case processor_t::ev_newprc:
      ptype = processor_subtype_t(va_arg(va, int));
//      msg("ptype = %s\n", ptype == prc_m32r ? "m32r" : ptype == prc_m32rx ? "m32rx" : "???");
      break;

    case processor_t::ev_ending_undo:
      // restore ptype
      ptype = processor_subtype_t(ph.get_proc_index());
      //fall through
    case processor_t::ev_oldfile:
      refresh_idpflags();
      ioh.restore_device();
      // patch register names according to idpflags
      patch_regnames();
      break;

    case processor_t::ev_create_switch_xrefs:
      {
        ea_t insn_ea = va_arg(va, ea_t);
        switch_info_t *si = va_arg(va, switch_info_t *);
        return m32r_create_switch_xrefs(insn_ea, *si);
      }

    case processor_t::ev_calc_switch_cases:
      {
        casevec_t *casevec = va_arg(va, casevec_t *);
        eavec_t *targets   = va_arg(va, eavec_t *);
        ea_t insn_ea       = va_arg(va, ea_t);
        switch_info_t *si  = va_arg(va, switch_info_t *);
        return m32r_calc_switch_cases(casevec, targets, insn_ea, *si);
      }

    case processor_t::ev_out_mnem:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        out_mnem(*ctx);
        return 1;
      }

    case processor_t::ev_out_header:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        m32r_header(*ctx);
        return 1;
      }

    case processor_t::ev_out_footer:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        m32r_footer(*ctx);
        return 1;
      }

    case processor_t::ev_out_segstart:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        segment_t *seg = va_arg(va, segment_t *);
        m32r_segstart(*ctx, seg);
        return 1;
      }

    case processor_t::ev_ana_insn:
      {
        insn_t *out = va_arg(va, insn_t *);
        return ana(out);
      }

    case processor_t::ev_emu_insn:
      {
        const insn_t *insn = va_arg(va, const insn_t *);
        return emu(*insn) ? 1 : -1;
      }

    case processor_t::ev_out_insn:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        out_insn(*ctx);
        return 1;
      }

    case processor_t::ev_out_operand:
      {
        outctx_t *ctx = va_arg(va, outctx_t *);
        const op_t *op = va_arg(va, const op_t *);
        return out_opnd(*ctx, *op) ? 1 : -1;
      }

    case processor_t::ev_can_have_type:
      {
        const op_t *op = va_arg(va, const op_t *);
        return can_have_type(*op) ? 1 : -1;
      }

    case processor_t::ev_is_sp_based:
      {
        int *mode = va_arg(va, int *);
        const insn_t *insn = va_arg(va, const insn_t *);
        const op_t *op = va_arg(va, const op_t *);
        *mode = is_sp_based(*insn, *op);
        return 1;
      }

    case processor_t::ev_create_func_frame:
      {
        func_t *pfn = va_arg(va, func_t *);
        create_func_frame(pfn);
        return 1;
      }

    case processor_t::ev_get_frame_retsize:
      {
        int *frsize = va_arg(va, int *);
        const func_t *pfn = va_arg(va, const func_t *);
        *frsize = m32r_get_frame_retsize(pfn);
        return 1;
      }

    case processor_t::ev_set_idp_options:
      {
        const char *keyword = va_arg(va, const char *);
        int value_type = va_arg(va, int);
        const char *value = va_arg(va, const char *);
        const char **errmsg = va_arg(va, const char **);
        bool idb_loaded = va_argi(va, bool);
        const char *ret = set_idp_options(keyword, value_type, value, idb_loaded);
        if ( ret == IDPOPT_OK )
          return 1;
        if ( errmsg != NULL )
          *errmsg = ret;
        return -1;
      }

    default:
      break;
  }
  return code;
}

//-----------------------------------------------------------------------
//      Processor Definition
//-----------------------------------------------------------------------
processor_t LPH =
{
  IDP_INTERFACE_VERSION,  // version
  PLFM_M32R,              // id
                          // flag
    PR_RNAMESOK           // can use register names for byte names
  | PR_BINMEM             // The module creates RAM/ROM segments for binary files
                          // (the kernel shouldn't ask the user about their sizes and addresses)
  | PR_USE32
  | PR_DEFSEG32,
                          // flag2
    PR2_IDP_OPTS,         // the module has processor-specific configuration options
  8,                      // 8 bits in a byte for code segments
  8,                      // 8 bits in a byte for other segments

  shnames,              // array of short processor names
                        // the short names are used to specify the processor
                        // with the -p command line switch)
  lnames,               // array of long processor names
                        // the long names are used to build the processor type
                        // selection menu

  asms,                 // array of target assemblers

  notify,               // the kernel event notification callback

  RegNames,             // Regsiter names
  qnumber(RegNames),    // Number of registers

  rVcs,rVds,
  0,                    // size of a segment register
  rVcs,rVds,

  NULL,                 // No known code start sequences
  retcodes,

  0,m32r_last,
  Instructions,         // instruc
  0,                    // int tbyte_size;  -- doesn't exist
  { 0, 7, 15, 0 },      // char real_width[4];
                        // number of symbols after decimal point
                        // 2byte float (0-does not exist)
                        // normal float
                        // normal double
                        // long double
  m32r_rte,             // Icode of return instruction. It is ok to give any of possible return instructions
};
