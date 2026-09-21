// Candidate VA 14030d940; RVA 0x30d940
// Ghidra inferred prototype: undefined FUN_14030d940()

longlong FUN_14030d940(longlong param_1,longlong param_2,longlong param_3)

{
  undefined4 uVar1;
  undefined8 *puVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  undefined8 uVar8;
  undefined1 local_90 [88];

  puVar2 = (undefined8 *)FUN_14030daa0(local_90,param_1 + 0x30);
  lVar7 = *(longlong *)(param_1 + 0x20);
  lVar3 = FUN_140492220(param_3,param_3 + 0x1490,lVar7,*(undefined8 *)(param_1 + 0x28));
  if (lVar3 != 0) {
    uVar8 = *puVar2;
    lVar4 = FUN_14033f6a0(param_3 + 0x300,uVar8);
    if (lVar4 != 0) {
      lVar5 = FUN_1402a2e80(param_3 + 0x1518,uVar8);
      if (lVar5 != 0) {
        uVar8 = puVar2[8];
        lVar6 = FUN_1402a7900(lVar5 + 0x70,uVar8);
        if (lVar6 != 0) {
          lVar4 = FUN_140492140(lVar4,uVar8);
          if (lVar4 != 0) {
            uVar1 = FUN_140492190(param_3);
            lVar7 = FUN_140492440(*(undefined8 *)(param_3 + 0x1508),lVar7 >> 0x30 & 0xffff,uVar1);
            if (*(longlong *)(lVar6 + 0x48) == lVar7) {
              puVar2[1] = 0;
              FUN_1404232e0(param_3 + 0x1508,lVar5 + 0x70,puVar2,lVar6,lVar4);
              FUN_140422720(lVar3,puVar2);
            }
          }
        }
      }
    }
  }
  FUN_14030dbd0(puVar2);
  *(undefined1 *)(param_2 + 0x1f0) = 0;
  return param_2;
}


// Incoming references
// 0xc17bd0 DATA caller none
// 0xa6d9c8 DATA caller none
