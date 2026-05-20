	.file	1 "ghost_replay.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=32
	.module	nooddspreg
	.module	arch=mips1
	.text
	.globl	g_ghostA
	.section	.bss,"aw",@nobits
	.align	2
	.type	g_ghostA, @object
	.size	g_ghostA, 36
g_ghostA:
	.space	36
	.globl	g_ghostB
	.align	2
	.type	g_ghostB, @object
	.size	g_ghostB, 36
g_ghostB:
	.space	36
	.globl	g_ghost_new_best
	.section	.sbss,"aw",@nobits
	.align	2
	.type	g_ghost_new_best, @object
	.size	g_ghost_new_best, 4
g_ghost_new_best:
	.space	4
	.globl	g_dbg_ghost_update_calls
	.align	2
	.type	g_dbg_ghost_update_calls, @object
	.size	g_dbg_ghost_update_calls, 4
g_dbg_ghost_update_calls:
	.space	4
	.globl	g_dbg_ghost_update_dragon_skips
	.align	2
	.type	g_dbg_ghost_update_dragon_skips, @object
	.size	g_dbg_ghost_update_dragon_skips, 4
g_dbg_ghost_update_dragon_skips:
	.space	4
	.globl	g_dbg_ghost_record_calls
	.align	2
	.type	g_dbg_ghost_record_calls, @object
	.size	g_dbg_ghost_record_calls, 4
g_dbg_ghost_record_calls:
	.space	4
	.globl	g_dbg_ghost_start_calls
	.align	2
	.type	g_dbg_ghost_start_calls, @object
	.size	g_dbg_ghost_start_calls, 4
g_dbg_ghost_start_calls:
	.space	4
	.globl	g_dbg_ghost_start_insta_calls
	.align	2
	.type	g_dbg_ghost_start_insta_calls, @object
	.size	g_dbg_ghost_start_insta_calls, 4
g_dbg_ghost_start_insta_calls:
	.space	4
	.globl	g_dbg_ghost_start_reg_calls
	.align	2
	.type	g_dbg_ghost_start_reg_calls, @object
	.size	g_dbg_ghost_start_reg_calls, 4
g_dbg_ghost_start_reg_calls:
	.space	4
	.globl	g_ghost_pos_snapshots
	.section	.bss
	.align	2
	.type	g_ghost_pos_snapshots, @object
	.size	g_ghost_pos_snapshots, 168
g_ghost_pos_snapshots:
	.space	168
	.globl	g_ghost_rec_time_line
	.align	2
	.type	g_ghost_rec_time_line, @object
	.size	g_ghost_rec_time_line, 40
g_ghost_rec_time_line:
	.space	40
	.local	s_playback_dragon_event_index
	.comm	s_playback_dragon_event_index,4,4
	.local	s_playback_dragon_pause_remaining
	.comm	s_playback_dragon_pause_remaining,4,4
	.local	s_recording_was_in_dragon
	.comm	s_recording_was_in_dragon,4,4
	.section	.sdata,"aw"
	.align	2
	.type	s_recording_dragon_event_index, @object
	.size	s_recording_dragon_event_index, 4
s_recording_dragon_event_index:
	.word	-1
	.local	s_recording_dragon_pause_frames
	.comm	s_recording_dragon_pause_frames,4,4
	.local	s_ghost_render_frame_ready
	.comm	s_ghost_render_frame_ready,4,4
	.local	s_recording_failed_too_long
	.comm	s_recording_failed_too_long,4,4
	.local	s_recording_too_long_message_timer
	.comm	s_recording_too_long_message_timer,4,4
	.text
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostHeaderHasCompletedRun
	.type	GhostHeaderHasCompletedRun, @function
GhostHeaderHasCompletedRun:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	lw	$2,8($fp)
	nop
	lw	$2,16($2)
	nop
	blez	$2,$L2
	nop

	lw	$2,8($fp)
	nop
	lw	$2,20($2)
	nop
	blez	$2,$L2
	nop

	li	$2,1			# 0x1
	b	$L4
	nop

$L2:
	move	$2,$0
$L4:
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostHeaderHasCompletedRun
	.size	GhostHeaderHasCompletedRun, .-GhostHeaderHasCompletedRun
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostHeaderIsV2
	.type	GhostHeaderIsV2, @function
GhostHeaderIsV2:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	lw	$2,8($fp)
	nop
	lw	$3,24($2)
	li	$2,1195900928			# 0x47480000
	ori	$2,$2,0x4432
	bne	$3,$2,$L6
	nop

	lw	$2,8($fp)
	nop
	lw	$3,28($2)
	li	$2,2			# 0x2
	bne	$3,$2,$L6
	nop

	lw	$2,8($fp)
	nop
	lw	$3,32($2)
	li	$2,168			# 0xa8
	bne	$3,$2,$L6
	nop

	li	$2,1			# 0x1
	b	$L8
	nop

$L6:
	move	$2,$0
$L8:
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostHeaderIsV2
	.size	GhostHeaderIsV2, .-GhostHeaderIsV2
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostHeaderFrameDataOffset
	.type	GhostHeaderFrameDataOffset, @function
GhostHeaderFrameDataOffset:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	lw	$4,24($fp)
	jal	GhostHeaderIsV2
	nop

	beq	$2,$0,$L10
	nop

	lw	$2,24($fp)
	nop
	lw	$2,32($2)
	b	$L11
	nop

$L10:
	li	$2,24			# 0x18
$L11:
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostHeaderFrameDataOffset
	.size	GhostHeaderFrameDataOffset, .-GhostHeaderFrameDataOffset
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostFramesForRegion
	.type	GhostFramesForRegion, @function
GhostFramesForRegion:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	lw	$2,32($fp)
	nop
	sw	$2,16($fp)
	lw	$4,16($fp)
	jal	GhostHeaderFrameDataOffset
	nop

	move	$3,$2
	lw	$2,32($fp)
	nop
	addu	$2,$3,$2
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostFramesForRegion
	.size	GhostFramesForRegion, .-GhostFramesForRegion
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostDragonEventCount
	.type	GhostDragonEventCount, @function
GhostDragonEventCount:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	lw	$4,32($fp)
	jal	GhostHeaderIsV2
	nop

	bne	$2,$0,$L15
	nop

	move	$2,$0
	b	$L16
	nop

$L15:
	lw	$2,32($fp)
	nop
	lw	$2,36($2)
	nop
	sw	$2,16($fp)
	lw	$2,16($fp)
	nop
	bgez	$2,$L17
	nop

	move	$2,$0
	b	$L16
	nop

$L17:
	lw	$2,16($fp)
	nop
	slt	$2,$2,17
	bne	$2,$0,$L18
	nop

	li	$2,16			# 0x10
	b	$L16
	nop

$L18:
	lw	$2,16($fp)
$L16:
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostDragonEventCount
	.size	GhostDragonEventCount, .-GhostDragonEventCount
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	ResetGhostDragonRuntime
	.type	ResetGhostDragonRuntime, @function
ResetGhostDragonRuntime:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$0,%gp_rel(s_playback_dragon_event_index)($28)
	sw	$0,%gp_rel(s_playback_dragon_pause_remaining)($28)
	sw	$0,%gp_rel(s_recording_was_in_dragon)($28)
	li	$2,-1			# 0xffffffffffffffff
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	sw	$0,%gp_rel(s_recording_dragon_pause_frames)($28)
	sw	$0,%gp_rel(s_ghost_render_frame_ready)($28)
	nop
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	ResetGhostDragonRuntime
	.size	ResetGhostDragonRuntime, .-ResetGhostDragonRuntime
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostHeaderMatchesCompletedLevel
	.type	GhostHeaderMatchesCompletedLevel, @function
GhostHeaderMatchesCompletedLevel:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	sw	$5,28($fp)
	lw	$4,24($fp)
	jal	GhostHeaderHasCompletedRun
	nop

	beq	$2,$0,$L21
	nop

	lw	$2,24($fp)
	nop
	lw	$2,12($2)
	lw	$3,28($fp)
	nop
	bne	$3,$2,$L21
	nop

	li	$2,1			# 0x1
	b	$L23
	nop

$L21:
	move	$2,$0
$L23:
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostHeaderMatchesCompletedLevel
	.size	GhostHeaderMatchesCompletedLevel, .-GhostHeaderMatchesCompletedLevel
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	StartRecording
	.type	StartRecording, @function
