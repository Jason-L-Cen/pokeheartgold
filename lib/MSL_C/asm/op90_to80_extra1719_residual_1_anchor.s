	.extern __pformatter_empty_string_1
	.extern __pformatter_empty_string_2
	.extern printf_double2hex_0p0
	.extern printf_double2hex_inf_lower
	.extern printf_double2hex_inf_upper
	.extern printf_double2hex_nan_lower
	.extern printf_double2hex_nan_upper
	.extern printf_double2hex_neg_inf_lower
	.extern printf_double2hex_neg_inf_upper
	.extern printf_double2hex_neg_nan_lower
	.extern printf_double2hex_neg_nan_upper
	.include "asm/macros.inc"
	.include "MSL_Common_printf.inc"
	.include "global.inc"
	.public long2str

.text
