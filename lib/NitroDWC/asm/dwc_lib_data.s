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
	.public ov00_0221643C
	.public ov00_02216464
	.public ov00_02216474
	.public ov00_02216484
	.public ov00_022164A4
	.public ov00_022164B4
	.public ov00_022164C0
	.public ov00_022164C8
	.public ov00_022164D4
	.public ov00_022164E0
	.public ov00_022164E8
	.public ov00_022164F0
	.public ov00_022164FC
	.public ov00_02216508
	.public ov00_02216514
	.public ov00_02216520
	.public ov00_02216528
	.public ov00_02216530
	.public ov00_02216538
	.public ov00_0221653C
	.public ov00_02216544
	.public ov00_02216550
	.public ov00_02216558
	.public ov00_02216560
	.public ov00_02216568
	.public ov00_02216574
	.public ov00_02216590
	.public ov00_022165A0
	.public ov00_022165AC
	.public ov00_022165B4
	.public ov00_022165BC
	.public ov00_022165C4
	.public ov00_022165CC
	.public ov00_022165D4
	.public ov00_022165DC
	.public ov00_022165E4
	.public ov00_022165EC
	.public ov00_022165F4
	.public ov00_022165FC
	.public ov00_02216604
	.public ov00_0221660C
	.public ov00_02216614
	.public ov00_0221661C
	.public ov00_02216634
	.public ov00_02216640
	.public ov00_02216650
	.public ov00_02216680
	.public ov00_02216698
	.public ov00_022166B0
	.public ov00_022166BC
	.public ov00_022166C4
	.public ov00_022166CC
	.public ov00_022166D0
	.public ov00_022166E0
	.public ov00_022166F4
	.public ov00_022166F8
	.public ov00_02216710
	.public ov00_02216728
	.public ov00_0221677C
	.public ov00_0221679C
	.public ov00_022167A8
	.public ov00_022167AC
	.public ov00_022167B4
	.public ov00_022167B8
	.public ov00_022167CC
	.public ov00_022167E0
	.public ov00_022167F0
	.public ov00_022167F8
	.public ov00_02216804
	.public ov00_02216808
	.public ov00_0221680C
	.public ov00_0221682C
	.public ov00_0221684C
	.public ov00_02216868
	.public ov00_02216884
	.public ov00_02216888
	.public ov00_02216894
	.public ov00_02216898
	.public ov00_0221689C
	.public ov00_022168A0
	.public ov00_022168A4
	.public ov00_022168C0
	.public ov00_022168DC
	.public ov00_022168E0
	.public ov00_02216904
	.public ov00_02216918
	.public ov00_02216928
	.public ov00_02216938
	.public ov00_02216954
	.public ov00_02216974
	.public ov00_02216988
	.public ov00_022169A4
	.public ov00_022169B0
	.public ov00_022169C0
	.public ov00_022169D0
	.public ov00_022169F0
	.public ov00_022169FC
	.public ov00_02216A1C
	.public ov00_02216A24
	.public ov00_02216A2C
	.public ov00_02216A3C
	.public ov00_02216A44
	.public ov00_02216A4C
	.public ov00_02216A58
	.public ov00_02216A5C
	.public ov00_02216A64
	.public ov00_02216A6C
	.public ov00_02216A78
	.public ov00_02216A84
	.public ov00_02216A90
	.public ov00_02216A94
	.public ov00_02216A98
	.public ov00_02216A9C
	.public ov00_02216AA0
	.public ov00_02216AA4
	.public ov00_02216AA8
	.public ov00_02216AAC
	.public ov00_02216AB0
	.public ov00_02216AB4
	.public ov00_02216AB8
	.public ov00_02216ABC
	.public ov00_02216AC0
	.public ov00_02216AF0
	.public ov00_02216B10
	.public ov00_02216B20
	.public ov00_02216B30
	.public ov00_02216B50
	.public ov00_02216B58
	.public ov00_02216B60
	.public ov00_02216B64
	.public ov00_02216B6C
	.public ov00_02216B78
	.public ov00_02216B84
	.public ov00_02216B8C
	.public ov00_02216B9C
	.public ov00_02216BA8
	.public ov00_02216BAC
	.public ov00_02216BF0
	.public ov00_02216BF4
	.public ov00_02216C08
	.public ov00_02216C3C
	.public ov00_02216D3C
	.public ov00_02216D40
	.public ov00_02216D54
	.public ov00_02216D8C
	.public ov00_02216E8C
	.public ov00_02216E90
	.public ov00_02216EA4
	.public ov00_02216EF4
	.public ov00_02216F74
	.public ov00_02216F78
	.public ov00_02216F8C
	.public ov00_02216FB8
	.public ov00_02217038
	.public ov00_0221703C
	.public ov00_02217050
	.public ov00_022170A0
	.public ov00_02217120
	.public ov00_02217124
	.public ov00_02217138
	.public ov00_022171B8
	.public ov00_02217240
	.public ov00_02217244
	.public ov00_02217258
	.public ov00_022182D8
	.public ov00_02217358
	.public ov00_0221735C
	.public ov00_02217370
	.public ov00_022173F0
	.public ov00_02217484
	.public ov00_02217488
	.public ov00_0221749C
	.public ov00_02217538
	.public ov00_02217638
	.public ov00_0221763C
	.public ov00_02217650
	.public ov00_02217694
	.public ov00_02217714
	.public ov00_02217718
	.public ov00_0221772C
	.public ov00_02217770
	.public ov00_022177F0
	.public ov00_022177F4
	.public ov00_02217808
	.public ov00_02217838
	.public ov00_02217938
	.public ov00_02217960
	.public ov00_02217984
	.public ov00_022179A4
	.public ov00_022179AC
	.public ov00_022179B0
	.public ov00_022179C4
	.public ov00_022179D8
	.public ov00_022179E0
	.public ov00_022179EC
	.public ov00_022179F0
	.public ov00_022179F4
	.public ov00_022179F8
	.public ov00_02217A00
	.public ov00_02217A04
	.public ov00_02217A08
	.public ov00_02217A0C
	.public ov00_02217A10
	.public ov00_02217A48
	.public ov00_02217A4C
	.public ov00_02217A58
	.public ov00_02217A60
	.public ov00_02217A64
	.public ov00_02217A68
	.public ov00_02217A6C
	.public ov00_02217A70
	.public ov00_02217A74
	.public ov00_02217A80
	.public ov00_02217A8C
	.public ov00_02217AA4
	.public ov00_02217AA8
	.public ov00_02217AAC
	.public ov00_02217AB4
	.public ov00_02217AC0
	.public ov00_02217ACC
	.public ov00_02217AD8
	.public ov00_02217AE4
	.public ov00_02217AF0
	.public ov00_02217AF8
	.public ov00_02217B50
	.public ov00_02217B54
	.public ov00_02217B5C
	.public ov00_02217B64
	.public ov00_02217B6C
	.public ov00_02217B70
	.public ov00_02217B7C
	.public ov00_02217B88
	.public ov00_02217B8C
	.public ov00_02217B98
	.public ov00_02217BA4
	.public ov00_02217BB4
	.public ov00_02217BC4
	.public ov00_02217BD4
	.public ov00_02217BF0
	.public ov00_02217C0C
	.public ov00_02217C18
	.public ov00_02217C24
	.public ov00_02217C48
	.public ov00_02217C4C
	.public ov00_02217C58
	.public ov00_02217C64
	.public ov00_02217C70
	.public ov00_02217C7C
	.public ov00_02217C88
	.public ov00_02217C94
	.public ov00_02217CA4
	.public ov00_02217CB4
	.public ov00_02217CC4
	.public ov00_02217CD4
	.public ov00_02217CE8
	.public ov00_02217CFC
	.public ov00_02217D10
	.public ov00_02217D1C
	.public ov00_02217D28
	.public ov00_02217D30
	.public ov00_02217D3C
	.public ov00_02217D5C
	.public ov00_02217D7C
	.public ov00_02217D88
	.public ov00_02217D8C
	.public ov00_02217D98
	.public ov00_02217DA4
	.public ov00_02217DB0
	.public ov00_02217DBC
	.public ov00_02217DC8
	.public ov00_02217DD4
	.public ov00_02217DE0
	.public ov00_02217DF0
	.public ov00_02217E00
	.public ov00_02217E08
	.public ov00_02217E14
	.public ov00_02217E1C
	.public ov00_02217E28
	.public ov00_02217E34
	.public ov00_02217E3C
	.public ov00_02217E4C
	.public ov00_02217E50
	.public ov00_02217E58
	.public ov00_02217E5C
	.public ov00_02217E64
	.public ov00_02217E74
	.public ov00_02217E94
	.public ov00_02217EA0
	.public ov00_02217EAC
	.public ov00_02217EC0
	.public ov00_02217ED0
	.public ov00_02217EE0
	.public ov00_02217EE4
	.public ov00_02217F14
	.public ov00_02217F24
	.public ov00_02217F30
	.public ov00_02217F3C
	.public ov00_02217F4C
	.public ov00_02217F58
	.public ov00_02217F60
	.public ov00_02217F70
	.public ov00_02217F80
	.public ov00_02217F8C
	.public ov00_02217F94
	.public ov00_02217FAC
	.public ov00_02217FC4
	.public ov00_02217FD0
	.public ov00_02217FE0
	.public ov00_02217FEC
	.public ov00_02218000
	.public ov00_0221800C
	.public ov00_02218030
	.public ov00_02218034
	.public ov00_02218060
	.public ov00_02218068
	.public ov00_02218074
	.public ov00_02218084
	.public ov00_0221808C
	.public ov00_022180B4
	.public ov00_022180BC
	.public ov00_022180C4
	.public ov00_022180F4
	.public ov00_0221811C
	.public ov00_02218214
	.public ov00_0221821C
	.public ov00_02218220
	.public ov00_02218230
	.public ov00_0221823C
	.public ov00_02218248
	.public ov00_02218258
	.public ov00_02218260
	.public ov00_02218268
	.public ov00_02218270
	.public ov00_022182A0
	.public ov00_022182A4
	.public ov00_022182AC
	.public ov00_022182BC
	.public ov00_022182C4
	.public ov00_022182D0
	.public ov00_022182D4
	.public ov00_022182DC
	.public ov00_022182E4
	.public ov00_022182EC
	.public ov00_022182F0
	.public ov00_022182F4
	.public ov00_022182F8
	.public ov00_022182FC
	.public ov00_02218310
	.public ov00_0221831C
	.public ov00_0221832C
	.public ov00_02218338
	.public ov00_02218344
	.public ov00_02218370
	.public ov00_02218380
	.public ov00_02218394
	.public ov00_022183A8
	.public ov00_022183C0
	.public ov00_022183DC
	.public ov00_022183F8
	.public ov00_02218410
	.public ov00_0221841C
	.public ov00_0221842C
	.public ov00_0221843C
	.public ov00_0221844C
	.public ov00_02218450
	.public ov00_02218478
	.public ov00_0221849C
	.public ov00_022184C0
	.public ov00_022184C4
	.public ov00_022184DC
	.public ov00_022184F0
	.public ov00_02218504
	.public ov00_02218518
	.public ov00_02218544
	.public ov00_02218568
	.public ov00_0221857C
	.public ov00_02218588
	.public ov00_02218594
	.public ov00_022185A0
	.public ov00_022185A8
	.public ov00_022185AC
	.public ov00_022185B4
	.public ov00_022185C4
	.public ov00_022185DC
	.public ov00_022185F0
	.public ov00_02218600
	.public ov00_02218618
	.public ov00_02218648
	.public ov00_02218658
	.public ov00_02218678
	.public ov00_0221868C
	.public ov00_0221869C
	.public ov00_022186AC
	.public ov00_022186EC
	.public ov00_0221872C
	.public ov00_02218754
	.public ov00_02218788
	.public ov00_022187B0
	.public ov00_022187DC
	.public ov00_0221880C
	.public ov00_02218840
	.public ov00_02218860
	.public ov00_02218870
	.public ov00_02218898
	.public ov00_022188AC
	.public ov00_022188C0
	.public ov00_022188D0
	.public ov00_022188D8
	.public ov00_022188E8
	.public ov00_0221891C
	.public ov00_02218924
	.public ov00_02218930
	.public ov00_0221893C
	.public ov00_0221894C
	.public ov00_02218954
	.public ov00_02218958
	.public ov00_02218964
	.public ov00_02218970
	.public ov00_0221897C
	.public ov00_02218988
	.public ov00_02218990
	.public ov00_0221899C
	.public ov00_022189A8
	.public ov00_022189B8
	.public ov00_022189C0
	.public ov00_022189C8
	.public ov00_022189D4
	.public ov00_022189DC
	.public ov00_022189E4
	.public ov00_022189F4
	.public ov00_02218A00
	.public ov00_02218A08
	.public ov00_02218A10
	.public ov00_02218A18
	.public ov00_02218A48
	.public ov00_02218A50
	.public ov00_02218A80
	.public ov00_02218A88
	.public ov00_02218A94
	.public ov00_02218A9C
	.public ov00_02218AA4
	.public ov00_02218AC4
	.public ov00_02218AEC
	.public ov00_02218B04
	.public ov00_02218B18
	.public ov00_02218B1C
	.public ov00_02218B2C
	.public ov00_02218B40
	.public ov00_02218B4C
	.public ov00_02218B5C
	.public ov00_02218B74
	.public ov00_02218B7C
	.public ov00_02218BAC
	.public ov00_02218BB8
	.public ov00_02218BC8
	.public ov00_02218BD0
	.public ov00_02218BE0
	.public ov00_02218BE8
	.public ov00_02218BF4
	.public ov00_02218C00
	.public ov00_02218C0C
	.public ov00_02218C18
	.public ov00_02218C24
	.public ov00_02218C34
	.public ov00_02218C3C
	.public ov00_02218C44
	.public ov00_02218C4C
	.public ov00_02218C58
	.public ov00_02218C60
	.public ov00_02218C68
	.public ov00_02218C70
	.public ov00_02218C78
	.public ov00_02218C80
	.public ov00_02218C88
	.public ov00_02218C90
	.public ov00_02218C98
	.public ov00_02218CA0
	.public ov00_02218CA8
	.public ov00_02218CB0
	.public ov00_02218CB8
	.public ov00_02218CC0
	.public ov00_02218CD0
	.public ov00_02218CE8
	.public ov00_02218CF0
	.public ov00_02218D04
	.public ov00_02218D18
	.public ov00_02218D1C
	.public ov00_02218D20
	.public ov00_02218D24
	.public ov00_02218D28
	.public ov00_02218D38
	.public ov00_02218D48
	.public ov00_02218D54
	.public ov00_02218D60
	.public ov00_02218D70
	.public ov00_02218D80
	.public ov00_02218D90
	.public ov00_02218DA4
	.public ov00_02218DB4
	.public ov00_02218DC4
	.public ov00_02218DD4
	.public ov00_02218DE0
	.public ov00_02218DF8
	.public ov00_02218E0C
	.public ov00_02218E20
	.public ov00_02218E2C
	.public ov00_02218E44
	.public ov00_02218E4C
	.public ov00_02218E60
	.public ov00_02218E74
	.public ov00_02218E88
	.public ov00_02218E9C
	.public ov00_02218EAC
	.public ov00_02218EC0
	.public ov00_02218ED4
	.public ov00_02218EE4
	.public ov00_02218EE8
	.public ov00_02218F08
	.public ov00_02218F50
	.public ov00_02218F60
	.public ov00_02218F70
	.public ov00_02218F84
	.public ov00_02218FA0
	.public ov00_02218FC0
	.public ov00_02218FE4
	.public ov00_02219008
	.public ov00_02219024
	.public ov00_0221902C
	.public ov00_02219034
	.public ov00_0221903C
	.public ov00_02219044
	.public ov00_0221904C
	.public ov00_02219050
	.public ov00_02219058
	.public ov00_0221907C
	.public ov00_02219084
	.public ov00_022190A4
	.public ov00_022190A8
	.public ov00_022190B4
	.public ov00_022190D4
	.public ov00_022190DC
	.public ov00_022190EC
	.public ov00_022190F0
	.public ov00_02219118
	.public ov00_02219128
	.public ov00_0221914C
	.public ov00_02219174
	.public ov00_022191A8
	.public ov00_022191D0
	.public ov00_022191E0
	.public ov00_022191E4
	.public ov00_022191EC
	.public ov00_022191F4
	.public ov00_02219200
	.public ov00_02219214
	.public ov00_02219218
	.public ov00_02219228
	.public ov00_0221923C
	.public ov00_02219244
	.public ov00_02219274
	.public ov00_02219280
	.public ov00_02219290
	.public ov00_02219298
	.public ov00_022192A8
	.public ov00_022192BC
	.public ov00_022192D0
	.public ov00_022192E8
	.public ov00_02219328
	.public ov00_02219338
	.public ov00_02219360
	.public ov00_02219394
	.public ov00_022193C4
	.public ov00_022193E4
	.public ov00_022193F0
	.public ov00_02219418
	.public ov00_0221942C
	.public ov00_02219430
	.public ov00_0221945C
	.public ov00_02219468
	.public ov00_02219474
	.public ov00_02219480
	.public ov00_02219490
	.public ov00_02219498
	.public ov00_022194A8
	.public ov00_022194B0
	.public ov00_022194BC
	.public ov00_022194C8
	.public ov00_022194D4
	.public ov00_022194DC
	.public ov00_022194E4
	.public ov00_022194EC
	.public ov00_022194F4
	.public ov00_02219500
	.public ov00_0221950C
	.public ov00_02219514
	.public ov00_02219520
	.public ov00_0221952C
	.public ov00_02219534
	.public ov00_02219540
	.public ov00_02219550
	.public ov00_02219560
	.public ov00_02219564
	.public ov00_02219570
	.public ov00_02219578
	.public ov00_022195A4
	.public ov00_022195AC
	.public ov00_022195B4
	.public ov00_022195B8
	.public ov00_022195C0
	.public ov00_022195CC
	.public ov00_022195D8
	.public ov00_022195E4
	.public ov00_022195EC
	.public ov00_02219614
	.public ov00_02219618
	.public ov00_0221961C
	.public ov00_02219624
	.public ov00_0221962C
	.public ov00_02219630
	.public ov00_02219638
	.public ov00_02219644
	.public ov00_02219648
	.public ov00_02219650
	.public ov00_02219654
	.public ov00_0221965C
	.public ov00_02219664
	.public ov00_02219668
	.public ov00_02219670
	.public ov00_02219678
	.public ov00_0221967C
	.public ov00_02219684
	.public ov00_022196A4
	.public ov00_022196C4
	.public ov00_022196C8
	.public ov00_022196E0
	.public ov00_022196E8
	.public ov00_022196F4
	.public ov00_022196FC
	.public ov00_0221972C
	.public ov00_0221973C
	.public ov00_02219744
	.public ov00_02219754
	.public ov00_02219768
	.public ov00_02219778
	.public ov00_02219788
	.public ov00_02219794
	.public ov00_0221979C
	.public ov00_022197A4
	.public ov00_022197B0
	.public ov00_022197B8
	.public ov00_022197C8
	.public ov00_022197D4
	.public ov00_022197E4
	.public ov00_022197EC
	.public ov00_02219800
	.public ov00_02219838
	.public ov00_02219850
	.public ov00_02219868
	.public ov00_02219878
	.public ov00_02219888
	.public ov00_02219890
	.public ov00_0221989C
	.public ov00_022198AC
	.public ov00_022198B0
	.public ov00_022198BC
	.public ov00_022198D8
	.public ov00_02219924
	.public ov00_02219934
	.public ov00_02219944
	.public ov00_02219948
	.public ov00_0221994C
	.public ov00_02219954
	.public ov00_02219958
	.public ov00_02219960
	.public ov00_02219968
	.public ov00_02219970
	.public ov00_02219974
	.public ov00_02219978
	.public ov00_02219980
	.public ov00_02219988
	.public ov00_02219990
	.public ov00_0221999C
	.public ov00_022199A8
	.public ov00_022199B4
	.public ov00_022199C0
	.public ov00_022199C8
	.public ov00_022199D0
	.public ov00_022199E4
	.public ov00_02219A08
	.public ov00_02219A10
	.public ov00_02219A1C
	.public ov00_02219A30
	.public ov00_02219A44
	.public ov00_02219A58
	.public ov00_02219A70
	.public ov00_02219A8C
	.public ov00_02219A98
	.public ov00_02219ABC
	.public ov00_02219AE0
	.public ov00_02219AF8
	.public ov00_02219B1C
	.public ov00_02219B30
	.public ov00_02219B44
	.public ov00_02219B58
	.public ov00_02219B70
	.public ov00_02219B88
	.public ov00_02219BA0
	.public ov00_02219BB8
	.public ov00_02219BD4
	.public ov00_02219BF0
	.public ov00_02219C10
	.public ov00_02219C18
	.public ov00_02219C28
	.public ov00_02219C40
	.public ov00_02219C4C
	.public ov00_02219C64
	.public ov00_02219C7C
	.public ov00_02219C88
	.public ov00_02219C94
	.public ov00_02219CAC
	.public ov00_02219CB4
	.public ov00_02219CB8
	.public ov00_02219CC0
	.public ov00_02219CD0
	.public ov00_02219CE4
	.public ov00_02219CEC
	.public ov00_02219CF0
	.public ov00_02219CF4
	.public ov00_02219D14
	.public ov00_02219D24
	.public ov00_02219D28
	.public ov00_02219D30
	.public ov00_02219D38
	.public ov00_02219D54
	.public current_rec
	.public static_qr2_rec
	.public ov00_02219E84
	.public ov00_02219EA4
	.public ov00_02219EDC
	.public ov00_02219EE0
	.public ov00_02219EF0
	.public ov00_02219EFC
	.public ov00_02219F04
	.public ov00_02219F08
	.public ov00_02219F10
	.public ov00_02219F24
	.public ov00_02219F30
	.public ov00_02219F3C
	.public ov00_02219F44
	.public ov00_02219F48
	.public ov00_02219F4C
	.public ov00_02219F5C
	.public ov00_02219F68
	.public ov00_02219F74
	.public ov00_02219F80
	.public ov00_02219F84
	.public ov00_02219F8C
	.public ov00_02219F94
	.public ov00_02219F9C
	.public ov00_02219FA4
	.public ov00_02219FAC
	.public ov00_02219FB4
	.public ov00_02219FBC
	.public ov00_02219FC4
	.public ov00_02219FCC
	.public ov00_02219FD4
	.public ov00_02219FDC
	.public ov00_02219FE4
	.public ov00_02219FF0
	.public ov00_02219FFC
	.public ov00_0221A008
	.public ov00_0221A014
	.public ov00_0221A020
	.public ov00_0221A02C
	.public ov00_0221A038
	.public ov00_0221A044
	.public ov00_0221A050
	.public ov00_0221A05C
	.public ov00_0221A068
	.public ov00_0221A074
	.public ov00_0221A080
	.public ov00_0221A08C
	.public ov00_0221A098
	.public ov00_0221A0A8
	.public qr2_registered_key_list
	.public ov00_0221A4B0
	.public ov00_0221A4C0
	.public ov00_0221A4CC
	.public ov00_0221A4D4
	.public ov00_0221A4DC
	.public ov00_0221A4E4
	.public ov00_0221A4FC
	.public ov00_0221A500
	.public ov00_0221A508
	.public ov00_0221A514
	.public ov00_0221A51C
	.public ov00_0221A520
	.public ov00_0221A528
	.public ov00_0221A52C
	.public ov00_0221A530
	.public ov00_0221A540
	.public ov00_0221A550
	.public ov00_0221A560
	.public ov00_0221A574
	.public ov00_0221A588
	.public ov00_0221A59C
	.public ov00_0221A5B8
	.public ov00_0221A5BC
	.public ov00_0221A5C8
	.public ov00_0221A5D8
	.public ov00_0221A5E8
	.public ov00_0221A5FC
	.public ov00_0221A618
	.public ov00_0221A638
	.public ov00_0221A63C
	.public ov00_0221A644
	.public ov00_0221A650
	.data
