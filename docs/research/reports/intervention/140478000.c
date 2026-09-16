
void FUN_140478000(longlong param_1,longlong *param_2,undefined8 param_3,undefined8 *param_4)

{
  undefined8 *******pppppppuVar1;
  int *piVar2;
  int iVar3;
  longlong *plVar4;
  longlong lVar5;
  void *pvVar6;
  void *pvVar7;
  undefined8 uVar8;
  longlong lVar9;
  undefined8 *puVar10;
  longlong lVar11;
  longlong *plVar12;
  undefined8 *puVar13;
  longlong local_228;
  longlong lStack_220;
  undefined8 local_210;
  undefined8 ******local_208;
  undefined8 ******ppppppuStack_200;
  undefined8 *local_1f8;
  undefined8 uStack_1f0;
  undefined8 local_1e8;
  undefined8 local_1d8;
  undefined8 local_1d0;
  void *local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  undefined8 uStack_1b0;
  undefined8 local_1a8;
  undefined8 *puStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  longlong lStack_180;
  undefined4 local_170;
  undefined8 ******local_168;
  undefined8 ******ppppppuStack_160;
  void *local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  undefined8 local_138;
  undefined1 local_130 [8];
  undefined8 *local_128;
  ulonglong local_120;
  undefined8 local_118;
  undefined4 local_110;
  undefined8 local_10c;
  undefined1 local_100 [8];
  undefined8 *local_f8;
  ulonglong local_f0;
  undefined8 local_e8;
  undefined4 local_e0;
  undefined8 local_dc;
  undefined1 *local_d0;
  undefined1 *local_c8;
  undefined1 *local_c0;
  undefined1 *local_b0;
  undefined1 local_a8 [30];
  undefined1 local_8a [2];
  undefined4 local_88;
  undefined8 local_80;
  undefined8 local_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 local_60;

  *(undefined8 *)(param_1 + 0x2128) = *param_4;
  *(undefined4 *)(param_1 + 0x2118) = *(undefined4 *)(param_4 + 1);
  FUN_1403505a0(*(longlong *)(param_1 + 0x2200) + 0x348);
  FUN_140350500(*(longlong *)(param_1 + 0x2200) + 0x318);
  plVar4 = *(longlong **)(param_1 + 0x2260);
  lVar11 = *plVar4;
  plVar12 = plVar4;
  if (lVar11 == 0) {
    plVar12 = plVar4 + 1;
    lVar11 = *plVar12;
    while (lVar11 == 0) {
      plVar12 = plVar12 + 1;
      lVar11 = *plVar12;
    }
  }
  lVar5 = plVar4[*(longlong *)(param_1 + 0x2268)];
  while (lVar11 != lVar5) {
    local_210 = 0;
    local_208 = &local_208;
    ppppppuStack_200 = &local_208;
    local_1f8 = (undefined8 *)0x0;
    uStack_1f0 = 0;
    local_1e8 = 0;
    local_1d8 = 0;
    FUN_140484240(*(longlong *)(param_1 + 0x2200) + 0x348,&local_228);
    lVar9 = local_228;
    *(undefined8 *)(local_228 + 8) = local_210;
    pppppppuVar1 = (undefined8 *******)(local_228 + 0x10);
    if (pppppppuVar1 != &local_208) {
      FUN_140418b70(pppppppuVar1);
      FUN_14032a510(pppppppuVar1,&local_208);
    }
    *(undefined8 *)(lVar9 + 0x40) = local_1d8;
    puVar10 = local_1f8;
    while (puVar10 != (undefined8 *)0x0) {
      FUN_14034d750(&local_208,*puVar10);
      puVar13 = (undefined8 *)puVar10[1];
      if ((void *)puVar10[5] != (void *)0x0) {
        free((void *)puVar10[5]);
      }
      free(puVar10);
      puVar10 = puVar13;
    }
    lVar11 = *(longlong *)(lVar11 + 8);
    while (lVar11 == 0) {
      plVar12 = plVar12 + 1;
      lVar11 = *plVar12;
    }
  }
  plVar4 = *(longlong **)(param_1 + 0x2230);
  lVar11 = *plVar4;
  plVar12 = plVar4;
  if (lVar11 == 0) {
    plVar12 = plVar4 + 1;
    lVar11 = *plVar12;
    while (lVar11 == 0) {
      plVar12 = plVar12 + 1;
      lVar11 = *plVar12;
    }
  }
  lVar5 = plVar4[*(longlong *)(param_1 + 0x2238)];
  while (lVar11 != lVar5) {
    local_1d0 = 0;
    local_1c8 = (void *)0x0;
    uStack_1c0 = 0;
    local_1b8 = 0;
    uStack_1b0 = 0;
    local_1a8 = 0;
    puStack_1a0 = (undefined8 *)0x0;
    local_198 = 0;
    uStack_190 = 0;
    local_188 = 0;
    lStack_180 = 0;
    FUN_140473370(&local_1c8,0);
    local_170 = 0;
    local_168 = &local_168;
    ppppppuStack_160 = &local_168;
    local_158 = (void *)0x0;
    uStack_150 = 0;
    local_148 = 0;
    local_138 = 0;
    local_110 = 0x3f800000;
    local_10c = 0x40000000;
    local_120 = 1;
    local_128 = &DAT_140b5abc0;
    local_118 = 0;
    local_e0 = 0x3f800000;
    local_dc = 0x40000000;
    local_f0 = 1;
    local_f8 = &DAT_140b5abc0;
    local_e8 = 0;
    local_b0 = local_a8;
    local_c8 = local_a8;
    local_d0 = local_a8;
    local_c0 = local_8a;
    local_88 = 0;
    local_80 = 0;
    local_78 = 0;
    local_70 = 0;
    local_68 = 0;
    local_60 = 0;
    FUN_140484070(*(longlong *)(param_1 + 0x2200) + 0x318,&local_228);
    FUN_1404816f0(local_228 + 8);
    if ((local_d0 != (undefined1 *)0x0) && (local_d0 != local_b0)) {
      free(local_d0);
    }
    FUN_14033b630(local_100);
    if (1 < local_f0) {
      free(local_f8);
    }
    FUN_14033b630(local_130);
    pvVar7 = local_158;
    if (1 < local_120) {
      free(local_128);
      pvVar7 = local_158;
    }
    while (pvVar7 != (void *)0x0) {
      FUN_14034d750(&local_168);
      pvVar6 = *(void **)((longlong)pvVar7 + 8);
      if (*(void **)((longlong)pvVar7 + 0x28) != (void *)0x0) {
        free(*(void **)((longlong)pvVar7 + 0x28));
      }
      free(pvVar7);
      pvVar7 = pvVar6;
    }
    if (local_1c8 != (void *)0x0) {
      puVar13 = (undefined8 *)(lStack_180 + 8);
      puVar10 = puStack_1a0;
      while (puVar10 < puVar13) {
        pvVar7 = (void *)*puVar10;
        puVar10 = puVar10 + 1;
        if (pvVar7 != (void *)0x0) {
          free(pvVar7);
        }
      }
      if (local_1c8 != (void *)0x0) {
        free(local_1c8);
      }
    }
    lVar11 = *(longlong *)(lVar11 + 8);
    while (lVar11 == 0) {
      plVar12 = plVar12 + 1;
      lVar11 = *plVar12;
    }
  }
  plVar12 = (longlong *)(param_1 + 0x2210);
  if ((*(longlong *)(*(longlong *)(param_1 + 0x2208) + 0x40) == 0) ||
     (*(longlong *)(*plVar12 + 0x40) == 0)) {
    FUN_1404501b0(param_1 + 0xa0);
    FUN_1404501b0(param_1 + 0xa0,*plVar12);
  }
  lVar11 = *(longlong *)(param_1 + 0x2208);
  lVar5 = *plVar12;
  *(longlong *)(param_1 + 0x2208) = lVar5;
  *plVar12 = lVar11;
  LOCK();
  *(undefined8 *)(lVar5 + 0x18) = 0;
  UNLOCK();
  LOCK();
  *(undefined8 *)(*(longlong *)(param_1 + 0x2208) + 0x20) = 0;
  UNLOCK();
  uVar8 = param_4[2];
  if (param_2[1] != 0) {
    LOCK();
    piVar2 = (int *)(param_2[1] + 8);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  local_228 = *param_2;
  lStack_220 = param_2[1];
  FUN_140477ce0(param_1,&local_228,param_3,uVar8);
  plVar12 = (longlong *)param_2[1];
  if (plVar12 != (longlong *)0x0) {
    LOCK();
    plVar4 = plVar12 + 1;
    lVar11 = *plVar4;
    *(int *)plVar4 = (int)*plVar4 + -1;
    UNLOCK();
    if ((int)lVar11 == 1) {
      (**(code **)*plVar12)(plVar12);
      LOCK();
      piVar2 = (int *)((longlong)plVar12 + 0xc);
      iVar3 = *piVar2;
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (iVar3 == 1) {
        (**(code **)(*plVar12 + 8))(plVar12);
      }
    }
  }
  return;
}


// ref 140c25088 caller none
// ref 1403495f9 caller 1403489d0
// ref 140349b5f caller 1403489d0
// ref 1404f3ea9 caller 1404f3520
