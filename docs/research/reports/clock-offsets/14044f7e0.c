
void FUN_14044f7e0(longlong param_1,longlong param_2,longlong param_3,uint param_4)

{
  undefined8 *puVar1;
  longlong lVar2;
  ulonglong *puVar3;
  ulonglong uVar4;
  short sVar5;
  longlong lVar6;
  longlong *plVar7;
  ulonglong uVar8;
  longlong lVar9;
  longlong lVar10;
  longlong *plVar11;
  longlong *plVar12;
  longlong *plVar13;
  ulonglong *puVar14;
  ulonglong *puVar15;
  void *local_1f0;
  undefined8 local_1e8;
  undefined8 local_1e0;
  void *local_1d8;
  undefined8 local_1d0;
  undefined8 local_1c8;
  undefined4 local_1c0 [2];
  void *local_1b8;
  ulonglong local_1b0;
  undefined8 local_1a8;
  undefined8 local_1a0;
  undefined4 local_198;
  undefined8 local_190;
  undefined8 local_188;
  undefined4 local_180;
  undefined8 local_178;
  undefined8 uStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  undefined8 uStack_140;
  undefined4 local_138;
  undefined4 local_134;
  void *local_130;
  void *local_118;
  undefined1 local_100 [8];
  void *local_f8;
  ulonglong local_f0;

  lVar2 = *(longlong *)(param_2 + 0x2200);
  plVar12 = *(longlong **)(lVar2 + 0x4c0);
  plVar13 = (longlong *)*plVar12;
  plVar11 = plVar12;
  if (plVar13 == (longlong *)0x0) {
    plVar13 = (longlong *)plVar12[1];
    plVar7 = plVar12;
    while (plVar11 = plVar7 + 1, plVar13 == (longlong *)0x0) {
      plVar13 = (longlong *)plVar7[2];
      plVar7 = plVar11;
    }
  }
  plVar12 = (longlong *)plVar12[*(longlong *)(lVar2 + 0x4c8)];
  while (plVar13 != plVar12) {
    lVar10 = *plVar13;
    lVar9 = lVar10;
    lVar6 = FUN_14045f300(param_1);
    if (((lVar6 == 0) && (sVar5 = (short)((ulonglong)lVar10 >> 0x30), sVar5 != -1)) && (sVar5 == 5))
    {
      plVar7 = (longlong *)FUN_14045f230(param_1,lVar9 >> 0x10);
      sVar5 = (short)((ulonglong)*plVar7 >> 0x30);
      if ((sVar5 != -1) && (sVar5 != 0)) {
        if ((ushort)lVar10 <= (ushort)*plVar7) goto LAB_14044f8f3;
        FUN_140351080(plVar7);
      }
      memset(plVar7 + 1,0,0x630);
      FUN_140459860(plVar7);
      *plVar7 = lVar10;
    }
LAB_14044f8f3:
    for (plVar13 = (longlong *)plVar13[1]; plVar13 == (longlong *)0x0;
        plVar13 = (longlong *)*plVar13) {
      plVar13 = plVar11 + 1;
      plVar11 = plVar11 + 1;
    }
  }
  if ((*(char *)(param_2 + 0x2114) == '\0') || (*(int *)(param_2 + 0x2118) != 10000)) {
    plVar13 = *(longlong **)(*(longlong *)(param_2 + 0x2208) + 0x30);
    puVar14 = (ulonglong *)*plVar13;
    plVar12 = plVar13;
    if (puVar14 == (ulonglong *)0x0) {
      puVar14 = (ulonglong *)plVar13[1];
      while (plVar12 = plVar12 + 1, puVar14 == (ulonglong *)0x0) {
        puVar14 = (ulonglong *)plVar12[1];
      }
    }
    puVar3 = (ulonglong *)plVar13[*(longlong *)(*(longlong *)(param_2 + 0x2208) + 0x38)];
    while (puVar14 != puVar3) {
      uVar4 = *puVar14;
      uVar8 = (uVar4 >> 0x21 ^ uVar4) * -0xae502812aa7333;
      lVar10 = (uVar8 >> 0x21 ^ uVar8) * -0x3b314601e57a13ad;
      if ((((uint)((ulonglong)lVar10 >> 0x21) ^ (uint)lVar10) & 0xf) == (param_4 & 0xf)) {
LAB_14044facc:
        lVar10 = FUN_14045f300(param_1,uVar4);
        if (lVar10 != 0) {
          FUN_14043e3b0(lVar10,puVar14 + 1);
        }
      }
      else {
        if ((((0.0 < *(double *)(param_3 + 0x18)) && (0.0 < *(double *)(param_3 + 0x20))) &&
            ((char)puVar14[0x3b] != '\0')) && (0.0 < *(double *)(param_3 + 0x20))) {
          if (((*(double *)(param_3 + 8) < (double)puVar14[0x2a] + (double)puVar14[0x2c]) &&
              ((double)puVar14[0x2a] < *(double *)(param_3 + 8) + *(double *)(param_3 + 0x18))) &&
             (((double)puVar14[0x2b] < *(double *)(param_3 + 0x10) + *(double *)(param_3 + 0x20) &&
              (*(double *)(param_3 + 0x10) < (double)puVar14[0x2b] + (double)puVar14[0x2d]))))
          goto LAB_14044facc;
        }
        puVar15 = *(ulonglong **)
                   (*(longlong *)(lVar2 + 0x1f8) + (uVar4 % (ulonglong)*(uint *)(lVar2 + 0x200)) * 8
                   );
        lVar10 = 0;
        if (puVar15 != (ulonglong *)0x0) {
          do {
            uVar8 = *puVar15;
            puVar15 = (ulonglong *)puVar15[1];
            lVar9 = lVar10 + 1;
            if (uVar4 != uVar8) {
              lVar9 = lVar10;
            }
            lVar10 = lVar9;
          } while (puVar15 != (ulonglong *)0x0);
          if (lVar9 != 0) goto LAB_14044facc;
        }
      }
      for (puVar14 = (ulonglong *)puVar14[200]; puVar14 == (ulonglong *)0x0;
          puVar14 = (ulonglong *)*puVar14) {
        puVar14 = (ulonglong *)(plVar12 + 1);
        plVar12 = plVar12 + 1;
      }
    }
    if (0 < *(int *)(param_2 + 0x2118)) {
      lVar2 = *(longlong *)(param_2 + 0x2208);
      puVar1 = (undefined8 *)(param_1 + 0x228);
      local_1f0 = (void *)*puVar1;
      *puVar1 = 0;
      local_1e8 = *(undefined8 *)(param_1 + 0x230);
      *(undefined8 *)(param_1 + 0x230) = 0;
      local_1e0 = *(undefined8 *)(param_1 + 0x238);
      *(undefined8 *)(param_1 + 0x238) = 0;
      local_1d8 = *(void **)(param_1 + 0x240);
      *(undefined8 *)(param_1 + 0x240) = 0;
      local_1d0 = *(undefined8 *)(param_1 + 0x248);
      *(undefined8 *)(param_1 + 0x248) = 0;
      local_1c8 = *(undefined8 *)(param_1 + 0x250);
      *(undefined8 *)(param_1 + 0x250) = 0;
      local_1a0 = *(undefined8 *)(param_1 + 0x278);
      local_1c0[0] = *(undefined4 *)(param_1 + 600);
      local_198 = *(undefined4 *)(param_1 + 0x280);
      *(undefined8 *)(param_1 + 0x278) = local_1a0;
      *(undefined4 *)(param_1 + 0x280) = 0;
      local_1b8 = *(void **)(param_1 + 0x260);
      *(undefined8 **)(param_1 + 0x260) = &DAT_140b5abc0;
      local_1b0 = *(ulonglong *)(param_1 + 0x268);
      local_1a8 = *(undefined8 *)(param_1 + 0x270);
      *(undefined8 *)(param_1 + 0x270) = 0;
      *(undefined8 *)(param_1 + 0x268) = 1;
      local_190 = *(undefined8 *)(param_1 + 0x288);
      local_188 = *(undefined8 *)(param_1 + 0x290);
      local_134 = 0;
      local_168 = 0;
      uStack_160 = 0;
      local_138 = 0xffffffff;
      local_158 = 0;
      uStack_150 = 0;
      local_180 = 2;
      local_148 = 0;
      uStack_140 = 0;
      local_178 = 0;
      uStack_170 = 0;
      FUN_14045a120(puVar1,lVar2 + 0xb8);
      FUN_14045a120(lVar2 + 0xb8,&local_1f0);
      FUN_14033b630(local_1c0);
      if (1 < local_1b0) {
        free(local_1b8);
      }
      if (local_1d8 != (void *)0x0) {
        free(local_1d8);
      }
      if (local_1f0 != (void *)0x0) {
        free(local_1f0);
      }
      lVar2 = *(longlong *)(param_2 + 0x2208);
      FUN_14045a880(&local_1f0,param_1 + 0x2f8);
      FUN_1404565e0(param_1 + 0x2f8,lVar2 + 0x178);
      FUN_1404565e0(lVar2 + 0x178,&local_1f0);
      FUN_14033b630(local_100);
      if (1 < local_f0) {
        free(local_f8);
      }
      if (local_118 != (void *)0x0) {
        free(local_118);
      }
      if (local_130 != (void *)0x0) {
        free(local_130);
      }
      FUN_14033b630(local_1c0);
      if (1 < local_1b0) {
        free(local_1b8);
      }
      if (local_1d8 != (void *)0x0) {
        free(local_1d8);
      }
      if (local_1f0 != (void *)0x0) {
        free(local_1f0);
      }
      FUN_1404575f0(param_1 + 0x1578,*(longlong *)(param_2 + 0x2208) + 0x2f8);
    }
  }
  if ((*(int *)(param_2 + 0x2118) == 0) && (*(longlong *)(param_1 + 0x1588) == 0)) {
    FUN_140456940(param_1 + 0x1578,*(longlong *)(param_2 + 0x2208) + 0x2f8);
  }
  return;
}
