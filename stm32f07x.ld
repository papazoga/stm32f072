ENTRY(_reset)

STACK_SIZE = 0x400;

MEMORY
{
	ram : ORIGIN = 0x20000000, LENGTH = 16k
	flash (rx) : ORIGIN = 0x08000000, LENGTH = 128k
	remap : ORIGIN = 0x00000000, LENGTH = 128k
}

SECTIONS
{
	.text :
	{
		. = 0x0;
		KEEP(start.o(.vectors))
		*(.text)
		. = ALIGN(4);
	} >remap AT>flash

	.flashdata :
	{
		*(.rodata)
		. = ALIGN(4);
	} >flash

	. = ALIGN(4);

	_data_src = .;

	.data : AT( ADDR(.flashdata) + SIZEOF(.flashdata) )
	{
		_data_dst = .;
		*(.data)
		_data_end = .;
	} >ram

	_data_size = _data_end - _data_dst;

	.bss :
	{
		*(.bss COMMON)
	} >ram

	.noinit (NOLOAD):
	{
		PROVIDE(__noinit_start = .);
		*(.noinit*)
		PROVIDE(__noinit_end = .);
	} >ram

	. = . + STACK_SIZE;

	_stack_top = .;
}

