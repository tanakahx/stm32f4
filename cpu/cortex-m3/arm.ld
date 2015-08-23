MEMORY
{
        vector (r)  : ORIGIN = 0x00000000, LENGTH = 0x400
        flash (rx)  : ORIGIN = 0x00000400, LENGTH = 0xFFC00
        sram (rwx)  : ORIGIN = 0x20000000, LENGTH = 192K
        load (rwx)  : ORIGIN = 0x20000400, LENGTH = 0x2FC00
        stack (rwx) : ORIGIN = 0x20020000, LENGTH = 0
}

SECTIONS {
         .vector : {
                 cpu/cortex-m3/vector.o (.rodata*)
         } > vector

         .text : {
               * (.text)
         } > flash

         .rodata : {
                 rodata_start = .;
                 * (.rodata*)
                 rodata_end = .;
         } > flash
         
         .data : {
               data_start = .;
               * (.data)
               data_end = .;
         } > load AT > flash
}

stack_bottom = ORIGIN(stack);
vector_start = ORIGIN(vector);
vector_end   = ORIGIN(vector) + LENGTH(sram);
sram_start   = ORIGIN(sram);
sram_end     = ORIGIN(sram) + LENGTH(sram);
