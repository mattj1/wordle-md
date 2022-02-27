.section .rodata
	.globl res_echoblob
	.align 2
res_echoblob:
	/* .incbin "res/echoblob.bin"*/
	.incbin "src/echo/echoblob.bin"