ov00_0221643C:
	.asciz "https://nas.test.nintendowifi.net/ac"
	.balign 4, 0
ov00_02216464:
	.asciz "ALLOC DWCauth"
	.balign 4, 0
ov00_02216474:
	.asciz "FREE DWCauth"
	.balign 4, 0
ov00_02216484:
	.asciz "https://nas.nintendowifi.net/ac"
	.balign 4, 0
ov00_022164A4:
	.asciz "ALLOC bmwork"
	.balign 4, 0
ov00_022164B4:
	.asciz "FREE bmwork"
	.balign 4, 0
ov00_022164C0:
	.asciz "Date"
	.balign 4, 0
ov00_022164C8:
	.asciz "httpresult"
	.balign 4, 0
ov00_022164D4:
	.asciz "returncd"
	.balign 4, 0
ov00_022164E0:
	.asciz "token"
	.balign 4, 0
ov00_022164E8:
	.asciz "locator"
	.balign 4, 0
ov00_022164F0:
	.asciz "challenge"
	.balign 4, 0
ov00_022164FC:
	.asciz "datetime"
	.balign 4, 0
ov00_02216508:
	.asciz "Set-Cookie"
	.balign 4, 0
ov00_02216514:
	.asciz "acctcreate"
	.balign 4, 0
ov00_02216520:
	.asciz "action"
	.balign 4, 0
ov00_02216528:
	.asciz "login"
	.balign 4, 0
ov00_02216530:
	.asciz "gsbrcd"
	.balign 4, 0
ov00_02216538:
	.asciz "Y"
	.balign 4, 0
ov00_0221653C:
	.asciz "iswfc"
	.balign 4, 0
ov00_02216544:
	.asciz "ingamesn"
	.balign 4, 0
ov00_02216550:
	.asciz "%013llu"
	.balign 4, 0
ov00_02216558:
	.asciz "%03u"
	.balign 4, 0
ov00_02216560:
	.asciz "%02x"
	.balign 4, 0
ov00_02216568:
	.asciz "%02x%02x"
	.balign 4, 0
ov00_02216574:
	.asciz "%02d%02d%02d%02d%02d%02d"
	.balign 4, 0
ov00_02216590:
	.asciz "%02d:0000000-00"
	.balign 4, 0
ov00_022165A0:
	.asciz "%03d%03d"
	.balign 4, 0
ov00_022165AC:
	.asciz "sdkver"
	.balign 4, 0
ov00_022165B4:
	.asciz "userid"
	.balign 4, 0
ov00_022165BC:
	.asciz "passwd"
	.balign 4, 0
ov00_022165C4:
	.asciz "bssid"
	.balign 4, 0
ov00_022165CC:
	.asciz "apinfo"
	.balign 4, 0
ov00_022165D4:
	.asciz "gamecd"
	.balign 4, 0
ov00_022165DC:
	.asciz "makercd"
	.balign 4, 0
ov00_022165E4:
	.asciz "unitcd"
	.balign 4, 0
ov00_022165EC:
	.asciz "macadr"
	.balign 4, 0
ov00_022165F4:
	.asciz "lang"
	.balign 4, 0
ov00_022165FC:
	.asciz "birth"
	.balign 4, 0
ov00_02216604:
	.asciz "devtime"
	.balign 4, 0
ov00_0221660C:
	.asciz "devname"
	.balign 4, 0
ov00_02216614:
	.asciz "ssid"
	.balign 4, 0
ov00_0221661C:
	.asciz "Nitro WiFi SDK/%d.%d"
	.balign 4, 0
ov00_02216634:
	.asciz "User-Agent"
	.balign 4, 0
ov00_02216640:
	.asciz "HTTP_X_GAMECD"
	.balign 4, 0
ov00_02216650:
	.word ov00_02217488
	.word ov00_0221735C
	.word ov00_0221763C
	.word ov00_02217718
	.word ov00_02216BF4
	.word ov00_02216E90
	.word ov00_02216F78
	.word ov00_02216D40
	.word ov00_02217244
	.word ov00_02217124
	.word ov00_022177F4
	.word ov00_0221703C
ov00_02216680:
	.asciz "ALLOC http->lowrecvbuf"
	.balign 4, 0
ov00_02216698:
	.asciz "ALLOC http->lowsendbuf"
	.balign 4, 0
ov00_022166B0:
	.asciz "Connection"
	.balign 4, 0
ov00_022166BC:
	.asciz "close"
	.balign 4, 0
ov00_022166C4:
	.asciz "\r\n\r\n"
	.balign 4, 0
ov00_022166CC:
	.asciz "%d"
	.balign 4, 0
ov00_022166D0:
	.asciz "Content-Length"
	.balign 4, 0
ov00_022166E0:
	.asciz "Content-Length: "
	.balign 4, 0
ov00_022166F4:
	.asciz "\r\n"
	.balign 4, 0
ov00_022166F8:
	.asciz "FREE http->lowrecvbuf"
	.balign 4, 0
ov00_02216710:
	.asciz "FREE http->lowsendbuf"
	.balign 4, 0
ov00_02216728:
	.asciz "POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nHost: %s\r\n\r\n"
	.balign 4, 0
ov00_0221677C:
	.asciz "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n"
	.balign 4, 0
ov00_0221679C:
	.asciz "%s: %s\r\n"
	.balign 4, 0
ov00_022167A8:
	.asciz "%s="
	.balign 4, 0
ov00_022167AC:
	.asciz "&%s="
	.balign 4, 0
ov00_022167B4:
	.asciz "%s"
	.balign 4, 0
ov00_022167B8:
	.asciz "ALLOC buf->buffer"
	.balign 4, 0
ov00_022167CC:
	.asciz "FREE buf->buffer"
	.balign 4, 0
ov00_022167E0:
	.asciz "ALLOC newptr"
	.balign 4, 0
ov00_022167F0:
	.asciz "http://"
	.balign 4, 0
ov00_022167F8:
	.asciz "https://"
	.balign 4, 0
ov00_02216804:
	.asciz ":"
	.balign 4, 0
ov00_02216808:
	.asciz "/"
	.balign 4, 0
ov00_0221680C:
	.asciz "ALLOC result->entry[i].label"
	.balign 4, 0
ov00_0221682C:
	.asciz "ALLOC result->entry[i].value"
	.balign 4, 0
ov00_0221684C:
	.asciz "FREE result->entry[i].label"
	.balign 4, 0
ov00_02216868:
	.asciz "FREE result->entry[i].value"
	.balign 4, 0
ov00_02216884:
	.asciz " "
	.balign 4, 0
ov00_02216888:
	.asciz "httpresult"
	.balign 4, 0
ov00_02216894:
	.asciz "200"
	.balign 4, 0
ov00_02216898:
	.asciz ": "
	.balign 4, 0
ov00_0221689C:
	.asciz "="
	.balign 4, 0
ov00_022168A0:
	.asciz "&"
	.balign 4, 0
ov00_022168A4:
	.asciz "FREE array_entry[i].label"
	.balign 4, 0
ov00_022168C0:
	.asciz "FREE array_entry[i].value"
	.balign 4, 0

ov00_022168DC:
	.word ov00_022168E0
ov00_022168E0:
	.asciz "http://conntest.nintendowifi.net/"
	.balign 4, 0
ov00_02216904:
	.asciz "ALLOC DWCnetcheck"
	.balign 4, 0
ov00_02216918:
	.asciz "ALLOC DWChttp"
	.balign 4, 0