StartRecording:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-16
	sw	$fp,12($sp)
	move	$fp,$sp
	sw	$4,16($fp)
	sw	$5,20($fp)
	lw	$2,16($fp)
	li	$3,1			# 0x1
	sw	$3,0($2)
	lw	$2,16($fp)
	li	$3,1			# 0x1
	sw	$3,4($2)
	lw	$2,16($fp)
	nop
	sw	$0,8($2)
	lw	$2,16($fp)
	lui	$3,%hi(_spyro)
	lw	$5,%lo(_spyro)($3)
	addiu	$4,$3,%lo(_spyro)
	lw	$4,4($4)
	addiu	$3,$3,%lo(_spyro)
	lw	$3,8($3)
	sw	$5,24($2)
	sw	$4,28($2)
	sw	$3,32($2)
	sw	$0,%gp_rel(s_recording_failed_too_long)($28)
	sw	$0,%gp_rel(s_recording_too_long_message_timer)($28)
	sw	$0,%gp_rel(s_recording_was_in_dragon)($28)
	li	$2,-1			# 0xffffffffffffffff
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	sw	$0,%gp_rel(s_recording_dragon_pause_frames)($28)
	lw	$2,20($fp)
	nop
	sw	$2,4($fp)
	lui	$2,%hi(_spyro)
	lw	$3,%lo(_spyro)($2)
	lw	$2,4($fp)
	nop
	sw	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lw	$3,4($2)
	lw	$2,4($fp)
	nop
	sw	$3,4($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lw	$3,8($2)
	lw	$2,4($fp)
	nop
	sw	$3,8($2)
	lb	$2,%gp_rel(_levelID)($28)
	nop
	move	$3,$2
	lw	$2,4($fp)
	nop
	sw	$3,12($2)
	lw	$2,4($fp)
	nop
	sw	$0,16($2)
	lw	$2,4($fp)
	nop
	sw	$0,20($2)
	lw	$2,4($fp)
	li	$3,1195900928			# 0x47480000
	ori	$3,$3,0x4432
	sw	$3,24($2)
	lw	$2,4($fp)
	li	$3,2			# 0x2
	sw	$3,28($2)
	lw	$2,4($fp)
	li	$3,168			# 0xa8
	sw	$3,32($2)
	lw	$2,4($fp)
	nop
	sw	$0,36($2)
	sw	$0,0($fp)
	b	$L25
	nop

$L26:
	lw	$3,4($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,0($2)
	lw	$3,4($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,4($2)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,1
	sw	$2,0($fp)
$L25:
	lw	$2,0($fp)
	nop
	slt	$2,$2,16
	bne	$2,$0,$L26
	nop

	nop
	nop
	move	$sp,$fp
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	StartRecording
	.size	StartRecording, .-StartRecording
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	StartPlayback
	.type	StartPlayback, @function
StartPlayback:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	sw	$5,36($fp)
	lw	$2,36($fp)
	nop
	sw	$2,16($fp)
	lw	$4,16($fp)
	jal	GhostHeaderHasCompletedRun
	nop

	beq	$2,$0,$L29
	nop

	lw	$2,32($fp)
	li	$3,1			# 0x1
	sw	$3,0($2)
	lw	$2,32($fp)
	nop
	sw	$0,4($2)
	lw	$2,32($fp)
	nop
	sw	$0,8($2)
	lw	$2,16($fp)
	nop
	lw	$3,0($2)
	lw	$2,32($fp)
	nop
	sw	$3,12($2)
	lw	$2,16($fp)
	nop
	lw	$3,4($2)
	lw	$2,32($fp)
	nop
	sw	$3,16($2)
	lw	$2,16($fp)
	nop
	lw	$3,8($2)
	lw	$2,32($fp)
	nop
	sw	$3,20($2)
	sw	$0,%gp_rel(s_playback_dragon_event_index)($28)
	sw	$0,%gp_rel(s_playback_dragon_pause_remaining)($28)
	sw	$0,%gp_rel(s_ghost_render_frame_ready)($28)
$L29:
	nop
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	StartPlayback
	.size	StartPlayback, .-StartPlayback
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	StopGhost
	.type	StopGhost, @function
StopGhost:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	lw	$2,8($fp)
	nop
	sw	$0,0($2)
	lw	$2,8($fp)
	nop
	sw	$0,4($2)
	lw	$2,8($fp)
	nop
	sw	$0,8($2)
	lw	$3,8($fp)
	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	bne	$3,$2,$L31
	nop

	sw	$0,%gp_rel(s_playback_dragon_event_index)($28)
	sw	$0,%gp_rel(s_playback_dragon_pause_remaining)($28)
	sw	$0,%gp_rel(s_ghost_render_frame_ready)($28)
	b	$L33
	nop

$L31:
	lw	$3,8($fp)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	bne	$3,$2,$L33
	nop

	sw	$0,%gp_rel(s_recording_was_in_dragon)($28)
	li	$2,-1			# 0xffffffffffffffff
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	sw	$0,%gp_rel(s_recording_dragon_pause_frames)($28)
$L33:
	nop
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	StopGhost
	.size	StopGhost, .-StopGhost
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	CopyGhostData
	.type	CopyGhostData, @function
CopyGhostData:
	.frame	$fp,64,$31		# vars= 40, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$fp,56($sp)
	move	$fp,$sp
	sw	$4,64($fp)
	sw	$5,68($fp)
	lw	$2,64($fp)
	nop
	sw	$2,24($fp)
	lw	$2,68($fp)
	nop
	sw	$2,28($fp)
	lw	$4,24($fp)
	jal	GhostHeaderFrameDataOffset
	nop

	sw	$2,16($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,24
	bne	$2,$0,$L35
	nop

	lw	$2,16($fp)
	nop
	sltu	$2,$2,169
	bne	$2,$0,$L36
	nop

$L35:
	li	$2,24			# 0x18
	sw	$2,16($fp)
$L36:
	lw	$2,16($fp)
	nop
	sw	$2,32($fp)
	lw	$2,24($fp)
	nop
	sw	$2,36($fp)
	lw	$2,28($fp)
	nop
	sw	$2,40($fp)
	sw	$0,20($fp)
	b	$L37
	nop

$L39:
	lw	$2,20($fp)
	nop
	sll	$2,$2,2
	lw	$3,36($fp)
	nop
	addu	$3,$3,$2
	lw	$2,20($fp)
	nop
	sll	$2,$2,2
	lw	$4,40($fp)
	nop
	addu	$2,$4,$2
	lw	$3,0($3)
	nop
	sw	$3,0($2)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,1
	sw	$2,20($fp)
$L37:
	lw	$2,32($fp)
	nop
	bgez	$2,$L38
	nop

	addiu	$2,$2,3
$L38:
	sra	$2,$2,2
	move	$3,$2
	lw	$2,20($fp)
	nop
	slt	$2,$2,$3
	bne	$2,$0,$L39
	nop

	b	$L40
	nop

$L41:
	lw	$2,20($fp)
	nop
	sll	$2,$2,2
	lw	$3,40($fp)
	nop
	addu	$2,$3,$2
	sw	$0,0($2)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,1
	sw	$2,20($fp)
$L40:
	lw	$2,20($fp)
	nop
	sltu	$2,$2,42
	bne	$2,$0,$L41
	nop

	lw	$4,64($fp)
	jal	GhostFramesForRegion
	nop

	sw	$2,44($fp)
	lw	$4,68($fp)
	jal	GhostFramesForRegion
	nop

	sw	$2,48($fp)
	lw	$2,24($fp)
	nop
	lw	$2,16($2)
	nop
	move	$3,$2
	move	$2,$3
	sll	$2,$2,1
	addu	$2,$2,$3
	sll	$2,$2,3
	sw	$2,52($fp)
	lw	$6,52($fp)
	lw	$5,44($fp)
	lw	$4,48($fp)
	jal	memcpy
	nop

	nop
	move	$sp,$fp
	lw	$31,60($sp)
	lw	$fp,56($sp)
	addiu	$sp,$sp,64
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	CopyGhostData
	.size	CopyGhostData, .-CopyGhostData
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	ClearGhostHeader
	.type	ClearGhostHeader, @function
ClearGhostHeader:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-16
	sw	$fp,12($sp)
	move	$fp,$sp
	sw	$4,16($fp)
	lw	$2,16($fp)
	nop
	sw	$2,4($fp)
	lw	$2,4($fp)
	nop
	sw	$0,0($2)
	lw	$2,4($fp)
	nop
	sw	$0,4($2)
	lw	$2,4($fp)
	nop
	sw	$0,8($2)
	lw	$2,4($fp)
	nop
	sw	$0,12($2)
	lw	$2,4($fp)
	nop
	sw	$0,16($2)
	lw	$2,4($fp)
	nop
	sw	$0,20($2)
	lw	$2,4($fp)
	nop
	sw	$0,24($2)
	lw	$2,4($fp)
	nop
	sw	$0,28($2)
	lw	$2,4($fp)
	nop
	sw	$0,32($2)
	lw	$2,4($fp)
	nop
	sw	$0,36($2)
	sw	$0,0($fp)
	b	$L43
	nop

$L44:
	lw	$3,4($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,0($2)
	lw	$3,4($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,4($2)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,1
	sw	$2,0($fp)
$L43:
	lw	$2,0($fp)
	nop
	slt	$2,$2,16
	bne	$2,$0,$L44
	nop

	nop
	nop
	move	$sp,$fp
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	ClearGhostHeader
	.size	ClearGhostHeader, .-ClearGhostHeader
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	RecordFrame
	.type	RecordFrame, @function
RecordFrame:
	.frame	$fp,48,$31		# vars= 24, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	move	$fp,$sp
	sw	$4,48($fp)
	sw	$5,52($fp)
	lw	$2,%gp_rel(g_dbg_ghost_record_calls)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_record_calls)($28)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	sltu	$2,$2,15011
	bne	$2,$0,$L46
	nop

	lw	$2,48($fp)
	nop
	sw	$0,0($2)
	lw	$2,48($fp)
	nop
	sw	$0,4($2)
	li	$2,1			# 0x1
	sw	$2,%gp_rel(s_recording_failed_too_long)($28)
	li	$2,180			# 0xb4
	sw	$2,%gp_rel(s_recording_too_long_message_timer)($28)
	b	$L45
	nop

$L46:
	lw	$2,52($fp)
	nop
	sw	$2,16($fp)
	lw	$4,52($fp)
	jal	GhostFramesForRegion
	nop

	sw	$2,20($fp)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	move	$3,$2
	move	$2,$3
	sll	$2,$2,1
	addu	$2,$2,$3
	sll	$2,$2,3
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$2,$3
	sw	$2,24($fp)
	lui	$2,%hi(_spyro)
	lw	$3,%lo(_spyro)($2)
	lw	$2,48($fp)
	nop
	lw	$2,24($2)
	nop
	subu	$2,$3,$2
	sw	$2,28($fp)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lw	$3,4($2)
	lw	$2,48($fp)
	nop
	lw	$2,28($2)
	nop
	subu	$2,$3,$2
	sw	$2,32($fp)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lw	$3,8($2)
	lw	$2,48($fp)
	nop
	lw	$2,32($2)
	nop
	subu	$2,$3,$2
	sw	$2,36($fp)
	lw	$2,28($fp)
	nop
	sll	$3,$2,16
	sra	$3,$3,16
	lw	$2,24($fp)
	nop
	sh	$3,0($2)
	lw	$2,32($fp)
	nop
	sll	$3,$2,16
	sra	$3,$3,16
	lw	$2,24($fp)
	nop
	sh	$3,2($2)
	lw	$2,36($fp)
	nop
	sll	$3,$2,16
	sra	$3,$3,16
	lw	$2,24($fp)
	nop
	sh	$3,4($2)
	lw	$2,24($fp)
	nop
	addiu	$3,$2,20
	li	$6,4			# 0x4
	lui	$2,%hi(_spyro+12)
	addiu	$5,$2,%lo(_spyro+12)
	move	$4,$3
	jal	memcpy
	nop

	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,24($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,6
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,25($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,7
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,30($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,8
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,31($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,9
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,88($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,10
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,26($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,11
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,27($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,12
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,32($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,13
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,33($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,14
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lui	$2,%hi(_spyro)
	addiu	$2,$2,%lo(_spyro)
	lb	$3,91($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,15
	andi	$3,$3,0x00ff
	sb	$3,0($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,16
	sb	$0,0($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,17
	sb	$0,0($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,18
	sb	$0,0($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,19
	sb	$0,0($2)
	lw	$2,48($fp)
	lui	$3,%hi(_spyro)
	lw	$5,%lo(_spyro)($3)
	addiu	$4,$3,%lo(_spyro)
	lw	$4,4($4)
	addiu	$3,$3,%lo(_spyro)
	lw	$3,8($3)
	sw	$5,24($2)
	sw	$4,28($2)
	sw	$3,32($2)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	addiu	$3,$2,1
	lw	$2,48($fp)
	nop
	sw	$3,8($2)
	lw	$2,48($fp)
	nop
	lw	$3,8($2)
	lw	$2,16($fp)
	nop
	sw	$3,16($2)
$L45:
	move	$sp,$fp
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	RecordFrame
	.size	RecordFrame, .-RecordFrame
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	PlaybackFrame
	.type	PlaybackFrame, @function
PlaybackFrame:
	.frame	$fp,48,$31		# vars= 24, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	move	$fp,$sp
	sw	$4,48($fp)
	sw	$5,52($fp)
	sw	$6,56($fp)
	lw	$2,52($fp)
	nop
	sw	$2,16($fp)
	lw	$2,48($fp)
	nop
	lw	$3,8($2)
	lw	$2,16($fp)
	nop
	lw	$2,16($2)
	nop
	slt	$2,$3,$2
	beq	$2,$0,$L55
	nop

	lw	$4,52($fp)
	jal	GhostFramesForRegion
	nop

	sw	$2,20($fp)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	move	$3,$2
	move	$2,$3
	sll	$2,$2,1
	addu	$2,$2,$3
	sll	$2,$2,3
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$2,$3
	sw	$2,24($fp)
	lw	$2,48($fp)
	nop
	lw	$2,12($2)
	lw	$3,24($fp)
	nop
	lh	$3,0($3)
	nop
	addu	$3,$2,$3
	lw	$2,48($fp)
	nop
	sw	$3,12($2)
	lw	$2,48($fp)
	nop
	lw	$2,16($2)
	lw	$3,24($fp)
	nop
	lh	$3,2($3)
	nop
	addu	$3,$2,$3
	lw	$2,48($fp)
	nop
	sw	$3,16($2)
	lw	$2,48($fp)
	nop
	lw	$2,20($2)
	lw	$3,24($fp)
	nop
	lh	$3,4($3)
	nop
	addu	$3,$2,$3
	lw	$2,48($fp)
	nop
	sw	$3,20($2)
	lw	$2,56($fp)
	lw	$3,48($fp)
	nop
	lw	$5,12($3)
	lw	$4,16($3)
	lw	$3,20($3)
	sw	$5,0($2)
	sw	$4,4($2)
	sw	$3,8($2)
	lw	$2,56($fp)
	nop
	addiu	$3,$2,12
	lw	$2,24($fp)
	nop
	addiu	$2,$2,20
	li	$6,4			# 0x4
	move	$5,$2
	move	$4,$3
	jal	memcpy
	nop

	lw	$2,24($fp)
	nop
	addiu	$2,$2,6
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,24($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,7
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,25($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,8
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,30($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,9
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,31($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,10
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,88($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,11
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,26($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,12
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,27($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,13
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,32($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,14
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,33($2)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,15
	lbu	$2,0($2)
	nop
	sll	$3,$2,24
	sra	$3,$3,24
	lw	$2,56($fp)
	nop
	sb	$3,91($2)
	lw	$2,56($fp)
	nop
	sb	$0,36($2)
	lw	$2,56($fp)
	nop
	sb	$0,37($2)
	lw	$2,56($fp)
	nop
	sb	$0,38($2)
	lw	$2,24($fp)
	nop
	lbu	$2,6($2)
	nop
	sb	$2,28($fp)
	lw	$2,24($fp)
	nop
	lbu	$2,7($2)
	nop
	sb	$2,29($fp)
	lw	$2,24($fp)
	nop
	lbu	$2,11($2)
	nop
	sb	$2,30($fp)
	lw	$2,24($fp)
	nop
	lbu	$2,12($2)
	nop
	sb	$2,31($fp)
	lbu	$3,28($fp)
	li	$2,44			# 0x2c
	beq	$3,$2,$L51
	nop

	lbu	$3,29($fp)
	li	$2,44			# 0x2c
	beq	$3,$2,$L51
	nop

	lbu	$3,30($fp)
	li	$2,44			# 0x2c
	beq	$3,$2,$L51
	nop

	lbu	$3,31($fp)
	li	$2,44			# 0x2c
	beq	$3,$2,$L51
	nop

	lbu	$3,28($fp)
	li	$2,45			# 0x2d
	beq	$3,$2,$L51
	nop

	lbu	$3,29($fp)
	li	$2,45			# 0x2d
	beq	$3,$2,$L51
	nop

	lbu	$3,30($fp)
	li	$2,45			# 0x2d
	beq	$3,$2,$L51
	nop

	lbu	$3,31($fp)
	li	$2,45			# 0x2d
	bne	$3,$2,$L52
	nop

$L51:
	li	$2,1			# 0x1
	b	$L53
	nop

$L52:
	move	$2,$0
$L53:
	sw	$2,32($fp)
	lw	$2,32($fp)
	nop
	beq	$2,$0,$L54
	nop

	lw	$2,56($fp)
	li	$3,11			# 0xb
	sb	$3,24($2)
	lw	$2,56($fp)
	li	$3,11			# 0xb
	sb	$3,25($2)
	lw	$2,56($fp)
	nop
	sb	$0,30($2)
	lw	$2,56($fp)
	nop
	sb	$0,31($2)
	lw	$2,56($fp)
	li	$3,11			# 0xb
	sb	$3,26($2)
	lw	$2,56($fp)
	li	$3,11			# 0xb
	sb	$3,27($2)
	lw	$2,56($fp)
	nop
	sb	$0,32($2)
	lw	$2,56($fp)
	nop
	sb	$0,33($2)
$L54:
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	addiu	$3,$2,1
	lw	$2,48($fp)
	nop
	sw	$3,8($2)
	b	$L48
	nop

$L55:
	nop
$L48:
	move	$sp,$fp
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	PlaybackFrame
	.size	PlaybackFrame, .-PlaybackFrame
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	UpdateRecordingDragonEvent
	.type	UpdateRecordingDragonEvent, @function
UpdateRecordingDragonEvent:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	lw	$2,32($fp)
	nop
	sw	$2,16($fp)
	lw	$4,16($fp)
	jal	GhostHeaderIsV2
	nop

	beq	$2,$0,$L62
	nop

	lw	$2,%gp_rel(s_recording_was_in_dragon)($28)
	nop
	bne	$2,$0,$L59
	nop

	lw	$4,16($fp)
	jal	GhostDragonEventCount
	nop

	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	slt	$2,$2,16
	bne	$2,$0,$L60
	nop

	li	$2,-1			# 0xffffffffffffffff
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	b	$L61
	nop

$L60:
	lw	$2,20($fp)
	nop
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	sw	$0,%gp_rel(s_recording_dragon_pause_frames)($28)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$3,8($2)
	lw	$4,16($fp)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$4,$2
	sw	$3,0($2)
	lw	$3,16($fp)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,4($2)
	lw	$2,20($fp)
	nop
	addiu	$3,$2,1
	lw	$2,16($fp)
	nop
	sw	$3,36($2)
$L61:
	li	$2,1			# 0x1
	sw	$2,%gp_rel(s_recording_was_in_dragon)($28)
$L59:
	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	nop
	bltz	$2,$L56
	nop

	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	nop
	slt	$2,$2,16
	beq	$2,$0,$L56
	nop

	lw	$2,%gp_rel(s_recording_dragon_pause_frames)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(s_recording_dragon_pause_frames)($28)
	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	lw	$3,%gp_rel(s_recording_dragon_pause_frames)($28)
	lw	$4,16($fp)
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$4,$2
	sw	$3,4($2)
	b	$L56
	nop

$L62:
	nop
$L56:
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	UpdateRecordingDragonEvent
	.size	UpdateRecordingDragonEvent, .-UpdateRecordingDragonEvent
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	FinishRecordingDragonEvent
	.type	FinishRecordingDragonEvent, @function
FinishRecordingDragonEvent:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$0,%gp_rel(s_recording_was_in_dragon)($28)
	li	$2,-1			# 0xffffffffffffffff
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	sw	$0,%gp_rel(s_recording_dragon_pause_frames)($28)
	nop
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	FinishRecordingDragonEvent
	.size	FinishRecordingDragonEvent, .-FinishRecordingDragonEvent
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostPlaybackDragonPauseActive
	.type	GhostPlaybackDragonPauseActive, @function
GhostPlaybackDragonPauseActive:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	lw	$2,40($fp)
	nop
	sw	$2,16($fp)
	lw	$4,16($fp)
	jal	GhostDragonEventCount
	nop

	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	bgtz	$2,$L67
	nop

	move	$2,$0
	b	$L66
	nop

$L70:
	lw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	lw	$3,16($fp)
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	lw	$2,0($2)
	nop
	sw	$2,24($fp)
	lw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	lw	$3,16($fp)
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	lw	$2,4($2)
	nop
	sw	$2,28($fp)
	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,8($2)
	lw	$3,24($fp)
	nop
	slt	$2,$2,$3
	bne	$2,$0,$L72
	nop

	lw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,8($2)
	lw	$3,24($fp)
	nop
	bne	$3,$2,$L67
	nop

	lw	$2,28($fp)
	nop
	blez	$2,$L67
	nop

	lw	$2,28($fp)
	nop
	sw	$2,%gp_rel(s_playback_dragon_pause_remaining)($28)
	b	$L69
	nop

$L67:
	lw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	lw	$3,20($fp)
	nop
	slt	$2,$2,$3
	bne	$2,$0,$L70
	nop

	b	$L69
	nop

$L72:
	nop
$L69:
	lw	$2,%gp_rel(s_playback_dragon_pause_remaining)($28)
	nop
	blez	$2,$L71
	nop

	lw	$2,%gp_rel(s_playback_dragon_pause_remaining)($28)
	nop
	addiu	$2,$2,-1
	sw	$2,%gp_rel(s_playback_dragon_pause_remaining)($28)
	li	$2,1			# 0x1
	b	$L66
	nop

$L71:
	move	$2,$0
$L66:
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostPlaybackDragonPauseActive
	.size	GhostPlaybackDragonPauseActive, .-GhostPlaybackDragonPauseActive
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	StartGhostOnFlyIn
	.type	StartGhostOnFlyIn, @function
StartGhostOnFlyIn:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	lw	$2,%gp_rel(g_dbg_ghost_start_calls)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_start_calls)($28)
	lw	$2,40($fp)
	nop
	beq	$2,$0,$L74
	nop

	lw	$2,%gp_rel(g_dbg_ghost_start_insta_calls)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_start_insta_calls)($28)
	b	$L75
	nop

$L74:
	lw	$2,%gp_rel(g_dbg_ghost_start_reg_calls)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_start_reg_calls)($28)
$L75:
	li	$2,-2141192192			# 0xffffffff80600000
	sw	$2,16($fp)
	li	$2,-2140864512			# 0xffffffff80650000
	ori	$2,$2,0x8000
	sw	$2,20($fp)
	lui	$2,%hi(g_ghostA)
	addiu	$4,$2,%lo(g_ghostA)
	jal	StopGhost
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StopGhost
	nop

	lb	$2,%gp_rel(_levelID)($28)
	nop
	move	$5,$2
	lw	$4,16($fp)
	jal	GhostHeaderMatchesCompletedLevel
	nop

	sw	$2,24($fp)
	lw	$2,40($fp)
	nop
	bne	$2,$0,$L76
	nop

	lb	$3,%gp_rel(_levelID)($28)
	li	$2,10			# 0xa
	div	$0,$3,$2
	bne	$2,$0,1f
	nop
	break	7
1:
	mfhi	$2
	sll	$2,$2,24
	sra	$2,$2,24
	beq	$2,$0,$L77
	nop

$L76:
	li	$2,1			# 0x1
	b	$L78
	nop

$L77:
	move	$2,$0
$L78:
	sw	$2,28($fp)
	lw	$2,24($fp)
	nop
	beq	$2,$0,$L79
	nop

	li	$5,-2141192192			# 0xffffffff80600000
	lui	$2,%hi(g_ghostA)
	addiu	$4,$2,%lo(g_ghostA)
	jal	StartPlayback
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$5,$2,0x8000
	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StartRecording
	nop

	b	$L81
	nop

$L79:
	lw	$2,28($fp)
	nop
	beq	$2,$0,$L81
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$4,$2,0x8000
	jal	ClearGhostHeader
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$5,$2,0x8000
	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StartRecording
	nop

$L81:
	nop
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	StartGhostOnFlyIn
	.size	StartGhostOnFlyIn, .-StartGhostOnFlyIn
	.align	2
	.globl	GhostButtonCheck
	.set	nomips16
	.set	nomicromips
	.ent	GhostButtonCheck
	.type	GhostButtonCheck, @function
GhostButtonCheck:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L86
	nop

	li	$6,608			# 0x260
	lui	$2,%hi(_spyro)
	addiu	$5,$2,%lo(_spyro)
	li	$4,-2142240768			# 0xffffffff80500000
	jal	memcpy
	nop

	lw	$2,%gp_rel(has_started_insta_fly_in)($28)
	nop
	beq	$2,$0,$L85
	nop

	lui	$2,%hi(il_menu)
	addiu	$2,$2,%lo(il_menu)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L85
	nop

	li	$4,1			# 0x1
	jal	StartGhostOnFlyIn
	nop

	b	$L82
	nop

$L85:
	lw	$2,%gp_rel(has_started_reg_fly_in)($28)
	nop
	beq	$2,$0,$L82
	nop

	lui	$2,%hi(il_menu)
	addiu	$2,$2,%lo(il_menu)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L82
	nop

	move	$4,$0
	jal	StartGhostOnFlyIn
	nop

	b	$L82
	nop

$L86:
	nop
$L82:
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostButtonCheck
	.size	GhostButtonCheck, .-GhostButtonCheck
	.align	2
	.globl	GhostFrameTick
	.set	nomips16
	.set	nomicromips
	.ent	GhostFrameTick
	.type	GhostFrameTick, @function
GhostFrameTick:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	lw	$2,%gp_rel(g_dbg_ghost_update_calls)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_update_calls)($28)
	sw	$0,%gp_rel(s_ghost_render_frame_ready)($28)
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L94
	nop

	lb	$2,%gp_rel(_gameState)($28)
	nop
	xori	$2,$2,0x8
	sltu	$2,$2,1
	andi	$2,$2,0x00ff
	sw	$2,16($fp)
	li	$2,-2142240768			# 0xffffffff80500000
	sw	$2,20($fp)
	lw	$2,16($fp)
	nop
	beq	$2,$0,$L90
	nop

	lw	$2,%gp_rel(g_dbg_ghost_update_dragon_skips)($28)
	nop
	addiu	$2,$2,1
	sw	$2,%gp_rel(g_dbg_ghost_update_dragon_skips)($28)
$L90:
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L91
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L91
	nop

	lw	$2,16($fp)
	nop
	beq	$2,$0,$L92
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$4,$2,0x8000
	jal	UpdateRecordingDragonEvent
	nop

	b	$L91
	nop

$L92:
	jal	FinishRecordingDragonEvent
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$5,$2,0x8000
	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	RecordFrame
	nop

$L91:
	lui	$2,%hi(g_ghostA)
	lw	$2,%lo(g_ghostA)($2)
	nop
	beq	$2,$0,$L87
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,4($2)
	nop
	bne	$2,$0,$L87
	nop

	li	$2,-2141192192			# 0xffffffff80600000
	sw	$2,24($fp)
	lw	$2,16($fp)
	nop
	beq	$2,$0,$L93
	nop

	lw	$4,24($fp)
	jal	GhostHeaderIsV2
	nop

	beq	$2,$0,$L87
	nop

$L93:
	li	$4,-2141192192			# 0xffffffff80600000
	jal	GhostPlaybackDragonPauseActive
	nop

	bne	$2,$0,$L87
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$3,8($2)
	lw	$2,24($fp)
	nop
	lw	$2,16($2)
	nop
	slt	$2,$3,$2
	beq	$2,$0,$L87
	nop

	lw	$6,20($fp)
	li	$5,-2141192192			# 0xffffffff80600000
	lui	$2,%hi(g_ghostA)
	addiu	$4,$2,%lo(g_ghostA)
	jal	PlaybackFrame
	nop

	li	$2,1			# 0x1
	sw	$2,%gp_rel(s_ghost_render_frame_ready)($28)
	b	$L87
	nop

$L94:
	nop
$L87:
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostFrameTick
	.size	GhostFrameTick, .-GhostFrameTick
	.align	2
	.globl	GhostOnRunComplete
	.set	nomips16
	.set	nomicromips
	.ent	GhostOnRunComplete
	.type	GhostOnRunComplete, @function
GhostOnRunComplete:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	lw	$2,%gp_rel(s_recording_failed_too_long)($28)
	nop
	beq	$2,$0,$L96
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StopGhost
	nop

	sw	$0,%gp_rel(s_recording_failed_too_long)($28)
	li	$2,180			# 0xb4
	sw	$2,%gp_rel(s_recording_too_long_message_timer)($28)
	b	$L95
	nop

$L96:
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L95
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L95
	nop

	li	$2,-2141192192			# 0xffffffff80600000
	sw	$2,16($fp)
	li	$2,-2140864512			# 0xffffffff80650000
	ori	$2,$2,0x8000
	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	lw	$2,12($2)
	nop
	move	$5,$2
	lw	$4,16($fp)
	jal	GhostHeaderMatchesCompletedLevel
	nop

	sw	$2,24($fp)
	lw	$2,20($fp)
	lw	$3,40($fp)
	nop
	sw	$3,20($2)
	lw	$2,24($fp)
	nop
	beq	$2,$0,$L98
	nop

	lw	$2,16($fp)
	nop
	lw	$2,20($2)
	lw	$3,40($fp)
	nop
	slt	$2,$3,$2
	beq	$2,$0,$L99
	nop

$L98:
	li	$5,-2141192192			# 0xffffffff80600000
	li	$2,-2140864512			# 0xffffffff80650000
	ori	$4,$2,0x8000
	jal	CopyGhostData
	nop

	li	$2,1			# 0x1
	sw	$2,%gp_rel(g_ghost_new_best)($28)
$L99:
	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StopGhost
	nop

$L95:
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostOnRunComplete
	.size	GhostOnRunComplete, .-GhostOnRunComplete
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	PatchDrawSpyroForGhost
	.type	PatchDrawSpyroForGhost, @function
PatchDrawSpyroForGhost:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x3afc
	li	$3,1008664576			# 0x3c1f0000
	ori	$3,$3,0x8050
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x3b00
	li	$3,671023104			# 0x27ff0000
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4424
	li	$3,1008271360			# 0x3c190000
	ori	$3,$3,0x8050
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4428
	li	$3,658046976			# 0x27390000
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4a5c
	li	$3,1006698496			# 0x3c010000
	ori	$3,$3,0x8050
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4a60
	li	$3,606142464			# 0x24210000
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4b60
	li	$3,1006895104			# 0x3c040000
	ori	$3,$3,0x8050
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4b64
	li	$3,612630528			# 0x24840000
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4110
	sw	$0,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4114
	sw	$0,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4118
	sw	$0,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x411c
	sw	$0,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4120
	sw	$0,0($2)
	nop
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	PatchDrawSpyroForGhost
	.size	PatchDrawSpyroForGhost, .-PatchDrawSpyroForGhost
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	RestoreDrawSpyro
	.type	RestoreDrawSpyro, @function
RestoreDrawSpyro:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x3afc
	li	$3,1008664576			# 0x3c1f0000
	ori	$3,$3,0x8008
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x3b00
	li	$3,671023104			# 0x27ff0000
	ori	$3,$3,0x8a58
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4424
	li	$3,1008271360			# 0x3c190000
	ori	$3,$3,0x8008
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4428
	li	$3,658046976			# 0x27390000
	ori	$3,$3,0x8a58
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4a5c
	li	$3,1006698496			# 0x3c010000
	ori	$3,$3,0x8008
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4a60
	li	$3,606142464			# 0x24210000
	ori	$3,$3,0x8a58
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4b60
	li	$3,1006895104			# 0x3c040000
	ori	$3,$3,0x8008
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4b64
	li	$3,612630528			# 0x24840000
	ori	$3,$3,0x8a58
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4110
	li	$3,-1406730240			# 0xffffffffac270000
	ori	$3,$3,0xb8
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4114
	li	$3,-1406664704			# 0xffffffffac280000
	ori	$3,$3,0xbc
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4118
	li	$3,-1406599168			# 0xffffffffac290000
	ori	$3,$3,0xc0
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x411c
	li	$3,-1406533632			# 0xffffffffac2a0000
	ori	$3,$3,0xc4
	sw	$3,0($2)
	li	$2,-2147352576			# 0xffffffff80020000
	ori	$2,$2,0x4120
	li	$3,-1406468096			# 0xffffffffac2b0000
	ori	$3,$3,0xc8
	sw	$3,0($2)
	nop
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	RestoreDrawSpyro
	.size	RestoreDrawSpyro, .-RestoreDrawSpyro
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	MakeGhostDrawSpyroPacketsTransparent
	.type	MakeGhostDrawSpyroPacketsTransparent, @function
MakeGhostDrawSpyroPacketsTransparent:
	.frame	$fp,88,$31		# vars= 80, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-88
	sw	$fp,84($sp)
	move	$fp,$sp
	sw	$4,88($fp)
	sw	$5,92($fp)
	lw	$2,88($fp)
	nop
	sw	$2,0($fp)
	b	$L103
	nop

$L109:
	lw	$2,0($fp)
	nop
	addiu	$2,$2,3
	lbu	$2,0($2)
	nop
	sw	$2,4($fp)
	lw	$2,4($fp)
	nop
	addiu	$2,$2,1
	sll	$2,$2,2
	sw	$2,8($fp)
	lw	$2,8($fp)
	nop
	blez	$2,$L110
	nop

	lw	$2,8($fp)
	lw	$3,0($fp)
	nop
	addu	$2,$3,$2
	lw	$3,92($fp)
	nop
	sltu	$2,$3,$2
	bne	$2,$0,$L111
	nop

	lw	$2,0($fp)
	nop
	addiu	$2,$2,7
	lbu	$3,0($2)
	li	$2,52			# 0x34
	bne	$3,$2,$L107
	nop

	lw	$2,0($fp)
	nop
	sw	$2,48($fp)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,4
	lw	$3,0($2)
	li	$2,16711680			# 0xff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	sw	$2,52($fp)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	sw	$2,56($fp)
	lw	$2,48($fp)
	nop
	lw	$2,16($2)
	nop
	sw	$2,60($fp)
	lw	$2,48($fp)
	nop
	lw	$2,20($2)
	nop
	sw	$2,64($fp)
	lw	$2,48($fp)
	nop
	lw	$2,28($2)
	nop
	sw	$2,68($fp)
	lw	$2,48($fp)
	nop
	lw	$2,32($2)
	nop
	sw	$2,72($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,3
	li	$3,7			# 0x7
	sb	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,4
	lw	$3,%gp_rel(DR_MODE_50_BLEND.1)($28)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,8
	lw	$4,52($fp)
	li	$3,838860800			# 0x32000000
	or	$3,$4,$3
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,12
	lw	$3,56($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,16
	lw	$3,60($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,20
	lw	$3,64($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,24
	lw	$3,68($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,28
	lw	$3,72($fp)
	nop
	sw	$3,0($2)
	b	$L108
	nop

$L107:
	lw	$2,0($fp)
	nop
	addiu	$2,$2,7
	lbu	$3,0($2)
	li	$2,60			# 0x3c
	bne	$3,$2,$L108
	nop

	lw	$2,0($fp)
	nop
	sw	$2,12($fp)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,4
	lw	$3,0($2)
	li	$2,16711680			# 0xff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	sw	$2,16($fp)
	lw	$2,12($fp)
	nop
	lw	$2,8($2)
	nop
	sw	$2,20($fp)
	lw	$2,12($fp)
	nop
	lw	$2,16($2)
	nop
	sw	$2,24($fp)
	lw	$2,12($fp)
	nop
	lw	$2,20($2)
	nop
	sw	$2,28($fp)
	lw	$2,12($fp)
	nop
	lw	$2,28($2)
	nop
	sw	$2,32($fp)
	lw	$2,12($fp)
	nop
	lw	$2,32($2)
	nop
	sw	$2,36($fp)
	lw	$2,12($fp)
	nop
	lw	$2,40($2)
	nop
	sw	$2,40($fp)
	lw	$2,12($fp)
	nop
	lw	$2,44($2)
	nop
	sw	$2,44($fp)
	lw	$2,0($fp)
	nop
	addiu	$2,$2,3
	li	$3,9			# 0x9
	sb	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,4
	lw	$3,%gp_rel(DR_MODE_50_BLEND.1)($28)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,8
	lw	$4,16($fp)
	li	$3,973078528			# 0x3a000000
	or	$3,$4,$3
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,12
	lw	$3,20($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,16
	lw	$3,24($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,20
	lw	$3,28($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,24
	lw	$3,32($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,28
	lw	$3,36($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,32
	lw	$3,40($fp)
	nop
	sw	$3,0($2)
	lw	$2,12($fp)
	nop
	addiu	$2,$2,36
	lw	$3,44($fp)
	nop
	sw	$3,0($2)
$L108:
	lw	$2,8($fp)
	lw	$3,0($fp)
	nop
	addu	$2,$3,$2
	sw	$2,0($fp)
$L103:
	lw	$3,0($fp)
	lw	$2,92($fp)
	nop
	sltu	$2,$3,$2
	bne	$2,$0,$L109
	nop

	b	$L112
	nop

$L110:
	nop
	b	$L112
	nop

$L111:
	nop
$L112:
	nop
	move	$sp,$fp
	lw	$fp,84($sp)
	addiu	$sp,$sp,88
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	MakeGhostDrawSpyroPacketsTransparent
	.size	MakeGhostDrawSpyroPacketsTransparent, .-MakeGhostDrawSpyroPacketsTransparent
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostWireEdgeLenSq
	.type	GhostWireEdgeLenSq, @function
GhostWireEdgeLenSq:
	.frame	$fp,32,$31		# vars= 24, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$fp,28($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	sw	$5,36($fp)
	lw	$2,32($fp)
	nop
	sll	$2,$2,16
	sra	$2,$2,16
	sw	$2,0($fp)
	lw	$2,32($fp)
	nop
	srl	$2,$2,16
	sll	$2,$2,16
	sra	$2,$2,16
	sw	$2,4($fp)
	lw	$2,36($fp)
	nop
	sll	$2,$2,16
	sra	$2,$2,16
	sw	$2,8($fp)
	lw	$2,36($fp)
	nop
	srl	$2,$2,16
	sll	$2,$2,16
	sra	$2,$2,16
	sw	$2,12($fp)
	lw	$3,8($fp)
	lw	$2,0($fp)
	nop
	subu	$2,$3,$2
	sw	$2,16($fp)
	lw	$3,12($fp)
	lw	$2,4($fp)
	nop
	subu	$2,$3,$2
	sw	$2,20($fp)
	lw	$2,16($fp)
	nop
	mult	$2,$2
	mflo	$3
	lw	$2,20($fp)
	nop
	mult	$2,$2
	mflo	$2
	addu	$2,$3,$2
	move	$sp,$fp
	lw	$fp,28($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostWireEdgeLenSq
	.size	GhostWireEdgeLenSq, .-GhostWireEdgeLenSq
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostWireMaxLenSq3
	.type	GhostWireMaxLenSq3, @function
GhostWireMaxLenSq3:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	sw	$5,44($fp)
	sw	$6,48($fp)
	lw	$5,44($fp)
	lw	$4,40($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,20($fp)
	lw	$5,48($fp)
	lw	$4,44($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,24($fp)
	lw	$5,40($fp)
	lw	$4,48($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,28($fp)
	lw	$2,20($fp)
	nop
	sw	$2,16($fp)
	lw	$3,24($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L116
	nop

	lw	$2,24($fp)
	nop
	sw	$2,16($fp)
$L116:
	lw	$3,28($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L117
	nop

	lw	$2,28($fp)
	nop
	sw	$2,16($fp)
$L117:
	lw	$2,16($fp)
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostWireMaxLenSq3
	.size	GhostWireMaxLenSq3, .-GhostWireMaxLenSq3
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostWireMaxLenSq4
	.type	GhostWireMaxLenSq4, @function
GhostWireMaxLenSq4:
	.frame	$fp,48,$31		# vars= 24, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	move	$fp,$sp
	sw	$4,48($fp)
	sw	$5,52($fp)
	sw	$6,56($fp)
	sw	$7,60($fp)
	lw	$5,52($fp)
	lw	$4,48($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,20($fp)
	lw	$5,56($fp)
	lw	$4,52($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,24($fp)
	lw	$5,60($fp)
	lw	$4,56($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,28($fp)
	lw	$5,48($fp)
	lw	$4,60($fp)
	jal	GhostWireEdgeLenSq
	nop

	sw	$2,32($fp)
	lw	$2,20($fp)
	nop
	sw	$2,16($fp)
	lw	$3,24($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L120
	nop

	lw	$2,24($fp)
	nop
	sw	$2,16($fp)
$L120:
	lw	$3,28($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L121
	nop

	lw	$2,28($fp)
	nop
	sw	$2,16($fp)
$L121:
	lw	$3,32($fp)
	lw	$2,16($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L122
	nop

	lw	$2,32($fp)
	nop
	sw	$2,16($fp)
$L122:
	lw	$2,16($fp)
	move	$sp,$fp
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostWireMaxLenSq4
	.size	GhostWireMaxLenSq4, .-GhostWireMaxLenSq4
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	MakeGhostDrawSpyroPacketsWireframe
	.type	MakeGhostDrawSpyroPacketsWireframe, @function
MakeGhostDrawSpyroPacketsWireframe:
	.frame	$fp,72,$31		# vars= 48, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$fp,64($sp)
	move	$fp,$sp
	sw	$4,72($fp)
	sw	$5,76($fp)
	lw	$2,72($fp)
	nop
	sw	$2,16($fp)
	b	$L125
	nop

$L133:
	lw	$2,16($fp)
	nop
	addiu	$2,$2,3
	lbu	$2,0($2)
	nop
	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,1
	sll	$2,$2,2
	sw	$2,24($fp)
	lw	$2,24($fp)
	nop
	blez	$2,$L134
	nop

	lw	$2,24($fp)
	lw	$3,16($fp)
	nop
	addu	$2,$3,$2
	lw	$3,76($fp)
	nop
	sltu	$2,$3,$2
	bne	$2,$0,$L135
	nop

	lw	$2,16($fp)
	nop
	addiu	$2,$2,7
	lbu	$3,0($2)
	li	$2,52			# 0x34
	bne	$3,$2,$L129
	nop

	lw	$2,16($fp)
	nop
	sw	$2,48($fp)
	lw	$2,48($fp)
	nop
	lw	$2,8($2)
	nop
	sw	$2,52($fp)
	lw	$2,48($fp)
	nop
	lw	$2,20($2)
	nop
	sw	$2,56($fp)
	lw	$2,48($fp)
	nop
	lw	$2,32($2)
	nop
	sw	$2,60($fp)
	lw	$6,60($fp)
	lw	$5,56($fp)
	lw	$4,52($fp)
	jal	GhostWireMaxLenSq3
	nop

	slt	$2,$2,64
	beq	$2,$0,$L130
	nop

	lw	$2,16($fp)
	nop
	addiu	$2,$2,3
	sb	$0,0($2)
	b	$L131
	nop

$L130:
	lw	$2,16($fp)
	nop
	addiu	$2,$2,3
	li	$3,7			# 0x7
	sb	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,4
	lw	$3,%gp_rel(DR_MODE_50_BLEND.0)($28)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,8
	li	$3,1258225664			# 0x4aff0000
	ori	$3,$3,0xe0e0
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,12
	lw	$3,52($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,16
	lw	$3,56($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,20
	lw	$3,60($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,24
	lw	$3,52($fp)
	nop
	sw	$3,0($2)
	lw	$2,48($fp)
	nop
	addiu	$2,$2,28
	li	$3,1431633920			# 0x55550000
	ori	$3,$3,0x5555
	sw	$3,0($2)
	b	$L131
	nop

$L129:
	lw	$2,16($fp)
	nop
	addiu	$2,$2,7
	lbu	$3,0($2)
	li	$2,60			# 0x3c
	bne	$3,$2,$L131
	nop

	lw	$2,16($fp)
	nop
	sw	$2,28($fp)
	lw	$2,28($fp)
	nop
	lw	$2,8($2)
	nop
	sw	$2,32($fp)
	lw	$2,28($fp)
	nop
	lw	$2,20($2)
	nop
	sw	$2,36($fp)
	lw	$2,28($fp)
	nop
	lw	$2,32($2)
	nop
	sw	$2,40($fp)
	lw	$2,28($fp)
	nop
	lw	$2,44($2)
	nop
	sw	$2,44($fp)
	lw	$7,40($fp)
	lw	$6,44($fp)
	lw	$5,36($fp)
	lw	$4,32($fp)
	jal	GhostWireMaxLenSq4
	nop

	slt	$2,$2,64
	beq	$2,$0,$L132
	nop

	lw	$2,16($fp)
	nop
	addiu	$2,$2,3
	sb	$0,0($2)
	b	$L131
	nop

$L132:
	lw	$2,16($fp)
	nop
	addiu	$2,$2,3
	li	$3,8			# 0x8
	sb	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,4
	lw	$3,%gp_rel(DR_MODE_50_BLEND.0)($28)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,8
	li	$3,1258225664			# 0x4aff0000
	ori	$3,$3,0xe0e0
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,12
	lw	$3,32($fp)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,16
	lw	$3,36($fp)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,20
	lw	$3,44($fp)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,24
	lw	$3,40($fp)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,28
	lw	$3,32($fp)
	nop
	sw	$3,0($2)
	lw	$2,28($fp)
	nop
	addiu	$2,$2,32
	li	$3,1431633920			# 0x55550000
	ori	$3,$3,0x5555
	sw	$3,0($2)
$L131:
	lw	$2,24($fp)
	lw	$3,16($fp)
	nop
	addu	$2,$3,$2
	sw	$2,16($fp)
$L125:
	lw	$3,16($fp)
	lw	$2,76($fp)
	nop
	sltu	$2,$3,$2
	bne	$2,$0,$L133
	nop

	b	$L136
	nop

$L134:
	nop
	b	$L136
	nop

$L135:
	nop
$L136:
	nop
	move	$sp,$fp
	lw	$31,68($sp)
	lw	$fp,64($sp)
	addiu	$sp,$sp,72
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	MakeGhostDrawSpyroPacketsWireframe
	.size	MakeGhostDrawSpyroPacketsWireframe, .-MakeGhostDrawSpyroPacketsWireframe
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	FormatFramesAsMinSec
	.type	FormatFramesAsMinSec, @function
FormatFramesAsMinSec:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	sw	$5,44($fp)
	lw	$3,40($fp)
	li	$2,30			# 0x1e
	div	$0,$3,$2
	bne	$2,$0,1f
	nop
	break	7
1:
	mfhi	$2
	mflo	$2
	sw	$2,16($fp)
	lw	$2,16($fp)
	nop
	bgez	$2,$L138
	nop

	sw	$0,16($fp)
$L138:
	lw	$3,16($fp)
	li	$2,60			# 0x3c
	div	$0,$3,$2
	bne	$2,$0,1f
	nop
	break	7
1:
	mfhi	$2
	mflo	$2
	sw	$2,20($fp)
	lw	$3,16($fp)
	li	$2,60			# 0x3c
	div	$0,$3,$2
	bne	$2,$0,1f
	nop
	break	7
1:
	mfhi	$2
	sw	$2,24($fp)
	lw	$7,24($fp)
	lw	$6,20($fp)
	lui	$2,%hi(deckard_str_fmt_min_sec)
	addiu	$5,$2,%lo(deckard_str_fmt_min_sec)
	lw	$4,44($fp)
	jal	sprintf
	nop

	nop
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	FormatFramesAsMinSec
	.size	FormatFramesAsMinSec, .-FormatFramesAsMinSec
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	GhostDrawRecordingTimeHud
	.type	GhostDrawRecordingTimeHud, @function
GhostDrawRecordingTimeHud:
	.frame	$fp,64,$31		# vars= 40, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$fp,56($sp)
	move	$fp,$sp
	li	$2,15011			# 0x3aa3
	sw	$2,20($fp)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,8($2)
	nop
	sw	$2,16($fp)
	lw	$2,16($fp)
	nop
	bgez	$2,$L140
	nop

	sw	$0,16($fp)
$L140:
	lw	$3,16($fp)
	lw	$2,20($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L141
	nop

	lw	$2,20($fp)
	nop
	sw	$2,16($fp)
$L141:
	addiu	$2,$fp,24
	move	$5,$2
	lw	$4,16($fp)
	jal	FormatFramesAsMinSec
	nop

	addiu	$2,$fp,40
	move	$5,$2
	lw	$4,20($fp)
	jal	FormatFramesAsMinSec
	nop

	addiu	$3,$fp,40
	addiu	$2,$fp,24
	move	$7,$3
	move	$6,$2
	lui	$2,%hi(deckard_str_fmt_s_dash_s)
	addiu	$5,$2,%lo(deckard_str_fmt_s_dash_s)
	lui	$2,%hi(g_ghost_rec_time_line)
	addiu	$4,$2,%lo(g_ghost_rec_time_line)
	jal	sprintf
	nop

	li	$6,200			# 0xc8
	li	$5,8			# 0x8
	lui	$2,%hi(g_ghost_rec_time_line)
	addiu	$4,$2,%lo(g_ghost_rec_time_line)
	jal	DrawTextSimple
	nop

	nop
	move	$sp,$fp
	lw	$31,60($sp)
	lw	$fp,56($sp)
	addiu	$sp,$sp,64
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostDrawRecordingTimeHud
	.size	GhostDrawRecordingTimeHud, .-GhostDrawRecordingTimeHud
	.align	2
	.globl	GhostUpdate
	.set	nomips16
	.set	nomicromips
	.ent	GhostUpdate
	.type	GhostUpdate, @function
GhostUpdate:
	.frame	$fp,56,$31		# vars= 32, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	move	$fp,$sp
	li	$2,-2142240768			# 0xffffffff80500000
	sw	$2,20($fp)
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L158
	nop

	lw	$2,%gp_rel(s_recording_too_long_message_timer)($28)
	nop
	blez	$2,$L145
	nop

	lb	$2,%gp_rel(_gameState)($28)
	nop
	bne	$2,$0,$L145
	nop

	li	$6,168			# 0xa8
	li	$5,8			# 0x8
	lui	$2,%hi(deckard_str_ghost_recording_too_long)
	addiu	$4,$2,%lo(deckard_str_ghost_recording_too_long)
	jal	DrawTextSimple
	nop

	lw	$2,%gp_rel(s_recording_too_long_message_timer)($28)
	nop
	addiu	$2,$2,-1
	sw	$2,%gp_rel(s_recording_too_long_message_timer)($28)
$L145:
	sw	$0,%gp_rel(savestate_selection)($28)
	lb	$2,%gp_rel(_gameState)($28)
	nop
	bne	$2,$0,$L159
	nop

	lw	$2,%gp_rel(s_ghost_render_frame_ready)($28)
	nop
	beq	$2,$0,$L147
	nop

	lw	$2,20($fp)
	nop
	addiu	$2,$2,40
	li	$3,102			# 0x66
	sb	$3,0($2)
	lw	$2,20($fp)
	nop
	addiu	$2,$2,43
	li	$3,-112			# 0xffffffffffffff90
	sb	$3,0($2)
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$2,8($2)
	nop
	beq	$2,$0,$L148
	nop

	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$3,8($2)
	li	$2,1			# 0x1
	bne	$3,$2,$L149
	nop

$L148:
	lw	$2,%gp_rel(_ptr_graphicsRelated)($28)
	nop
	sw	$2,24($fp)
	jal	PatchDrawSpyroForGhost
	nop

	jal	DrawSpyro
	nop

	jal	RestoreDrawSpyro
	nop

	lw	$2,%gp_rel(_ptr_graphicsRelated)($28)
	nop
	sw	$2,28($fp)
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$3,8($2)
	li	$2,1			# 0x1
	bne	$3,$2,$L150
	nop

	lw	$5,28($fp)
	lw	$4,24($fp)
	jal	MakeGhostDrawSpyroPacketsWireframe
	nop

	b	$L147
	nop

$L150:
	lw	$5,28($fp)
	lw	$4,24($fp)
	jal	MakeGhostDrawSpyroPacketsTransparent
	nop

	b	$L147
	nop

$L149:
	lw	$4,20($fp)
	jal	DrawGhostDiamond
	nop

$L147:
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L142
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L142
	nop

	lb	$2,%gp_rel(_gameState)($28)
	nop
	bne	$2,$0,$L142
	nop

	li	$2,15011			# 0x3aa3
	sw	$2,32($fp)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,8($2)
	nop
	sw	$2,16($fp)
	lw	$2,16($fp)
	nop
	bgez	$2,$L152
	nop

	sw	$0,16($fp)
$L152:
	lw	$3,16($fp)
	lw	$2,32($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L153
	nop

	lw	$2,32($fp)
	nop
	sw	$2,16($fp)
$L153:
	lw	$3,32($fp)
	lw	$2,16($fp)
	nop
	subu	$2,$3,$2
	sw	$2,36($fp)
	lw	$2,36($fp)
	nop
	blez	$2,$L154
	nop

	lw	$2,36($fp)
	nop
	slt	$2,$2,301
	beq	$2,$0,$L154
	nop

	li	$2,1			# 0x1
	b	$L155
	nop

$L154:
	move	$2,$0
$L155:
	sw	$2,40($fp)
	lw	$2,40($fp)
	nop
	beq	$2,$0,$L156
	nop

	li	$6,168			# 0xa8
	li	$5,8			# 0x8
	lui	$2,%hi(deckard_str_ghost_recording_space_warn)
	addiu	$4,$2,%lo(deckard_str_ghost_recording_space_warn)
	jal	DrawTextSimple
	nop

$L156:
	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	lw	$2,12($2)
	nop
	bne	$2,$0,$L157
	nop

	lw	$2,40($fp)
	nop
	beq	$2,$0,$L142
	nop

$L157:
	jal	GhostDrawRecordingTimeHud
	nop

	b	$L142
	nop

$L158:
	nop
	b	$L142
	nop

$L159:
	nop
$L142:
	move	$sp,$fp
	lw	$31,52($sp)
	lw	$fp,48($sp)
	addiu	$sp,$sp,56
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostUpdate
	.size	GhostUpdate, .-GhostUpdate
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	DrawGhostPurpleLine
	.type	DrawGhostPurpleLine, @function
DrawGhostPurpleLine:
	.frame	$fp,56,$31		# vars= 24, regs= 2/0, args= 24, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	move	$fp,$sp
	sw	$4,56($fp)
	sw	$5,60($fp)
	lw	$2,56($fp)
	nop
	lw	$2,0($2)
	nop
	andi	$2,$2,0xffff
	sh	$2,32($fp)
	lw	$2,56($fp)
	nop
	lw	$2,4($2)
	nop
	andi	$2,$2,0xffff
	sh	$2,34($fp)
	lw	$2,60($fp)
	nop
	lw	$2,0($2)
	nop
	andi	$2,$2,0xffff
	sh	$2,40($fp)
	lw	$2,60($fp)
	nop
	lw	$2,4($2)
	nop
	andi	$2,$2,0xffff
	sh	$2,42($fp)
	li	$2,-80			# 0xffffffffffffffb0
	sb	$2,28($fp)
	li	$2,64			# 0x40
	sb	$2,29($fp)
	li	$2,-1			# 0xffffffffffffffff
	sb	$2,30($fp)
	li	$2,-80			# 0xffffffffffffffb0
	sb	$2,36($fp)
	li	$2,64			# 0x40
	sb	$2,37($fp)
	li	$2,-1			# 0xffffffffffffffff
	sb	$2,38($fp)
	lw	$2,40($fp)
	nop
	sw	$2,16($sp)
	lw	$4,24($fp)
	lw	$5,28($fp)
	lw	$6,32($fp)
	lw	$7,36($fp)
	jal	CustomDrawLineShaded
	nop

	nop
	move	$sp,$fp
	lw	$31,52($sp)
	lw	$fp,48($sp)
	addiu	$sp,$sp,56
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	DrawGhostPurpleLine
	.size	DrawGhostPurpleLine, .-DrawGhostPurpleLine
	.align	2
	.set	nomips16
	.set	nomicromips
	.ent	DrawGhostDiamond
	.type	DrawGhostDiamond, @function
DrawGhostDiamond:
	.frame	$fp,240,$31		# vars= 216, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-240
	sw	$31,236($sp)
	sw	$fp,232($sp)
	move	$fp,$sp
	sw	$4,240($fp)
	lw	$2,240($fp)
	nop
	lw	$4,0($2)
	lw	$3,4($2)
	lw	$2,8($2)
	sw	$4,72($fp)
	sw	$3,76($fp)
	sw	$2,80($fp)
	lw	$2,72($fp)
	nop
	sw	$2,16($fp)
	lw	$2,76($fp)
	nop
	sw	$2,20($fp)
	lw	$2,80($fp)
	nop
	addiu	$2,$2,20
	sw	$2,24($fp)
	lw	$2,240($fp)
	nop
	addiu	$2,$2,14
	lbu	$2,0($2)
	nop
	sw	$2,28($fp)
	lw	$2,240($fp)
	nop
	addiu	$2,$2,13
	lbu	$2,0($2)
	nop
	sw	$2,32($fp)
	lw	$2,28($fp)
	nop
	sll	$2,$2,4
	subu	$2,$0,$2
	andi	$2,$2,0xfff
	sw	$2,36($fp)
	lw	$2,32($fp)
	nop
	sll	$2,$2,4
	andi	$2,$2,0xfff
	sw	$2,40($fp)
	lw	$2,40($fp)
	nop
	addiu	$2,$2,1024
	andi	$2,$2,0xfff
	move	$4,$2
	jal	SinScaled
	nop

	sw	$2,44($fp)
	lw	$2,40($fp)
	nop
	move	$4,$2
	jal	SinScaled
	nop

	sw	$2,48($fp)
	lw	$2,36($fp)
	nop
	addiu	$2,$2,1024
	andi	$2,$2,0xfff
	move	$4,$2
	jal	SinScaled
	nop

	sw	$2,52($fp)
	lw	$2,36($fp)
	nop
	move	$4,$2
	jal	SinScaled
	nop

	subu	$2,$0,$2
	sw	$2,56($fp)
	lw	$2,36($fp)
	nop
	move	$4,$2
	jal	SinScaled
	nop

	sw	$2,60($fp)
	lw	$2,36($fp)
	nop
	addiu	$2,$2,1024
	andi	$2,$2,0xfff
	move	$4,$2
	jal	SinScaled
	nop

	sw	$2,64($fp)
	lw	$2,48($fp)
	nop
	sw	$2,68($fp)
	lw	$3,52($fp)
	lw	$2,44($fp)
	nop
	mult	$3,$2
	mflo	$2
	bgez	$2,$L162
	nop

	addiu	$2,$2,4095
$L162:
	sra	$2,$2,12
	sw	$2,52($fp)
	lw	$3,56($fp)
	lw	$2,44($fp)
	nop
	mult	$3,$2
	mflo	$2
	bgez	$2,$L163
	nop

	addiu	$2,$2,4095
$L163:
	sra	$2,$2,12
	sw	$2,56($fp)
	lw	$2,16($fp)
	nop
	sw	$2,84($fp)
	lw	$2,20($fp)
	nop
	sw	$2,88($fp)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,170
	sw	$2,92($fp)
	lw	$2,16($fp)
	nop
	sw	$2,96($fp)
	lw	$2,20($fp)
	nop
	sw	$2,100($fp)
	lw	$2,24($fp)
	nop
	addiu	$2,$2,-170
	sw	$2,104($fp)
	lw	$3,60($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L164
	nop

	addiu	$2,$2,4095
$L164:
	sra	$2,$2,12
	subu	$2,$0,$2
	move	$3,$2
	lw	$2,16($fp)
	nop
	addu	$2,$3,$2
	sw	$2,108($fp)
	lw	$3,64($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L165
	nop

	addiu	$2,$2,4095
$L165:
	sra	$2,$2,12
	subu	$2,$0,$2
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$3,$2
	sw	$2,112($fp)
	lw	$2,24($fp)
	nop
	sw	$2,116($fp)
	lw	$3,60($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L166
	nop

	addiu	$2,$2,4095
$L166:
	sra	$2,$2,12
	move	$3,$2
	lw	$2,16($fp)
	nop
	addu	$2,$3,$2
	sw	$2,120($fp)
	lw	$3,64($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L167
	nop

	addiu	$2,$2,4095
$L167:
	sra	$2,$2,12
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$3,$2
	sw	$2,124($fp)
	lw	$2,24($fp)
	nop
	sw	$2,128($fp)
	lw	$3,52($fp)
	nop
	move	$2,$3
	sll	$2,$2,5
	subu	$2,$2,$3
	sll	$2,$2,2
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L168
	nop

	addiu	$2,$2,4095
$L168:
	sra	$2,$2,12
	move	$3,$2
	lw	$2,16($fp)
	nop
	addu	$2,$3,$2
	sw	$2,132($fp)
	lw	$3,56($fp)
	nop
	move	$2,$3
	sll	$2,$2,5
	subu	$2,$2,$3
	sll	$2,$2,2
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L169
	nop

	addiu	$2,$2,4095
$L169:
	sra	$2,$2,12
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$3,$2
	sw	$2,136($fp)
	lw	$3,68($fp)
	nop
	move	$2,$3
	sll	$2,$2,5
	subu	$2,$2,$3
	sll	$2,$2,2
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L170
	nop

	addiu	$2,$2,4095
$L170:
	sra	$2,$2,12
	move	$3,$2
	lw	$2,24($fp)
	nop
	addu	$2,$3,$2
	sw	$2,140($fp)
	lw	$3,52($fp)
	nop
	move	$2,$3
	sll	$2,$2,6
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L171
	nop

	addiu	$2,$2,4095
$L171:
	sra	$2,$2,12
	subu	$2,$0,$2
	move	$3,$2
	lw	$2,16($fp)
	nop
	addu	$2,$3,$2
	sw	$2,144($fp)
	lw	$3,56($fp)
	nop
	move	$2,$3
	sll	$2,$2,6
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L172
	nop

	addiu	$2,$2,4095
$L172:
	sra	$2,$2,12
	subu	$2,$0,$2
	move	$3,$2
	lw	$2,20($fp)
	nop
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$3,68($fp)
	nop
	move	$2,$3
	sll	$2,$2,6
	addu	$2,$2,$3
	sll	$2,$2,2
	bgez	$2,$L173
	nop

	addiu	$2,$2,4095
$L173:
	sra	$2,$2,12
	subu	$2,$0,$2
	move	$3,$2
	lw	$2,24($fp)
	nop
	addu	$2,$3,$2
	sw	$2,152($fp)
	addiu	$3,$fp,84
	addiu	$2,$fp,156
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,96
	addiu	$2,$fp,168
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,108
	addiu	$2,$fp,180
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,120
	addiu	$2,$fp,192
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,132
	addiu	$2,$fp,204
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,144
	addiu	$2,$fp,216
	move	$5,$3
	move	$4,$2
	jal	WorldToScreen
	nop

	addiu	$3,$fp,180
	addiu	$2,$fp,156
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,192
	addiu	$2,$fp,156
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,204
	addiu	$2,$fp,156
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,216
	addiu	$2,$fp,156
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,180
	addiu	$2,$fp,168
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,192
	addiu	$2,$fp,168
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,204
	addiu	$2,$fp,168
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,216
	addiu	$2,$fp,168
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,204
	addiu	$2,$fp,180
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,192
	addiu	$2,$fp,204
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,216
	addiu	$2,$fp,192
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	addiu	$3,$fp,180
	addiu	$2,$fp,216
	move	$5,$3
	move	$4,$2
	jal	DrawGhostPurpleLine
	nop

	nop
	move	$sp,$fp
	lw	$31,236($sp)
	lw	$fp,232($sp)
	addiu	$sp,$sp,240
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	DrawGhostDiamond
	.size	DrawGhostDiamond, .-DrawGhostDiamond
	.align	2
	.globl	GhostSaveState
	.set	nomips16
	.set	nomicromips
	.ent	GhostSaveState
	.type	GhostSaveState, @function
GhostSaveState:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	li	$2,-2140864512			# 0xffffffff80650000
	ori	$2,$2,0x8000
	sw	$2,16($fp)
	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$4,8($2)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	sw	$4,0($2)
	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lui	$3,%hi(g_ghostA)
	addiu	$3,$3,%lo(g_ghostA)
	lw	$5,12($3)
	lw	$4,16($3)
	lw	$3,20($3)
	sw	$5,4($2)
	sw	$4,8($2)
	sw	$3,12($2)
	lw	$4,%gp_rel(s_playback_dragon_event_index)($28)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	sw	$4,16($2)
	lw	$4,%gp_rel(s_playback_dragon_pause_remaining)($28)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	addiu	$5,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$5,$2
	sw	$4,20($2)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$4,8($2)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	sw	$4,24($2)
	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lui	$3,%hi(g_ghostB)
	addiu	$3,$3,%lo(g_ghostB)
	lw	$5,24($3)
	lw	$4,28($3)
	lw	$3,32($3)
	sw	$5,28($2)
	sw	$4,32($2)
	sw	$3,36($2)
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L175
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L175
	nop

	lw	$4,16($fp)
	jal	GhostHeaderIsV2
	nop

	beq	$2,$0,$L175
	nop

	lw	$4,16($fp)
	jal	GhostDragonEventCount
	nop

	move	$5,$2
	b	$L176
	nop

$L175:
	move	$5,$0
$L176:
	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	sw	$5,40($2)
	lw	$4,%gp_rel(s_recording_dragon_event_index)($28)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	addiu	$5,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$5,$2
	sw	$4,44($2)
	lw	$4,%gp_rel(s_recording_dragon_pause_frames)($28)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	sw	$4,48($2)
	lw	$4,%gp_rel(s_recording_was_in_dragon)($28)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,32($fp)
	addiu	$5,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$5,$2
	sw	$4,52($2)
	nop
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostSaveState
	.size	GhostSaveState, .-GhostSaveState
	.align	2
	.globl	GhostLoadState
	.set	nomips16
	.set	nomicromips
	.ent	GhostLoadState
	.type	GhostLoadState, @function
GhostLoadState:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	sw	$4,40($fp)
	lui	$2,%hi(g_ghostA)
	lw	$2,%lo(g_ghostA)($2)
	nop
	beq	$2,$0,$L178
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,4($2)
	nop
	bne	$2,$0,$L178
	nop

	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lw	$3,0($2)
	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	sw	$3,8($2)
	lui	$2,%hi(g_ghostA)
	addiu	$3,$2,%lo(g_ghostA)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$4,40($fp)
	nop
	move	$2,$4
	sll	$2,$2,3
	subu	$2,$2,$4
	sll	$2,$2,3
	addiu	$4,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$4
	lw	$5,4($2)
	lw	$4,8($2)
	lw	$2,12($2)
	sw	$5,12($3)
	sw	$4,16($3)
	sw	$2,20($3)
	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lw	$2,16($2)
	nop
	sw	$2,%gp_rel(s_playback_dragon_event_index)($28)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	addiu	$4,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$4,$2
	lw	$2,20($2)
	nop
	sw	$2,%gp_rel(s_playback_dragon_pause_remaining)($28)
$L178:
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L185
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L185
	nop

	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lw	$3,24($2)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	sw	$3,8($2)
	lui	$2,%hi(g_ghostB)
	addiu	$3,$2,%lo(g_ghostB)
	lui	$5,%hi(g_ghost_pos_snapshots)
	lw	$4,40($fp)
	nop
	move	$2,$4
	sll	$2,$2,3
	subu	$2,$2,$4
	sll	$2,$2,3
	addiu	$4,$5,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$4
	lw	$5,28($2)
	lw	$4,32($2)
	lw	$2,36($2)
	sw	$5,24($3)
	sw	$4,28($3)
	sw	$2,32($3)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	addiu	$4,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$4,$2
	lw	$2,44($2)
	nop
	sw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lw	$2,48($2)
	nop
	sw	$2,%gp_rel(s_recording_dragon_pause_frames)($28)
	lui	$2,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	addiu	$4,$2,%lo(g_ghost_pos_snapshots)
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addu	$2,$4,$2
	lw	$2,52($2)
	nop
	sw	$2,%gp_rel(s_recording_was_in_dragon)($28)
	li	$2,-2140864512			# 0xffffffff80650000
	ori	$2,$2,0x8000
	sw	$2,24($fp)
	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$3,8($2)
	lw	$2,24($fp)
	nop
	sw	$3,16($2)
	lw	$4,24($fp)
	jal	GhostHeaderIsV2
	nop

	beq	$2,$0,$L185
	nop

	lui	$4,%hi(g_ghost_pos_snapshots)
	lw	$3,40($fp)
	nop
	move	$2,$3
	sll	$2,$2,3
	subu	$2,$2,$3
	sll	$2,$2,3
	addiu	$3,$4,%lo(g_ghost_pos_snapshots)
	addu	$2,$2,$3
	lw	$2,40($2)
	nop
	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	bgez	$2,$L180
	nop

	sw	$0,20($fp)
$L180:
	lw	$2,20($fp)
	nop
	slt	$2,$2,17
	bne	$2,$0,$L181
	nop

	li	$2,16			# 0x10
	sw	$2,20($fp)
$L181:
	lw	$2,24($fp)
	lw	$3,20($fp)
	nop
	sw	$3,36($2)
	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	nop
	bltz	$2,$L182
	nop

	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	lw	$3,20($fp)
	nop
	slt	$2,$2,$3
	beq	$2,$0,$L182
	nop

	lw	$2,%gp_rel(s_recording_dragon_event_index)($28)
	lw	$3,%gp_rel(s_recording_dragon_pause_frames)($28)
	lw	$4,24($fp)
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$4,$2
	sw	$3,4($2)
$L182:
	lw	$2,20($fp)
	nop
	sw	$2,16($fp)
	b	$L183
	nop

$L184:
	lw	$3,24($fp)
	lw	$2,16($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,0($2)
	lw	$3,24($fp)
	lw	$2,16($fp)
	nop
	addiu	$2,$2,5
	sll	$2,$2,3
	addu	$2,$3,$2
	sw	$0,4($2)
	lw	$2,16($fp)
	nop
	addiu	$2,$2,1
	sw	$2,16($fp)
$L183:
	lw	$2,16($fp)
	nop
	slt	$2,$2,16
	bne	$2,$0,$L184
	nop

$L185:
	nop
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostLoadState
	.size	GhostLoadState, .-GhostLoadState
	.align	2
	.globl	GhostGetRecordedFrameCount
	.set	nomips16
	.set	nomicromips
	.ent	GhostGetRecordedFrameCount
	.type	GhostGetRecordedFrameCount, @function
GhostGetRecordedFrameCount:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L187
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L187
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,8($2)
	b	$L188
	nop

$L187:
	move	$2,$0
$L188:
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostGetRecordedFrameCount
	.size	GhostGetRecordedFrameCount, .-GhostGetRecordedFrameCount
	.align	2
	.globl	GhostGetPlaybackFrameCount
	.set	nomips16
	.set	nomicromips
	.ent	GhostGetPlaybackFrameCount
	.type	GhostGetPlaybackFrameCount, @function
GhostGetPlaybackFrameCount:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-16
	sw	$fp,12($sp)
	move	$fp,$sp
	lui	$2,%hi(g_ghostA)
	lw	$2,%lo(g_ghostA)($2)
	nop
	beq	$2,$0,$L190
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,4($2)
	nop
	bne	$2,$0,$L190
	nop

	li	$2,-2141192192			# 0xffffffff80600000
	sw	$2,0($fp)
	lw	$2,0($fp)
	nop
	lw	$2,16($2)
	b	$L191
	nop

$L190:
	move	$2,$0
$L191:
	move	$sp,$fp
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostGetPlaybackFrameCount
	.size	GhostGetPlaybackFrameCount, .-GhostGetPlaybackFrameCount
	.align	2
	.globl	GhostGetPlaybackCurrentFrame
	.set	nomips16
	.set	nomicromips
	.ent	GhostGetPlaybackCurrentFrame
	.type	GhostGetPlaybackCurrentFrame, @function
GhostGetPlaybackCurrentFrame:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	lui	$2,%hi(g_ghostA)
	lw	$2,%lo(g_ghostA)($2)
	nop
	beq	$2,$0,$L193
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,4($2)
	nop
	bne	$2,$0,$L193
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,8($2)
	b	$L194
	nop

$L193:
	move	$2,$0
$L194:
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostGetPlaybackCurrentFrame
	.size	GhostGetPlaybackCurrentFrame, .-GhostGetPlaybackCurrentFrame
	.align	2
	.globl	GhostGetMaxFrames
	.set	nomips16
	.set	nomicromips
	.ent	GhostGetMaxFrames
	.type	GhostGetMaxFrames, @function
GhostGetMaxFrames:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	li	$2,15011			# 0x3aa3
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostGetMaxFrames
	.size	GhostGetMaxFrames, .-GhostGetMaxFrames
	.align	2
	.globl	GhostIsRacing
	.set	nomips16
	.set	nomicromips
	.ent	GhostIsRacing
	.type	GhostIsRacing, @function
GhostIsRacing:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	lui	$2,%hi(g_ghostA)
	lw	$2,%lo(g_ghostA)($2)
	nop
	beq	$2,$0,$L198
	nop

	lui	$2,%hi(g_ghostA)
	addiu	$2,$2,%lo(g_ghostA)
	lw	$2,4($2)
	nop
	bne	$2,$0,$L198
	nop

	lui	$2,%hi(g_ghostB)
	lw	$2,%lo(g_ghostB)($2)
	nop
	beq	$2,$0,$L198
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$2,$2,%lo(g_ghostB)
	lw	$2,4($2)
	nop
	beq	$2,$0,$L198
	nop

	li	$2,1			# 0x1
	b	$L200
	nop

$L198:
	move	$2,$0
$L200:
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostIsRacing
	.size	GhostIsRacing, .-GhostIsRacing
	.align	2
	.globl	GhostInvalidateStoredGhosts
	.set	nomips16
	.set	nomicromips
	.ent	GhostInvalidateStoredGhosts
	.type	GhostInvalidateStoredGhosts, @function
GhostInvalidateStoredGhosts:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	lui	$2,%hi(g_ghostA)
	addiu	$4,$2,%lo(g_ghostA)
	jal	StopGhost
	nop

	lui	$2,%hi(g_ghostB)
	addiu	$4,$2,%lo(g_ghostB)
	jal	StopGhost
	nop

	jal	ResetGhostDragonRuntime
	nop

	li	$4,-2141192192			# 0xffffffff80600000
	jal	ClearGhostHeader
	nop

	li	$2,-2140864512			# 0xffffffff80650000
	ori	$4,$2,0x8000
	jal	ClearGhostHeader
	nop

	li	$6,168			# 0xa8
	move	$5,$0
	lui	$2,%hi(g_ghost_pos_snapshots)
	addiu	$4,$2,%lo(g_ghost_pos_snapshots)
	jal	memset
	nop

	sw	$0,%gp_rel(g_ghost_new_best)($28)
	sw	$0,%gp_rel(s_recording_failed_too_long)($28)
	sw	$0,%gp_rel(s_recording_too_long_message_timer)($28)
	nop
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostInvalidateStoredGhosts
	.size	GhostInvalidateStoredGhosts, .-GhostInvalidateStoredGhosts
	.align	2
	.globl	GhostResetAll
	.set	nomips16
	.set	nomicromips
	.ent	GhostResetAll
	.type	GhostResetAll, @function
GhostResetAll:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	jal	GhostsAutoplayResetExternalLoadState
	nop

	jal	GhostInvalidateStoredGhosts
	nop

	nop
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostResetAll
	.size	GhostResetAll, .-GhostResetAll
	.align	2
	.globl	GhostAutoEnableIfLoaded
	.set	nomips16
	.set	nomicromips
	.ent	GhostAutoEnableIfLoaded
	.type	GhostAutoEnableIfLoaded, @function
GhostAutoEnableIfLoaded:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	li	$2,-2141192192			# 0xffffffff80600000
	sw	$2,16($fp)
	lw	$4,16($fp)
	jal	GhostHeaderHasCompletedRun
	nop

	beq	$2,$0,$L205
	nop

	lui	$2,%hi(ghost_menu)
	addiu	$2,$2,%lo(ghost_menu)
	li	$3,1			# 0x1
	sw	$3,4($2)
	lui	$2,%hi(il_menu)
	addiu	$2,$2,%lo(il_menu)
	li	$3,1			# 0x1
	sw	$3,4($2)
$L205:
	nop
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	GhostAutoEnableIfLoaded
	.size	GhostAutoEnableIfLoaded, .-GhostAutoEnableIfLoaded
	.section	.sdata
	.align	2
	.type	DR_MODE_50_BLEND.1, @object
	.size	DR_MODE_50_BLEND.1, 4
DR_MODE_50_BLEND.1:
	.word	-520092672
	.align	2
	.type	DR_MODE_50_BLEND.0, @object
	.size	DR_MODE_50_BLEND.0, 4
DR_MODE_50_BLEND.0:
	.word	-520092672
	.ident	"GCC: (GNU) 12.1.0"
