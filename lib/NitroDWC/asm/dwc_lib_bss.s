	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.extern BucketSetValue
	.extern BucketAdd
	.extern BucketSub
	.extern BucketMult
	.extern BucketDiv
	.extern BucketConcat
	.extern BucketAvg
	.extern BucketIntOp
	.extern BucketDoubleOp
	.extern BucketStringOp
	.extern TeamBucketIntOp
	.extern TeamBucketDoubleOp
	.extern TeamBucketStringOp
	.extern PlayerBucketIntOp
	.extern PlayerBucketDoubleOp
	.extern PlayerBucketStringOp
	.public ov00_0221A784
	.public ov00_0221A78C
	.public ov00_0221A80C
	.public ov00_0221A90C
	.public ov00_0221A918
	.public ov00_0221A928
	.public ov00_0221A948
	.public ov00_0221A949
	.public ov00_0221A94A
	.public ov00_0221A94C
	.public ov00_0221A950
	.public ov00_0221B080
	.public ov00_0221B0A0
	.public ov00_0221B0A4
	.public ov00_0221B0AC
	.public ov00_0221B0C0
	.public ov00_0221B0C4
	.bss
ov00_0221A784:
	.space 0x8

ov00_0221A78C:
	.space 0x80

ov00_0221A80C:
	.space 0x100

ov00_0221A90C:
	.space 0xC

ov00_0221A918:
	.space 0x10

ov00_0221A928:
	.space 0x20

ov00_0221A948:
	.space 0x1

ov00_0221A949:
	.space 0x1

ov00_0221A94A:
	.space 0x2

ov00_0221A94C:
	.space 0x4

ov00_0221A950:
	.space 0x730

ov00_0221B080:
	.space 0x20

ov00_0221B0A0:
	.space 0x4

ov00_0221B0A4:
	.space 0x8

ov00_0221B0AC:
	.space 0x14

ov00_0221B0C0:
	.space 0x4

ov00_0221B0C4:
	.space 0xC
