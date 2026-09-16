
void FUN_140451820(longlong param_1,ulonglong param_2)

{
  longlong lVar1;
  ulonglong uVar2;
  ulonglong *puVar3;
  undefined8 *puVar4;
  void *pvVar5;
  undefined1 auVar6 [16];
  undefined1 auVar7 [16];
  void *_Memory;
  ulonglong *puVar8;
  undefined8 uVar9;
  ulonglong uVar10;
  longlong lVar11;
  longlong lVar12;
  ulonglong *puVar13;
  byte bVar14;
  undefined8 *puVar15;
  ulonglong *puVar16;
  ulonglong *puVar17;
  ulonglong uVar18;
  ulonglong uVar19;
  ulonglong local_res10;
  char local_res18 [4];
  uint local_res1c;
  ulonglong *local_res20;
  undefined4 uVar20;
  undefined8 local_a8;
  char local_a0;
  char local_98 [8];
  longlong local_90;
  longlong local_88;
  ulonglong *local_78;
  ulonglong local_70;
  ulonglong local_68;
  undefined4 local_60;
  undefined8 local_5c;

  local_60 = 0x3f800000;
  local_5c = 0x40000000;
  uVar19 = 1;
  local_70 = 1;
  puVar16 = &DAT_140b5abc0;
  local_78 = &DAT_140b5abc0;
  local_68 = 0;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = param_2;
  lVar12 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,8) +
           SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,0);
  local_a8 = 0;
  local_a0 = '\0';
  puVar15 = &local_a8;
  local_res10 = param_2;
  FUN_14045d710(param_1 + 0x880,local_98,lVar12,&local_res10,puVar15);
  if (local_98[0] != '\0') {
    lVar11 = local_90 * 0x20;
    lVar1 = *(longlong *)(local_88 + 0x58);
    *(ulonglong *)(lVar1 + lVar11) = local_res10;
    *(undefined8 *)(lVar1 + 8 + lVar11) = 0;
    *(undefined8 *)(lVar1 + 0x10 + lVar11) = 0;
    *(undefined8 *)(lVar1 + 0x18 + lVar11) = 0;
    bVar14 = (byte)lVar12 & 0x7f;
    *(byte *)(local_90 + *(longlong *)(local_88 + 0x50)) = bVar14;
    *(byte *)((local_90 - 0x10U & *(ulonglong *)(local_88 + 0x68)) +
              (ulonglong)((uint)*(ulonglong *)(local_88 + 0x68) & 0xf) + 1 +
             *(longlong *)(local_88 + 0x50)) = bVar14;
  }
  lVar12 = local_90 * 0x20 + *(longlong *)(local_88 + 0x58);
  if (local_a0 != '\0') {
    _Mtx_unlock(local_a8);
  }
  puVar13 = *(ulonglong **)(lVar12 + 8);
  local_res20 = *(ulonglong **)(lVar12 + 0x10);
  if (puVar13 != local_res20) {
    uVar10 = 1;
    do {
      uVar20 = (undefined4)((ulonglong)puVar15 >> 0x20);
      uVar2 = *puVar13;
      uVar18 = uVar2 % (uVar10 & 0xffffffff);
      for (puVar8 = (ulonglong *)puVar16[uVar18]; puVar8 != (ulonglong *)0x0;
          puVar8 = (ulonglong *)puVar8[1]) {
        if (uVar2 == *puVar8) goto LAB_140451a83;
      }
      puVar8 = (ulonglong *)thunk_FUN_140983da8(0x10);
      *puVar8 = *puVar13;
      puVar8[1] = 0;
      puVar15 = (undefined8 *)CONCAT44(uVar20,1);
      FUN_14001db20(&local_60,local_res18,uVar10 & 0xffffffff,local_68 & 0xffffffff,puVar15);
      puVar16 = local_78;
      uVar19 = local_70;
      if (local_res18[0] != '\0') {
        uVar19 = (ulonglong)local_res1c;
        uVar18 = uVar2 % uVar19;
        uVar10 = uVar19 * 8 + 8;
        if (uVar10 < 8) {
          uVar10 = 8;
        }
        auVar7._8_8_ = 0;
        auVar7._0_8_ = uVar10 >> 3;
        uVar9 = SUB168(ZEXT816(8) * auVar7,0);
        if (SUB168(ZEXT816(8) * auVar7,8) != 0) {
          uVar9 = 0xffffffffffffffff;
        }
        puVar16 = (ulonglong *)thunk_FUN_140983da8(uVar9);
        memset(puVar16,0,uVar19 * 8);
        puVar16[uVar19] = 0xffffffffffffffff;
        puVar17 = local_78;
        for (uVar10 = local_70; uVar10 != 0; uVar10 = uVar10 - 1) {
          puVar3 = (ulonglong *)*puVar17;
          while (puVar3 != (ulonglong *)0x0) {
            uVar2 = *puVar3;
            *puVar17 = puVar3[1];
            puVar3[1] = puVar16[uVar2 % uVar19];
            puVar16[uVar2 % uVar19] = (ulonglong)puVar3;
            puVar3 = (ulonglong *)*puVar17;
          }
          puVar17 = puVar17 + 1;
        }
        if (1 < local_70) {
          free(local_78);
        }
      }
      local_70 = uVar19;
      local_78 = puVar16;
      puVar8[1] = local_78[uVar18];
      local_78[uVar18] = (ulonglong)puVar8;
      local_68 = local_68 + 1;
      uVar10 = local_70;
      puVar16 = local_78;
      uVar19 = local_70;
LAB_140451a83:
      puVar13 = puVar13 + 4;
    } while (puVar13 != local_res20);
  }
  uVar10 = 0;
  puVar15 = (undefined8 *)*puVar16;
  puVar13 = puVar16;
  if (puVar15 == (undefined8 *)0x0) {
    puVar13 = puVar16 + 1;
    uVar2 = *puVar13;
    while (uVar2 == 0) {
      puVar13 = puVar13 + 1;
      uVar2 = *puVar13;
    }
    puVar15 = (undefined8 *)*puVar13;
  }
  puVar4 = (undefined8 *)puVar16[uVar19];
  while (puVar15 != puVar4) {
    FUN_1404512d0(param_1,*puVar15);
    puVar15 = (undefined8 *)puVar15[1];
    while (puVar15 == (undefined8 *)0x0) {
      puVar13 = puVar13 + 1;
      puVar15 = (undefined8 *)*puVar13;
    }
  }
  if (uVar19 != 0) {
    do {
      _Memory = (void *)puVar16[uVar10];
      while (_Memory != (void *)0x0) {
        pvVar5 = *(void **)((longlong)_Memory + 8);
        free(_Memory);
        _Memory = pvVar5;
      }
      puVar16[uVar10] = 0;
      uVar10 = uVar10 + 1;
    } while (uVar10 < uVar19);
  }
  if (1 < uVar19) {
    free(puVar16);
  }
  return;
}