ov00_02216928:
	.asciz "FREE DWChttp"
	.balign 4, 0
ov00_02216938:
	.asciz "FREE DWCnetcheck->body_302"
	.balign 4, 0
ov00_02216954:
	.asciz "FREE DWCnetcheck->body_wayport"
	.balign 4, 0
ov00_02216974:
	.asciz "FREE DWCnetcheck"
	.balign 4, 0
ov00_02216988:
	.asciz "ALLOC DWCnetcheck->body_302"
	.balign 4, 0
ov00_022169A4:
	.asciz "ALLOC url"
	.balign 4, 0
ov00_022169B0:
	.asciz "ALLOC data_len"
	.balign 4, 0
ov00_022169C0:
	.asciz "ALLOC wait_len"
	.balign 4, 0
ov00_022169D0:
	.asciz "ALLOC DWCnetcheck->body_wayport"
	.balign 4, 0
ov00_022169F0:
	.asciz "httpresult"
	.balign 4, 0
ov00_022169FC:
	.asciz "https://nas.nintendowifi.net/ac"
	.balign 4, 0
ov00_02216A1C:
	.asciz "action"
	.balign 4, 0
ov00_02216A24:
	.asciz "message"
	.balign 4, 0
ov00_02216A2C:
	.asciz "HotSpotResponse"
	.balign 4, 0
ov00_02216A3C:
	.asciz "parse"
	.balign 4, 0
ov00_02216A44:
	.asciz "HTML"
	.balign 4, 0
ov00_02216A4C:
	.asciz "returncd"
	.balign 4, 0
ov00_02216A58:
	.asciz "url"
	.balign 4, 0
ov00_02216A5C:
	.asciz "data"
	.balign 4, 0
ov00_02216A64:
	.asciz "wait"
	.balign 4, 0
ov00_02216A6C:
	.asciz "FREE url"
	.balign 4, 0
ov00_02216A78:
	.asciz "FREE data"
	.balign 4, 0
ov00_02216A84:
	.asciz "FREE wait"
	.balign 4, 0
ov00_02216A90:
	.asciz "Jun"
	.balign 4, 0
ov00_02216A94:
	.asciz "Jul"
	.balign 4, 0
ov00_02216A98:
	.asciz "Dec"
	.balign 4, 0
ov00_02216A9C:
	.asciz "Sep"
	.balign 4, 0
ov00_02216AA0:
	.asciz "Aug"
	.balign 4, 0
ov00_02216AA4:
	.asciz "Nov"
	.balign 4, 0
ov00_02216AA8:
	.asciz "Oct"
	.balign 4, 0
ov00_02216AAC:
	.asciz "May"
	.balign 4, 0
ov00_02216AB0:
	.asciz "Apr"
	.balign 4, 0
ov00_02216AB4:
	.asciz "Mar"
	.balign 4, 0
ov00_02216AB8:
	.asciz "Feb"
	.balign 4, 0
ov00_02216ABC:
	.asciz "Jan"
	.balign 4, 0

ov00_02216AC0:
	.word ov00_02216ABC
	.word ov00_02216AB8
	.word ov00_02216AB4
	.word ov00_02216AB0
	.word ov00_02216AAC
	.word ov00_02216A90
	.word ov00_02216A94
	.word ov00_02216AA0
	.word ov00_02216A9C
	.word ov00_02216AA8
	.word ov00_02216AA4
	.word ov00_02216A98
ov00_02216AF0:
	.asciz "Fri, 03 Mar 2006 01:28:13 GMT"
	.balign 4, 0
ov00_02216B10:
	.asciz "ALLOC intwork"
	.balign 4, 0
ov00_02216B20:
	.asciz "FREE intwork"
	.balign 4, 0
ov00_02216B30:
	.asciz "https://nas.nintendowifi.net/ac"
	.balign 4, 0
ov00_02216B50:
	.asciz "action"
	.balign 4, 0
ov00_02216B58:
	.asciz "SVCLOC"
	.balign 4, 0
ov00_02216B60:
	.asciz "svc"
	.balign 4, 0
ov00_02216B64:
	.asciz "Date"
	.balign 4, 0
ov00_02216B6C:
	.asciz "httpresult"
	.balign 4, 0
ov00_02216B78:
	.asciz "returncd"
	.balign 4, 0
ov00_02216B84:
	.asciz "svchost"
	.balign 4, 0
ov00_02216B8C:
	.asciz "servicetoken"
	.balign 4, 0
ov00_02216B9C:
	.asciz "statusdata"
	.balign 4, 0

ov00_02216BA8:
	.word ov00_02216BAC

ov00_02216BAC:
	.asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"
	.balign 4, 0

ov00_02216BF0:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0
ov00_02216BF4:
	.word ov00_02216C08
	.word 0x00000100
	.word ov00_02216C3C
	.word 0x00000003
	.word ov00_02216BF0
ov00_02216C08:
	.asciz "BE, GlobalSign nv-sa, Root CA, GlobalSign Root CA"
	.balign 4, 0
ov00_02216C3C:
	.byte 0xDA, 0x0E, 0xE6, 0x99
	.byte 0x8D, 0xCE, 0xA3, 0xE3, 0x4F, 0x8A, 0x7E, 0xFB, 0xF1, 0x8B, 0x83, 0x25, 0x6B, 0xEA, 0x48, 0x1F
	.byte 0xF1, 0x2A, 0xB0, 0xB9, 0x95, 0x11, 0x04, 0xBD, 0xF0, 0x63, 0xD1, 0xE2, 0x67, 0x66, 0xCF, 0x1C
	.byte 0xDD, 0xCF, 0x1B, 0x48, 0x2B, 0xEE, 0x8D, 0x89, 0x8E, 0x9A, 0xAF, 0x29, 0x80, 0x65, 0xAB, 0xE9
	.byte 0xC7, 0x2D, 0x12, 0xCB, 0xAB, 0x1C, 0x4C, 0x70, 0x07, 0xA1, 0x3D, 0x0A, 0x30, 0xCD, 0x15, 0x8D
	.byte 0x4F, 0xF8, 0xDD, 0xD4, 0x8C, 0x50, 0x15, 0x1C, 0xEF, 0x50, 0xEE, 0xC4, 0x2E, 0xF7, 0xFC, 0xE9
	.byte 0x52, 0xF2, 0x91, 0x7D, 0xE0, 0x6D, 0xD5, 0x35, 0x30, 0x8E, 0x5E, 0x43, 0x73, 0xF2, 0x41, 0xE9
	.byte 0xD5, 0x6A, 0xE3, 0xB2, 0x89, 0x3A, 0x56, 0x39, 0x38, 0x6F, 0x06, 0x3C, 0x88, 0x69, 0x5B, 0x2A
	.byte 0x4D, 0xC5, 0xA7, 0x54, 0xB8, 0x6C, 0x89, 0xCC, 0x9B, 0xF9, 0x3C, 0xCA, 0xE5, 0xFD, 0x89, 0xF5
	.byte 0x12, 0x3C, 0x92, 0x78, 0x96, 0xD6, 0xDC, 0x74, 0x6E, 0x93, 0x44, 0x61, 0xD1, 0x8D, 0xC7, 0x46
	.byte 0xB2, 0x75, 0x0E, 0x86, 0xE8, 0x19, 0x8A, 0xD5, 0x6D, 0x6C, 0xD5, 0x78, 0x16, 0x95, 0xA2, 0xE9
	.byte 0xC8, 0x0A, 0x38, 0xEB, 0xF2, 0x24, 0x13, 0x4F, 0x73, 0x54, 0x93, 0x13, 0x85, 0x3A, 0x1B, 0xBC
	.byte 0x1E, 0x34, 0xB5, 0x8B, 0x05, 0x8C, 0xB9, 0x77, 0x8B, 0xB1, 0xDB, 0x1F, 0x20, 0x91, 0xAB, 0x09
	.byte 0x53, 0x6E, 0x90, 0xCE, 0x7B, 0x37, 0x74, 0xB9, 0x70, 0x47, 0x91, 0x22, 0x51, 0x63, 0x16, 0x79
	.byte 0xAE, 0xB1, 0xAE, 0x41, 0x26, 0x08, 0xC8, 0x19, 0x2B, 0xD1, 0x46, 0xAA, 0x48, 0xD6, 0x64, 0x2A
	.byte 0xD7, 0x83, 0x34, 0xFF, 0x2C, 0x2A, 0xC1, 0x6C, 0x19, 0x43, 0x4A, 0x07, 0x85, 0xE7, 0xD3, 0x7C
	.byte 0xF6, 0x21, 0x68, 0xEF, 0xEA, 0xF2, 0x52, 0x9F, 0x7F, 0x93, 0x90, 0xCF

	.balign 4, 0
ov00_02216D3C:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02216D40:
	.word ov00_02216D54
	.word 0x00000100
	.word ov00_02216D8C
	.word 0x00000003
	.word ov00_02216D3C

ov00_02216D54:
	.asciz "IE, Baltimore, CyberTrust, Baltimore CyberTrust Root"
	.balign 4, 0
ov00_02216D8C:
	.byte 0xA3, 0x04, 0xBB, 0x22
	.byte 0xAB, 0x98, 0x3D, 0x57, 0xE8, 0x26, 0x72, 0x9A, 0xB5, 0x79, 0xD4, 0x29, 0xE2, 0xE1, 0xE8, 0x95
	.byte 0x80, 0xB1, 0xB0, 0xE3, 0x5B, 0x8E, 0x2B, 0x29, 0x9A, 0x64, 0xDF, 0xA1, 0x5D, 0xED, 0xB0, 0x09
	.byte 0x05, 0x6D, 0xDB, 0x28, 0x2E, 0xCE, 0x62, 0xA2, 0x62, 0xFE, 0xB4, 0x88, 0xDA, 0x12, 0xEB, 0x38
	.byte 0xEB, 0x21, 0x9D, 0xC0, 0x41, 0x2B, 0x01, 0x52, 0x7B, 0x88, 0x77, 0xD3, 0x1C, 0x8F, 0xC7, 0xBA
	.byte 0xB9, 0x88, 0xB5, 0x6A, 0x09, 0xE7, 0x73, 0xE8, 0x11, 0x40, 0xA7, 0xD1, 0xCC, 0xCA, 0x62, 0x8D
	.byte 0x2D, 0xE5, 0x8F, 0x0B, 0xA6, 0x50, 0xD2, 0xA8, 0x50, 0xC3, 0x28, 0xEA, 0xF5, 0xAB, 0x25, 0x87
	.byte 0x8A, 0x9A, 0x96, 0x1C, 0xA9, 0x67, 0xB8, 0x3F, 0x0C, 0xD5, 0xF7, 0xF9, 0x52, 0x13, 0x2F, 0xC2
	.byte 0x1B, 0xD5, 0x70, 0x70, 0xF0, 0x8F, 0xC0, 0x12, 0xCA, 0x06, 0xCB, 0x9A, 0xE1, 0xD9, 0xCA, 0x33
	.byte 0x7A, 0x77, 0xD6, 0xF8, 0xEC, 0xB9, 0xF1, 0x68, 0x44, 0x42, 0x48, 0x13, 0xD2, 0xC0, 0xC2, 0xA4
	.byte 0xAE, 0x5E, 0x60, 0xFE, 0xB6, 0xA6, 0x05, 0xFC, 0xB4, 0xDD, 0x07, 0x59, 0x02, 0xD4, 0x59, 0x18
	.byte 0x98, 0x63, 0xF5, 0xA5, 0x63, 0xE0, 0x90, 0x0C, 0x7D, 0x5D, 0xB2, 0x06, 0x7A, 0xF3, 0x85, 0xEA
	.byte 0xEB, 0xD4, 0x03, 0xAE, 0x5E, 0x84, 0x3E, 0x5F, 0xFF, 0x15, 0xED, 0x69, 0xBC, 0xF9, 0x39, 0x36
	.byte 0x72, 0x75, 0xCF, 0x77, 0x52, 0x4D, 0xF3, 0xC9, 0x90, 0x2C, 0xB9, 0x3D, 0xE5, 0xC9, 0x23, 0x53
	.byte 0x3F, 0x1F, 0x24, 0x98, 0x21, 0x5C, 0x07, 0x99, 0x29, 0xBD, 0xC6, 0x3A, 0xEC, 0xE7, 0x6E, 0x86
	.byte 0x3A, 0x6B, 0x97, 0x74, 0x63, 0x33, 0xBD, 0x68, 0x18, 0x31, 0xF0, 0x78, 0x8D, 0x76, 0xBF, 0xFC
	.byte 0x9E, 0x8E, 0x5D, 0x2A, 0x86, 0xA7, 0x4D, 0x90, 0xDC, 0x27, 0x1A, 0x39

	.balign 4, 0
ov00_02216E8C:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02216E90:
	.word ov00_02216EA4
	.word 0x00000080
	.word ov00_02216EF4
	.word 0x00000003
	.word ov00_02216E8C

ov00_02216EA4:
	.asciz "US, GTE Corporation, GTE CyberTrust Solutions, Inc., GTE CyberTrust Global Root"

	.balign 4, 0
ov00_02216EF4:
	.byte 0x95, 0x0F, 0xA0, 0xB6, 0xF0, 0x50, 0x9C, 0xE8, 0x7A, 0xC7, 0x88, 0xCD
	.byte 0xDD, 0x17, 0x0E, 0x2E, 0xB0, 0x94, 0xD0, 0x1B, 0x3D, 0x0E, 0xF6, 0x94, 0xC0, 0x8A, 0x94, 0xC7
	.byte 0x06, 0xC8, 0x90, 0x97, 0xC8, 0xB8, 0x64, 0x1A, 0x7A, 0x7E, 0x6C, 0x3C, 0x53, 0xE1, 0x37, 0x28
	.byte 0x73, 0x60, 0x7F, 0xB2, 0x97, 0x53, 0x07, 0x9F, 0x53, 0xF9, 0x6D, 0x58, 0x94, 0xD2, 0xAF, 0x8D
	.byte 0x6D, 0x88, 0x67, 0x80, 0xE6, 0xED, 0xB2, 0x95, 0xCF, 0x72, 0x31, 0xCA, 0xA5, 0x1C, 0x72, 0xBA
	.byte 0x5C, 0x02, 0xE7, 0x64, 0x42, 0xE7, 0xF9, 0xA9, 0x2C, 0xD6, 0x3A, 0x0D, 0xAC, 0x8D, 0x42, 0xAA
	.byte 0x24, 0x01, 0x39, 0xE6, 0x9C, 0x3F, 0x01, 0x85, 0x57, 0x0D, 0x58, 0x87, 0x45, 0xF8, 0xD3, 0x85
	.byte 0xAA, 0x93, 0x69, 0x26, 0x85, 0x70, 0x48, 0x80, 0x3F, 0x12, 0x15, 0xC7, 0x79, 0xB4, 0x1F, 0x05
	.byte 0x2F, 0x3B, 0x62, 0x99

	.balign 4, 0
ov00_02216F74:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02216F78:
	.word ov00_02216F8C
	.word 0x00000080
	.word ov00_02216FB8
	.word 0x00000003
	.word ov00_02216F74

ov00_02216F8C:
	.asciz "US, GTE Corporation, GTE CyberTrust Root"
	.balign 4, 0

