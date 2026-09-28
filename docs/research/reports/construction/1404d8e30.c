
void FUN_1404d8e30(longlong *param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong *plVar2;
  void **ppvVar3;
  longlong *plVar4;
  longlong lVar5;
  void *pvVar6;
  longlong lVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  longlong *plVar10;
  void *pvVar11;
  longlong *plVar12;
  longlong *plVar13;
  longlong *plVar14;
  longlong *plVar15;
  longlong local_418;
  undefined4 local_410;
  undefined1 local_40c;
  undefined4 local_408;
  undefined4 uStack_404;
  undefined4 uStack_400;
  undefined4 uStack_3fc;
  undefined1 local_3f8 [32];
  undefined1 local_3d8;
  undefined4 local_3d4;
  void *local_3d0;
  longlong local_3c8;
  longlong local_3c0;
  void *local_3b8;
  undefined8 local_3b0;
  void *local_398;
  void *local_190;
  undefined8 local_188;
  void *local_170;
  void *local_e8;
  undefined8 local_e0;
  void *local_c8;
  longlong local_a0;
  longlong lStack_98;
  longlong local_90;
  longlong lStack_88;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined1 local_74;
  undefined4 local_70;
  void *local_68;
  void *local_60;
  
  if ((param_1 != param_2) && (plVar14 = param_1 + 0x7d, plVar14 != param_2)) {
    plVar15 = param_1 + 0x88;
    do {
      FUN_14033ff70(&local_418,plVar14);
      plVar12 = plVar14;
      plVar10 = plVar15;
      plVar13 = plVar15;
      plVar1 = plVar14;
      while (plVar1 != param_1) {
        plVar4 = plVar10 + -0x7d;
        if (plVar10[-0x88] <= local_418) break;
        *plVar12 = plVar10[-0x88];
        *(int *)(plVar13 + -10) = (int)plVar10[-0x87];
        *(undefined1 *)((longlong)plVar13 + -0x4c) = *(undefined1 *)((longlong)plVar10 + -0x434);
        uVar8 = *(undefined4 *)((longlong)plVar10 + -0x42c);
        lVar5 = plVar10[-0x85];
        uVar9 = *(undefined4 *)((longlong)plVar10 + -0x424);
        *(int *)(plVar13 + -9) = (int)plVar10[-0x86];
        *(undefined4 *)((longlong)plVar13 + -0x44) = uVar8;
        *(int *)(plVar13 + -8) = (int)lVar5;
        *(undefined4 *)((longlong)plVar13 + -0x3c) = uVar9;
        FUN_140025630(plVar13 + -7,plVar10 + -0x84);
        *(char *)(plVar13 + -3) = (char)plVar10[-0x80];
        *(undefined4 *)((longlong)plVar13 + -0x14) = *(undefined4 *)((longlong)plVar10 + -0x3fc);
        plVar1 = plVar10 + -0x7f;
        plVar2 = plVar13 + -2;
        if (plVar2 != plVar1) {
          lVar5 = *plVar2;
          *plVar2 = lVar5;
          plVar13[-1] = lVar5;
          *plVar13 = *plVar13;
          lVar5 = *plVar2;
          *plVar2 = *plVar1;
          *plVar1 = lVar5;
          lVar5 = plVar13[-1];
          plVar13[-1] = plVar10[-0x7e];
          plVar10[-0x7e] = lVar5;
          lVar5 = *plVar13;
          *plVar13 = *plVar4;
          *plVar4 = lVar5;
        }
        plVar1 = plVar13 + 1;
        if (plVar1 != plVar10 + -0x7c) {
          plVar13[2] = *plVar1;
          FUN_1403443a0(plVar1,plVar10[-0x7c],plVar10[-0x7b]);
        }
        plVar1 = plVar13 + 0x46;
        if (plVar1 != plVar10 + -0x37) {
          plVar13[0x47] = *plVar1;
          FUN_1403443a0(plVar1,plVar10[-0x37],plVar10[-0x36]);
        }
        plVar1 = plVar13 + 0x5b;
        if (plVar1 != plVar10 + -0x22) {
          plVar13[0x5c] = *plVar1;
          FUN_1403443a0(plVar1,plVar10[-0x22],plVar10[-0x21]);
        }
        uVar8 = *(undefined4 *)((longlong)plVar10 + -0xc4);
        lVar5 = plVar10[-0x18];
        uVar9 = *(undefined4 *)((longlong)plVar10 + -0xbc);
        *(int *)(plVar13 + 100) = (int)plVar10[-0x19];
        *(undefined4 *)((longlong)plVar13 + 0x324) = uVar8;
        *(int *)(plVar13 + 0x65) = (int)lVar5;
        *(undefined4 *)((longlong)plVar13 + 0x32c) = uVar9;
        uVar8 = *(undefined4 *)((longlong)plVar10 + -0xb4);
        lVar5 = plVar10[-0x16];
        uVar9 = *(undefined4 *)((longlong)plVar10 + -0xac);
        *(int *)(plVar13 + 0x66) = (int)plVar10[-0x17];
        *(undefined4 *)((longlong)plVar13 + 0x334) = uVar8;
        *(int *)(plVar13 + 0x67) = (int)lVar5;
        *(undefined4 *)((longlong)plVar13 + 0x33c) = uVar9;
        *(int *)(plVar13 + 0x68) = (int)plVar10[-0x15];
        *(undefined4 *)((longlong)plVar13 + 0x344) = *(undefined4 *)((longlong)plVar10 + -0xa4);
        *(int *)(plVar13 + 0x69) = (int)plVar10[-0x14];
        *(undefined1 *)((longlong)plVar13 + 0x34c) = *(undefined1 *)((longlong)plVar10 + -0x9c);
        *(int *)(plVar13 + 0x6a) = (int)plVar10[-0x13];
        FUN_140329e20(plVar13 + 0x6b,plVar10 + -0x12);
        plVar12 = plVar12 + -0x7d;
        plVar13 = plVar13 + -0x7d;
        plVar1 = plVar10 + -0x88;
        plVar10 = plVar4;
      }
      *plVar12 = local_418;
      *(undefined4 *)(plVar12 + 1) = local_410;
      *(undefined1 *)((longlong)plVar12 + 0xc) = local_40c;
      *(undefined4 *)(plVar12 + 2) = local_408;
      *(undefined4 *)((longlong)plVar12 + 0x14) = uStack_404;
      *(undefined4 *)(plVar12 + 3) = uStack_400;
      *(undefined4 *)((longlong)plVar12 + 0x1c) = uStack_3fc;
      FUN_140025630(plVar12 + 4,local_3f8);
      *(undefined1 *)(plVar12 + 8) = local_3d8;
      *(undefined4 *)((longlong)plVar12 + 0x44) = local_3d4;
      ppvVar3 = (void **)(plVar12 + 9);
      if (ppvVar3 != &local_3d0) {
        pvVar11 = *ppvVar3;
        *ppvVar3 = (void *)0x0;
        lVar5 = plVar12[0xb];
        plVar12[0xb] = 0;
        pvVar6 = *ppvVar3;
        *ppvVar3 = pvVar11;
        plVar12[10] = (longlong)pvVar11;
        plVar12[0xb] = lVar5;
        if (pvVar6 != (void *)0x0) {
          free(pvVar6);
        }
        pvVar11 = *ppvVar3;
        *ppvVar3 = local_3d0;
        lVar5 = plVar12[10];
        plVar12[10] = local_3c8;
        lVar7 = plVar12[0xb];
        plVar12[0xb] = local_3c0;
        local_3d0 = pvVar11;
        local_3c8 = lVar5;
        local_3c0 = lVar7;
      }
      ppvVar3 = (void **)(plVar12 + 0xc);
      if (ppvVar3 != &local_3b8) {
        plVar12[0xd] = (longlong)*ppvVar3;
        FUN_1403443a0(ppvVar3,local_3b8,local_3b0);
      }
      ppvVar3 = (void **)(plVar12 + 0x51);
      if (ppvVar3 != &local_190) {
        plVar12[0x52] = (longlong)*ppvVar3;
        FUN_1403443a0(ppvVar3,local_190,local_188);
      }
      ppvVar3 = (void **)(plVar12 + 0x66);
      if (ppvVar3 != &local_e8) {
        plVar12[0x67] = (longlong)*ppvVar3;
        FUN_1403443a0(ppvVar3,local_e8,local_e0);
      }
      plVar12[0x6f] = local_a0;
      plVar12[0x70] = lStack_98;
      plVar12[0x71] = local_90;
      plVar12[0x72] = lStack_88;
      *(undefined4 *)(plVar12 + 0x73) = local_80;
      *(undefined4 *)((longlong)plVar12 + 0x39c) = local_7c;
      *(undefined4 *)(plVar12 + 0x74) = local_78;
      *(undefined1 *)((longlong)plVar12 + 0x3a4) = local_74;
      *(undefined4 *)(plVar12 + 0x75) = local_70;
      FUN_140329e20(plVar12 + 0x76,&local_68);
      pvVar6 = local_60;
      for (pvVar11 = local_68; pvVar11 != pvVar6; pvVar11 = (void *)((longlong)pvVar11 + 0x50)) {
        FUN_14030dbd0(pvVar11);
      }
      if (local_68 != (void *)0x0) {
        free(local_68);
      }
      if ((local_e8 != (void *)0x0) && (local_e8 != local_c8)) {
        free(local_e8);
      }
      if ((local_190 != (void *)0x0) && (local_190 != local_170)) {
        free(local_190);
      }
      if ((local_3b8 != (void *)0x0) && (local_3b8 != local_398)) {
        free(local_3b8);
      }
      if (local_3d0 != (void *)0x0) {
        free(local_3d0);
      }
      FUN_140002d30(local_3f8);
      plVar14 = plVar14 + 0x7d;
      plVar15 = plVar15 + 0x7d;
    } while (plVar14 != param_2);
  }
  return;
}

