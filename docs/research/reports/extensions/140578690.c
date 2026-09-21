// Candidate VA 140578690; RVA 0x578690
// Ghidra inferred prototype: undefined FUN_140578690()

void FUN_140578690(longlong *param_1,longlong param_2,longlong param_3,longlong param_4,
                  undefined8 param_5)

{
  ulonglong *puVar1;
  ulonglong uVar2;
  undefined1 uVar3;
  longlong lVar4;
  ulonglong *puVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  longlong lVar9;
  char local_res8 [8];
  longlong local_res10;
  longlong local_res18;
  char *local_58;
  longlong lStack_50;
  longlong local_48;

  local_res10 = param_2;
  local_res18 = param_3;
  FUN_140344bb0(param_1 + 2,param_5);
  lVar4 = param_1[0xe];
  uVar7 = 0;
  if (param_1[0xf] - lVar4 == param_1[5] - param_1[4]) {
    uVar6 = uVar7;
    uVar8 = uVar7;
    if (param_1[0xf] - lVar4 >> 4 != 0) {
      do {
        if (*(char *)(uVar8 + lVar4 + 8) != *(char *)(param_1[4] + uVar8 + 8)) goto LAB_1405786df;
        local_res8[0] = '\0';
        local_58 = local_res8;
        lStack_50 = uVar8 + lVar4;
        local_48 = param_1[4] + uVar8;
        FUN_140439a60(&local_58);
        if (local_res8[0] == '\0') goto LAB_1405786df;
        uVar6 = uVar6 + 1;
        lVar4 = param_1[0xe];
        uVar8 = uVar8 + 0x10;
      } while (uVar6 < (ulonglong)(param_1[0xf] - lVar4 >> 4));
    }
    uVar3 = 0;
  }
  else {
LAB_1405786df:
    uVar3 = 1;
  }
  *(undefined1 *)(param_1 + 0x1c) = uVar3;
  *(undefined1 *)((longlong)param_1 + 0xe1) = 0;
  if ((((*param_1 != local_res18) || (param_1[1] != param_4)) || (param_1[0xc] != param_1[2])) ||
     (param_1[0xd] != param_1[3])) {
    *param_1 = 0;
    param_1[1] = 0;
    FUN_1403455e0(param_1 + 0xe);
    param_1[0x12] = param_1[0x11];
    param_1[0xc] = 0;
    param_1[0x14] = 0;
    param_1[0xd] = 0;
    lVar4 = param_1[0x1a];
    for (lVar9 = param_1[0x19]; lVar9 != lVar4; lVar9 = lVar9 + 0x28) {
      FUN_140002d30(lVar9 + 8);
    }
    param_1[0x1a] = param_1[0x19];
    param_1[0x17] = param_1[0x16];
    *(undefined1 *)(param_1 + 0x1c) = 0;
    lVar4 = FUN_14033f6a0(local_res10 + 0x300,param_1[2]);
    if (lVar4 != 0) {
      *param_1 = local_res18;
      param_1[1] = param_4;
      FUN_140344bb0(param_1 + 0xc);
      param_1[0x17] = param_1[0x16];
      for (puVar1 = *(ulonglong **)
                     (*(longlong *)(lVar4 + 0x128) +
                     ((ulonglong)param_1[0x14] % (ulonglong)*(uint *)(lVar4 + 0x130)) * 8);
          puVar1 != (ulonglong *)0x0; puVar1 = (ulonglong *)puVar1[4]) {
        if (param_1[0x14] == *puVar1) {
          if (puVar1 != *(ulonglong **)
                         (*(longlong *)(lVar4 + 0x128) + *(longlong *)(lVar4 + 0x130) * 8)) {
            local_58 = (char *)0x0;
            lStack_50 = 0;
            local_48 = 0;
            lVar4 = param_1[0xe];
            uVar6 = uVar7;
            uVar8 = uVar7;
            if (param_1[0xf] - lVar4 >> 4 != 0) {
              do {
                if ((uVar7 < (ulonglong)(param_1[0x12] - param_1[0x11] >> 4)) &&
                   (uVar2 = puVar1[1], uVar7 < (ulonglong)((longlong)(puVar1[2] - uVar2) >> 5))) {
                  puVar5 = (ulonglong *)FUN_140586140(&local_58,param_1[0x11] + uVar6);
                  *puVar5 = uVar7;
                  puVar5[1] = uVar6 + lVar4;
                  puVar5[2] = uVar2 + uVar8;
                }
                uVar7 = uVar7 + 1;
                lVar4 = param_1[0xe];
                uVar6 = uVar6 + 0x10;
                uVar8 = uVar8 + 0x20;
              } while (uVar7 < (ulonglong)(param_1[0xf] - lVar4 >> 4));
            }
            if (local_58 != (char *)0x0) {
              free(local_58);
            }
          }
          break;
        }
      }
      FUN_140578040(param_1);
    }
  }
  return;
}


// Incoming references
// 0xc2e4fc DATA caller none
// 0x57ca2b UNCONDITIONAL_CALL caller 14057c820
