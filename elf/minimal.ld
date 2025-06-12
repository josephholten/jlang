ENTRY(_start)

PHDRS
{
    text_segment PT_LOAD FILEHDR PHDRS FLAGS(5) ;
}

SECTIONS
{
  . = 0x400000;

  .text : {
    *(.text)
  } : text_segment
}
