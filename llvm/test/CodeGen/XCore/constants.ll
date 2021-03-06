; RUN: llc < %s -march=xcore -mcpu=xs1b-generic | FileCheck %s

; CHECK: .section .cp.rodata.cst4,"aMc",@progbits,4
; CHECK: .LCPI1_0:
; CHECK: .long 12345678
; CHECK: f:
; CHECK: ldw r0, cp[.LCPI1_0]
define i32 @f() {
entry:
	ret i32 12345678
}
