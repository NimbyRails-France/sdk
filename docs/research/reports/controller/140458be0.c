
longlong * FUN_140458be0(longlong param_1,longlong *param_2,undefined8 *param_3,undefined8 *param_4)

{
  undefined8 uVar1;
  void *pvVar2;
  longlong lVar3;
  void *_Src;
  longlong lVar4;
  undefined8 *puVar5;
  longlong *plVar6;
  longlong lVar7;
  undefined8 *puVar8;
  longlong lVar9;
  longlong lVar10;
  longlong lVar11;
  longlong lVar12;
  ulonglong _Size;
  void *local_118;
  longlong lStack_110;
  undefined8 local_108;
  undefined8 *puStack_100;
  void *local_f8;
  longlong local_f0;
  longlong local_e8;
  longlong local_e0;
  undefined8 *local_d8;
  longlong local_d0;
  void *local_c8;
  longlong local_c0;
  undefined8 local_b8;
  undefined8 *local_b0;
  longlong local_a8;
  longlong local_a0;
  undefined8 local_98;
  undefined8 *local_90;
  longlong local_88;
  void *local_80;
  longlong local_78;
  longlong local_70;
  undefined8 local_68;
  undefined8 local_60;
  undefined1 local_58 [32];

  uVar1 = param_4[2];
  plVar6 = (longlong *)(param_1 + 0x30);
  pvVar2 = (void *)*param_3;
  local_118 = *(void **)(param_1 + 0x10);
  lVar7 = param_3[1];
  lVar9 = param_3[2];
  lVar3 = param_3[3];
  _Src = (void *)*param_4;
  lVar12 = param_4[1];
  local_d8 = (undefined8 *)param_4[3];
  local_88 = lVar7;
  if ((pvVar2 == local_118) && (_Src == (void *)*plVar6)) {
    FUN_14045f080();
    *param_2 = *plVar6;
    param_2[1] = *(longlong *)(param_1 + 0x38);
    param_2[2] = *(longlong *)(param_1 + 0x40);
    lVar7 = *(longlong *)(param_1 + 0x48);
  }
  else {
    puVar8 = *(undefined8 **)(param_1 + 0x28);
    lVar10 = *(longlong *)(param_1 + 0x20) - (longlong)local_118 >> 5;
    lVar11 = (lVar9 - (longlong)pvVar2 >> 5) + -8 +
             ((longlong)_Src - lVar12 >> 5) + ((longlong)local_d8 - lVar3 >> 3) * 8;
    lVar4 = *plVar6;
    local_d0 = ((longlong)pvVar2 - lVar7 >> 5) + lVar10 + (lVar3 - (longlong)puVar8 >> 3) * 8 + -8;
    if (local_d0 <
        (longlong)
        ((ulonglong)
         (lVar10 + -8 +
         (((lVar4 - *(longlong *)(param_1 + 0x38) >> 5) +
          (*(longlong *)(param_1 + 0x48) - (longlong)puVar8 >> 3) * 8) - lVar11)) >> 1)) {
      lStack_110 = *(longlong *)(param_1 + 0x18);
      local_108 = *(undefined8 *)(param_1 + 0x20);
      puStack_100 = puVar8;
      plVar6 = (longlong *)FUN_140461910(&local_118,lVar11);
      lVar4 = *plVar6;
      puVar5 = (undefined8 *)plVar6[3];
      lVar10 = *(longlong *)(param_1 + 0x18);
      if ((lVar10 == lVar7) && (lVar10 == lVar12)) {
        _Size = (longlong)pvVar2 - *(longlong *)(param_1 + 0x10);
        memmove((void *)((longlong)_Src - (_Size & 0xffffffffffffffe0)),*(void **)(param_1 + 0x10),
                _Size);
      }
      else {
        puStack_100 = local_d8;
        local_a8 = *(longlong *)(param_1 + 0x10);
        local_98 = *(undefined8 *)(param_1 + 0x20);
        local_118 = _Src;
        lStack_110 = lVar12;
        local_108 = uVar1;
        local_f8 = pvVar2;
        local_f0 = lVar7;
        local_e8 = lVar9;
        local_e0 = lVar3;
        local_a0 = lVar10;
        local_90 = puVar8;
        FUN_140462db0(&local_c8,&local_a8,&local_f8,&local_118);
      }
      lVar7 = *(longlong *)(param_1 + 0x10);
      if (lVar7 != lVar4) {
        lVar9 = *(longlong *)(param_1 + 0x20);
        do {
          lVar7 = lVar7 + 0x20;
          *(longlong *)(param_1 + 0x10) = lVar7;
          if (lVar7 == lVar9) {
            *(longlong *)(param_1 + 0x28) = *(longlong *)(param_1 + 0x28) + 8;
            lVar7 = **(longlong **)(param_1 + 0x28);
            *(longlong *)(param_1 + 0x18) = lVar7;
            *(longlong *)(param_1 + 0x10) = lVar7;
            lVar9 = lVar7 + 0x100;
            *(longlong *)(param_1 + 0x20) = lVar9;
          }
        } while (lVar7 != lVar4);
      }
      while (puVar8 < puVar5) {
        pvVar2 = (void *)*puVar8;
        puVar8 = puVar8 + 1;
        if (pvVar2 != (void *)0x0) {
          free(pvVar2);
        }
      }
    }
    else {
      local_a0 = *(longlong *)(param_1 + 0x38);
      local_98 = *(undefined8 *)(param_1 + 0x40);
      local_90 = *(undefined8 **)(param_1 + 0x48);
      local_a8 = lVar4;
      puVar8 = (undefined8 *)FUN_140461910(&local_a8,-lVar11);
      local_118 = (void *)*puVar8;
      lStack_110 = puVar8[1];
      local_108 = puVar8[2];
      puVar8 = (undefined8 *)puVar8[3];
      puStack_100 = puVar8;
      local_80 = local_118;
      if ((lVar12 == *(longlong *)(param_1 + 0x38)) && (lVar12 == lVar7)) {
        memmove(pvVar2,_Src,lVar4 - (longlong)_Src);
        local_f0 = local_88;
        local_f8 = pvVar2;
        local_e8 = lVar9;
        local_e0 = lVar3;
        FUN_140461910(&local_f8,*plVar6 - (longlong)_Src >> 5);
      }
      else {
        local_68 = *(undefined8 *)(param_1 + 0x40);
        local_60 = *(undefined8 *)(param_1 + 0x48);
        local_b0 = local_d8;
        local_f8 = pvVar2;
        local_f0 = lVar7;
        local_e8 = lVar9;
        local_e0 = lVar3;
        local_c8 = _Src;
        local_c0 = lVar12;
        local_b8 = uVar1;
        local_78 = lVar4;
        local_70 = *(longlong *)(param_1 + 0x38);
        FUN_140462cc0(local_58,&local_c8,&local_78,&local_f8);
      }
      lVar7 = *(longlong *)(param_1 + 0x48);
      while (puVar8 = puVar8 + 1, puVar8 < (undefined8 *)(lVar7 + 8)) {
        if ((void *)*puVar8 != (void *)0x0) {
          free((void *)*puVar8);
        }
      }
      *(void **)(param_1 + 0x30) = local_118;
      *(longlong *)(param_1 + 0x38) = lStack_110;
      *(undefined8 *)(param_1 + 0x40) = local_108;
      *(undefined8 **)(param_1 + 0x48) = puStack_100;
    }
    local_c8 = *(void **)(param_1 + 0x10);
    local_c0 = *(undefined8 *)(param_1 + 0x18);
    local_b8 = *(undefined8 *)(param_1 + 0x20);
    local_b0 = *(undefined8 **)(param_1 + 0x28);
    plVar6 = (longlong *)FUN_140461910(&local_c8,local_d0);
    *param_2 = *plVar6;
    param_2[1] = plVar6[1];
    lVar7 = plVar6[3];
    param_2[2] = plVar6[2];
  }
  param_2[3] = lVar7;
  return param_2;
}