ov00_02216FB8:
	.byte 0xB8, 0xE6, 0x4F, 0xBA, 0xDB, 0x98, 0x7C, 0x71
	.byte 0x7C, 0xAF, 0x44, 0xB7, 0xD3, 0x0F, 0x46, 0xD9, 0x64, 0xE5, 0x93, 0xC1, 0x42, 0x8E, 0xC7, 0xBA
	.byte 0x49, 0x8D, 0x35, 0x2D, 0x7A, 0xE7, 0x8B, 0xBD, 0xE5, 0x05, 0x31, 0x59, 0xC6, 0xB1, 0x2F, 0x0A
	.byte 0x0C, 0xFB, 0x9F, 0xA7, 0x3F, 0xA2, 0x09, 0x66, 0x84, 0x56, 0x1E, 0x37, 0x29, 0x1B, 0x87, 0xE9
	.byte 0x7E, 0x0C, 0xCA, 0x9A, 0x9F, 0xA5, 0x7F, 0xF5, 0x15, 0x94, 0xA3, 0xD5, 0xA2, 0x46, 0x82, 0xD8
	.byte 0x68, 0x4C, 0xD1, 0x37, 0x15, 0x06, 0x68, 0xAF, 0xBD, 0xF8, 0xB0, 0xB3, 0xF0, 0x29, 0xF5, 0x95
	.byte 0x5A, 0x09, 0x16, 0x61, 0x77, 0x0A, 0x22, 0x25, 0xD4, 0x4F, 0x45, 0xAA, 0xC7, 0xBD, 0xE5, 0x96
	.byte 0xDF, 0xF9, 0xD4, 0xA8, 0x8E, 0x42, 0xCC, 0x24, 0xC0, 0x1E, 0x91, 0x27, 0x4A, 0xB5, 0x6D, 0x06
	.byte 0x80, 0x63, 0x39, 0xC4, 0xA2, 0x5E, 0x38, 0x03

	.balign 4, 0
ov00_02217038:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_0221703C:
	.word ov00_02217050
	.word 0x00000080
	.word ov00_022170A0
	.word 0x00000003
	.word ov00_02217038

ov00_02217050:
	.asciz "US, Washington, Nintendo of America Inc, NOA, Nintendo CA, ca@noa.nintendo.com"
	.balign 4, 0

ov00_022170A0:
	.byte 0xB3, 0xCD, 0x79, 0x97, 0x77, 0x5D, 0x8A, 0xAF, 0x86, 0xA8, 0xE8, 0xD7, 0x73, 0x1C, 0x77, 0xDF
	.byte 0x10, 0x90, 0x1F, 0x81, 0xF8, 0x41, 0x9E, 0x21, 0x55, 0xDF, 0xBC, 0xFC, 0x63, 0xFB, 0x19, 0x43
	.byte 0xF1, 0xF6, 0xC4, 0x72, 0x42, 0x49, 0xBD, 0xAD, 0x44, 0x68, 0x4E, 0xF3, 0xDA, 0x1D, 0xE6, 0x4D
	.byte 0xD8, 0xF9, 0x59, 0x88, 0xDC, 0xAE, 0x3E, 0x9B, 0x38, 0x09, 0xCA, 0x7F, 0xFF, 0xDC, 0x24, 0xA2
	.byte 0x44, 0x78, 0x78, 0x49, 0x93, 0xD4, 0x84, 0x40, 0x10, 0xB8, 0xEC, 0x3E, 0xDB, 0x2D, 0x93, 0xC8
	.byte 0x11, 0xC8, 0xFD, 0x78, 0x2D, 0x61, 0xAD, 0x31, 0xAE, 0x86, 0x26, 0xB0, 0xFD, 0x5A, 0x3F, 0xA1
	.byte 0x3D, 0xBF, 0xE2, 0x4B, 0x49, 0xEC, 0xCE, 0x66, 0x98, 0x58, 0x26, 0x12, 0xC0, 0xFB, 0xF4, 0x77
	.byte 0x65, 0x1B, 0xEA, 0xFB, 0xCB, 0x7F, 0xE0, 0x8C, 0xCB, 0x02, 0xA3, 0x4E, 0x5E, 0x8C, 0xEA, 0x9B

	.balign 4, 0
ov00_02217120:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02217124:
	.word ov00_022171B8
	.word 0x00000080
	.word ov00_02217138
	.word 0x00000003
	.word ov00_02217120

ov00_02217138:
	.byte 0xD2, 0x36, 0x36, 0x6A, 0x8B, 0xD7, 0xC2, 0x5B
	.byte 0x9E, 0xDA, 0x81, 0x41, 0x62, 0x8F, 0x38, 0xEE, 0x49, 0x04, 0x55, 0xD6, 0xD0, 0xEF, 0x1C, 0x1B
	.byte 0x95, 0x16, 0x47, 0xEF, 0x18, 0x48, 0x35, 0x3A, 0x52, 0xF4, 0x2B, 0x6A, 0x06, 0x8F, 0x3B, 0x2F
	.byte 0xEA, 0x56, 0xE3, 0xAF, 0x86, 0x8D, 0x9E, 0x17, 0xF7, 0x9E, 0xB4, 0x65, 0x75, 0x02, 0x4D, 0xEF
	.byte 0xCB, 0x09, 0xA2, 0x21, 0x51, 0xD8, 0x9B, 0xD0, 0x67, 0xD0, 0xBA, 0x0D, 0x92, 0x06, 0x14, 0x73
	.byte 0xD4, 0x93, 0xCB, 0x97, 0x2A, 0x00, 0x9C, 0x5C, 0x4E, 0x0C, 0xBC, 0xFA, 0x15, 0x52, 0xFC, 0xF2
	.byte 0x44, 0x6E, 0xDA, 0x11, 0x4A, 0x6E, 0x08, 0x9F, 0x2F, 0x2D, 0xE3, 0xF9, 0xAA, 0x3A, 0x86, 0x73
	.byte 0xB6, 0x46, 0x53, 0x58, 0xC8, 0x89, 0x05, 0xBD, 0x83, 0x11, 0xB8, 0x73, 0x3F, 0xAA, 0x07, 0x8D
	.byte 0xF4, 0x42, 0x4D, 0xE7, 0x40, 0x9D, 0x1C, 0x37

ov00_022171B8:
	.asciz "ZA, Western Cape, Cape Town, Thawte Consulting cc, Certification Services Division, Thawte Premium Server CA, premium-server@thawte.com"

	.balign 4, 0
ov00_02217240:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02217244:
	.word ov00_02217258
	.word 0x00000080
	.word ov00_022182D8
	.word 0x00000003
	.word ov00_02217240

ov00_02217258:
	.asciz "ZA, Western Cape, Cape Town, Thawte Consulting cc, Certification Services Division, Thawte Server CA, server-certs@thawte.com"

	.balign 4, 0
ov00_022182D8:
	.byte 0xD3, 0xA4, 0x50, 0x6E, 0xC8, 0xFF, 0x56, 0x6B
	.byte 0xE6, 0xCF, 0x5D, 0xB6, 0xEA, 0x0C, 0x68, 0x75, 0x47, 0xA2, 0xAA, 0xC2, 0xDA, 0x84, 0x25, 0xFC
	.byte 0xA8, 0xF4, 0x47, 0x51, 0xDA, 0x85, 0xB5, 0x20, 0x74, 0x94, 0x86, 0x1E, 0x0F, 0x75, 0xC9, 0xE9
	.byte 0x08, 0x61, 0xF5, 0x06, 0x6D, 0x30, 0x6E, 0x15, 0x19, 0x02, 0xE9, 0x52, 0xC0, 0x62, 0xDB, 0x4D
	.byte 0x99, 0x9E, 0xE2, 0x6A, 0x0C, 0x44, 0x38, 0xCD, 0xFE, 0xBE, 0xE3, 0x64, 0x09, 0x70, 0xC5, 0xFE
	.byte 0xB1, 0x6B, 0x29, 0xB6, 0x2F, 0x49, 0xC8, 0x3B, 0xD4, 0x27, 0x04, 0x25, 0x10, 0x97, 0x2F, 0xE7
	.byte 0x90, 0x6D, 0xC0, 0x28, 0x42, 0x99, 0xD7, 0x4C, 0x43, 0xDE, 0xC3, 0xF5, 0x21, 0x6D, 0x54, 0x9F
	.byte 0x5D, 0xC3, 0x58, 0xE1, 0xC0, 0xE4, 0xD9, 0x5B, 0xB0, 0xB8, 0xDC, 0xB4, 0x7B, 0xDF, 0x36, 0x3A
	.byte 0xC2, 0xB5, 0x66, 0x22, 0x12, 0xD6, 0x87, 0x0D

	.balign 4, 0
ov00_02217358:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_0221735C:
	.word ov00_022173F0
	.word 0x00000080
	.word ov00_02217370
	.word 0x00000003
	.word ov00_02217358

ov00_02217370:
	.byte 0xCC, 0x5E, 0xD1, 0x11, 0x5D, 0x5C, 0x69, 0xD0, 0xAB, 0xD3, 0xB9, 0x6A, 0x4C, 0x99, 0x1F, 0x59
	.byte 0x98, 0x30, 0x8E, 0x16, 0x85, 0x20, 0x46, 0x6D, 0x47, 0x3F, 0xD4, 0x85, 0x20, 0x84, 0xE1, 0x6D
	.byte 0xB3, 0xF8, 0xA4, 0xED, 0x0C, 0xF1, 0x17, 0x0F, 0x3B, 0xF9, 0xA7, 0xF9, 0x25, 0xD7, 0xC1, 0xCF
	.byte 0x84, 0x63, 0xF2, 0x7C, 0x63, 0xCF, 0xA2, 0x47, 0xF2, 0xC6, 0x5B, 0x33, 0x8E, 0x64, 0x40, 0x04
	.byte 0x68, 0xC1, 0x80, 0xB9, 0x64, 0x1C, 0x45, 0x77, 0xC7, 0xD8, 0x6E, 0xF5, 0x95, 0x29, 0x3C, 0x50
	.byte 0xE8, 0x34, 0xD7, 0x78, 0x1F, 0xA8, 0xBA, 0x6D, 0x43, 0x91, 0x95, 0x8F, 0x45, 0x57, 0x5E, 0x7E
	.byte 0xC5, 0xFB, 0xCA, 0xA4, 0x04, 0xEB, 0xEA, 0x97, 0x37, 0x54, 0x30, 0x6F, 0xBB, 0x01, 0x47, 0x32
	.byte 0x33, 0xCD, 0xDC, 0x57, 0x9B, 0x64, 0x69, 0x61, 0xF8, 0x9B, 0x1D, 0x1C, 0x89, 0x4F, 0x5C, 0x67

ov00_022173F0:
	.asciz "US, VeriSign, Inc., Class 3 Public Primary Certification Authority - G2, (c) 1998 VeriSign, Inc. - For authorized use only, VeriSign Trust Network"
	.balign 4, 0

	.balign 4, 0
ov00_02217484:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02217488:
	.word ov00_0221749C
	.word 0x00000100
	.word ov00_02217538
	.word 0x00000003
	.word ov00_02217484

ov00_0221749C:
	.asciz "US, VeriSign, Inc., VeriSign Trust Network, (c) 1999 VeriSign, Inc. - For authorized use only, VeriSign Class 3 Public Primary Certification Authority - G3"
	.balign 4, 0

ov00_02217538:
	.byte 0xCB, 0xBA, 0x9C, 0x52, 0xFC, 0x78, 0x1F, 0x1A
	.byte 0x1E, 0x6F, 0x1B, 0x37, 0x73, 0xBD, 0xF8, 0xC9, 0x6B, 0x94, 0x12, 0x30, 0x4F, 0xF0, 0x36, 0x47
	.byte 0xF5, 0xD0, 0x91, 0x0A, 0xF5, 0x17, 0xC8, 0xA5, 0x61, 0xC1, 0x16, 0x40, 0x4D, 0xFB, 0x8A, 0x61
	.byte 0x90, 0xE5, 0x76, 0x20, 0xC1, 0x11, 0x06, 0x7D, 0xAB, 0x2C, 0x6E, 0xA6, 0xF5, 0x11, 0x41, 0x8E
	.byte 0xFA, 0x2D, 0xAD, 0x2A, 0x61, 0x59, 0xA4, 0x67, 0x26, 0x4C, 0xD0, 0xE8, 0xBC, 0x52, 0x5B, 0x70
	.byte 0x20, 0x04, 0x58, 0xD1, 0x7A, 0xC9, 0xA4, 0x69, 0xBC, 0x83, 0x17, 0x64, 0xAD, 0x05, 0x8B, 0xBC
	.byte 0xD0, 0x58, 0xCE, 0x8D, 0x8C, 0xF5, 0xEB, 0xF0, 0x42, 0x49, 0x0B, 0x9D, 0x97, 0x27, 0x67, 0x32
	.byte 0x6E, 0xE1, 0xAE, 0x93, 0x15, 0x1C, 0x70, 0xBC, 0x20, 0x4D, 0x2F, 0x18, 0xDE, 0x92, 0x88, 0xE8
	.byte 0x6C, 0x85, 0x57, 0x11, 0x1A, 0xE9, 0x7E, 0xE3, 0x26, 0x11, 0x54, 0xA2, 0x45, 0x96, 0x55, 0x83
	.byte 0xCA, 0x30, 0x89, 0xE8, 0xDC, 0xD8, 0xA3, 0xED, 0x2A, 0x80, 0x3F, 0x7F, 0x79, 0x65, 0x57, 0x3E
	.byte 0x15, 0x20, 0x66, 0x08, 0x2F, 0x95, 0x93, 0xBF, 0xAA, 0x47, 0x2F, 0xA8, 0x46, 0x97, 0xF0, 0x12
	.byte 0xE2, 0xFE, 0xC2, 0x0A, 0x2B, 0x51, 0xE6, 0x76, 0xE6, 0xB7, 0x46, 0xB7, 0xE2, 0x0D, 0xA6, 0xCC
	.byte 0xA8, 0xC3, 0x4C, 0x59, 0x55, 0x89, 0xE6, 0xE8, 0x53, 0x5C, 0x1C, 0xEA, 0x9D, 0xF0, 0x62, 0x16
	.byte 0x0B, 0xA7, 0xC9, 0x5F, 0x0C, 0xF0, 0xDE, 0xC2, 0x76, 0xCE, 0xAF, 0xF7, 0x6A, 0xF2, 0xFA, 0x41
	.byte 0xA6, 0xA2, 0x33, 0x14, 0xC9, 0xE5, 0x7A, 0x63, 0xD3, 0x9E, 0x62, 0x37, 0xD5, 0x85, 0x65, 0x9E
	.byte 0x0E, 0xE6, 0x53, 0x24, 0x74, 0x1B, 0x5E, 0x1D, 0x12, 0x53, 0x5B, 0xC7, 0x2C, 0xE7, 0x83, 0x49
	.byte 0x3B, 0x15, 0xAE, 0x8A, 0x68, 0xB9, 0x57, 0x97

	.balign 4, 0
ov00_02217638:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_0221763C:
	.word ov00_02217650
	.word 0x00000080
	.word ov00_02217694
	.word 0x00000003
	.word ov00_02217638

ov00_02217650:
	.asciz "US, VeriSign, Inc., Class 3 Public Primary Certification Authority"
	.balign 4, 0

