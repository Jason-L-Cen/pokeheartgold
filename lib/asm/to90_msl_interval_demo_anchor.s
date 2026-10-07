	.public _021116E8
	.public _021116F4
	.public _0211170C
	.public _02111734
	.public _02111738
	.public _02111788
	.public _021117E4
	.public _021117F4
	.public _02111848
	.public _ZSt13__new_handler
	.public _ZSt7nothrow
	.public _ZTVSt9bad_alloc
	.public _ZZNKSt13bad_exception4whatEvEs
	.public __global_destructor_chain
	.extern _02111504
	.extern _ExitProcess
	.extern _Z10NextActionP14ActionIterator
	.extern _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
	.extern _Z14__ThrowHandlerP12ThrowContext
	.extern _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc
	.extern _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo
	.extern _Z16__SkipUnwindInfoPc
	.extern _Z17IsInSpecificationPcP16ex_specification
	.extern _Z19FindExceptionRecordPcP13ExceptionInfo
	.extern _Z20FindExceptionHandlerP12ThrowContextP13ExceptionInfoPl
	.extern _Z23FindMostRecentExceptionP12ThrowContextP13ExceptionInfo
	.extern _ZN10__cxxabiv117__class_type_infoD0Ev
	.extern _ZN10__cxxabiv117__class_type_infoD1Ev
	.extern _ZN10__cxxabiv117__class_type_infoD2Ev
	.extern _ZN10__cxxabiv120__si_class_type_infoD0Ev
	.extern _ZN10__cxxabiv120__si_class_type_infoD1Ev
	.extern _ZNKSt13bad_exception4whatEv
	.extern _ZNKSt9bad_alloc4whatEv
	.extern _ZNKSt9type_infoeqERKS_
	.extern _ZNSt13bad_exceptionD0Ev
	.extern _ZNSt13bad_exceptionD1Ev
	.extern _ZNSt9bad_allocD0Ev
	.extern _ZNSt9bad_allocD1Ev
	.extern _ZSt10unexpectedv
	.extern _ZSt17__throw_bad_allocv
	.extern _ZSt9dthandlerv
	.extern _ZSt9duhandlerv
	.extern _ZSt9terminatev
	.extern _ZdaPv
	.extern _ZdlPv
	.extern _Znam
	.extern _ZnamRKSt9nothrow_t
	.extern _Znwm
	.extern _ZnwmRKSt9nothrow_t
	.extern __call_static_initializers
	.extern __destroy_global_chain
	.extern __end__catch
	.extern __read_console
	.extern __unexpected
	.extern __write_console
	.extern OSi_ThreadInfo
	.type OSi_ThreadInfo, @function
	.extern __sinit__
	.type __sinit__, @function
	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"

	.bss
_ZSt7nothrow:
	.space 0x4

_ZSt13__new_handler:
	.space 0x4

__global_destructor_chain: ; 0x021E58E8
	.space 4

.text
	.data

	.balign 4, 0
_021116B0:
	.word _021117E4
	.word _021116CC
	.word _02111504
_ZZNKSt13bad_exception4whatEvEs:
	.asciz "bad_exception"
	.balign 4, 0
_021116CC:
	.asciz "St13bad_exception"
	.balign 4, 0
_021116E0:
	.word 0
	.word _021116B0
_021116E8:
	.word _ZNSt13bad_exceptionD1Ev
	.word _ZNSt13bad_exceptionD0Ev
	.word _ZNKSt13bad_exception4whatEv
_021116F4:
	.asciz "!std::bad_exception!!"
	.balign 4, 0
_0211170C:
	.asciz "!std::exception!!std::bad_exception!!"
	.balign 4, 0
_02111734:
	.byte 1
	.balign 4, 0
_02111738:
	.asciz "bad_alloc"
	.balign 4, 0
_02111744:
	.word _021117E4
	.word _02111750
	.word _02111504
_02111750:
	.asciz "St9bad_alloc"
	.balign 4, 0
_02111760:
	.asciz "!std::bad_alloc!"
	.balign 4, 0
_02111774:
	.word 0
	.word _02111744
_ZTVSt9bad_alloc:
	.word _ZNSt9bad_allocD1Ev
	.word _ZNSt9bad_allocD0Ev
	.word _ZNKSt9bad_alloc4whatEv
_02111788:
	.asciz "!std::exception!!std::bad_alloc!!"
	.balign 4, 0
_021117AC:
	.word _021117F4
	.word _021117CC
_021117B4:
	.word _021117E4
	.word _02111820
	.word _021117C0
_021117C0:
	.word _021117E4
	.word _021117FC
	.word _021117AC
_021117CC:
	.asciz "St9type_info"
	.balign 4, 0
_021117DC:
	.word 0
	.word _021117B4
_021117E4:
	.word _ZN10__cxxabiv120__si_class_type_infoD1Ev
	.word _ZN10__cxxabiv120__si_class_type_infoD0Ev
	.word 0
	.word _021117C0
_021117F4:
	.word _ZN10__cxxabiv117__class_type_infoD1Ev
	.word _ZN10__cxxabiv117__class_type_infoD0Ev
_021117FC:
	.asciz "N10__cxxabiv117__class_type_infoE"
	.balign 4, 0
_02111820:
	.asciz "N10__cxxabiv120__si_class_type_infoE"
	.balign 4, 0
