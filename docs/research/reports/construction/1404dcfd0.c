
void FUN_1404dcfd0(longlong *param_1,longlong *param_2,longlong *param_3,undefined1 param_4)

{
  longlong *plVar1;
  longlong *plVar2;
  undefined1 auVar3 [16];
  undefined4 uVar4;
  undefined4 uVar5;
  void *pvVar6;
  longlong *plVar7;
  void *pvVar8;
  ulonglong uVar9;
  longlong lVar10;
  longlong lVar11;
  undefined1 local_420 [32];
  undefined1 local_400 [40];
  void *local_3d8;
  void *local_3c0;
  void *local_3a0;
  void *local_198;
  void *local_178;
  void *local_f0;
  void *local_d0;
  void *local_70;
  void *local_68;
  
  uVar9 = (longlong)param_2 - (longlong)param_1;
  lVar10 = ((longlong)uVar9 >> 3) * 0x1cac083126e978d5;
  if (1 < lVar10) {
    lVar11 = (lVar10 + -2 >> 1) + 1;
    plVar7 = param_1 + lVar11 * 0x7d;
    do {
      lVar11 = lVar11 + -1;
      plVar7 = plVar7 + -0x7d;
      FUN_14033ff70(local_420,plVar7);
      FUN_1404df2b0(param_1,lVar11,lVar10,lVar11,local_420,param_4);
      pvVar6 = local_68;
      for (pvVar8 = local_70; pvVar8 != pvVar6; pvVar8 = (void *)((longlong)pvVar8 + 0x50)) {
        FUN_14030dbd0(pvVar8);
      }
      if (local_70 != (void *)0x0) {
        free(local_70);
      }
      if ((local_f0 != (void *)0x0) && (local_f0 != local_d0)) {
        free(local_f0);
      }
      if ((local_198 != (void *)0x0) && (local_198 != local_178)) {
        free(local_198);
      }
      if ((local_3c0 != (void *)0x0) && (local_3c0 != local_3a0)) {
        free(local_3c0);
      }
      if (local_3d8 != (void *)0x0) {
        free(local_3d8);
      }
      FUN_140002d30(local_400);
    } while (lVar11 != 0);
  }
  if (param_2 < param_3) {
    plVar7 = param_2 + 0xb;
    do {
      if (plVar7[-0xb] < *param_1) {
        FUN_14033ff70(local_420,plVar7 + -0xb);
        plVar7[-0xb] = *param_1;
        *(int *)(plVar7 + -10) = (int)param_1[1];
        *(undefined1 *)((longlong)plVar7 + -0x4c) = *(undefined1 *)((longlong)param_1 + 0xc);
        uVar4 = *(undefined4 *)((longlong)param_1 + 0x14);
        lVar11 = param_1[3];
        uVar5 = *(undefined4 *)((longlong)param_1 + 0x1c);
        *(int *)(plVar7 + -9) = (int)param_1[2];
        *(undefined4 *)((longlong)plVar7 + -0x44) = uVar4;
        *(int *)(plVar7 + -8) = (int)lVar11;
        *(undefined4 *)((longlong)plVar7 + -0x3c) = uVar5;
        FUN_140025630(plVar7 + -7,param_1 + 4);
        *(char *)(plVar7 + -3) = (char)param_1[8];
        *(undefined4 *)((longlong)plVar7 + -0x14) = *(undefined4 *)((longlong)param_1 + 0x44);
        plVar1 = param_1 + 9;
        plVar2 = plVar7 + -2;
        if (plVar2 != plVar1) {
          lVar11 = *plVar2;
          *plVar2 = lVar11;
          plVar7[-1] = lVar11;
          *plVar7 = *plVar7;
          lVar11 = *plVar2;
          *plVar2 = *plVar1;
          *plVar1 = lVar11;
          lVar11 = plVar7[-1];
          plVar7[-1] = param_1[10];
          param_1[10] = lVar11;
          lVar11 = *plVar7;
          *plVar7 = param_1[0xb];
          param_1[0xb] = lVar11;
        }
        plVar1 = plVar7 + 1;
        if (plVar1 != param_1 + 0xc) {
          plVar7[2] = *plVar1;
          FUN_1403443a0(plVar1,param_1[0xc],param_1[0xd]);
        }
        plVar1 = plVar7 + 0x46;
        if (plVar1 != param_1 + 0x51) {
          plVar7[0x47] = *plVar1;
          FUN_1403443a0(plVar1,param_1[0x51],param_1[0x52]);
        }
        plVar1 = plVar7 + 0x5b;
        if (plVar1 != param_1 + 0x66) {
          plVar7[0x5c] = *plVar1;
          FUN_1403443a0(plVar1,param_1[0x66],param_1[0x67]);
        }
        uVar4 = *(undefined4 *)((longlong)param_1 + 0x37c);
        lVar11 = param_1[0x70];
        uVar5 = *(undefined4 *)((longlong)param_1 + 900);
        *(int *)(plVar7 + 100) = (int)param_1[0x6f];
        *(undefined4 *)((longlong)plVar7 + 0x324) = uVar4;
        *(int *)(plVar7 + 0x65) = (int)lVar11;
        *(undefined4 *)((longlong)plVar7 + 0x32c) = uVar5;
        uVar4 = *(undefined4 *)((longlong)param_1 + 0x38c);
        lVar11 = param_1[0x72];
        uVar5 = *(undefined4 *)((longlong)param_1 + 0x394);
        *(int *)(plVar7 + 0x66) = (int)param_1[0x71];
        *(undefined4 *)((longlong)plVar7 + 0x334) = uVar4;
        *(int *)(plVar7 + 0x67) = (int)lVar11;
        *(undefined4 *)((longlong)plVar7 + 0x33c) = uVar5;
        *(int *)(plVar7 + 0x68) = (int)param_1[0x73];
        *(undefined4 *)((longlong)plVar7 + 0x344) = *(undefined4 *)((longlong)param_1 + 0x39c);
        *(int *)(plVar7 + 0x69) = (int)param_1[0x74];
        *(undefined1 *)((longlong)plVar7 + 0x34c) = *(undefined1 *)((longlong)param_1 + 0x3a4);
        *(int *)(plVar7 + 0x6a) = (int)param_1[0x75];
        FUN_140329e20(plVar7 + 0x6b,param_1 + 0x76);
        FUN_1404df2b0(param_1,0,lVar10,0,local_420,param_4);
        pvVar6 = local_68;
        for (pvVar8 = local_70; pvVar8 != pvVar6; pvVar8 = (void *)((longlong)pvVar8 + 0x50)) {
          FUN_14030dbd0(pvVar8);
        }
        if (local_70 != (void *)0x0) {
          free(local_70);
        }
        if ((local_f0 != (void *)0x0) && (local_f0 != local_d0)) {
          free(local_f0);
        }
        if ((local_198 != (void *)0x0) && (local_198 != local_178)) {
          free(local_198);
        }
        if ((local_3c0 != (void *)0x0) && (local_3c0 != local_3a0)) {
          free(local_3c0);
        }
        if (local_3d8 != (void *)0x0) {
          free(local_3d8);
        }
        FUN_140002d30(local_400);
      }
      plVar1 = plVar7 + 0x72;
      plVar7 = plVar7 + 0x7d;
    } while (plVar1 < param_3);
  }
  if (1 < lVar10) {
    do {
      FUN_14033ff70(local_420,(longlong)param_1 + (uVar9 - 1000));
      FUN_1404d54c0((longlong)param_1 + (uVar9 - 1000),param_1);
      auVar3._8_8_ = 0;
      auVar3._0_8_ = uVar9;
      lVar10 = SUB168(ZEXT816(0x624dd2f1a9fbe77) * auVar3,8);
      FUN_1404df2b0(param_1,0,((uVar9 - lVar10 >> 1) + lVar10 >> 9) - 1,0,local_420,param_4);
      FUN_14032c9f0(local_420);
      uVar9 = uVar9 - 1000;
    } while (1999 < (longlong)uVar9);
  }
  return;
}

