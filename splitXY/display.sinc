.importzp _oam_off			;make this ZP variable accessible to assembly code

.segment "CODE"

;void __fastcall__ oam_clear_fast(void);

.export _oam_clear_fast		;make this function accessible to C code

_oam_clear_fast:

	lda #240

	.repeat 64,I
	sta OAM_BUF+I*4
	.endrepeat

	rts

	
	
;void __fastcall__ oam_meta_spr_pal(unsigned char x,unsigned char y,unsigned char pal,const unsigned char *metasprite);

.export _oam_meta_spr_pal

_oam_meta_spr_pal:

	sta <PTR
	stx <PTR+1

	ldy #2				;three popa calls replacement, performed in reversed order
	lda (sp),y
	dey
	sta <SCRX
	lda (sp),y
	dey
	sta <SCRY
	lda (sp),y
	sta <DST			;this is palette, just repurposing RAM variable name

	ldx _oam_off

@1:

	lda (PTR),y			;x offset
	cmp #$80
	beq @2
	iny
	clc
	adc <SCRX
	sta OAM_BUF+3,x
	lda (PTR),y			;y offset
	iny
	clc
	adc <SCRY
	sta OAM_BUF+0,x
	lda (PTR),y			;tile
	iny
	sta OAM_BUF+1,x
	lda (PTR),y			;attribute
	iny
	and #$fc			;remove palette bits
	ora <DST			;add new palette bits
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @1

@2:

	stx _oam_off
	
	lda <sp
	adc #2				;carry is always set here, so it adds 3
	sta <sp
	bcc @3
	inc <sp+1

@3:

	rts

	
	
;void __fastcall__ oam_meta_spr_clip(signed int x,unsigned char y,const unsigned char *metasprite);

.segment "ZEROPAGE"

SPR_XOFF:	.res 1
SPR_YOFF:	.res 1

.segment "CODE"

.export _oam_meta_spr_clip

_oam_meta_spr_clip:

	sta <PTR
	stx <PTR+1

	jsr popa			;y
	sta <SCRY
	
	jsr popax			;x
	sta <SCRX
	stx <DST			;this is X MSB, repurposing RAM variable name

	ldx _oam_off
	ldy #0

@1:

	lda (PTR),y			;x offset
	cmp #$80
	beq @2
	iny

	sta <SPR_XOFF+0
	clc
	adc <SCRX
	sta <SPR_XOFF+1

	lda <SPR_XOFF+0
	ora #$7f
	bmi :+
	lda #0
:

	adc <DST
	bne @skip

	lda <SPR_XOFF+1
	sta OAM_BUF+3,x
	lda (PTR),y			;y offset
	iny
	clc
	adc <SCRY
	sta OAM_BUF+0,x
	lda (PTR),y			;tile
	iny
	sta OAM_BUF+1,x
	lda (PTR),y			;attribute
	iny
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @1
	
@skip:

	iny
	iny
	iny
	jmp @1

@2:

	stx _oam_off
	
	rts
	
	; static unsigned char sx,sy,tile,pal,off;
	; static signed int sxx;

	; off=0;

	; while(1)
	; {
		; sx=metasprite[off++];

		; if(sx==128) return;

		; sxx =x+(signed char)sx;
		; sy  =metasprite[off++]+y;
		; tile=metasprite[off++];
		; pal =metasprite[off++];

		; if(!(sxx&0xff00))
		; {
			; oam_spr(sxx,sy,tile,pal,oam_off);
			; oam_off+=4;
		; }
	; }
