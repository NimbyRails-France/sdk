// Candidate VA 1402a7900; RVA 0x2a7900
// Ghidra inferred prototype: undefined FUN_1402a7900()

longlong FUN_1402a7900(longlong *param_1,ulonglong param_2)

{
  longlong lVar1;
  ulonglong uVar2;
  longlong lVar3;
  longlong lVar4;
  longlong *plVar5;

  if ((*param_1 == 0) || (lVar3 = FUN_1402a7900(), lVar3 == 0)) {
    lVar1 = param_1[10];
    plVar5 = (longlong *)
             ((((((((((param_2 & 0xff ^ 0xcbf29ce484222325) * 0x100000001b3 ^ param_2 >> 8 & 0xff) *
                     0x100000001b3 ^ param_2 >> 0x10 & 0xff) * 0x100000001b3 ^
                   param_2 >> 0x18 & 0xff) * 0x100000001b3 ^ param_2 >> 0x20 & 0xff) * 0x100000001b3
                 ^ param_2 >> 0x28 & 0xff) * 0x100000001b3 ^ param_2 >> 0x30 & 0xff) * 0x100000001b3
               ^ param_2 >> 0x38) * 0x100000001b3 & param_1[0xf]) * 0x10 + param_1[0xc]);
    lVar4 = plVar5[1];
    if (lVar4 == lVar1) {
LAB_1402a79d2:
      lVar4 = 0;
    }
    else {
      uVar2 = *(ulonglong *)(lVar4 + 0x10);
      while (param_2 != uVar2) {
        if (lVar4 == *plVar5) goto LAB_1402a79d2;
        lVar4 = *(longlong *)(lVar4 + 8);
        uVar2 = *(ulonglong *)(lVar4 + 0x10);
      }
    }
    if (lVar4 == 0) {
      lVar4 = lVar1;
    }
    lVar3 = 0;
    if (lVar4 != lVar1) {
      lVar3 = lVar4 + 0x18;
    }
  }
  return lVar3;
}


// Incoming references
// 0xc14228 DATA caller none
// 0x2a791d UNCONDITIONAL_CALL caller 1402a7900
// 0x438ec7 UNCONDITIONAL_CALL caller 140438e10
// 0x49268f UNCONDITIONAL_CALL caller 1404925d0
// 0x492576 UNCONDITIONAL_CALL caller 1404924f0
// 0x30d9f3 UNCONDITIONAL_CALL caller 14030d940
// 0x43b179 UNCONDITIONAL_CALL caller 14043b060
// 0x43b192 UNCONDITIONAL_CALL caller 14043b060
// 0x43b3e5 UNCONDITIONAL_CALL caller 14043b300
// 0x43b3fe UNCONDITIONAL_CALL caller 14043b300
// 0x4244ab UNCONDITIONAL_CALL caller 140424410
// 0x4244c4 UNCONDITIONAL_CALL caller 140424410
// 0x43cbbc UNCONDITIONAL_CALL caller 14043ca80
// 0x43cbd5 UNCONDITIONAL_CALL caller 14043ca80
// 0x43d1a9 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d1c2 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d319 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d332 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d7b7 UNCONDITIONAL_CALL caller 14043d6d0
// 0x43d7d0 UNCONDITIONAL_CALL caller 14043d6d0
// 0x43dc10 UNCONDITIONAL_CALL caller 14043db20
// 0x43dc31 UNCONDITIONAL_CALL caller 14043db20
// 0x455d1a UNCONDITIONAL_CALL caller 140455cb0
// 0x4560da UNCONDITIONAL_CALL caller 140456070
// 0x462378 UNCONDITIONAL_CALL caller 140462340
// 0x578fb1 UNCONDITIONAL_CALL caller 140578ee0
// 0x57c245 UNCONDITIONAL_CALL caller 14057c0a0
// 0x57c8f7 UNCONDITIONAL_CALL caller 14057c820
// 0x79d390 UNCONDITIONAL_CALL caller 14079cd60
// 0x79d48c UNCONDITIONAL_CALL caller 14079cd60
// 0x2a7820 UNCONDITIONAL_CALL caller 1402a7720