ov00_02217694:
	.byte 0xC9, 0x5C, 0x59, 0x9E, 0xF2, 0x1B, 0x8A, 0x01, 0x14, 0xB4, 0x10, 0xDF
	.byte 0x04, 0x40, 0xDB, 0xE3, 0x57, 0xAF, 0x6A, 0x45, 0x40, 0x8F, 0x84, 0x0C, 0x0B, 0xD1, 0x33, 0xD9
	.byte 0xD9, 0x11, 0xCF, 0xEE, 0x02, 0x58, 0x1F, 0x25, 0xF7, 0x2A, 0xA8, 0x44, 0x05, 0xAA, 0xEC, 0x03
	.byte 0x1F, 0x78, 0x7F, 0x9E, 0x93, 0xB9, 0x9A, 0x00, 0xAA, 0x23, 0x7D, 0xD6, 0xAC, 0x85, 0xA2, 0x63
	.byte 0x45, 0xC7, 0x72, 0x27, 0xCC, 0xF4, 0x4C, 0xC6, 0x75, 0x71, 0xD2, 0x39, 0xEF, 0x4F, 0x42, 0xF0
	.byte 0x75, 0xDF, 0x0A, 0x90, 0xC6, 0x8E, 0x20, 0x6F, 0x98, 0x0F, 0xF8, 0xAC, 0x23, 0x5F, 0x70, 0x29
	.byte 0x36, 0xA4, 0xC9, 0x86, 0xE7, 0xB1, 0x9A, 0x20, 0xCB, 0x53, 0xA5, 0x85, 0xE7, 0x3D, 0xBE, 0x7D
	.byte 0x9A, 0xFE, 0x24, 0x45, 0x33, 0xDC, 0x76, 0x15, 0xED, 0x0F, 0xA2, 0x71, 0x64, 0x4C, 0x65, 0x2E
	.byte 0x81, 0x68, 0x45, 0xA7

	.balign 4, 0
ov00_02217714:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_02217718:
	.word ov00_0221772C
	.word 0x0000007D
	.word ov00_02217770
	.word 0x00000003
	.word ov00_02217714

ov00_0221772C:
	.asciz "US, RSA Data Security, Inc., Secure Server Certification Authority"
	.balign 4, 0

ov00_02217770:
	.byte 0x92, 0xCE, 0x7A, 0xC1, 0xAE, 0x83, 0x3E, 0x5A, 0xAA, 0x89, 0x83, 0x57, 0xAC, 0x25, 0x01, 0x76
	.byte 0x0C, 0xAD, 0xAE, 0x8E, 0x2C, 0x37, 0xCE, 0xEB, 0x35, 0x78, 0x64, 0x54, 0x03, 0xE5, 0x84, 0x40
	.byte 0x51, 0xC9, 0xBF, 0x8F, 0x08, 0xE2, 0x8A, 0x82, 0x08, 0xD2, 0x16, 0x86, 0x37, 0x55, 0xE9, 0xB1
	.byte 0x21, 0x02, 0xAD, 0x76, 0x68, 0x81, 0x9A, 0x05, 0xA2, 0x4B, 0xC9, 0x4B, 0x25, 0x66, 0x22, 0x56
	.byte 0x6C, 0x88, 0x07, 0x8F, 0xF7, 0x81, 0x59, 0x6D, 0x84, 0x07, 0x65, 0x70, 0x13, 0x71, 0x76, 0x3E
	.byte 0x9B, 0x77, 0x4C, 0xE3, 0x50, 0x89, 0x56, 0x98, 0x48, 0xB9, 0x1D, 0xA7, 0x29, 0x1A, 0x13, 0x2E
	.byte 0x4A, 0x11, 0x59, 0x9C, 0x1E, 0x15, 0xD5, 0x49, 0x54, 0x2C, 0x73, 0x3A, 0x69, 0x82, 0xB1, 0x97
	.byte 0x39, 0x9C, 0x6D, 0x70, 0x67, 0x48, 0xE5, 0xDD, 0x2D, 0xD6, 0xC8, 0x1E, 0x7B

	.balign 4, 0
ov00_022177F0:
	.byte 0x01, 0x00, 0x01
	.balign 4, 0

ov00_022177F4:
	.word ov00_02217808
	.word 0x00000100
	.word ov00_02217838
	.word 0x00000003
	.word ov00_022177F0

ov00_02217808:
	.asciz "GlobalSign Root CA - R2, GlobalSign, GlobalSign"
	.balign 4, 0

ov00_02217838:
	.byte 0xA6, 0xCF, 0x24, 0x0E, 0xBE, 0x2E, 0x6F, 0x28
	.byte 0x99, 0x45, 0x42, 0xC4, 0xAB, 0x3E, 0x21, 0x54, 0x9B, 0x0B, 0xD3, 0x7F, 0x84, 0x70, 0xFA, 0x12
	.byte 0xB3, 0xCB, 0xBF, 0x87, 0x5F, 0xC6, 0x7F, 0x86, 0xD3, 0xB2, 0x30, 0x5C, 0xD6, 0xFD, 0xAD, 0xF1
	.byte 0x7B, 0xDC, 0xE5, 0xF8, 0x60, 0x96, 0x09, 0x92, 0x10, 0xF5, 0xD0, 0x53, 0xDE, 0xFB, 0x7B, 0x7E
	.byte 0x73, 0x88, 0xAC, 0x52, 0x88, 0x7B, 0x4A, 0xA6, 0xCA, 0x49, 0xA6, 0x5E, 0xA8, 0xA7, 0x8C, 0x5A
	.byte 0x11, 0xBC, 0x7A, 0x82, 0xEB, 0xBE, 0x8C, 0xE9, 0xB3, 0xAC, 0x96, 0x25, 0x07, 0x97, 0x4A, 0x99
	.byte 0x2A, 0x07, 0x2F, 0xB4, 0x1E, 0x77, 0xBF, 0x8A, 0x0F, 0xB5, 0x02, 0x7C, 0x1B, 0x96, 0xB8, 0xC5
	.byte 0xB9, 0x3A, 0x2C, 0xBC, 0xD6, 0x12, 0xB9, 0xEB, 0x59, 0x7D, 0xE2, 0xD0, 0x06, 0x86, 0x5F, 0x5E
	.byte 0x49, 0x6A, 0xB5, 0x39, 0x5E, 0x88, 0x34, 0xEC, 0xBC, 0x78, 0x0C, 0x08, 0x98, 0x84, 0x6C, 0xA8
	.byte 0xCD, 0x4B, 0xB4, 0xA0, 0x7D, 0x0C, 0x79, 0x4D, 0xF0, 0xB8, 0x2D, 0xCB, 0x21, 0xCA, 0xD5, 0x6C
	.byte 0x5B, 0x7D, 0xE1, 0xA0, 0x29, 0x84, 0xA1, 0xF9, 0xD3, 0x94, 0x49, 0xCB, 0x24, 0x62, 0x91, 0x20
	.byte 0xBC, 0xDD, 0x0B, 0xD5, 0xD9, 0xCC, 0xF9, 0xEA, 0x27, 0x0A, 0x2B, 0x73, 0x91, 0xC6, 0x9D, 0x1B
	.byte 0xAC, 0xC8, 0xCB, 0xE8, 0xE0, 0xA0, 0xF4, 0x2F, 0x90, 0x8B, 0x4D, 0xFB, 0xB0, 0x36, 0x1B, 0xF6
	.byte 0x19, 0x7A, 0x85, 0xE0, 0x6D, 0xF2, 0x61, 0x13, 0x88, 0x5C, 0x9F, 0xE0, 0x93, 0x0A, 0x51, 0x97
	.byte 0x8A, 0x5A, 0xCE, 0xAF, 0xAB, 0xD5, 0xF7, 0xAA, 0x09, 0xAA, 0x60, 0xBD, 0xDC, 0xD9, 0x5F, 0xDF
	.byte 0x72, 0xA9, 0x60, 0x13, 0x5E, 0x00, 0x01, 0xC9, 0x4A, 0xFA, 0x3F, 0xA4, 0xEA, 0x07, 0x03, 0x21
	.byte 0x02, 0x8E, 0x82, 0xCA, 0x03, 0xC2, 0x9B, 0x8F
	.balign 4, 0
ov00_02217938:
	.asciz "https://nas.test.nintendowifi.net/ac"
	.balign 4, 0
ov00_02217960:
	.asciz "https://nas.dev.nintendowifi.net/ac"
	.balign 4, 0
ov00_02217984:
	.asciz "https://nas.nintendowifi.net/ac"
	.balign 4, 0
ov00_022179A4:
	.asciz "0000"
	.balign 4, 0
ov00_022179AC:
	.asciz ""
	.balign 4, 0
ov00_022179B0:
	.asciz "https:///download"
	.balign 4, 0
ov00_022179C4:
	.asciz "https://%s/download"
	.balign 4, 0
ov00_022179D8:
	.asciz "9000"
	.balign 4, 0
ov00_022179E0:
	.asciz "%c%s%c%s"
	.balign 4, 0
ov00_022179EC:
	.asciz "SCM"
	.balign 4, 0
ov00_022179F0:
	.asciz "SCN"
	.balign 4, 0
ov00_022179F4:
	.asciz ""
	.balign 4, 0
ov00_022179F8:
	.asciz "GPCM"
	.balign 4, 0
ov00_02217A00:
	.asciz "MAT"
	.balign 4, 0
ov00_02217A04:
	.asciz ""
	.balign 4, 0
ov00_02217A08:
	.asciz "SCM"
	.balign 4, 0
ov00_02217A0C:
	.asciz "SCN"
	.balign 4, 0
ov00_02217A10:
	.asciz "I have authorized your request to add me to your list"
	.balign 4, 0
ov00_02217A48:
	.asciz ""
	.balign 4, 0
ov00_02217A4C:
	.asciz "%c%c%c%c"
	.balign 4, 0
ov00_02217A58:
	.asciz " and ()"
	.balign 4, 0
ov00_02217A60:
	.asciz ""
	.balign 4, 0
ov00_02217A64:
	.asciz "%u"
	.balign 4, 0
ov00_02217A68:
	.asciz "FME"
	.balign 4, 0
ov00_02217A6C:
	.asciz "MDF"
	.balign 4, 0
ov00_02217A70:
	.asciz "VER"
	.balign 4, 0
ov00_02217A74:
	.asciz "Init state"
	.balign 4, 0
ov00_02217A80:
	.asciz "Server full"
	.balign 4, 0
ov00_02217A8C:
	.asciz "Unknown connect attempt"
	.balign 4, 0
ov00_02217AA4:
	.asciz "SCM"
	.balign 4, 0
ov00_02217AA8:
	.asciz "SCN"
	.balign 4, 0
ov00_02217AAC:
	.asciz "dwc_pid"
	.balign 4, 0
ov00_02217AB4:
	.asciz "dwc_mtype"
	.balign 4, 0
ov00_02217AC0:
	.asciz "dwc_mresv"
	.balign 4, 0
ov00_02217ACC:
	.asciz "dwc_mver"
	.balign 4, 0
ov00_02217AD8:
	.asciz "dwc_eval"
	.balign 4, 0
ov00_02217AE4:
	.asciz " and (%s)"
	.balign 4, 0
ov00_02217AF0:
	.asciz "%s = %u"
	.balign 4, 0
ov00_02217AF8:
	.asciz "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s = %d and %s != %s"
	.balign 4, 0
ov00_02217B50:
	.asciz "/%u"
	.balign 4, 0
ov00_02217B54:
	.asciz "SBCM"
	.balign 4, 0
ov00_02217B5C:
	.asciz "%s%dv%s"
	.balign 4, 0
ov00_02217B64:
	.asciz "GPCM"
	.balign 4, 0
ov00_02217B6C:
	.asciz "MAT"
	.balign 4, 0
ov00_02217B70:
	.asciz "numplayers"
	.balign 4, 0
ov00_02217B7C:
	.asciz "maxplayers"
	.balign 4, 0
ov00_02217B88:
	.asciz "DT"
	.balign 4, 0
ov00_02217B8C:
	.asciz "NWCUSBAP"
	.balign 4, 0
ov00_02217B98:
	.asciz "NINTENDO-DS"
	.balign 4, 0
ov00_02217BA4:
	.word ov00_02217BF0
	.word ov00_02217BB4
	.word ov00_02217BD4
	.word ov00_02217BC4
ov00_02217BB4:
	.asciz "gwi'6&fs=0Nf~"
	.balign 4, 0
ov00_02217BC4:
	.asciz "%(egEr)ag(s&m"
	.balign 4, 0
ov00_02217BD4:
	.asciz "952uybjnpmu903bia@bk5m[-"
	.balign 4, 0
ov00_02217BF0:
	.asciz "38g6zxjk20gvmv]6^=j&%vY1"
	.balign 4, 0
ov00_02217C0C:
	.asciz "NDWCSHAP"
	.balign 4, 0
ov00_02217C18:
	.asciz "NWCUSBAP"
	.balign 4, 0
ov00_02217C24:
	.asciz "%s.available.gs.nintendowifi.net"
	.balign 4, 0
ov00_02217C48:
	.byte 0xFE, 0xFD, 0x09, 0x00
ov00_02217C4C:
	.asciz "ArrayMap"
	.balign 4, 0
ov00_02217C58:
	.asciz "ArrayNth"
	.balign 4, 0
ov00_02217C64:
	.asciz "ArrayNew"
	.balign 4, 0
ov00_02217C70:
	.asciz "ArrayFree"
	.balign 4, 0
ov00_02217C7C:
	.asciz "ArrayGrow"
	.balign 4, 0
ov00_02217C88:
	.asciz "ArrayAppend"
	.balign 4, 0
ov00_02217C94:
	.asciz "ArrayRemoveAt"
	.balign 4, 0
ov00_02217CA4:
	.asciz "ArrayDeleteAt"
	.balign 4, 0
ov00_02217CB4:
	.asciz "ArrayInsertAt"
	.balign 4, 0
ov00_02217CC4:
	.asciz "ArrayReplaceAt"
	.balign 4, 0
ov00_02217CD4:
	.asciz "ArrayMapBackwards"
	.balign 4, 0
ov00_02217CE8:
	.asciz "ArrayInsertSorted"
	.balign 4, 0
ov00_02217CFC:
	.asciz "ArrayMapBackwards2"
	.balign 4, 0
ov00_02217D10:
	.asciz "array->list"
	.balign 4, 0
ov00_02217D1C:
	.asciz "darray.c"
	.balign 4, 0
ov00_02217D28:
	.asciz "array"
	.balign 4, 0
ov00_02217D30:
	.asciz "elemSize"
	.balign 4, 0
ov00_02217D3C:
	.asciz "(n >= 0) && (n < array->count)"
	.balign 4, 0
ov00_02217D5C:
	.asciz "(n >= 0) && (n <= array->count)"
	.balign 4, 0
ov00_02217D7C:
	.asciz "comparator"
	.balign 4, 0
ov00_02217D88:
	.asciz "fn"
	.balign 4, 0
ov00_02217D8C:
	.asciz "TableMap"
	.balign 4, 0
ov00_02217D98:
	.asciz "TableNew2"
	.balign 4, 0
ov00_02217DA4:
	.asciz "TableFree"
	.balign 4, 0
ov00_02217DB0:
	.asciz "TableEnter"
	.balign 4, 0
ov00_02217DBC:
	.asciz "TableCount"
	.balign 4, 0
ov00_02217DC8:
	.asciz "TableRemove"
	.balign 4, 0
ov00_02217DD4:
	.asciz "TableLookup"
	.balign 4, 0
ov00_02217DE0:
	.asciz "TableMapSafe"
	.balign 4, 0
ov00_02217DF0:
	.asciz "TableMapSafe2"
	.balign 4, 0
ov00_02217E00:
	.asciz "hashFn"
	.balign 4, 0
ov00_02217E08:
	.asciz "hashtable.c"
	.balign 4, 0
ov00_02217E14:
	.asciz "compFn"
	.balign 4, 0
ov00_02217E1C:
	.asciz "elemSize"
	.balign 4, 0
ov00_02217E28:
	.asciz "nBuckets"
	.balign 4, 0
ov00_02217E34:
	.asciz "table"
	.balign 4, 0
ov00_02217E3C:
	.asciz "table->buckets"
	.balign 4, 0
ov00_02217E4C:
	.asciz "fn"
	.balign 4, 0
ov00_02217E50:
	.asciz "%02x"
	.balign 4, 0
ov00_02217E58:
	.asciz ""
	.balign 4, 0
ov00_02217E5C:
	.asciz "time"
	.balign 4, 0
