
void FUN_140451f70(undefined8 *param_1,ulonglong param_2,double param_3,double param_4)

{
  longlong lVar1;
  longlong lVar2;
  double dVar3;
  undefined8 uVar4;
  longlong *plVar5;
  undefined8 *puVar6;
  longlong lVar7;
  undefined1 auVar8 [16];
  undefined1 auVar9 [16];
  double *pdVar10;
  undefined8 uVar11;
  double *pdVar12;
  byte bVar13;
  longlong lVar14;
  double dVar15;
  undefined4 uVar16;
  double local_res18;
  double local_res20;
  void *local_d8;
  undefined8 local_c0;
  ulonglong uStack_b8;
  undefined1 local_b0 [12];
  undefined4 uStack_a4;
  undefined8 local_a0;
  char local_98;
  double local_90;
  double local_88;
  double local_80;
  double local_78;
  char local_70 [8];
  longlong local_68;
  longlong local_60;
  undefined8 local_58;
  ulonglong uStack_50;
  undefined4 local_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;

  uVar4 = **(undefined8 **)*param_1;
  local_88 = DAT_140aabd08;
  local_90 = 0.0;
  pdVar12 = &local_90;
  if (0.0 <= param_3) {
    pdVar12 = &local_res18;
  }
  pdVar10 = &local_88;
  if (param_3 <= DAT_140aabd08) {
    pdVar10 = pdVar12;
  }
  dVar15 = *pdVar10;
  local_78 = DAT_140aabd08;
  local_80 = 0.0;
  pdVar12 = &local_80;
  if (0.0 <= param_4) {
    pdVar12 = &local_res20;
  }
  pdVar10 = &local_78;
  if (param_4 <= DAT_140aabd08) {
    pdVar10 = pdVar12;
  }
  dVar3 = *pdVar10;
  if (*(char *)param_1[1] != '\0') {
    if (dVar3 <= dVar15) {
      dVar15 = dVar15 - DAT_140aab990;
      if (dVar15 <= dVar3) {
        dVar15 = dVar3;
      }
    }
    else {
      dVar15 = dVar15 + DAT_140aab990;
      if (dVar3 <= dVar15) {
        dVar15 = dVar3;
      }
    }
    *(char *)param_1[1] = '\0';
  }
  stack0xffffffffffffff58 = dVar3;
  local_b0._0_8_ = dVar15;
  plVar5 = (longlong *)param_1[2];
  puVar6 = (undefined8 *)plVar5[6];
  local_res18 = param_3;
  local_res20 = param_4;
  local_c0 = uVar4;
  uStack_b8 = param_2;
  if (puVar6 + 4 == (undefined8 *)plVar5[8]) {
    uVar16 = (undefined4)((ulonglong)dVar3 >> 0x20);
    local_b0._8_4_ = SUB84(dVar3,0);
    uStack_a4 = uVar16;
    if (plVar5[1] <= (plVar5[9] - *plVar5 >> 3) + 1) {
      FUN_140374950(plVar5,1,1);
    }
    uVar11 = thunk_FUN_140983da8(0x100);
    *(undefined8 *)(plVar5[9] + 8) = uVar11;
    puVar6 = (undefined8 *)plVar5[6];
    *puVar6 = uVar4;
    puVar6[1] = param_2;
    auVar9._8_4_ = SUB84(dVar3,0);
    auVar9._0_8_ = dVar15;
    auVar9._12_4_ = uVar16;
    *(undefined1 (*) [16])(puVar6 + 2) = auVar9;
    lVar1 = plVar5[9];
    plVar5[9] = lVar1 + 8;
    lVar1 = *(longlong *)(lVar1 + 8);
    plVar5[7] = lVar1;
    plVar5[8] = lVar1 + 0x100;
    plVar5[6] = plVar5[7];
  }
  else {
    plVar5[6] = (longlong)(puVar6 + 4);
    *puVar6 = uVar4;
    puVar6[1] = param_2;
    puVar6[2] = dVar15;
    puVar6[3] = dVar3;
  }
  lVar7 = param_1[3];
  local_58 = local_c0;
  uStack_50 = uStack_b8;
  local_48 = local_b0._0_4_;
  uStack_44 = local_b0._4_4_;
  uStack_40 = local_b0._8_4_;
  uStack_3c = uStack_a4;
  local_d8 = (void *)thunk_FUN_140983da8(0x20);
  lVar1 = (longlong)local_d8 + 0x20;
  memmove(local_d8,&local_58,0x20);
  auVar8._8_8_ = 0;
  auVar8._0_8_ = uStack_b8;
  lVar2 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar8,8) +
          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar8,0);
  local_a0 = 0;
  local_98 = '\0';
  FUN_14045d710(lVar7 + 0x880,local_70,lVar2,&uStack_b8,&local_a0);
  if (local_70[0] == '\0') {
    FUN_1403bc2c0(*(longlong *)(local_60 + 0x58) + local_68 * 0x20 + 8,&local_c0);
  }
  else {
    lVar14 = local_68 * 0x20;
    lVar7 = *(longlong *)(local_60 + 0x58);
    *(ulonglong *)(lVar7 + lVar14) = uStack_b8;
    *(undefined8 *)(lVar7 + 8 + lVar14) = 0;
    *(undefined8 *)(lVar7 + 0x10 + lVar14) = 0;
    *(undefined8 *)(lVar7 + 0x18 + lVar14) = 0;
    *(void **)(lVar7 + 8 + lVar14) = local_d8;
    local_d8 = (void *)0x0;
    *(longlong *)(lVar7 + 0x10 + lVar14) = lVar1;
    *(longlong *)(lVar7 + 0x18 + lVar14) = lVar1;
    bVar13 = (byte)lVar2 & 0x7f;
    *(byte *)(local_68 + *(longlong *)(local_60 + 0x50)) = bVar13;
    *(byte *)((local_68 - 0x10U & *(ulonglong *)(local_60 + 0x68)) +
              (ulonglong)((uint)*(ulonglong *)(local_60 + 0x68) & 0xf) + 1 +
             *(longlong *)(local_60 + 0x50)) = bVar13;
  }
  if (local_98 != '\0') {
    _Mtx_unlock(local_a0);
  }
  if (local_d8 != (void *)0x0) {
    free(local_d8);
  }
  return;
}
