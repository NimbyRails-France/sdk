// Candidate VA 1404aafd0; RVA 0x4aafd0
// Ghidra inferred prototype: undefined FUN_1404aafd0()

longlong FUN_1404aafd0(longlong param_1,longlong param_2,undefined8 *param_3)

{
  void **ppvVar1;
  double dVar2;
  double dVar3;
  undefined4 *puVar4;
  void *pvVar5;
  double dVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 *****pppppuVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  uint *puVar15;
  void *pvVar16;
  uint uVar17;
  uint uVar18;
  undefined8 uVar19;
  uint *puVar20;
  void *pvVar21;
  undefined8 *_Memory;
  undefined8 *puVar22;
  longlong lVar23;
  uint uVar24;
  undefined8 ******ppppppuVar25;
  uint uVar26;
  int iVar27;
  int iVar28;
  double *pdVar29;
  longlong lVar30;
  uint uVar31;
  uint uVar32;
  uint uVar33;
  uint *puVar34;
  uint *puVar35;
  int iVar36;
  uint uVar37;
  int iVar38;
  uint uVar39;
  undefined4 uVar40;
  undefined8 local_res10;
  int local_res18;
  int local_res1c;
  longlong local_res20;
  uint *local_180;
  uint *puStack_178;
  uint *local_170;
  void *local_168;
  void *pvStack_160;
  undefined8 local_158;
  void *local_150;
  void *pvStack_148;
  undefined8 local_140;
  void *local_138;
  void *pvStack_130;
  undefined8 local_128;
  double *local_120;
  undefined8 *****local_118;
  undefined8 *****pppppuStack_110;
  ulonglong local_108;
  ulonglong auStack_100 [3];
  longlong local_e8;
  longlong local_e0;
  void *local_d8;
  undefined8 uStack_d0;
  longlong local_c8;
  ulonglong local_c0;

  FUN_1404a9ca0();
  FUN_1404b9300(param_2,param_1 + 0x90);
  puVar4 = *(undefined4 **)(param_2 + 0x400);
  uVar40 = puVar4[1];
  uVar7 = puVar4[2];
  uVar8 = puVar4[3];
  *(undefined4 *)(param_1 + 0xb58) = *puVar4;
  *(undefined4 *)(param_1 + 0xb5c) = uVar40;
  *(undefined4 *)(param_1 + 0xb60) = uVar7;
  *(undefined4 *)(param_1 + 0xb64) = uVar8;
  uVar40 = puVar4[5];
  uVar7 = puVar4[6];
  uVar8 = puVar4[7];
  *(undefined4 *)(param_1 + 0xb68) = puVar4[4];
  *(undefined4 *)(param_1 + 0xb6c) = uVar40;
  *(undefined4 *)(param_1 + 0xb70) = uVar7;
  *(undefined4 *)(param_1 + 0xb74) = uVar8;
  if ((longlong *)(param_1 + 0xb78) != (longlong *)(puVar4 + 8)) {
    lVar23 = *(longlong *)(puVar4 + 8);
    FUN_1402a3ab0((longlong *)(param_1 + 0xb78),lVar23,*(longlong *)(puVar4 + 10) - lVar23 >> 5);
  }
  *(undefined8 *)(param_1 + 0x3a8) = *(undefined8 *)(param_2 + 0xa40);
  uVar40 = *(undefined4 *)((longlong)param_3 + 0x24);
  uVar7 = *(undefined4 *)(param_3 + 5);
  uVar8 = *(undefined4 *)((longlong)param_3 + 0x2c);
  *(undefined4 *)(param_1 + 0x3b0) = *(undefined4 *)(param_3 + 4);
  *(undefined4 *)(param_1 + 0x3b4) = uVar40;
  *(undefined4 *)(param_1 + 0x3b8) = uVar7;
  *(undefined4 *)(param_1 + 0x3bc) = uVar8;
  FUN_140356b40(param_1 + 0x3c0,*(undefined8 *)(param_2 + 0x410));
  *(undefined8 *)(param_1 + 0x4f0) = *param_3;
  if ((undefined8 *)(param_1 + 0x4f8) != param_3 + 1) {
    FUN_140364060((undefined8 *)(param_1 + 0x4f8),param_3[1],param_3[2]);
  }
  FUN_140488f80(param_1 + 0x510,*(undefined8 *)(param_2 + 0x408));
  FUN_14048fad0(param_1 + 0xb90,param_2 + 0x11f8);
  local_168 = (void *)0x0;
  pvStack_160 = (void *)0x0;
  local_158 = 0;
  FUN_14045ece0(param_3 + 0x14,&local_168);
  ppvVar1 = (void **)(param_1 + 0x210);
  pvVar21 = local_168;
  pvVar5 = pvStack_160;
  if (ppvVar1 != &local_168) {
    FUN_140353640(ppvVar1);
    pvVar21 = *ppvVar1;
    *ppvVar1 = local_168;
    pvVar5 = *(void **)(param_1 + 0x218);
    *(void **)(param_1 + 0x218) = pvStack_160;
    uVar19 = *(undefined8 *)(param_1 + 0x220);
    *(undefined8 *)(param_1 + 0x220) = local_158;
    local_168 = pvVar21;
    local_158 = uVar19;
    pvStack_160 = pvVar5;
  }
  for (; pvVar16 = pvStack_160, pvVar21 != pvStack_160;
      pvVar21 = (void *)((longlong)pvVar21 + 0x638)) {
    pvStack_160 = pvVar5;
    FUN_140351080(pvVar21);
    pvVar5 = pvStack_160;
    pvStack_160 = pvVar16;
  }
  pvStack_160 = pvVar5;
  if (local_168 != (void *)0x0) {
    free(local_168);
  }
  local_150 = (void *)0x0;
  pvStack_148 = (void *)0x0;
  local_140 = 0;
  FUN_14045eb20(param_3 + 0x1a,&local_150);
  ppvVar1 = (void **)(param_1 + 0x240);
  pvVar21 = local_150;
  pvVar5 = pvStack_148;
  if (ppvVar1 != &local_150) {
    FUN_1403536a0(ppvVar1);
    pvVar21 = *ppvVar1;
    *ppvVar1 = local_150;
    pvVar5 = *(void **)(param_1 + 0x248);
    *(void **)(param_1 + 0x248) = pvStack_148;
    uVar19 = *(undefined8 *)(param_1 + 0x250);
    *(undefined8 *)(param_1 + 0x250) = local_140;
    local_150 = pvVar21;
    local_140 = uVar19;
    pvStack_148 = pvVar5;
  }
  for (; pvVar16 = pvStack_148, pvVar21 != pvStack_148; pvVar21 = (void *)((longlong)pvVar21 + 0x40)
      ) {
    pvStack_148 = pvVar5;
    FUN_14034b770((longlong)pvVar21 + 8);
    pvVar5 = pvStack_148;
    pvStack_148 = pvVar16;
  }
  pvStack_148 = pvVar5;
  if (local_150 != (void *)0x0) {
    free(local_150);
  }
  local_138 = (void *)0x0;
  pvStack_130 = (void *)0x0;
  local_128 = 0;
  FUN_140486830(param_3 + 8,&local_138);
  ppvVar1 = (void **)(param_1 + 0x228);
  pvVar21 = local_138;
  pvVar5 = pvStack_130;
  if (ppvVar1 != &local_138) {
    FUN_140353700(ppvVar1);
    pvVar21 = *ppvVar1;
    *ppvVar1 = local_138;
    pvVar5 = *(void **)(param_1 + 0x230);
    *(void **)(param_1 + 0x230) = pvStack_130;
    uVar19 = *(undefined8 *)(param_1 + 0x238);
    *(undefined8 *)(param_1 + 0x238) = local_128;
    local_138 = pvVar21;
    local_128 = uVar19;
    pvStack_130 = pvVar5;
  }
  for (; pvVar16 = pvStack_130, pvVar21 != pvStack_130;
      pvVar21 = (void *)((longlong)pvVar21 + 0x178)) {
    pvStack_130 = pvVar5;
    FUN_140351180(pvVar21);
    pvVar5 = pvStack_130;
    pvStack_130 = pvVar16;
  }
  pvStack_130 = pvVar5;
  if (local_138 != (void *)0x0) {
    free(local_138);
  }
  uVar19 = FUN_140485b00(param_3 + 8,&local_180);
  FUN_140350460(param_1 + 600,uVar19);
  for (puVar20 = local_180; puVar20 != puStack_178; puVar20 = puVar20 + 0x12) {
    FUN_140353b70(puVar20 + 4);
  }
  if (local_180 != (uint *)0x0) {
    free(local_180);
  }
  FUN_140481d00(&local_118,param_3 + 0x476);
  FUN_1404aba70(param_1 + 0x288,&local_118);
  FUN_14033b630(&local_118);
  if (1 < local_108) {
    free(pppppuStack_110);
  }
  uVar19 = FUN_14047cce0(param_3,&local_118);
  FUN_1404abaf0(param_1 + 0x2b8,uVar19);
  FUN_140351fd0(auStack_100);
  for (ppppppuVar25 = (undefined8 ******)local_118;
      ppppppuVar25 != (undefined8 ******)pppppuStack_110; ppppppuVar25 = ppppppuVar25 + 3) {
    if (*ppppppuVar25 != (undefined8 *****)0x0) {
      free(*ppppppuVar25);
    }
  }
  if ((undefined8 ******)local_118 != (undefined8 ******)0x0) {
    free(local_118);
  }
  FUN_140488ea0(param_1 + 0x2e8,*(undefined8 *)(param_2 + 0x420));
  dVar14 = DAT_140aac2c0;
  dVar13 = DAT_140aac298;
  dVar12 = DAT_140aabf88;
  dVar11 = DAT_140aabbc8;
  dVar10 = DAT_140aab8f0;
  local_180 = (uint *)0x0;
  puStack_178 = (uint *)0x0;
  local_170 = (uint *)0x0;
  local_res20 = *(longlong *)(param_1 + 0xa8);
  local_e0 = *(longlong *)(param_1 + 0xb0);
  puVar20 = (uint *)0x0;
  if (local_res20 != local_e0) {
    local_e8 = param_1 + 0xe28;
    pdVar29 = (double *)(local_res20 + 0x198);
    puVar35 = puVar20;
    do {
      puVar15 = puStack_178;
      dVar2 = pdVar29[-1];
      dVar3 = *pdVar29;
      iVar36 = (int)(dVar2 * dVar12);
      local_120 = pdVar29;
      dVar6 = exp(dVar3 * dVar10);
      dVar6 = atan(dVar6);
      iVar27 = (int)(dVar6 * dVar14 - dVar13);
      iVar38 = (int)((dVar2 + pdVar29[1]) * dVar12);
      dVar2 = exp((dVar3 + pdVar29[2]) * dVar10);
      dVar2 = atan(dVar2);
      iVar28 = (int)(dVar2 * dVar14 - dVar13);
      if ((iVar36 < iVar38) && (iVar27 < iVar28)) {
        local_res10 = (undefined8 *)CONCAT44(iVar27,iVar36);
        uVar40 = FUN_1404997c0(local_e8,&local_res10);
        local_res18 = iVar38;
        local_res1c = iVar28;
        FUN_1404997c0(uVar40,&local_res18);
      }
      uVar17 = lround(SUB84(pdVar29[-0x2d] * dVar11,0));
      uVar18 = lround(SUB84(pdVar29[-0x2c] * dVar11,0));
      uVar39 = uVar18 ^ uVar17;
      uVar26 = (uVar18 | uVar17) ^ 0xffff;
      uVar17 = (uVar18 ^ 0xffff) & uVar17;
      uVar24 = uVar17 >> 1;
      uVar18 = uVar26 >> 1;
      uVar31 = uVar24 & (uVar39 ^ 0xffff) ^ uVar26 ^ uVar18;
      uVar24 = uVar39 & uVar18 ^ uVar17 ^ uVar24;
      uVar26 = (uVar39 ^ 0xffff) >> 1 | uVar39;
      uVar18 = uVar39 >> 1 ^ uVar39;
      uVar37 = uVar24 >> 2;
      uVar17 = uVar31 >> 2;
      uVar31 = uVar37 & uVar18 ^ uVar17 & uVar26 ^ uVar31;
      uVar24 = uVar37 & (uVar18 ^ uVar26) ^ uVar17 & uVar18 ^ uVar24;
      uVar37 = uVar26 >> 2 & uVar26 ^ uVar18 >> 2 & uVar18;
      uVar18 = (uVar18 ^ uVar26) >> 2 & uVar18 ^ uVar18 >> 2 & uVar26;
      uVar26 = uVar37 >> 4 & uVar37 ^ uVar18 >> 4 & uVar18;
      uVar33 = (uVar18 ^ uVar37) >> 4 & uVar18 ^ uVar18 >> 4 & uVar37;
      uVar32 = uVar24 >> 4;
      uVar17 = uVar31 >> 4;
      uVar31 = uVar32 & uVar18 ^ uVar17 & uVar37 ^ uVar31;
      uVar24 = uVar32 & (uVar18 ^ uVar37) ^ uVar17 & uVar18 ^ uVar24;
      uVar17 = uVar24 >> 8;
      uVar18 = uVar31 >> 8;
      uVar31 = uVar17 & uVar33 ^ uVar18 & uVar26 ^ uVar31;
      uVar24 = (uVar33 ^ uVar26) & uVar17 ^ uVar18 & uVar33 ^ uVar24;
      uVar17 = (uVar31 >> 1 ^ uVar31 | uVar39) ^ 0xffff | uVar24 >> 1 ^ uVar24;
      uVar17 = (uVar17 << 8 | uVar17) & 0xff00ff;
      uVar17 = (uVar17 << 4 | uVar17) & 0xf0f0f0f;
      uVar18 = (uVar17 * 4 | uVar17) & 0x33333333;
      uVar17 = (uVar39 << 8 | uVar39) & 0xff00ff;
      uVar17 = (uVar17 << 4 | uVar17) & 0xf0f0f0f;
      uVar17 = uVar17 * 4 | uVar17;
      uVar17 = ((uVar18 * 2 | uVar18) & 0xd5555555 | uVar17 & 0x22222222) * 2 | uVar17 & 0x11111111;
      if (puVar15 < local_170) {
        *puVar15 = uVar17;
        puVar20 = puVar35;
        puVar34 = puVar15;
      }
      else {
        lVar23 = (longlong)puVar15 - (longlong)puVar35 >> 2;
        if (lVar23 == 0) {
          lVar30 = 1;
LAB_1404ab747:
          lVar23 = lVar30 * 4;
          puVar20 = (uint *)thunk_FUN_140983da8(lVar23);
        }
        else {
          lVar30 = lVar23 * 2;
          if (lVar30 != 0) goto LAB_1404ab747;
          puVar20 = (uint *)0x0;
          lVar23 = lVar23 << 3;
        }
        puVar34 = puVar20;
        if (puVar35 != puVar15) {
          pvVar21 = memmove(puVar20,puVar35,(longlong)puVar15 - (longlong)puVar35);
          puVar34 = (uint *)((longlong)pvVar21 + ((longlong)puVar15 - (longlong)puVar35 >> 2) * 4);
        }
        *puVar34 = uVar17;
        if (puVar35 != (uint *)0x0) {
          free(puVar35);
        }
        local_170 = (uint *)((longlong)puVar20 + lVar23);
        local_180 = puVar20;
      }
      puStack_178 = puVar34 + 1;
      local_res20 = local_res20 + 0x4e8;
      pdVar29 = local_120 + 0x9d;
      puVar35 = puVar20;
      local_120 = pdVar29;
    } while (local_res20 != local_e0);
  }
  puVar35 = puStack_178;
  FUN_1404b9ef0(puVar20,puStack_178);
  if ((ulonglong)((longlong)puVar35 - (longlong)puVar20 >> 2) < 0x80) {
    local_res10 = (undefined8 *)((ulonglong)local_res10 & 0xffffffff00000000);
    FUN_1404b7580(&local_180);
    puVar35 = puStack_178;
    puVar20 = local_180;
  }
  _Memory = (undefined8 *)FUN_140983da8(0x48);
  *_Memory = 0;
  *(undefined4 *)(_Memory + 2) = 0;
  _Memory[7] = 0;
  *(undefined1 *)(_Memory + 8) = 0;
  *(undefined4 *)(_Memory + 1) = 0;
  *(undefined1 *)((longlong)_Memory + 0xc) = 0;
  *(undefined8 *)((longlong)_Memory + 0x14) = 0;
  *(undefined8 *)((longlong)_Memory + 0x1c) = 0;
  *(undefined8 *)((longlong)_Memory + 0x24) = 0;
  *(undefined8 *)((longlong)_Memory + 0x2c) = 0;
  *(undefined2 *)((longlong)_Memory + 0x34) = 0;
  *(undefined1 *)((longlong)_Memory + 0x36) = 0;
  uVar17 = (int)puVar35 - (int)puVar20 & 0xfffffffc;
  local_res10 = _Memory;
  if ((puVar20 != (uint *)0x0) && (uVar17 != 0)) {
    FUN_14018eed0(_Memory,puVar20,uVar17);
  }
  FUN_14018f7d0(_Memory);
  lVar23 = _Memory[7];
  if (lVar23 == 0) {
    puVar22 = (undefined8 *)thunk_FUN_140983da8();
    _Memory[7] = puVar22;
    *puVar22 = 0;
    puVar22[1] = 0;
    puVar22[2] = 0;
    puVar22[3] = 0;
    puVar22[4] = 0;
    puVar22[5] = 0;
    puVar22[6] = 0;
    puVar22[7] = 0;
    puVar22[8] = 0;
    *(undefined1 *)(puVar22 + 9) = 0;
    lVar23 = FUN_14018fac0(_Memory,_Memory[7]);
  }
  local_d8 = (void *)0x0;
  uStack_d0 = 0;
  local_c8 = 0;
  local_c0 = 0;
  lVar30 = -1;
  do {
    lVar30 = lVar30 + 1;
  } while (*(char *)(lVar23 + lVar30) != '\0');
  FUN_140002c00(&local_d8,lVar23);
  if (local_c8 == 0x46) {
    FUN_1404992e0(&local_118,&local_d8);
    ppppppuVar25 = &local_118;
    if (0xf < auStack_100[0]) {
      ppppppuVar25 = (undefined8 ******)local_118;
    }
    pppppuVar9 = ppppppuVar25[1];
    *(undefined8 ******)(param_1 + 0xe38) = *ppppppuVar25;
    *(undefined8 ******)(param_1 + 0xe40) = pppppuVar9;
    pppppuVar9 = ppppppuVar25[3];
    *(undefined8 ******)(param_1 + 0xe48) = ppppppuVar25[2];
    *(undefined8 ******)(param_1 + 0xe50) = pppppuVar9;
    *(undefined2 *)(param_1 + 0xe58) = *(undefined2 *)(ppppppuVar25 + 4);
    *(undefined1 *)(param_1 + 0xe5a) = *(undefined1 *)((longlong)ppppppuVar25 + 0x22);
    if (0xf < auStack_100[0]) {
      ppppppuVar25 = (undefined8 ******)local_118;
      if ((0xfff < auStack_100[0] + 1) &&
         (ppppppuVar25 = (undefined8 ******)local_118[-1],
         0x1f < (ulonglong)((longlong)local_118 + (-8 - (longlong)ppppppuVar25)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(ppppppuVar25);
    }
  }
  else {
    *(undefined8 *)(param_1 + 0xe38) = 0;
    *(undefined8 *)(param_1 + 0xe40) = 0;
    *(undefined8 *)(param_1 + 0xe48) = 0;
    *(undefined8 *)(param_1 + 0xe50) = 0;
    *(undefined2 *)(param_1 + 0xe58) = 0;
    *(undefined1 *)(param_1 + 0xe5a) = 0;
  }
  if (0xf < local_c0) {
    pvVar21 = local_d8;
    if ((0xfff < local_c0 + 1) &&
       (pvVar21 = *(void **)((longlong)local_d8 + -8),
       0x1f < (ulonglong)((longlong)local_d8 + (-8 - (longlong)pvVar21)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar21);
  }
  free((void *)*_Memory);
  free((void *)_Memory[7]);
  free(_Memory);
  if (puVar20 != (uint *)0x0) {
    free(puVar20);
  }
  return param_1;
}


// Incoming references
// 0xc26954 DATA caller none
// 0x4e49d2 UNCONDITIONAL_CALL caller 1404e47c0
// 0x4abc6d UNCONDITIONAL_CALL caller 1404abc50
// 0x4ac4a9 UNCONDITIONAL_CALL caller 1404abd50
// 0x4f4604 UNCONDITIONAL_CALL caller 1404f3520
