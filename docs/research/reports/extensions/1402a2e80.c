// Candidate VA 1402a2e80; RVA 0x2a2e80
// Ghidra inferred prototype: undefined FUN_1402a2e80()

longlong FUN_1402a2e80(longlong *param_1,ulonglong param_2)

{
  longlong lVar1;
  ulonglong uVar2;
  longlong lVar3;
  longlong *plVar4;
  uint7 uVar5;
  longlong lVar6;

  uVar5 = (uint7)(param_2 >> 8);
  lVar3 = *param_1;
  lVar1 = *(longlong *)(lVar3 + 8);
  plVar4 = (longlong *)
           ((((((ulonglong)uVar5 & 0xff0000000000) >> 0x28 ^
              ((((((param_2 & 0xff ^ 0xcbf29ce484222325) * 0x100000001b3 ^ (ulonglong)uVar5 & 0xff)
                  * 0x100000001b3 ^ ((ulonglong)uVar5 & 0xff00) >> 8) * 0x100000001b3 ^
                ((ulonglong)uVar5 & 0xff0000) >> 0x10) * 0x100000001b3 ^
               ((ulonglong)uVar5 & 0xff000000) >> 0x18) * 0x100000001b3 ^
              ((ulonglong)uVar5 & 0xff00000000) >> 0x20) * 0x100000001b3) * 0x100000001b3 ^
             (ulonglong)(uVar5 >> 0x30)) * 0x100000001b3 & *(ulonglong *)(lVar3 + 0x30)) * 0x10 +
           *(longlong *)(lVar3 + 0x18));
  lVar3 = plVar4[1];
  if (lVar3 == lVar1) {
LAB_1402a2f51:
    lVar3 = 0;
  }
  else {
    uVar2 = *(ulonglong *)(lVar3 + 0x10);
    while (param_2 != uVar2) {
      if (lVar3 == *plVar4) goto LAB_1402a2f51;
      lVar3 = *(longlong *)(lVar3 + 8);
      uVar2 = *(ulonglong *)(lVar3 + 0x10);
    }
  }
  if (lVar3 == 0) {
    lVar3 = lVar1;
  }
  lVar6 = 0;
  if (lVar3 != lVar1) {
    lVar6 = lVar3 + 0x18;
  }
  return lVar6;
}


// Incoming references
// 0x427832 UNCONDITIONAL_CALL caller 1404277a0
// 0x438e9e UNCONDITIONAL_CALL caller 140438e10
// 0x42725d UNCONDITIONAL_CALL caller 140427220
// 0x424c6f UNCONDITIONAL_CALL caller 140424c00
// 0x424d73 UNCONDITIONAL_CALL caller 140424c00
// 0x492677 UNCONDITIONAL_CALL caller 1404925d0
// 0x492565 UNCONDITIONAL_CALL caller 1404924f0
// 0x30d9d7 UNCONDITIONAL_CALL caller 14030d940
// 0x44ea28 UNCONDITIONAL_CALL caller 14044d6b0
// 0x4423af UNCONDITIONAL_CALL caller 1404422e0
// 0x48fa5f UNCONDITIONAL_CALL caller 14048fa20
// 0x43af14 UNCONDITIONAL_CALL caller 14043ae80
// 0x43b128 UNCONDITIONAL_CALL caller 14043b060
// 0x43b162 UNCONDITIONAL_CALL caller 14043b060
// 0x43b394 UNCONDITIONAL_CALL caller 14043b300
// 0x43b3ce UNCONDITIONAL_CALL caller 14043b300
// 0x424455 UNCONDITIONAL_CALL caller 140424410
// 0x424494 UNCONDITIONAL_CALL caller 140424410
// 0x43cb67 UNCONDITIONAL_CALL caller 14043ca80
// 0x43cba6 UNCONDITIONAL_CALL caller 14043ca80
// 0x43d158 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d192 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d2c8 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d302 UNCONDITIONAL_CALL caller 14043d0f0
// 0x43d761 UNCONDITIONAL_CALL caller 14043d6d0
// 0x43d7a1 UNCONDITIONAL_CALL caller 14043d6d0
// 0x43dbb2 UNCONDITIONAL_CALL caller 14043db20
// 0x43dbf9 UNCONDITIONAL_CALL caller 14043db20
// 0x578f84 UNCONDITIONAL_CALL caller 140578ee0
// 0x57c230 UNCONDITIONAL_CALL caller 14057c0a0
// 0x57c8e1 UNCONDITIONAL_CALL caller 14057c820
// 0x57ea9a UNCONDITIONAL_CALL caller 14057ea10
// 0x79d1b5 UNCONDITIONAL_CALL caller 14079cd60
// 0x79d477 UNCONDITIONAL_CALL caller 14079cd60
