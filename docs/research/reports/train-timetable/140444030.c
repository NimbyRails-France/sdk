// Candidate VA 140444030; RVA 0x444030
// Ghidra inferred prototype: undefined FUN_140444030()

ulonglong FUN_140444030(longlong param_1,undefined8 param_2,undefined8 param_3,longlong param_4,
                       undefined1 param_5)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  ulonglong in_RAX;
  longlong lVar4;
  ulonglong uVar5;
  ulonglong local_res10;
  longlong local_78;
  longlong local_70;
  undefined1 local_68 [80];

  if (param_4 == 0) goto LAB_140444213;
  if ((*(char *)(param_4 + 0x5d0) == '\0') || (*(char *)(param_4 + 0x1d0) == '\0')) {
    in_RAX = 0;
    *(undefined8 *)(param_4 + 0xa8) = 1;
    if (*(char *)(param_4 + 0xb0) == '\0') {
      *(undefined1 *)(param_4 + 0xb0) = 1;
    }
    goto LAB_140444213;
  }
  FUN_14043f150(&local_78,*(undefined8 *)(param_1 + 0x38),param_4);
  if (*(char *)(param_4 + 0x5f0) == '\0') {
LAB_1404440b9:
    if (*(longlong *)(param_4 + 0x5a8) != 0) {
      lVar4 = FUN_14033f7f0(**(longlong **)(param_1 + 0x30) + 0x180);
      if (lVar4 != 0) {
        if (*(int *)(lVar4 + 0xfc) != 0) {
          uVar1 = *(undefined4 *)(param_4 + 0x5a0);
          iVar3 = FUN_140355790((undefined4 *)(param_4 + 0x5a0),
                                (*(longlong *)(lVar4 + 0x120) - *(longlong *)(lVar4 + 0x118) >> 3) *
                                -0x7d05f417d05f417d);
          iVar2 = *(int *)(param_4 + 0x5c8);
          FUN_14043f1d0(local_68,param_4,local_70);
          FUN_14043ef00(local_68,*(undefined8 *)(param_1 + 0x30));
          uVar5 = FUN_140443ac0(param_1,local_68,param_3,param_4,iVar3 < iVar2 + 1,uVar1,param_5);
          return uVar5;
        }
        local_res10 = 3;
        goto LAB_1404441b9;
      }
    }
LAB_1404441ad:
    local_res10 = 1;
  }
  else {
    if ((local_78 == 0) || (local_70 == 0)) goto LAB_1404441ad;
    if (*(int *)(local_78 + 0x70) == 1) goto LAB_1404440b9;
    local_res10 = 4;
  }
LAB_1404441b9:
  *(ulonglong *)(param_4 + 0xa8) = local_res10;
  in_RAX = local_res10;
  if (*(char *)(param_4 + 0xb0) == '\0') {
    *(undefined1 *)(param_4 + 0xb0) = 1;
    return 0;
  }
LAB_140444213:
  return in_RAX & 0xffffffffffffff00;
}


// Incoming references
// 0xc23408 DATA caller none
// 0xb079d0 DATA caller none
// 0xb079e0 DATA caller none
// 0x443a57 UNCONDITIONAL_CALL caller 140443200
