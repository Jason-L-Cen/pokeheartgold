	.extern buff_ptr$6E40
	.extern strptr$6E44
	.extern wstr$6DE4
	.extern wstr$6DF0
	.extern wstr$6DFC
	.extern wstr$6E08
	.extern wstr$6E10
	.extern wstr$6E18
	.extern wstr$6E24
	.extern wstr$6E30
	.extern wstr$6E38
	.include "asm/macros.inc"
	.include "MSL_Common_wprintf.inc"
	.include "global.inc"
	.public long2str__wide

.text
