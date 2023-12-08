
.extern perf_default_handler
.globl handler_jal_area
.globl level_2_handler
.set noat

.text


.ent level_2_handler
level_2_handler:
	sq	$k0, -0x10($zero)
	sq	$k1, -0x20($zero)
	lui	$k0,(8)
	sq	$zero, 0($k0)
	sq	$1, 0x10($k0)
	sq	$2, 0x20($k0)
	sq	$3, 0x30($k0)
	sq	$4, 0x40($k0)
	sq	$5, 0x50($k0)
	sq	$6, 0x60($k0)
	sq	$7, 0x70($k0)
	sq	$8, 0x80($k0)
	sq	$9, 0x90($k0)
	sq	$10, 0xA0($k0)
	sq	$11, 0xB0($k0)
	sq	$12, 0xC0($k0)
	sq	$13, 0xD0($k0)
	sq	$14, 0xE0($k0)
	sq	$15, 0xF0($k0)
	sq	$16, 0x100($k0)
	sq	$17, 0x110($k0)
	sq	$18, 0x120($k0)
	sq	$19, 0x130($k0)
	sq	$20, 0x140($k0)
	sq	$21, 0x150($k0)
	sq	$22, 0x160($k0)
	sq	$23, 0x170($k0)
	sq	$24, 0x180($k0)
	sq	$25, 0x190($k0)
					# 0x1A0 must be set later, to initial $k0 value
					# 0x1B0 must be set later, to initial $k1 value
	sq	$28, 0x1C0($k0)
	sq	$29, 0x1D0($k0)
	sq	$30, 0x1E0($k0)
	sq	$31, 0x1F0($k0)

	lq	$t0, -0x10($zero)	# t0 = entry k0
	lq	$t1, -0x20($zero)	# t1 = entry k1
	sq	$t0, 0x1A0($k0)		# store entry k0 in register array
	sq	$t1, 0x1B0($k0)		# store entry k1 in register array

	mfpc $a0, 0
	mfpc $a1, 1
handler_jal_area:
	jal perf_default_handler # jal will be inserted here
	nop

	lui	$k0,(8)
	lq	$zero, 0($k0)
	lq	$1, 0x10($k0)
	lq	$2, 0x20($k0)
	lq	$3, 0x30($k0)
	lq	$4, 0x40($k0)
	lq	$5, 0x50($k0)
	lq	$6, 0x60($k0)
	lq	$7, 0x70($k0)
	lq	$8, 0x80($k0)
	lq	$9, 0x90($k0)
	lq	$10, 0xA0($k0)
	lq	$11, 0xB0($k0)
	lq	$12, 0xC0($k0)
	lq	$13, 0xD0($k0)
	lq	$14, 0xE0($k0)
	lq	$15, 0xF0($k0)
	lq	$16, 0x100($k0)
	lq	$17, 0x110($k0)
	lq	$18, 0x120($k0)
	lq	$19, 0x130($k0)
	lq	$20, 0x140($k0)
	lq	$21, 0x150($k0)
	lq	$22, 0x160($k0)
	lq	$23, 0x170($k0)
	lq	$24, 0x180($k0)
	lq	$25, 0x190($k0)
					# 0x1A0 must be set later, to initial $k0 value
					# 0x1B0 must be set later, to initial $k1 value
	lq	$28, 0x1C0($k0)
	lq	$29, 0x1D0($k0)
	lq	$30, 0x1E0($k0)
	lq	$31, 0x1F0($k0)

#	mfc0 $k0, $30
#	addi $k0, $k0, 4
#	mtc0 $k0, $30

	lq	$k0, -0x10($zero)
	lq	$k1, -0x20($zero)

	sync.p
	sync.l
	eret
	nop

.end level_2_handler