ov00_02217E64:
	.asciz "current_time"
	.balign 4, 0
ov00_02217E74:
	.asciz "OS_IsTickAvailable() == TRUE"
	.balign 4, 0
ov00_02217E94:
	.asciz "nonport.c"
	.balign 4, 0
ov00_02217EA0:
	.asciz "localhost"
	.balign 4, 0
ov00_02217EAC:
	.asciz "gpGetBuddyStatus"
	.balign 4, 0
ov00_02217EC0:
	.asciz "Invalid func."
	.balign 4, 0
ov00_02217ED0:
	.asciz "No callback."
	.balign 4, 0
ov00_02217EE0:
	.asciz ""
	.balign 4, 0
ov00_02217EE4:
	.asciz "The connection has already been disconnected."
	.balign 4, 0
ov00_02217F14:
	.asciz "Invalid reason."
	.balign 4, 0
ov00_02217F24:
	.asciz "\\addbuddy\\"
	.balign 4, 0
ov00_02217F30:
	.asciz "\\sesskey\\"
	.balign 4, 0
ov00_02217F3C:
	.asciz "\\newprofileid\\"
	.balign 4, 0
ov00_02217F4C:
	.asciz "\\reason\\"
	.balign 4, 0
ov00_02217F58:
	.asciz "\\final\\"
	.balign 4, 0
ov00_02217F60:
	.asciz "Invalid status."
	.balign 4, 0
ov00_02217F70:
	.asciz "Invalid index."
	.balign 4, 0
ov00_02217F80:
	.asciz "buddyStatus"
	.balign 4, 0
ov00_02217F8C:
	.asciz "gp.c"
	.balign 4, 0
ov00_02217F94:
	.asciz "Invalid statusString."
	.balign 4, 0
ov00_02217FAC:
	.asciz "Invalid locationString."
	.balign 4, 0
ov00_02217FC4:
	.asciz "\\status\\"
	.balign 4, 0
ov00_02217FD0:
	.asciz "\\statstring\\"
	.balign 4, 0
ov00_02217FE0:
	.asciz "\\locstring\\"
	.balign 4, 0
ov00_02217FEC:
	.asciz "Invalid message."
	.balign 4, 0
ov00_02218000:
	.asciz "gpiProcess"
	.balign 4, 0
ov00_0221800C:
	.asciz "\n\n\n\n\n*************\ngpiInitialize\n"
	.balign 4, 0
ov00_02218030:
	.asciz "CM"
	.balign 4, 0
ov00_02218034:
	.asciz "There was an error reading from the server."
	.balign 4, 0
ov00_02218060:
	.asciz "\\final\\"
	.balign 4, 0
ov00_02218068:
	.asciz "CMD: %s\n"
	.balign 4, 0
ov00_02218074:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02218084:
	.asciz "\\id\\"
	.balign 4, 0
ov00_0221808C:
	.asciz "No matching operation found for id %d\n"
	.balign 4, 0
ov00_022180B4:
	.asciz "\\bm\\"
	.balign 4, 0
ov00_022180BC:
	.asciz "\\ka\\"
	.balign 4, 0
ov00_022180C4:
	.asciz "Received an unrecognized, unsolicited message.\n"
	.balign 4, 0
ov00_022180F4:
	.asciz "The server has closed the connection."
	.balign 4, 0
ov00_0221811C:
	.asciz "(iconnection->connectState == GPI_NOT_CONNECTED) || (iconnection->connectState == GPI_CONNECTING) || (iconnection->connectState == GPI_NEGOTIATING) || (iconnection->connectState == GPI_CONNECTED) || (iconnection->connectState == GPI_DISCONNECTED)"
	.balign 4, 0
ov00_02218214:
	.asciz "gpi.c"
	.balign 4, 0
ov00_0221821C:
	.asciz "0"
	.balign 4, 0
ov00_02218220:
	.asciz "gpiDeleteBuddy"
	.balign 4, 0
ov00_02218230:
	.asciz "\\authadd\\"
	.balign 4, 0
ov00_0221823C:
	.asciz "\\sesskey\\"
	.balign 4, 0
ov00_02218248:
	.asciz "\\fromprofileid\\"
	.balign 4, 0
ov00_02218258:
	.asciz "\\sig\\"
	.balign 4, 0
ov00_02218260:
	.asciz "\\final\\"
	.balign 4, 0
ov00_02218268:
	.asciz "\\bm\\"
	.balign 4, 0
ov00_02218270:
	.asciz "Unexpected data was received from the server."
	.balign 4, 0
ov00_022182A0:
	.asciz "\\f\\"
	.balign 4, 0
ov00_022182A4:
	.asciz "\\date\\"
	.balign 4, 0
ov00_022182AC:
	.asciz "Out of memory."
	.balign 4, 0
ov00_022182BC:
	.asciz "\\msg\\"
	.balign 4, 0
ov00_022182C4:
	.asciz "|signed|"
	.balign 4, 0
ov00_022182D0:
	.asciz "|s|"
	.balign 4, 0
ov00_022182D4:
	.asciz "|ss|"
	.balign 4, 0
ov00_022182DC:
	.asciz "|ls|"
	.balign 4, 0
ov00_022182E4:
	.asciz "|ip|"
	.balign 4, 0
ov00_022182EC:
	.asciz "|p|"
	.balign 4, 0
ov00_022182F0:
	.asciz "|l|"
	.balign 4, 0
ov00_022182F4:
	.asciz "1"
	.balign 4, 0
ov00_022182F8:
	.asciz "\\t\\"
	.balign 4, 0
ov00_022182FC:
	.asciz "Invalid profile."
	.balign 4, 0
ov00_02218310:
	.asciz "\\delbuddy\\"
	.balign 4, 0
ov00_0221831C:
	.asciz "\\delprofileid\\"
	.balign 4, 0
ov00_0221832C:
	.asciz "index >= 0"
	.balign 4, 0
ov00_02218338:
	.asciz "gpiBuddy.c"
	.balign 4, 0
ov00_02218344:
	.asciz "iconnection->profileList.numBuddies >= 0"
	.balign 4, 0
ov00_02218370:
	.asciz "gpiRecvToBuffer"
	.balign 4, 0
ov00_02218380:
	.asciz "gpiSendFromBuffer"
	.balign 4, 0
ov00_02218394:
	.asciz "gpiSendOrBufferChar"
	.balign 4, 0
ov00_022183A8:
	.asciz "gpiAppendCharToBuffer"
	.balign 4, 0
ov00_022183C0:
	.asciz "gpiSendOrBufferStringLen"
	.balign 4, 0
ov00_022183DC:
	.asciz "gpiAppendStringToBufferLen"
	.balign 4, 0
ov00_022183F8:
	.asciz "outputBuffer != NULL"
	.balign 4, 0
ov00_02218410:
	.asciz "gpiBuffer.c"
	.balign 4, 0
ov00_0221841C:
	.asciz "Out of memory."
	.balign 4, 0
ov00_0221842C:
	.asciz "string != NULL"
	.balign 4, 0
ov00_0221843C:
	.asciz "stringLen >= 0"
	.balign 4, 0
ov00_0221844C:
	.asciz "%d"
	.balign 4, 0
ov00_02218450:
	.asciz "There was an error sending on a socket."
	.balign 4, 0
ov00_02218478:
	.asciz "SENDXXXX(%s): Connection closed\n"
	.balign 4, 0
ov00_0221849C:
	.asciz "peer->outputBuffer.buffer != NULL"
	.balign 4, 0
ov00_022184C0:
	.asciz "PT"
	.balign 4, 0
ov00_022184C4:
	.asciz "sock != INVALID_SOCKET"
	.balign 4, 0
ov00_022184DC:
	.asciz "inputBuffer != NULL"
	.balign 4, 0
ov00_022184F0:
	.asciz "bytesRead != NULL"
	.balign 4, 0
ov00_02218504:
	.asciz "connClosed != NULL"
	.balign 4, 0
ov00_02218518:
	.asciz "There was an error reading from a socket."
	.balign 4, 0
ov00_02218544:
	.asciz "RECVXXXX(%s): Connection closed\n"
	.balign 4, 0
ov00_02218568:
	.asciz "RECVTOTL(%s): %d\n"
	.balign 4, 0
ov00_0221857C:
	.asciz "len >= 0"
	.balign 4, 0
ov00_02218588:
	.asciz "pos >= 0"
	.balign 4, 0
ov00_02218594:
	.asciz "pos <= len"
	.balign 4, 0
ov00_022185A0:
	.asciz "\\msg\\"
	.balign 4, 0
ov00_022185A8:
	.asciz "\\m\\"
	.balign 4, 0
ov00_022185AC:
	.asciz "\\len\\"
	.balign 4, 0
ov00_022185B4:
	.asciz "gpiCallCallback"
	.balign 4, 0
ov00_022185C4:
	.asciz "gpiCallErrorCallback"
	.balign 4, 0
ov00_022185DC:
	.asciz "iconnection != NULL"
	.balign 4, 0
ov00_022185F0:
	.asciz "gpiCallback.c"
	.balign 4, 0
ov00_02218600:
	.asciz "result != GP_NO_ERROR"
	.balign 4, 0
ov00_02218618:
	.asciz "(fatal == GP_FATAL) || (fatal == GP_NON_FATAL)"
	.balign 4, 0
ov00_02218648:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02218658:
	.asciz "data->callback.callback != NULL"
	.balign 4, 0
ov00_02218678:
	.asciz "data->arg != NULL"
	.balign 4, 0
ov00_0221868C:
	.asciz "gpiStartConnect"
	.balign 4, 0
ov00_0221869C:
	.asciz "gpiCheckConnect"
	.balign 4, 0
ov00_022186AC:
	.asciz "gpcm.gs.nintendowifi.net"
	.balign 4, 0
	.space 0x40-(.-ov00_022186AC)
ov00_022186EC:
	.asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
	.balign 4, 0
ov00_0221872C:
	.asciz "There was an error creating a socket."
	.balign 4, 0
ov00_02218754:
	.asciz "There was an error making a socket non-blocking."
	.balign 4, 0
ov00_02218788:
	.asciz "There was an error binding a socket."
	.balign 4, 0
ov00_022187B0:
	.asciz "There was an error listening on a socket."
	.balign 4, 0
ov00_022187DC:
	.asciz "There was an error getting a socket's addres."
	.balign 4, 0
ov00_0221880C:
	.asciz "Could not resolve connection mananger host name."
	.balign 4, 0
ov00_02218840:
	.asciz "address.sin_addr.s_addr != 0"
	.balign 4, 0
ov00_02218860:
	.asciz "gpiConnect.c"
	.balign 4, 0
ov00_02218870:
	.asciz "There was an error connecting a socket."
	.balign 4, 0
ov00_02218898:
	.asciz "Invalid connection."
	.balign 4, 0
ov00_022188AC:
	.asciz "Invalid firewall."
	.balign 4, 0
ov00_022188C0:
	.asciz "Out of memory."
	.balign 4, 0
ov00_022188D0:
	.asciz "%s@%s"
	.balign 4, 0
ov00_022188D8:
	.asciz "%s%s%s%s%s%s"
	.balign 4, 0
ov00_022188E8:
	.asciz "                                                "
	.balign 4, 0
ov00_0221891C:
	.asciz "\\login\\"
	.balign 4, 0
ov00_02218924:
	.asciz "\\challenge\\"
	.balign 4, 0
ov00_02218930:
	.asciz "\\authtoken\\"
	.balign 4, 0
ov00_0221893C:
	.asciz "\\uniquenick\\"
	.balign 4, 0
ov00_0221894C:
	.asciz "\\user\\"
	.balign 4, 0
ov00_02218954:
	.asciz "@"
	.balign 4, 0
ov00_02218958:
	.asciz "\\userid\\"
	.balign 4, 0
ov00_02218964:
	.asciz "\\profileid\\"
	.balign 4, 0
ov00_02218970:
	.asciz "\\response\\"
	.balign 4, 0
ov00_0221897C:
	.asciz "\\firewall\\1"
	.balign 4, 0
ov00_02218988:
	.asciz "\\port\\"
	.balign 4, 0
ov00_02218990:
	.asciz "\\productid\\"
	.balign 4, 0
ov00_0221899C:
	.asciz "\\gamename\\"
	.balign 4, 0
ov00_022189A8:
	.asciz "\\namespaceid\\"
	.balign 4, 0
ov00_022189B8:
	.asciz "\\id\\1"
	.balign 4, 0
ov00_022189C0:
	.asciz "\\final\\"
	.balign 4, 0
ov00_022189C8:
	.asciz "\\newuser\\"
	.balign 4, 0
ov00_022189D4:
	.asciz "\\email\\"
	.balign 4, 0
ov00_022189DC:
	.asciz "\\nick\\"
	.balign 4, 0
ov00_022189E4:
	.asciz "\\passwordenc\\"
	.balign 4, 0
ov00_022189F4:
	.asciz "\\cdkeyenc\\"
	.balign 4, 0
ov00_02218A00:
	.asciz "\\pid\\"
	.balign 4, 0
ov00_02218A08:
	.asciz "\\fatal\\"
	.balign 4, 0
ov00_02218A10:
	.asciz "\\lc\\1"
	.balign 4, 0
ov00_02218A18:
	.asciz "Unexpected data was received from the server."
	.balign 4, 0
ov00_02218A48:
	.asciz "\\nur\\"
	.balign 4, 0
ov00_02218A50:
	.asciz "Unexepected data was received from the server."
	.balign 4, 0
ov00_02218A80:
	.asciz "\\lc\\2"
	.balign 4, 0
ov00_02218A88:
	.asciz "\\sesskey\\"
	.balign 4, 0
ov00_02218A94:
	.asciz "\\lt\\"
	.balign 4, 0
ov00_02218A9C:
	.asciz "\\proof\\"
	.balign 4, 0
ov00_02218AA4:
	.asciz "Could not authenticate server."
	.balign 4, 0
ov00_02218AC4:
	.asciz "The server has refused the connection."
	.balign 4, 0
ov00_02218AEC:
	.asciz "state == GPI_CONNECTED"
	.balign 4, 0
ov00_02218B04:
	.asciz "\\logout\\\\sesskey\\"
	.balign 4, 0
ov00_02218B18:
	.asciz "CM"
	.balign 4, 0
ov00_02218B1C:
	.asciz "gpiIntToDate"
	.balign 4, 0
ov00_02218B2C:
	.asciz "gpiProcessGetInfo"
	.balign 4, 0
ov00_02218B40:
	.asciz "gpiInfo.c"
	.balign 4, 0
ov00_02218B4C:
	.asciz "Invalid date."
	.balign 4, 0
ov00_02218B5C:
	.asciz "gpiIsValidDate(d, m, y)"
	.balign 4, 0
ov00_02218B74:
	.asciz "\\pi\\"
	.balign 4, 0
ov00_02218B7C:
	.asciz "Unexpected data was received from the server."
	.balign 4, 0
ov00_02218BAC:
	.asciz "\\profileid\\"
	.balign 4, 0
ov00_02218BB8:
	.asciz "profileid > 0"
	.balign 4, 0
ov00_02218BC8:
	.asciz "\\nick\\"
	.balign 4, 0
ov00_02218BD0:
	.asciz "\\uniquenick\\"
	.balign 4, 0
ov00_02218BE0:
	.asciz "\\email\\"
	.balign 4, 0
ov00_02218BE8:
	.asciz "\\firstname\\"
	.balign 4, 0
ov00_02218BF4:
	.asciz "\\lastname\\"
	.balign 4, 0
ov00_02218C00:
	.asciz "\\icquin\\"
	.balign 4, 0
ov00_02218C0C:
	.asciz "\\homepage\\"
	.balign 4, 0
ov00_02218C18:
	.asciz "\\zipcode\\"
	.balign 4, 0
