/** \file compiler.h
  * \author Maarten Brock
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
  *
  * In other words, you are welcome to use, share and improve this program.
  * You are forbidden to forbid anyone else to use, share and improve
  * what you give them. Help stamp out software-hoarding!
  *
  * Header file to overcome 8051 compiler differences for specifying
  * special function registers. The following compilers are supported:
  * SDCC, Keil, Raisonance, IAR, Hi-Tech, Tasking, Crossware, Wickenhaeuser.
  * Unfortunately not for use with Dunfield. The compilers are identified by
  * their unique predefined macros. See also:
  * http://predef.sourceforge.net/precomp.html
  *
  * SBIT and SFR define special bit and special function registers at the given
  * address. SFR16 and SFR32 define sfr combinations at adjacent addresses in
  * little-endian format. SFR16E and SFR32E define sfr combinations without
  * prerequisite byte order or adjacency. None of these multi-byte sfr
  * combinations will guarantee the order in which they are accessed when read
  * or written.
  * SFR16X and SFR32X for 16 bit and 32 bit xdata registers are not defined
  * to avoid portability issues because of compiler endianness.
  * This file is to be included in every microcontroller specific header file.
  * Example:
  *
  * // my_mcu.h: sfr definitions for my mcu
  * #include <compiler.h>
  *
  * SBIT  (P0_1, 0x80, 1);      // Port 0 pin 1
  *
  * SFR   (P0, 0x80);           // Port 0
  *
  * SFRX  (CPUCS, 0xE600);      // Cypress FX2 Control and Status register in xdata memory at 0xE600
  *
  * SFR16 (TMR2, 0xCC);         // Timer 2, lsb at 0xCC, msb at 0xCD
  *
  * SFR16E(TMR0, 0x8C8A);       // Timer 0, lsb at 0x8A, msb at 0x8C
  *
  * SFR32 (MAC0ACC, 0x93);      // SiLabs C8051F120 32 bits MAC0 Accumulator, lsb at 0x93, msb at 0x96
  *
  * SFR32E(SUMR, 0xE5E4E3E2);   // TI MSC1210 SUMR 32 bits Summation register, lsb at 0xE2, msb at 0xE5
  *
 */

/*-------------------------------------------------------------------------
   Updated by - Ekawahyu Susilo / ekawahyu at yahoo dot com (September 2009)

   Added/redefined:
   - VECT(num, addr)
   - SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit3, bit2, bit1, bit0)
   - ASM(instruction)

-------------------------------------------------------------------------*/

#ifndef COMPILER_H
#define COMPILER_H

/** SDCC - Small Device C Compiler
  * http://sdcc.sf.net
 */
#if defined SDCC || defined __SDCC
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  __sbit  __at(addr+bit)                  name
# define SFR(name, addr)        __sfr   __at(addr)                      name
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       __xdata volatile unsigned char __at(addr) name
# define SFR16(name, addr)      __sfr16 __at(((addr+1U)<<8) | addr)     name
# define SFR16E(name, fulladdr) __sfr16 __at(fulladdr)                  name
# define SFR32(name, addr)      __sfr32 __at(((addr+3UL)<<24) | ((addr+2UL)<<16) | ((addr+1UL)<<8) | addr) name
# define SFR32E(name, fulladdr) __sfr32 __at(fulladdr)                  name
# define ASM(...) __VA_ARGS__
# define __asm_begin __asm
# define __asm_end __endasm

/** Keil C51
  * http://www.keil.com
 */
#elif defined __CX51__
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  sbit  name = addr^bit
# define SFR(name, addr)        sfr   name = addr
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       volatile unsigned char xdata name _at_ addr
# define SFR16(name, addr)      sfr16 name = addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */
# define ASM(...) __VA_ARGS__
# define __asm_begin __asm{
# define __asm_end }

/** Raisonance
  * http://www.raisonance.com
 */
#elif defined __RC51__
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  at (addr+bit) sbit                   name
# define SFR(name, addr)        sfr at addr                          name
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       xdata at addr volatile unsigned char name
# define SFR16(name, addr)      sfr16 at addr                        name
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** IAR 8051
  * http://www.iar.com
 */
#elif __IAR_SYSTEMS_ICC__
# define __SFRBIT_IN_USE__
# define VECT(num, addr) addr
# define SBIT(name, addr, bit)  /* not used (to be removed) */
# define SFR(name, addr)        __sfr __no_init volatile unsigned char name @ addr
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) \
__sfr __no_init volatile union \
{                              \
  unsigned char name;          \
  struct {                     \
    unsigned char bit0 : 1;    \
    unsigned char bit1 : 1;    \
    unsigned char bit2 : 1;    \
    unsigned char bit3 : 1;    \
    unsigned char bit4 : 1;    \
    unsigned char bit5 : 1;    \
    unsigned char bit6 : 1;    \
    unsigned char bit7 : 1;    \
  };                           \
} @ addr;
# define SFRX(name, addr)       __xdata __no_init volatile unsigned char name @ addr
# define SFR16(name, addr)      __sfr __no_init volatile unsigned int  name @ addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  __sfr __no_init volatile unsigned long name @ addr
# define SFR32E(name, fulladdr) /* not supported */
# define ASM(...) asm(#__VA_ARGS__);
# define __asm_begin
# define __asm_end

/** Tasking / Altium
  * http://www.altium.com/tasking
 */
#elif defined _CC51
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  _sfrbit  name _at(addr+bit)
# define SFR(name, addr)        _sfrbyte name _at(addr)
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       _xdat volatile unsigned char name _at(addr)
#if _CC51 > 71
# define SFR16(name, addr)      _sfrword _little name _at(addr)
#else
# define SFR16(name, addr)      /* not supported */
#endif
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** Hi-Tech 8051
  * http://www.htsoft.com
 */
#elif defined HI_TECH_C
# define VECT(num, addr) addr
# define SBIT(name, addr, bit)  volatile bit           name @ (addr+bit)
# define SFR(name, addr)        volatile unsigned char name @ addr
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       volatile far unsigned char name @ addr
# define SFR16(name, addr)      /* not supported */
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** Crossware
  * http://www.crossware.com
 */
#elif defined _XC51_VER
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  _sfrbit  name = (addr+bit)
# define SFR(name, addr)        _sfr     name = addr
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       volatile unsigned char _xdata name _at addr
# define SFR16(name, addr)      _sfrword name = addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** Wickenhaeuser
  * http://www.wickenhaeuser.de
 */
#elif defined __UC__
# define VECT(num, addr) addr
# define SBIT(name, addr, bit)  unsigned char bit  name @ (addr+bit)
# define SFR(name, addr)        near unsigned char name @ addr
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) /* not used */
# define SFRX(name, addr)       xdata volatile unsigned char name @ addr
# define SFR16(name, addr)      /* not supported */
# define SFR16E(name, fulladdr) /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** default
  * unrecognized compiler
 */
#else
# warning unrecognized compiler
# define VECT(num, addr) num
# define SBIT(name, addr, bit)  volatile bool           name
# define SFR(name, addr)        volatile unsigned char  name
# define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) name
# define SFRX(name, addr)       volatile unsigned char  name
# define SFR16(name, addr)      volatile unsigned short name
# define SFR16E(name, fulladdr) volatile unsigned short name
# define SFR32(name, fulladdr)  volatile unsigned long  name
# define SFR32E(name, fulladdr) volatile unsigned long  name

#endif

#endif //COMPILER_H
