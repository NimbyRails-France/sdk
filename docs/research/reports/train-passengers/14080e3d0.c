// Candidate VA 14080e3d0; RVA 0x80e3d0
// Ghidra inferred prototype: undefined FUN_14080e3d0()

undefined8 * FUN_14080e3d0(longlong *param_1,undefined8 *param_2,undefined8 *param_3)

{
  char cVar1;
  longlong lVar2;
  undefined8 uVar3;
  char cVar4;
  longlong *plVar5;
  undefined8 *puVar6;
  longlong *plVar7;
  longlong *plVar8;
  longlong *plVar9;
  float fVar10;
  uint local_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined8 uStack_7c;
  byte local_71;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;

  plVar9 = (longlong *)*param_1;
  fVar10 = *(float *)(*plVar9 + 0x1550);
  if (fVar10 < 0.0) {
    fVar10 = DAT_140aabae4;
  }
  if (param_1[7] == 0) {
    plVar5 = (longlong *)0x0;
  }
  else {
    plVar5 = param_1 + 5;
    if (0xf < (ulonglong)param_1[8]) {
      plVar5 = (longlong *)*plVar5;
    }
  }
  plVar8 = param_1 + 1;
  plVar7 = plVar8;
  if (0xf < (ulonglong)param_1[4]) {
    plVar7 = (longlong *)*plVar8;
  }
  local_68 = *param_3;
  uStack_60 = param_3[1];
  local_58 = param_3[2];
  uStack_50 = param_3[3];
  FUN_140814bd0(&local_88,plVar9 + 3,*plVar9,
                (float)*(uint *)(plVar9 + 0x13) * *(float *)(param_1 + 9) +
                fVar10 * *(float *)(param_1 + 0xb),
                *(float *)((longlong)param_1 + 0x4c) * (float)*(uint *)((longlong)plVar9 + 0x9c) +
                fVar10 * *(float *)((longlong)param_1 + 0x5c),(int)param_1[10],
                *(undefined4 *)((longlong)param_1 + 0x54),fVar10 * *(float *)(param_1 + 0xd),
                fVar10 * *(float *)((longlong)param_1 + 100),fVar10 * *(float *)(param_1 + 0xe),
                fVar10 * *(float *)((longlong)param_1 + 0x6c),fVar10,(float)*(uint *)(plVar9 + 0x13)
                ,(float)*(uint *)((longlong)plVar9 + 0x9c),(int)param_1[0xc],plVar7,plVar5,&local_68
               );
  uVar3 = CONCAT44(uStack_80,uStack_84);
  cVar4 = (char)local_88;
  if ((char)local_88 != '\0') {
    lVar2 = *(longlong *)*param_1;
    if (0xf < (ulonglong)param_1[4]) {
      plVar8 = (longlong *)*plVar8;
    }
    local_88 = local_88 & 0xffffff00;
    local_71 = 0x17;
    cVar1 = (char)*plVar8;
    plVar9 = plVar8;
    while (cVar1 != '\0') {
      plVar9 = (longlong *)((longlong)plVar9 + 1);
      cVar1 = *(char *)plVar9;
    }
    FUN_14041a770(&local_88,plVar8);
    puVar6 = (undefined8 *)FUN_1408240c0(lVar2 + 0x5ff8,&local_88);
    *puVar6 = uVar3;
    puVar6[1] = uStack_7c;
    if (((local_71 & 0x80) != 0) && ((void *)CONCAT44(uStack_84,local_88) != (void *)0x0)) {
      free((void *)CONCAT44(uStack_84,local_88));
    }
  }
  *(char *)((longlong)param_1 + 0x74) = cVar4;
  *param_2 = uVar3;
  param_2[1] = uStack_7c;
  return param_2;
}


// Incoming references
// 0xc3b138 DATA caller none
// 0x80c0fa UNCONDITIONAL_CALL caller 14080bea0