ov00_02218C24:
	.asciz "\\countrycode\\"
	.balign 4, 0
ov00_02218C34:
	.asciz "\\lon\\"
	.balign 4, 0
ov00_02218C3C:
	.asciz "\\lat\\"
	.balign 4, 0
ov00_02218C44:
	.asciz "\\loc\\"
	.balign 4, 0
ov00_02218C4C:
	.asciz "\\birthday\\"
	.balign 4, 0
ov00_02218C58:
	.asciz "\\sex\\"
	.balign 4, 0
ov00_02218C60:
	.asciz "\\pmask\\"
	.balign 4, 0
ov00_02218C68:
	.asciz "\\aim\\"
	.balign 4, 0
ov00_02218C70:
	.asciz "\\pic\\"
	.balign 4, 0
ov00_02218C78:
	.asciz "\\occ\\"
	.balign 4, 0
ov00_02218C80:
	.asciz "\\ind\\"
	.balign 4, 0
ov00_02218C88:
	.asciz "\\inc\\"
	.balign 4, 0
ov00_02218C90:
	.asciz "\\mar\\"
	.balign 4, 0
ov00_02218C98:
	.asciz "\\chc\\"
	.balign 4, 0
ov00_02218CA0:
	.asciz "\\i1\\"
	.balign 4, 0
ov00_02218CA8:
	.asciz "\\o1\\"
	.balign 4, 0
ov00_02218CB0:
	.asciz "\\conn\\"
	.balign 4, 0
ov00_02218CB8:
	.asciz "\\sig\\"
	.balign 4, 0
ov00_02218CC0:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02218CD0:
	.asciz "\\updatepro\\\\sesskey\\"
	.balign 4, 0
ov00_02218CE8:
	.asciz "\\final\\"
	.balign 4, 0
ov00_02218CF0:
	.asciz "\\updateui\\\\sesskey\\"
	.balign 4, 0
ov00_02218D04:
	.asciz "Invalid zipcode."
	.balign 4, 0
ov00_02218D18:
	.asciz "%d"
	.balign 4, 0
ov00_02218D1C:
	.asciz "0"
	.balign 4, 0
ov00_02218D20:
	.asciz "1"
	.balign 4, 0
ov00_02218D24:
	.asciz "2"
	.balign 4, 0
ov00_02218D28:
	.asciz "Invalid sex."
	.balign 4, 0
ov00_02218D38:
	.asciz "\\cpubrandid\\"
	.balign 4, 0
ov00_02218D48:
	.asciz "\\cpuspeed\\"
	.balign 4, 0
ov00_02218D54:
	.asciz "\\memory\\"
	.balign 4, 0
ov00_02218D60:
	.asciz "\\videocard1ram\\"
	.balign 4, 0
ov00_02218D70:
	.asciz "\\videocard2ram\\"
	.balign 4, 0
ov00_02218D80:
	.asciz "\\connectionid\\"
	.balign 4, 0
ov00_02218D90:
	.asciz "\\connectionspeed\\"
	.balign 4, 0
ov00_02218DA4:
	.asciz "\\hasnetwork\\"
	.balign 4, 0
ov00_02218DB4:
	.asciz "Invalid info."
	.balign 4, 0
ov00_02218DC4:
	.asciz "Invalid value."
	.balign 4, 0
ov00_02218DD4:
	.asciz "\\password\\"
	.balign 4, 0
ov00_02218DE0:
	.asciz "Invalid countrycode."
	.balign 4, 0
ov00_02218DF8:
	.asciz "\\videocard1string\\"
	.balign 4, 0
ov00_02218E0C:
	.asciz "\\videocard2string\\"
	.balign 4, 0
ov00_02218E20:
	.asciz "\\osstring\\"
	.balign 4, 0
ov00_02218E2C:
	.asciz "\\getprofile\\\\sesskey\\"
	.balign 4, 0
ov00_02218E44:
	.asciz "\\id\\"
	.balign 4, 0
ov00_02218E4C:
	.asciz "gpiDestroyOperation"
	.balign 4, 0
ov00_02218E60:
	.asciz "gpiFailedOpCallback"
	.balign 4, 0
ov00_02218E74:
	.asciz "gpiProcessOperation"
	.balign 4, 0
ov00_02218E88:
	.asciz "connection != NULL"
	.balign 4, 0
ov00_02218E9C:
	.asciz "gpiOperation.c"
	.balign 4, 0
ov00_02218EAC:
	.asciz "*connection != NULL"
	.balign 4, 0
ov00_02218EC0:
	.asciz "operation != NULL"
	.balign 4, 0
ov00_02218ED4:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02218EE4:
	.asciz "0"
	.balign 4, 0
ov00_02218EE8:
	.asciz "iconnection->numSearches >= 0"
	.balign 4, 0
ov00_02218F08:
	.asciz "gpiProcessOperation was passed an operation with an invalid type (%d)\n"
	.balign 4, 0
ov00_02218F50:
	.asciz "gpiRemovePeer"
	.balign 4, 0
ov00_02218F60:
	.asciz "gpiProcessPeer"
	.balign 4, 0
ov00_02218F70:
	.asciz "gpiPeerAddMessage"
	.balign 4, 0
ov00_02218F84:
	.asciz "gpiPeerStartTransferMessage"
	.balign 4, 0
ov00_02218FA0:
	.asciz "gpiPeerFinishTransferMessage"
	.balign 4, 0
ov00_02218FC0:
	.asciz "gpiProcessPeerAcceptingConnection"
	.balign 4, 0
ov00_02218FE4:
	.asciz "gpiProcessPeerInitiatingConnection"
	.balign 4, 0
ov00_02219008:
	.asciz "Error connecting to a peer."
	.balign 4, 0
ov00_02219024:
	.asciz "\\auth\\"
	.balign 4, 0
ov00_0221902C:
	.asciz "\\pid\\"
	.balign 4, 0
ov00_02219034:
	.asciz "\\nick\\"
	.balign 4, 0
ov00_0221903C:
	.asciz "\\sig\\"
	.balign 4, 0
ov00_02219044:
	.asciz "\\final\\"
	.balign 4, 0
ov00_0221904C:
	.asciz "PR"
	.balign 4, 0
ov00_02219050:
	.asciz "\\anack\\"
	.balign 4, 0
ov00_02219058:
	.asciz "Error getting buddy authorization."
	.balign 4, 0
ov00_0221907C:
	.asciz "\\aack\\"
	.balign 4, 0
ov00_02219084:
	.asciz "Error parsing buddy message."
	.balign 4, 0
ov00_022190A4:
	.asciz "0"
	.balign 4, 0
ov00_022190A8:
	.asciz "gpiPeer.c"
	.balign 4, 0
ov00_022190B4:
	.asciz "peer->state == GPI_PEER_WAITING"
	.balign 4, 0
ov00_022190D4:
	.asciz "%s%d%d"
	.balign 4, 0
ov00_022190DC:
	.asciz "Out of memory."
	.balign 4, 0
ov00_022190EC:
	.asciz "1"
	.balign 4, 0
ov00_022190F0:
	.asciz "peer->state != GPI_PEER_NOT_CONNECTED"
	.balign 4, 0
ov00_02219118:
	.asciz "peer != NULL"
	.balign 4, 0
ov00_02219128:
	.asciz "Tried to remove peer not in list."
	.balign 4, 0
ov00_0221914C:
	.asciz "There was an error creating a socket."
	.balign 4, 0
ov00_02219174:
	.asciz "There was an error making a socket non-blocking."
	.balign 4, 0
ov00_022191A8:
	.asciz "There was an error connecting a socket."
	.balign 4, 0
ov00_022191D0:
	.asciz "message != NULL"
	.balign 4, 0
ov00_022191E0:
	.asciz "\\m\\"
	.balign 4, 0
ov00_022191E4:
	.asciz "\\len\\"
	.balign 4, 0
ov00_022191EC:
	.asciz "\\msg\\\n"
	.balign 4, 0
ov00_022191F4:
	.asciz "transferID"
	.balign 4, 0
ov00_02219200:
	.asciz "\\m\\%d\\xfer\\%d %u %u"
	.balign 4, 0
ov00_02219214:
	.asciz ""
	.balign 4, 0
ov00_02219218:
	.asciz "\\len\\%d\\msg\\\n"
	.balign 4, 0
ov00_02219228:
	.asciz "gpiProfileListAdd"
	.balign 4, 0
ov00_0221923C:
	.asciz "\\npr\\"
	.balign 4, 0
ov00_02219244:
	.asciz "Unexpected data was received from the server."
	.balign 4, 0
ov00_02219274:
	.asciz "\\profileid\\"
	.balign 4, 0
ov00_02219280:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02219290:
	.asciz "id > 0"
	.balign 4, 0
ov00_02219298:
	.asciz "gpiProfile.c"
	.balign 4, 0
ov00_022192A8:
	.asciz "gpiProcessSearch"
	.balign 4, 0
ov00_022192BC:
	.asciz "gpiProcessSearches"
	.balign 4, 0
ov00_022192D0:
	.asciz "gpiStartProfileSearch"
	.balign 4, 0
ov00_022192E8:
	.asciz "gpsp.gs.nintendowifi.net"
	.balign 4, 0
	.space 0x40-(.-ov00_022192E8)
ov00_02219328:
	.asciz "Out of memory."
	.balign 4, 0
ov00_02219338:
	.asciz "There was an error creating a socket."
	.balign 4, 0
ov00_02219360:
	.asciz "There was an error making a socket non-blocking."
	.balign 4, 0
ov00_02219394:
	.asciz "Could not resolve search mananger host name."
	.balign 4, 0
ov00_022193C4:
	.asciz "address.sin_addr.s_addr != 0"
	.balign 4, 0
ov00_022193E4:
	.asciz "gpiSearch.c"
	.balign 4, 0
ov00_022193F0:
	.asciz "There was an error connecting a socket."
	.balign 4, 0
ov00_02219418:
	.asciz "No search criteria."
	.balign 4, 0
ov00_0221942C:
	.asciz "SM"
	.balign 4, 0
ov00_02219430:
	.asciz "Could not connect to the search manager."
	.balign 4, 0
ov00_0221945C:
	.asciz "\\search\\"
	.balign 4, 0
ov00_02219468:
	.asciz "\\sesskey\\"
	.balign 4, 0
ov00_02219474:
	.asciz "\\profileid\\"
	.balign 4, 0
ov00_02219480:
	.asciz "\\namespaceid\\"
	.balign 4, 0
ov00_02219490:
	.asciz "\\nick\\"
	.balign 4, 0
ov00_02219498:
	.asciz "\\uniquenick\\"
	.balign 4, 0
ov00_022194A8:
	.asciz "\\email\\"
	.balign 4, 0
ov00_022194B0:
	.asciz "\\firstname\\"
	.balign 4, 0
ov00_022194BC:
	.asciz "\\lastname\\"
	.balign 4, 0
ov00_022194C8:
	.asciz "\\icquin\\"
	.balign 4, 0
ov00_022194D4:
	.asciz "\\skip\\"
	.balign 4, 0
ov00_022194DC:
	.asciz "\\valid\\"
	.balign 4, 0
ov00_022194E4:
	.asciz "\\nicks\\"
	.balign 4, 0
ov00_022194EC:
	.asciz "\\pass\\"
	.balign 4, 0
ov00_022194F4:
	.asciz "\\pmatch\\"
	.balign 4, 0
ov00_02219500:
	.asciz "\\productid\\"
	.balign 4, 0
ov00_0221950C:
	.asciz "\\check\\"
	.balign 4, 0
ov00_02219514:
	.asciz "\\newuser\\"
	.balign 4, 0
ov00_02219520:
	.asciz "\\productID\\"
	.balign 4, 0
ov00_0221952C:
	.asciz "\\cdkey\\"
	.balign 4, 0
ov00_02219534:
	.asciz "\\others\\"
	.balign 4, 0
ov00_02219540:
	.asciz "\\uniquesearch\\"
	.balign 4, 0
ov00_02219550:
	.asciz "\\preferrednick\\"
	.balign 4, 0
ov00_02219560:
	.asciz "0"
	.balign 4, 0
ov00_02219564:
	.asciz "\\gamename\\"
	.balign 4, 0
ov00_02219570:
	.asciz "\\final\\"
	.balign 4, 0
ov00_02219578:
	.asciz "There was an error reading from the server."
	.balign 4, 0
ov00_022195A4:
	.asciz "bsrdone"
	.balign 4, 0
ov00_022195AC:
	.asciz "more"
	.balign 4, 0
ov00_022195B4:
	.asciz "bsr"
	.balign 4, 0
ov00_022195B8:
	.asciz "nick"
	.balign 4, 0
ov00_022195C0:
	.asciz "uniquenick"
	.balign 4, 0
ov00_022195CC:
	.asciz "firstname"
	.balign 4, 0
ov00_022195D8:
	.asciz "lastname"
	.balign 4, 0
ov00_022195E4:
	.asciz "email"
	.balign 4, 0
ov00_022195EC:
	.asciz "Error reading from the search server."
	.balign 4, 0
ov00_02219614:
	.asciz "vr"
	.balign 4, 0
ov00_02219618:
	.asciz "nr"
	.balign 4, 0
ov00_0221961C:
	.asciz "ndone"
	.balign 4, 0
ov00_02219624:
	.asciz "psrdone"
	.balign 4, 0
ov00_0221962C:
	.asciz "psr"
	.balign 4, 0
ov00_02219630:
	.asciz "status"
	.balign 4, 0
ov00_02219638:
	.asciz "statuscode"
	.balign 4, 0
ov00_02219644:
	.asciz "cur"
	.balign 4, 0
ov00_02219648:
	.asciz "\\pid\\"
	.balign 4, 0
ov00_02219650:
	.asciz "nur"
	.balign 4, 0
ov00_02219654:
	.asciz "others"
	.balign 4, 0
ov00_0221965C:
	.asciz "odone"
	.balign 4, 0
ov00_02219664:
	.asciz "o"
	.balign 4, 0
ov00_02219668:
	.asciz "first"
	.balign 4, 0
ov00_02219670:
	.asciz "last"
	.balign 4, 0
ov00_02219678:
	.asciz "us"
	.balign 4, 0
ov00_0221967C:
	.asciz "usdone"
	.balign 4, 0
ov00_02219684:
	.asciz "count == arg->numSuggestedNicks"
	.balign 4, 0
ov00_022196A4:
	.asciz "num < iconnection->numSearches"
	.balign 4, 0
ov00_022196C4:
	.asciz ""
	.balign 4, 0
ov00_022196C8:
	.asciz "\\version\\%d\\result\\%d"
	.balign 4, 0
ov00_022196E0:
	.asciz "\\xfer\\"
	.balign 4, 0
ov00_022196E8:
	.asciz "%d %u %u"
	.balign 4, 0
ov00_022196F4:
	.asciz "\\rn\\"
	.balign 4, 0
ov00_022196FC:
	.asciz "Unexpected data was received from the server."
	.balign 4, 0
ov00_0221972C:
	.asciz "Out of memory."
	.balign 4, 0
ov00_0221973C:
	.asciz "strzcpy"
	.balign 4, 0
ov00_02219744:
	.asciz "gpiValueForKey"
	.balign 4, 0
ov00_02219754:
	.asciz "gpiReadKeyAndValue"
	.balign 4, 0
ov00_02219768:
	.asciz "dest != NULL"
	.balign 4, 0
ov00_02219778:
	.asciz "gpiUtility.c"
	.balign 4, 0
ov00_02219788:
	.asciz "src != NULL"
	.balign 4, 0
ov00_02219794:
	.asciz "\\error\\"
	.balign 4, 0
ov00_0221979C:
	.asciz "\\err\\"
	.balign 4, 0
