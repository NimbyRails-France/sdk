// Candidate VA 1405a1f30; RVA 0x5a1f30
// Ghidra inferred prototype: undefined FUN_1405a1f30()

void FUN_1405a1f30(longlong param_1,undefined8 param_2,undefined8 param_3,longlong *param_4)

{
  longlong *_Src;
  ulonglong uVar1;
  undefined8 *puVar2;
  longlong *plVar3;
  undefined1 uVar4;
  char cVar5;
  longlong lVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  ulonglong uVar9;
  undefined8 *puVar10;
  ulonglong uVar11;
  longlong *plVar12;
  longlong lVar13;
  undefined8 *puVar14;
  bool bVar15;
  void *local_e8;
  undefined8 uStack_e0;
  ulonglong local_d8;
  ulonglong local_d0;
  undefined8 *local_c8;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 *local_a8;
  longlong *local_a0;
  void **local_98;
  undefined4 local_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined4 uStack_7c;
  undefined1 local_78 [8];
  undefined1 local_70 [48];

  uVar4 = *(undefined1 *)(param_1 + 0x70);
  lVar6 = FUN_1402d82e0("script_extensions","Extensions");
  local_e8 = (void *)0x0;
  uStack_e0 = 0;
  local_d8 = 0;
  local_d0 = 0;
  lVar13 = -1;
  do {
    lVar13 = lVar13 + 1;
  } while (*(char *)(lVar6 + lVar13) != '\0');
  FUN_140002c00(&local_e8,lVar6);
  uVar4 = FUN_140572e60(param_2,param_4,&local_e8,uVar4);
  *(undefined1 *)(param_1 + 0x71) = uVar4;
  if (*(char *)(param_1 + 0x70) != '\0') {
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x40800000;
    *(undefined1 *)(param_4 + 6) = 1;
    (**(code **)(*param_4 + 200))(param_4);
    local_b8 = nimby::shell::ScriptStructInstancesTabs::StructsAdaptor::vftable;
    uStack_b0 = param_1 + 0x1b8;
    *(undefined4 *)((longlong)param_4 + 0x2c) = 0x43c60000;
    *(undefined1 *)(param_4 + 5) = 1;
    *(undefined4 *)((longlong)param_4 + 0x34) = 0x43960000;
    *(undefined1 *)(param_4 + 6) = 1;
    FUN_1405733e0(param_1 + 0x90,param_2,&local_b8,param_4,0);
  }
  local_c8 = *(undefined8 **)(param_1 + 0x58);
  local_a8 = *(undefined8 **)(param_1 + 0x60);
  if (local_c8 != local_a8) {
    do {
      lVar6 = local_c8[4];
      uVar1 = local_c8[5];
      local_a0 = *(longlong **)(param_1 + 0x48);
      plVar12 = *(longlong **)(param_1 + 0x40);
      uVar9 = (longlong)local_a0 - (longlong)plVar12 >> 4;
      while (uVar11 = uVar9, 0 < (longlong)uVar11) {
        uVar9 = uVar11 >> 1;
        if ((plVar12[uVar9 * 2] < lVar6) ||
           ((plVar12[uVar9 * 2] <= lVar6 && ((ulonglong)plVar12[uVar9 * 2 + 1] < uVar1)))) {
          plVar12 = plVar12 + uVar9 * 2 + 2;
          uVar9 = uVar11 + (-1 - uVar9);
        }
      }
      if ((((plVar12 == local_a0) || (lVar6 < *plVar12)) ||
          ((lVar6 <= *plVar12 && (uVar1 < (ulonglong)plVar12[1])))) || (plVar12 == plVar12 + 2)) {
        plVar12 = local_a0;
      }
      bVar15 = plVar12 != local_a0;
      local_e8 = (void *)0x0;
      uStack_e0 = 0;
      local_d8 = 0;
      local_d0 = 0;
      uVar9 = local_c8[2];
      puVar14 = local_c8;
      if (0xf < (ulonglong)local_c8[3]) {
        puVar14 = (undefined8 *)*local_c8;
      }
      local_b8 = (undefined **)lVar6;
      uStack_b0 = uVar1;
      if (0x7fffffffffffffff < uVar9) {
                    /* WARNING: Subroutine does not return */
        FUN_140001c70();
      }
      if (uVar9 < 0x10) {
        local_d0 = 0xf;
        local_e8 = (void *)*puVar14;
        uStack_e0 = puVar14[1];
        local_d8 = uVar9;
      }
      else {
        uVar11 = uVar9 | 0xf;
        if (uVar11 < 0x8000000000000000) {
          if (uVar11 < 0x16) {
            uVar11 = 0x16;
          }
        }
        else {
          uVar11 = 0x7fffffffffffffff;
        }
        local_e8 = (void *)FUN_140003270(uVar11 + 1);
        local_d8 = uVar9;
        local_d0 = uVar11;
        memcpy(local_e8,puVar14,uVar9 + 1);
      }
      local_98 = &local_e8;
      uVar7 = FUN_140019ca0(local_70,&local_e8);
      cVar5 = FUN_140572c70(param_2,param_4,uVar7,0,bVar15);
      FUN_140002d30(&local_e8);
      if (plVar12 != local_a0) {
        puVar14 = (undefined8 *)(param_1 + 0x10);
        puVar2 = *(undefined8 **)(param_1 + 0x20);
        puVar8 = puVar14;
        while (puVar2 != (undefined8 *)0x0) {
          puVar10 = puVar2;
          if ((lVar6 <= (longlong)puVar2[4]) &&
             ((lVar6 < (longlong)puVar2[4] || (uVar1 <= (ulonglong)puVar2[5])))) {
            puVar10 = puVar2 + 1;
            puVar8 = puVar2;
          }
          puVar2 = (undefined8 *)*puVar10;
        }
        if (((puVar8 == puVar14) || (lVar6 < (longlong)puVar8[4])) ||
           ((lVar6 <= (longlong)puVar8[4] && (uVar1 < (ulonglong)puVar8[5])))) {
          puVar8 = (undefined8 *)FUN_140586c70(puVar14,local_78,puVar14,puVar8,&local_b8);
          puVar8 = (undefined8 *)*puVar8;
        }
        FUN_140578ee0(puVar8 + 6,param_2,param_3,param_4);
      }
      plVar12 = *(longlong **)(param_1 + 0x78);
      plVar3 = *(longlong **)(param_1 + 0x80);
      uVar9 = (longlong)plVar3 - (longlong)plVar12 >> 4;
      if (cVar5 == '\0') {
        while (uVar11 = uVar9, 0 < (longlong)uVar11) {
          uVar9 = uVar11 >> 1;
          if ((plVar12[uVar9 * 2] < lVar6) ||
             ((plVar12[uVar9 * 2] <= lVar6 && ((ulonglong)plVar12[uVar9 * 2 + 1] < uVar1)))) {
            plVar12 = plVar12 + uVar9 * 2 + 2;
            uVar9 = uVar11 + (-1 - uVar9);
          }
        }
        if ((((plVar12 != plVar3) && (*plVar12 <= lVar6)) &&
            ((*plVar12 < lVar6 || ((ulonglong)plVar12[1] <= uVar1)))) &&
           (_Src = plVar12 + 2, plVar12 != _Src)) {
          if (_Src < plVar3) {
            memmove(plVar12,_Src,(longlong)plVar3 - (longlong)_Src);
          }
          *(longlong *)(param_1 + 0x80) = *(longlong *)(param_1 + 0x80) + -0x10;
        }
      }
      else {
        local_88 = (undefined4)local_b8;
        uStack_84 = local_b8._4_4_;
        uStack_80 = (undefined4)uStack_b0;
        uStack_7c = uStack_b0._4_4_;
        while (uVar11 = uVar9, 0 < (longlong)uVar11) {
          uVar9 = uVar11 >> 1;
          if ((plVar12[uVar9 * 2] < lVar6) ||
             ((plVar12[uVar9 * 2] <= lVar6 && ((ulonglong)plVar12[uVar9 * 2 + 1] < uVar1)))) {
            plVar12 = plVar12 + uVar9 * 2 + 2;
            uVar9 = uVar11 + (-1 - uVar9);
          }
        }
        if (((plVar12 == plVar3) || (lVar6 < *plVar12)) ||
           ((lVar6 <= *plVar12 && (uVar1 < (ulonglong)plVar12[1])))) {
          if ((plVar3 == *(longlong **)(param_1 + 0x88)) || (plVar12 != plVar3)) {
            FUN_140341ac0(param_1 + 0x78,plVar12,&local_88);
          }
          else {
            *(undefined4 *)plVar3 = (undefined4)local_b8;
            *(undefined4 *)((longlong)plVar3 + 4) = local_b8._4_4_;
            *(undefined4 *)(plVar3 + 1) = (undefined4)uStack_b0;
            *(undefined4 *)((longlong)plVar3 + 0xc) = uStack_b0._4_4_;
            *(longlong *)(param_1 + 0x80) = *(longlong *)(param_1 + 0x80) + 0x10;
          }
        }
      }
      local_c8 = local_c8 + 6;
    } while (local_c8 != local_a8);
  }
  return;
}


// Incoming references
// 0xc2ef28 DATA caller none
// 0x5b0da1 UNCONDITIONAL_CALL caller 1405ada90
// 0x5b753b UNCONDITIONAL_CALL caller 1405b34b0
// 0x66ebb9 UNCONDITIONAL_CALL caller 14066e410
// 0x6f6b97 UNCONDITIONAL_CALL caller 1406f5a70
// 0x6f82d2 UNCONDITIONAL_CALL caller 1406f7a60
// 0x759b8b UNCONDITIONAL_CALL caller 140759af0
// 0x7a25a5 UNCONDITIONAL_CALL caller 1407a0a40
// 0x7f4c54 UNCONDITIONAL_CALL caller 1407f3600
