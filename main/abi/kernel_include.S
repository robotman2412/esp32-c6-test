/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

// Whether to preserve GP
// #define PRESERVE_GP
// Whether to preserve TP
// #define PRESERVE_TP
// Whether to forward non-ecall traps from U-mode.
// #define FORWARD_NON_ECALL_U_TRAPS
// Whether to forward non-ecall traps from M-mode.
// #define FORWARD_NON_ECALL_M_TRAPS
// Whether to use a dual M-mode system instead of an M-mode, U-mode system.
// #define DUAL_M_MODE

	# System call numbers.
	.equ sys_u_exit, 512
	.equ sys_u_abicall, 513
	.equ sys_m_userjump, 514
	.equ sys_m_userenter, 515

	# Named register indicies in registers struct.
	.equ rs_ra,  4
	.equ rs_sp,  8
	.equ rs_gp,  12
	.equ rs_tp,  16
	.equ rs_t0,  20
	.equ rs_t1,  24
	.equ rs_t2,  28
	.equ rs_s0,  32
	.equ rs_s1,  36
	.equ rs_a0,  40
	.equ rs_a1,  44
	.equ rs_a2,  48
	.equ rs_a3,  52
	.equ rs_a4,  56
	.equ rs_a5,  60
	.equ rs_a6,  64
	.equ rs_a7,  68
	.equ rs_s2,  72
	.equ rs_s3,  76
	.equ rs_s4,  80
	.equ rs_s5,  84
	.equ rs_s6,  88
	.equ rs_s7,  92
	.equ rs_s8,  96
	.equ rs_s9,  100
	.equ rs_s10, 104
	.equ rs_s11, 108
	.equ rs_t3,  112
	.equ rs_t4,  116
	.equ rs_t5,  120
	.equ rs_t6,  124
	
	# Named register indices in ctx->u_reg.
	.equ ctx_u_reg_ra,  4
	.equ ctx_u_reg_sp,  8
	.equ ctx_u_reg_gp,  12
	.equ ctx_u_reg_tp,  16
	.equ ctx_u_reg_t0,  20
	.equ ctx_u_reg_t1,  24
	.equ ctx_u_reg_t2,  28
	.equ ctx_u_reg_s0,  32
	.equ ctx_u_reg_s1,  36
	.equ ctx_u_reg_a0,  40
	.equ ctx_u_reg_a1,  44
	.equ ctx_u_reg_a2,  48
	.equ ctx_u_reg_a3,  52
	.equ ctx_u_reg_a4,  56
	.equ ctx_u_reg_a5,  60
	.equ ctx_u_reg_a6,  64
	.equ ctx_u_reg_a7,  68
	.equ ctx_u_reg_s2,  72
	.equ ctx_u_reg_s3,  76
	.equ ctx_u_reg_s4,  80
	.equ ctx_u_reg_s5,  84
	.equ ctx_u_reg_s6,  88
	.equ ctx_u_reg_s7,  92
	.equ ctx_u_reg_s8,  96
	.equ ctx_u_reg_s9,  100
	.equ ctx_u_reg_s10, 104
	.equ ctx_u_reg_s11, 108
	.equ ctx_u_reg_t3,  112
	.equ ctx_u_reg_t4,  116
	.equ ctx_u_reg_t5,  120
	.equ ctx_u_reg_t6,  124
	
	# Named register indices in ctx->m_reg.
	.equ ctx_m_reg_ra,  128+4
	.equ ctx_m_reg_sp,  128+8
	.equ ctx_m_reg_gp,  128+12
	.equ ctx_m_reg_tp,  128+16
	.equ ctx_m_reg_t0,  128+20
	.equ ctx_m_reg_t1,  128+24
	.equ ctx_m_reg_t2,  128+28
	.equ ctx_m_reg_s0,  128+32
	.equ ctx_m_reg_s1,  128+36
	.equ ctx_m_reg_a0,  128+40
	.equ ctx_m_reg_a1,  128+44
	.equ ctx_m_reg_a2,  128+48
	.equ ctx_m_reg_a3,  128+52
	.equ ctx_m_reg_a4,  128+56
	.equ ctx_m_reg_a5,  128+60
	.equ ctx_m_reg_a6,  128+64
	.equ ctx_m_reg_a7,  128+68
	.equ ctx_m_reg_s2,  128+72
	.equ ctx_m_reg_s3,  128+76
	.equ ctx_m_reg_s4,  128+80
	.equ ctx_m_reg_s5,  128+84
	.equ ctx_m_reg_s6,  128+88
	.equ ctx_m_reg_s7,  128+92
	.equ ctx_m_reg_s8,  128+96
	.equ ctx_m_reg_s9,  128+100
	.equ ctx_m_reg_s10, 128+104
	.equ ctx_m_reg_s11, 128+108
	.equ ctx_m_reg_t3,  128+112
	.equ ctx_m_reg_t4,  128+116
	.equ ctx_m_reg_t5,  128+120
	.equ ctx_m_reg_t6,  128+124

	# Offset of u_reg in ctx_t.
	.equ ctx_u_reg, 0
	# Offset of m_reg in ctx_t.
	.equ ctx_m_reg, 128
	# Offset of u_pc in ctx_t.
	.equ ctx_u_pc,  288
	# Offset of u_pc in ctx_t.
	.equ ctx_m_pc,  292
	
	# Offset of priv_is_m in ctx_t.
	.equ ctx_is_super, 320

	# Offset of scratch in ctx_t.
	.equ ctx_scratch_0, 256
	.equ ctx_scratch_1, 260
	.equ ctx_scratch_2, 264
	.equ ctx_scratch_3, 268
	.equ ctx_scratch_4, 272
	.equ ctx_scratch_5, 276
	.equ ctx_scratch_6, 280
	.equ ctx_scratch_7, 284
	
	# Offset of u_abi_table in ctx_t.
	.equ ctx_abi_table, 312