ov00_022197A4:
	.asciz "\\errmsg\\"
	.balign 4, 0
ov00_022197B0:
	.asciz "\\fatal\\"
	.balign 4, 0
ov00_022197B8:
	.asciz "command != NULL"
	.balign 4, 0
ov00_022197C8:
	.asciz "key != NULL"
	.balign 4, 0
ov00_022197D4:
	.asciz "value != NULL"
	.balign 4, 0
ov00_022197E4:
	.asciz "len > 0"
	.balign 4, 0
ov00_022197EC:
	.asciz "Error connecting\n"
	.balign 4, 0
ov00_02219800:
	.asciz "There was an error checking for a completed connection."
	.balign 4, 0
ov00_02219838:
	.asciz "Connection rejected\n"
	.balign 4, 0
ov00_02219850:
	.asciz "Connection accepted\n"
	.balign 4, 0
ov00_02219868:
	.asciz "buffer != NULL"
	.balign 4, 0
ov00_02219878:
	.asciz "Parse Error."
	.balign 4, 0
ov00_02219888:
	.asciz "DoFind"
	.balign 4, 0
ov00_02219890:
	.asciz "BucketNew"
	.balign 4, 0
ov00_0221989C:
	.asciz "BucketConcat"
	.balign 4, 0
ov00_022198AC:
	.asciz "set"
	.balign 4, 0
ov00_022198B0:
	.asciz "gbucket.c"
	.balign 4, 0
ov00_022198BC:
	.asciz "pbucket->type == bt_string"
	.balign 4, 0
ov00_022198D8:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x4E, 0x00, 0x00
	.word ov00_02219924
	.word BucketSetValue
	.word BucketAdd
	.word BucketSub
	.word BucketMult
	.word BucketDiv
	.word BucketConcat
	.word BucketAvg
	.word BucketIntOp
	.word BucketDoubleOp
	.word BucketStringOp
	.word TeamBucketIntOp
	.word TeamBucketDoubleOp
	.word TeamBucketStringOp
	.word PlayerBucketIntOp
	.word PlayerBucketDoubleOp
	.word PlayerBucketStringOp
ov00_02219924:
	.asciz "\x00ameSpy3D"
	.balign 4, 0
	.space 0x10-(.-ov00_02219924)
ov00_02219934:
	.asciz "\x00rojectAphex"
	.balign 4, 0
	.space 0x10-(.-ov00_02219934)
ov00_02219944:
	.asciz ""
	.balign 4, 0
ov00_02219948:
	.asciz "\\"
	.balign 4, 0
ov00_0221994C:
	.asciz "pauthr"
	.balign 4, 0
ov00_02219954:
	.asciz "lid"
	.balign 4, 0
ov00_02219958:
	.asciz "errmsg"
	.balign 4, 0
ov00_02219960:
	.asciz "getpidr"
	.balign 4, 0
ov00_02219968:
	.asciz "getpdr"
	.balign 4, 0
ov00_02219970:
	.asciz "pid"
	.balign 4, 0
ov00_02219974:
	.asciz "mod"
	.balign 4, 0
ov00_02219978:
	.asciz "length"
	.balign 4, 0
ov00_02219980:
	.asciz "\\data\\"
	.balign 4, 0
ov00_02219988:
	.asciz "setpdr"
	.balign 4, 0
ov00_02219990:
	.asciz "\\pauthr\\"
	.balign 4, 0
ov00_0221999C:
	.asciz "\\getpidr\\"
	.balign 4, 0
ov00_022199A8:
	.asciz "\\getpdr\\"
	.balign 4, 0
ov00_022199B4:
	.asciz "\\setpdr\\"
	.balign 4, 0
ov00_022199C0:
	.asciz "%s_t%d"
	.balign 4, 0
ov00_022199C8:
	.asciz "%s_%d"
	.balign 4, 0
ov00_022199D0:
	.asciz "gti2GetChallenge"
	.balign 4, 0
ov00_022199E4:
	.asciz "3b8dd8995f7c40a9a5c5b7dd5b481341"
	.balign 4, 0
ov00_02219A08:
	.asciz "buffer"
	.balign 4, 0
ov00_02219A10:
	.asciz "gt2Auth.c"
	.balign 4, 0
ov00_02219A1C:
	.asciz "gti2BufferShorten"
	.balign 4, 0
ov00_02219A30:
	.asciz "gti2BufferWriteData"
	.balign 4, 0
ov00_02219A44:
	.asciz "gti2BufferWriteByte"
	.balign 4, 0
ov00_02219A58:
	.asciz "gti2BufferWriteUShort"
	.balign 4, 0
ov00_02219A70:
	.asciz "buffer->len < buffer->size"
	.balign 4, 0
ov00_02219A8C:
	.asciz "gt2Buffer.c"
	.balign 4, 0
ov00_02219A98:
	.asciz "(buffer->len + 2) <= buffer->size"
	.balign 4, 0
ov00_02219ABC:
	.asciz "(buffer->len + len) <= buffer->size"
	.balign 4, 0
ov00_02219AE0:
	.asciz "start <= buffer->len"
	.balign 4, 0
ov00_02219AF8:
	.asciz "shortenBy <= (buffer->len - start)"
	.balign 4, 0
ov00_02219B1C:
	.asciz "gti2DumpCallback"
	.balign 4, 0
ov00_02219B30:
	.asciz "gti2PingCallback"
	.balign 4, 0
ov00_02219B44:
	.asciz "gti2ClosedCallback"
	.balign 4, 0
ov00_02219B58:
	.asciz "gti2ReceivedCallback"
	.balign 4, 0
ov00_02219B70:
	.asciz "gti2ConnectedCallback"
	.balign 4, 0
ov00_02219B88:
	.asciz "gti2SendFilterCallback"
	.balign 4, 0
ov00_02219BA0:
	.asciz "gti2SocketErrorCallback"
	.balign 4, 0
ov00_02219BB8:
	.asciz "gti2ReceiveFilterCallback"
	.balign 4, 0
ov00_02219BD4:
	.asciz "gti2ConnectAttemptCallback"
	.balign 4, 0
ov00_02219BF0:
	.asciz "gti2UnrecognizedMessageCallback"
	.balign 4, 0
ov00_02219C10:
	.asciz "socket"
	.balign 4, 0
ov00_02219C18:
	.asciz "gt2Callback.c"
	.balign 4, 0
ov00_02219C28:
	.asciz "socket && connection"
	.balign 4, 0
ov00_02219C40:
	.asciz "connection"
	.balign 4, 0
ov00_02219C4C:
	.asciz "gt2SetConnectionData"
	.balign 4, 0
ov00_02219C64:
	.asciz "gt2GetConnectionData"
	.balign 4, 0
ov00_02219C7C:
	.asciz "gt2Main.c"
	.balign 4, 0
ov00_02219C88:
	.asciz "connection"
	.balign 4, 0
ov00_02219C94:
	.asciz "gti2EndReliableMessage"
	.balign 4, 0
ov00_02219CAC:
	.asciz "time"
	.balign 4, 0
ov00_02219CB4:
	.byte 0xFE, 0xFE, 0x00, 0x00
ov00_02219CB8:
	.asciz "len > 0"
	.balign 4, 0
ov00_02219CC0:
	.asciz "gt2Message.c"
	.balign 4, 0
ov00_02219CD0:
	.asciz "gt2StringToAddress"
	.balign 4, 0
ov00_02219CE4:
	.asciz "%s:%d"
	.balign 4, 0
ov00_02219CEC:
	.asciz "%s"
	.balign 4, 0
ov00_02219CF0:
	.asciz ":%d"
	.balign 4, 0
ov00_02219CF4:
	.asciz "len < GTI2_STACK_HOSTLEN_MAX"
	.balign 4, 0
ov00_02219D14:
	.asciz "gt2Utility.c"
	.balign 4, 0
ov00_02219D24:
	.asciz ""
	.balign 4, 0
ov00_02219D28:
	.byte 0xFD, 0xFC, 0x1E, 0x66, 0x6A, 0xB2, 0x00, 0x00
ov00_02219D30:
	.asciz "%s.%s"
	.balign 4, 0
ov00_02219D38:
	.asciz "natneg1.gs.nintendowifi.net"
	.balign 4, 0
ov00_02219D54:
	.asciz "natneg2.gs.nintendowifi.net"
	.balign 4, 0
current_rec:
	.word static_qr2_rec
static_qr2_rec:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00
ov00_02219E84:
	.asciz "%s.master.gs.nintendowifi.net"
	.balign 4, 0
ov00_02219EA4:
	.asciz "No challenge value was received from the master server."
	.balign 4, 0
ov00_02219EDC:
	.asciz "%d"
	.balign 4, 0
ov00_02219EE0:
	.asciz "255.255.255.255"
	.balign 4, 0
ov00_02219EF0:
	.asciz "%08X%04X"
	.balign 4, 0
ov00_02219EFC:
	.asciz "unknown"
	.balign 4, 0
ov00_02219F04:
	.asciz ""
	.balign 4, 0
ov00_02219F08:
	.asciz "%s%d"
	.balign 4, 0
ov00_02219F10:
	.asciz "final\\\\queryid\\1.1"
	.balign 4, 0
ov00_02219F24:
	.asciz "localip%d"
	.balign 4, 0
ov00_02219F30:
	.asciz "localport"
	.balign 4, 0
ov00_02219F3C:
	.asciz "natneg"
	.balign 4, 0
ov00_02219F44:
	.asciz "1"
	.balign 4, 0
ov00_02219F48:
	.asciz "0"
	.balign 4, 0
ov00_02219F4C:
	.asciz "statechanged"
	.balign 4, 0
ov00_02219F5C:
	.asciz "gamename"
	.balign 4, 0
ov00_02219F68:
	.asciz "publicip"
	.balign 4, 0
ov00_02219F74:
	.asciz "publicport"
	.balign 4, 0
ov00_02219F80:
	.asciz ""
	.balign 4, 0
ov00_02219F84:
	.asciz "pid_"
	.balign 4, 0
ov00_02219F8C:
	.asciz "ping_"
	.balign 4, 0
ov00_02219F94:
	.asciz "team_"
	.balign 4, 0
ov00_02219F9C:
	.asciz "score_"
	.balign 4, 0
ov00_02219FA4:
	.asciz "team_t"
	.balign 4, 0
ov00_02219FAC:
	.asciz "skill_"
	.balign 4, 0
ov00_02219FB4:
	.asciz "groupid"
	.balign 4, 0
ov00_02219FBC:
	.asciz "deaths_"
	.balign 4, 0
ov00_02219FC4:
	.asciz "gamever"
	.balign 4, 0
ov00_02219FCC:
	.asciz "player_"
	.balign 4, 0
ov00_02219FD4:
	.asciz "score_t"
	.balign 4, 0
ov00_02219FDC:
	.asciz "mapname"
	.balign 4, 0
ov00_02219FE4:
	.asciz "gamename"
	.balign 4, 0
ov00_02219FF0:
	.asciz "hostport"
	.balign 4, 0
ov00_02219FFC:
	.asciz "password"
	.balign 4, 0
ov00_0221A008:
	.asciz "gametype"
	.balign 4, 0
ov00_0221A014:
	.asciz "numteams"
	.balign 4, 0
ov00_0221A020:
	.asciz "gamemode"
	.balign 4, 0
ov00_0221A02C:
	.asciz "teamplay"
	.balign 4, 0
ov00_0221A038:
	.asciz "hostname"
	.balign 4, 0
ov00_0221A044:
	.asciz "fraglimit"
	.balign 4, 0
ov00_0221A050:
	.asciz "timelimit"
	.balign 4, 0
ov00_0221A05C:
	.asciz "roundtime"
	.balign 4, 0
ov00_0221A068:
	.asciz "numplayers"
	.balign 4, 0
ov00_0221A074:
	.asciz "maxplayers"
	.balign 4, 0
ov00_0221A080:
	.asciz "gamevariant"
	.balign 4, 0
ov00_0221A08C:
	.asciz "timeelapsed"
	.balign 4, 0
ov00_0221A098:
	.asciz "roundelapsed"
	.balign 4, 0
ov00_0221A0A8:
	.asciz "teamfraglimit"
	.balign 4, 0
qr2_registered_key_list:
	.word ov00_02219F80
	.word ov00_0221A038
	.word ov00_02219FE4
	.word ov00_02219FC4
	.word ov00_02219FF0
	.word ov00_02219FDC
	.word ov00_0221A008
	.word ov00_0221A080
	.word ov00_0221A068
	.word ov00_0221A014
	.word ov00_0221A074
	.word ov00_0221A020
	.word ov00_0221A02C
	.word ov00_0221A044
	.word ov00_0221A0A8
	.word ov00_0221A08C
	.word ov00_0221A050
	.word ov00_0221A05C
	.word ov00_0221A098
	.word ov00_02219FFC
	.word ov00_02219FB4
	.word ov00_02219FCC
	.word ov00_02219F9C
	.word ov00_02219FAC
	.word ov00_02219F8C
	.word ov00_02219F94
	.word ov00_02219FBC
	.word ov00_02219F84
	.word ov00_02219FA4
	.word ov00_02219FD4
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
ov00_0221A4B0:
	.asciz "\\basic\\\\info\\"
	.balign 4, 0
ov00_0221A4C0:
	.asciz "\\status\\"
	.balign 4, 0
ov00_0221A4CC:
	.asciz "\\final\\"
	.balign 4, 0

ov00_0221A4D4:
	.asciz "final"
	.balign 4, 0

ov00_0221A4DC:
	.asciz "queryid"
	.balign 4, 0
ov00_0221A4E4:
	.asciz "SBServerGetStringValueA"
	.balign 4, 0
ov00_0221A4FC:
	.asciz "%d"
	.balign 4, 0
ov00_0221A500:
	.asciz "server"
	.balign 4, 0
ov00_0221A508:
	.asciz "sb_server.c"
	.balign 4, 0
ov00_0221A514:
	.asciz "ping"
	.balign 4, 0
ov00_0221A51C:
	.asciz ""
	.balign 4, 0
ov00_0221A520:
	.asciz "%s%d"
	.balign 4, 0
ov00_0221A528:
	.asciz "\\%s"
	.balign 4, 0
ov00_0221A52C:
	.word ov00_0221A540
ov00_0221A530:
	.asciz "SBReleaseStr"
	.balign 4, 0
ov00_0221A540:
	.asciz "Query Error: "
	.balign 4, 0
ov00_0221A550:
	.asciz "AllKeysPresent"
	.balign 4, 0
ov00_0221A560:
	.asciz "SBServerListInit"
	.balign 4, 0
ov00_0221A574:
	.asciz "ProcessAdHocData"
	.balign 4, 0
ov00_0221A588:
	.asciz "ProcessMainListData"
	.balign 4, 0
ov00_0221A59C:
	.asciz "SBServerListConnectAndQuery"
	.balign 4, 0
ov00_0221A5B8:
	.asciz ""
	.balign 4, 0
ov00_0221A5BC:
	.asciz "val != NULL"
	.balign 4, 0
ov00_0221A5C8:
	.asciz "sb_serverlist.c"
	.balign 4, 0
ov00_0221A5D8:
	.asciz "slist != NULL"
	.balign 4, 0
ov00_0221A5E8:
	.asciz "callback != NULL"
	.balign 4, 0
ov00_0221A5FC:
	.asciz "%s.ms%d.gs.nintendowifi.net"
	.balign 4, 0
ov00_0221A618:
	.asciz "slist->state == sl_disconnected"
	.balign 4, 0
ov00_0221A638:
	.asciz "0"
	.balign 4, 0
ov00_0221A63C:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
ov00_0221A644:
	.asciz "inlen >= 0"
	.balign 4, 0
ov00_0221A650:
	.asciz "slist->inbufferlen >= 0"
	.balign 4, 0

