/* The <a.out> header file describes the format of executable files. */

#ifndef _AOUT_H
#define _AOUT_H
#include <stdint.h>


struct header{			/* a.out header */
  uint8_t	a_magic[2];	/* magic number */
  uint8_t	a_flags;	/* flags, see below */
  uint8_t	a_cpu;		/* cpu id */
  uint8_t	a_hdrlen;	/* length of header */
  uint8_t	a_unused;	/* reserved for future use */
  uint8_t a_version;	/* version stamp (not used at present) */
  int32_t		a_text;		/* size of text segement in bytes */
  int32_t		a_data;		/* size of data segment in bytes */
  int32_t		a_bss;		/* size of bss segment in bytes */
  int32_t		a_entry;	/* entry point */
  int32_t		a_total;	/* total memory allocated */
  int32_t		a_syms;		/* size of symbol table */

  /* SHORT FORM ENDS HERE */
  int32_t		a_trsize;	/* text relocation size */
  int32_t		a_drsize;	/* data relocation size */
  int32_t		a_tbase;	/* text relocation base */
  int32_t		a_dbase;	/* data relocation base */
};

#endif /* _AOUT_H */