_02111848:
	.word _ZSt9dthandlerv
	.word _ZSt9duhandlerv

	exception __read_console, 0x0051, 0x00300F00
	exception __write_console, 0x0035, 0x00200700
	exception _Z19FindExceptionRecordPcP13ExceptionInfo, 0x0109, 0x00503F00
	exception _Z10NextActionP14ActionIterator, 0x0388, _020F4FF8
	exception _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc, 0x08B0, _020F5000
	exception _Z23FindMostRecentExceptionP12ThrowContextP13ExceptionInfo, 0x0154, _020F5010
	exception _Z17IsInSpecificationPcP16ex_specification, 0x0085, 0x00401F00
	exception __unexpected, 0x00A8, _020F5018
	exception _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc, 0x0060, _020F502C
	exception _Z20FindExceptionHandlerP12ThrowContextP13ExceptionInfoPl, 0x01DC, _020F5034
	exception _Z14__ThrowHandlerP12ThrowContext, 0x00ED, 0x00700300
	exception __end__catch, 0x0029, 0x00100000
	exception _ZNSt13bad_exceptionD0Ev, 0x0015, 0x00100100
	exception _ZSt17__throw_bad_allocv, 0x0041, 0x00100000
	exception _Znwm, 0x0064, _020F503C
	exception _ZnwmRKSt9nothrow_t, 0x0044, _020F5054
	exception _ZdlPv, 0x0015, 0x00100000
	exception _Znam, 0x002C, _020F5070
	exception _ZnamRKSt9nothrow_t, 0x002C, _020F5088
	exception _ZdaPv, 0x000D, 0x00000000
	exception _ZNSt9bad_allocD0Ev, 0x0015, 0x00100100
	exception __call_static_initializers, 0x002D, 0x00100100
	exception __destroy_global_chain, 0x0045, 0x00200300
	exception _ZNKSt9type_infoeqERKS_, 0x0031, 0x00100000
	exception _ZN10__cxxabiv117__class_type_infoD1Ev, 0x0015, 0x00100100
	exception _ZN10__cxxabiv117__class_type_infoD0Ev, 0x001D, 0x00100100
	exception _ZN10__cxxabiv117__class_type_infoD2Ev, 0x0015, 0x00100100
	exception _ZN10__cxxabiv120__si_class_type_infoD1Ev, 0x0015, 0x00100100
	exception _ZN10__cxxabiv120__si_class_type_infoD0Ev, 0x001D, 0x00100100
	exception _ZSt9dthandlerv, 0x000D, 0x00000000
	exception _ZSt9duhandlerv, 0x000D, 0x00000000
	exception _ZSt9terminatev, 0x0019, 0x00100000
	exception _ZSt10unexpectedv, 0x0019, 0x00100000
	exception _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo, 0x00A1, 0x00200700
	exception _Z16__SkipUnwindInfoPc, 0x0031, 0x00200100
	exception _ExitProcess, 0x000D, 0x00000000

	.section .exception,4,1,2
	; MSL symbols have exceptions for C++ mode, even when compiling C.

_020F4FF8:
	.byte 0x00, 0x03, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00
_020F5000:
	.byte 0x00, 0xFF, 0x01, 0xC8, 0x58, 0x1D, 0xB8, 0x1A, 0x01, 0x8C, 0x50, 0x1A, 0x00, 0x8E, 0x00, 0x00
_020F5010:
	.byte 0x00, 0x03, 0x01, 0x98, 0x00, 0x00, 0x00, 0x00
_020F5018:
	.byte 0x40, 0x81, 0x70, 0x00, 0x30, 0x00, 0x16, 0x20
	.byte 0xB0, 0x24, 0x00, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x38, 0x08, 0x8D, 0x08
_020F502C:
	.byte 0x00, 0x0F, 0x30, 0x38, 0x80, 0x0E, 0x00, 0x8E
_020F5034:
	.byte 0x00, 0xFF, 0x01, 0xC8, 0x00, 0x00, 0x00, 0x00
_020F503C:
	.byte 0x40, 0x87, 0x60, 0x00
	.byte 0x40, 0x40, 0x16, 0x28, 0x00, 0x26, 0x00, 0x8F, 0x02, 0x98, 0x00, 0x60, 0x17, 0x11, 0x02, 0x8D
	.byte 0x00, 0x00, 0x00, 0x00
_020F5054:
	.byte 0x40, 0x80, 0x70, 0x00, 0x28, 0x00, 0x1C, 0x20, 0x00, 0x2A, 0x28, 0x00
	.byte 0x32, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x38, 0x30, 0x8F, 0x00, 0x60, 0x00, 0x8D, 0x00, 0x00
_020F5070:
	.byte 0x40, 0x80, 0x40, 0x00, 0x20, 0x00, 0x16, 0x20, 0x00, 0x26, 0x00, 0x8F, 0x02, 0x30, 0x00, 0x60
	.byte 0x17, 0x11, 0x02, 0x8D, 0x00, 0x00, 0x00, 0x00
_020F5088:
	.byte 0x40, 0x80, 0x40, 0x00, 0x20, 0x00, 0x16, 0x20
	.byte 0x00, 0x1E, 0x00, 0x8F, 0x00, 0x30, 0x00, 0x8D, 0x00, 0x00, 0x00, 0x00
