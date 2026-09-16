// Candidate VA 1405bfb90; RVA 0x5bfb90
// Ghidra inferred prototype: undefined FUN_1405bfb90()

undefined8 * FUN_1405bfb90(undefined8 *param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  longlong *plVar3;
  undefined8 *puVar4;
  longlong lVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  longlong *plVar9;

  puVar6 = (undefined8 *)0x0;
  puVar4 = (undefined8 *)FUN_140983da8(0x58);
  *puVar4 = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4[3] = 0;
  puVar4[4] = 0;
  puVar4[5] = 0;
  puVar4[6] = 0;
  puVar4[7] = 0;
  puVar4[8] = 0;
  puVar4[9] = 0;
  puVar4[10] = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4[3] = 0;
  *puVar4 = nimby::model::cmd::train::Intervention::vftable;
  puVar1 = puVar4 + 5;
  *(undefined4 *)((longlong)puVar4 + 0x41) = 0;
  *(undefined2 *)((longlong)puVar4 + 0x45) = 0;
  *(undefined1 *)((longlong)puVar4 + 0x47) = 0;
  *puVar1 = puVar1;
  puVar4[6] = puVar1;
  puVar4[7] = 0;
  *(undefined1 *)(puVar4 + 8) = 0;
  puVar4[9] = 0;
  puVar4[1] = *param_1;
  puVar4[2] = param_1[1];
  puVar1 = (undefined8 *)param_1[3];
  if (puVar1 < (undefined8 *)param_1[4]) {
    *puVar1 = puVar4;
    param_1[3] = param_1[3] + 8;
    return puVar4;
  }
  lVar5 = (longlong)puVar1 - param_1[2] >> 3;
  if (lVar5 == 0) {
    lVar5 = 1;
  }
  else {
    lVar5 = lVar5 * 2;
    if (lVar5 == 0) goto LAB_1405bfc6a;
  }
  puVar6 = (undefined8 *)thunk_FUN_140983da8(lVar5 * 8);
LAB_1405bfc6a:
  puVar1 = (undefined8 *)param_1[3];
  puVar8 = puVar6;
  for (puVar7 = (undefined8 *)param_1[2]; puVar7 != puVar1; puVar7 = puVar7 + 1) {
    uVar2 = *puVar7;
    *puVar7 = 0;
    *puVar8 = uVar2;
    puVar8 = puVar8 + 1;
  }
  *puVar8 = puVar4;
  plVar3 = (longlong *)param_1[3];
  for (plVar9 = (longlong *)param_1[2]; plVar9 != plVar3; plVar9 = plVar9 + 1) {
    puVar1 = (undefined8 *)*plVar9;
    if (puVar1 != (undefined8 *)0x0) {
      (**(code **)*puVar1)(puVar1,1);
    }
  }
  if ((void *)param_1[2] != (void *)0x0) {
    free((void *)param_1[2]);
  }
  param_1[2] = puVar6;
  param_1[3] = puVar8 + 1;
  param_1[4] = puVar6 + lVar5;
  return puVar4;
}


// Incoming references
// 0xc2f528 DATA caller none
// 0x5bc284 UNCONDITIONAL_CALL caller 1405bc270
