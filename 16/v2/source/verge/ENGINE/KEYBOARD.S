#
# Copyright (C) 1998 BJ Eirich (aka vecna)
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
# See the GNU General Public Lic
# See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

                .file   "keyboard.s"

                .extern ___djgpp_base_address
                .extern ___djgpp_ds_alias
                .extern ___djgpp_dos_sel

 # public functions and variables:

                .global _key
                .global _InitKeyboard
                .global _ShutdownKeyboard
                .global _keyboard_chain
                .global _last_pressed

                .text

                .align  4

locking_region_start:

_key:           .space  0x80, 0
_last_pressed:  .byte  0
old_vector:
old_vector_ofs: .long   0
old_vector_sel: .word   0
chain_flag:     .long   1


                .align  4

handler_procedure:

 #
 # .. will be called every time a key is pressed/released
 #
                pushl   %eax
                pushl   %edx
                pushw   %ds
 #
 # Load DS with our data selector
 #
                movw    %cs:___djgpp_ds_alias, %ds
 #
 # Read the scancode from keyboard port and update key[]
 #
                inb     $0x60, %al
                movb    %al, %dl
                andl    $0x7f, %edx
                testb   $0x80, %al
                setz    _key(%edx)
                cmp     $127, %al
                ja      nolastpressed
                movb    %edx, _last_pressed
nolastpressed:

 #
 # Chain if flag is set, otherwise do what's necessary and return
 #
                cmpl    $0, chain_flag
                jne     handler_chain
 #
 # Acknowledge keyboard and interrupt contollers
 #
                inb     $0x61, %al
                orb     $0x80, %al
                outb    %al, $0x61
                andb    $0x7f, %al
                outb    %al, $0x61
                movb    $0x20, %al
                outb    %al, $0x20

                popw    %ds
                popl    %edx
                popl    %eax
                sti
                iret

                .align  4

handler_chain:  popw    %ds
                popl    %edx
                popl    %eax
                ljmp    %cs:(old_vector)

locking_region_end:


                .align  4
_InitKeyboard:

 #
 # int keyboard_init(void);
 #
 # Initializes the keyboard handler and hooks the keyboard interrupt.
 # Returns -1 on failure, zero on success
 #
                pushl   %esi
                pushl   %edi
                pushl   %ebx
 #
 # First, we need to lock the handler and memory it touches, so
 # it doesn't get swapped out to disk.
 #
                leal    locking_region_start, %ecx
                leal    locking_region_end, %edi
                subl    %ecx, %edi
                addl    ___djgpp_base_address, %ecx
                shldl   $16, %ecx, %ebx         # ecx -> bx:cx
                shldl   $16, %edi, %esi         # edi -> si:di
                movw    $0x0600, %ax            # lock linear region
                int     $0x31
                jc      init_error
 #
 # Now we need to save the old interrupt vector, so we can restore
 # it later and also to know where to jump if chaining.
 #
                movw    $0x0204, %ax            # get pm int vector
                movb    $0x09, %bl
                int     $0x31
                movw    %cx, old_vector_sel
                movl    %edx, old_vector_ofs
 #
 # Make sure we chain after initialization.
 #
                movl    $1, chain_flag
 #
 # Set the interrupt vector to point to our handler.
 #
                movw    %cs, %cx
                leal    handler_procedure, %edx
                movb    $0x09, %bl
                movw    $0x0205, %ax            # set pm int vector
                int     $0x31
 #*
 #* Actually we would have to unlock the locked region on failure
 #* here. But since most programs would exit with an error message
 #* in such case, there's no need to worry.
 #*

init_error:

 #
 # This sets EAX to -1 if CF is set and to 0 atherwise
 #
                movl    $0, %eax
                sbbl    $0, %eax

                popl    %ebx
                popl    %edi
                popl    %esi
                ret


                .align 4
_ShutdownKeyboard:

 #
 # void keyboard_close(void);
 #
 # Shuts the keyboard handler down.
 #
                pushl   %esi
                pushl   %edi
                pushl   %ebx
 #
 # Unlock the region we locked at initialization
 #
                leal    locking_region_start, %ecx
                leal    locking_region_end, %edi
                subl    %ecx, %edi
                addl    ___djgpp_base_address, %ecx
                shldl   $16, %ecx, %ebx
                shldl   $16, %edi, %esi
                movw    $0x0601, %ax            # unlock linear region
                int     $0x31
 #
 # Restore the interrupt vector to its previous value
 #
                movw    old_vector_sel, %cx
                movl    old_vector_ofs, %edx
                movb    $0x09, %bl
                movw    $0x0205, %ax            # set pm int vector
                int     $0x31

                popl    %ebx
                popl    %edi
                popl    %esi
                ret
 #
 # void keyboard_chain(int toggle);
 #
                .align  4
_keyboard_chain:
                cmpl    $0, 4(%esp)
                je      chain_off
chain_on:

 #
 # Set the chain_flag and clear BIOS shift/ctrl/alt status bits:
 #
                movl    $1, chain_flag

                push    %es
                movw    ___djgpp_dos_sel, %es
                andb    $0xf0, %es:0x417
                pop     %es
                jmp     chain_done
chain_off:
                movl    $0, chain_flag
chain_done:     ret

