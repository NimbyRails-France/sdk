
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140477ce0(double *param_1,undefined8 *param_2,longlong param_3,longlong param_4)

{
  undefined8 *puVar1;
  longlong *plVar2;
  int *piVar3;
  undefined4 uVar4;
  longlong lVar5;
  double dVar6;
  undefined8 *puVar7;
  longlong *plVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  undefined8 *local_68;
  undefined8 *puStack_60;

  if (param_4 < 1) {
    plVar8 = (longlong *)param_2[1];
  }
  else {
    if (0xa6 < param_4) {
      param_4 = 0xa6;
    }
    *(undefined4 *)(param_1 + 6) = *(undefined4 *)(param_1 + 4);
    *(undefined4 *)((longlong)param_1 + 0x34) = *(undefined4 *)((longlong)param_1 + 0x24);
    *(undefined4 *)(param_1 + 7) = *(undefined4 *)(param_1 + 5);
    *(undefined4 *)((longlong)param_1 + 0x3c) = *(undefined4 *)((longlong)param_1 + 0x2c);
    lVar5 = ((longlong)*(int *)(*(longlong *)(param_3 + 0x410) + 0x6c) * 100 + (longlong)param_1[5])
            * 10000 + (longlong)param_1[4] * 1000000;
    iVar9 = (int)(lVar5 / 86400000000);
    if (((longlong)iVar9 * 86400000000 - lVar5 != 0) && (lVar5 <= (longlong)iVar9 * 86400000000)) {
      iVar9 = iVar9 + -1;
    }
    dVar6 = (double)((longlong)param_1[5] + param_4);
    param_1[5] = dVar6;
    lVar5 = ((longlong)*(int *)(*(longlong *)(param_3 + 0x410) + 0x6c) * 100 + (longlong)dVar6) *
            10000 + (longlong)param_1[4] * 1000000;
    iVar10 = (int)(lVar5 / 86400000000);
    if (((longlong)iVar10 * 86400000000 - lVar5 != 0) && (lVar5 <= (longlong)iVar10 * 86400000000))
    {
      iVar10 = iVar10 + -1;
    }
    uVar4 = FUN_1404775e0(param_1 + 4,lVar5 % 86400000000);
    *(undefined4 *)(param_1 + 0x41f) = uVar4;
    if (iVar9 != iVar10) {
      FUN_14047eae0(param_1,param_3 + 0x400);
    }
    dVar6 = (double)(((longlong)param_1[5] * 10000) / 1000000 + (longlong)param_1[4]);
    param_1[0x421] = dVar6;
    param_1[0x420] = (double)((longlong)dVar6 - (longlong)*(int *)(param_1 + 0x41f));
    FUN_14047e890(param_1,*(undefined4 *)(*(longlong *)(param_3 + 0x410) + 0x6c));
    FUN_140479030(param_1,&local_68,param_1 + 4);
    if (local_68 != puStack_60) {
      uVar11 = (uint)DAT_140aad600;
      uVar12 = (uint)((ulonglong)DAT_140aad600 >> 0x20);
      puVar7 = local_68 + 2;
      do {
        *param_1 = (double)CONCAT44((uint)((ulonglong)puVar7[-1] >> 0x20) ^ uVar12,
                                    (uint)puVar7[-1] ^ uVar11) + *param_1;
        FUN_140380bb0(param_1 + 0x47c,0,1);
        *param_1 = (double)CONCAT44((uint)((ulonglong)*puVar7 >> 0x20) ^ uVar12,
                                    (uint)*puVar7 ^ uVar11) + *param_1;
        FUN_140380bb0(param_1 + 0x47c,0,2);
        puVar1 = puVar7 + 1;
        puVar7 = puVar7 + 3;
      } while (puVar1 != puStack_60);
    }
    if (local_68 != (undefined8 *)0x0) {
      free(local_68);
    }
    if (iVar9 != iVar10) {
      FUN_14047f000(param_1,*(undefined8 *)(param_3 + 0x410));
      FUN_14047f440(param_1,param_3);
    }
    FUN_14044d6b0(param_1 + 0x14,param_3,param_1,param_4);
    if (param_2[1] != 0) {
      LOCK();
      piVar3 = (int *)(param_2[1] + 8);
      *piVar3 = *piVar3 + 1;
      UNLOCK();
    }
    local_68 = (undefined8 *)*param_2;
    puStack_60 = (undefined8 *)param_2[1];
    FUN_14046cbd0(param_1 + 0x2cb,&local_68,param_3,param_1,param_4);
    dVar6 = (double)FUN_140249580(param_1 + 0x42b);
    if (DAT_140aabc08 <= dVar6) {
      *(float *)(param_1 + 0x42d) =
           (float)((double)((longlong)param_1[5] * 10000 - (longlong)param_1[0x42c]) *
                  _DAT_140aab958);
      param_1[0x42c] = (double)((longlong)param_1[5] * 10000);
      FUN_140477430(param_1 + 0x42b);
    }
    plVar8 = (longlong *)param_2[1];
  }
  if (plVar8 != (longlong *)0x0) {
    LOCK();
    plVar2 = plVar8 + 1;
    lVar5 = *plVar2;
    *(int *)plVar2 = (int)*plVar2 + -1;
    UNLOCK();
    if ((int)lVar5 == 1) {
      (**(code **)*plVar8)(plVar8);
      LOCK();
      piVar3 = (int *)((longlong)plVar8 + 0xc);
      iVar9 = *piVar3;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (iVar9 == 1) {
        (**(code **)(*plVar8 + 8))(plVar8);
      }
    }
  }
  return;
}


// ref 140c2507c caller none
// ref 1404784f4 caller 140478000
