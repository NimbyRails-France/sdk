
void FUN_14030e020(longlong param_1,undefined8 param_2,undefined8 *param_3,longlong *param_4,
                  undefined8 param_5)

{
  int *piVar1;
  int iVar2;
  longlong *plVar3;
  undefined8 uVar4;
  undefined4 uVar5;
  longlong lVar6;
  longlong lVar7;
  longlong *plVar8;
  undefined4 uVar9;
  longlong local_res20;
  undefined1 local_248 [40];
  undefined1 local_220 [48];
  char local_1f0;
  undefined1 local_58;
  
  plVar8 = (longlong *)*param_4;
  plVar3 = (longlong *)param_4[1];
  if (plVar8 != plVar3) {
    uVar9 = (undefined4)DAT_140aac1d8;
    do {
      FUN_1403272e0(param_1);
      FUN_140327200(param_1 + 0x80);
      FUN_140327120(param_1 + 0x100);
      FUN_140327040(param_1 + 0x180);
      FUN_140326f60(param_1 + 0x200);
      FUN_140326e80(param_1 + 0x280);
      FUN_140326da0(param_1 + 0x300);
      FUN_140326cc0(param_1 + 0x380);
      lVar6 = FUN_14031f740(param_5);
      *(undefined8 *)(lVar6 + 0x7f8) = *(undefined8 *)(*plVar8 + 8);
      *(undefined8 *)(lVar6 + 0x800) = *(undefined8 *)(*plVar8 + 0x10);
      uVar5 = (**(code **)(*(longlong *)*plVar8 + 0x28))();
      *(undefined4 *)(lVar6 + 0x808) = uVar5;
      *(undefined8 *)(lVar6 + 0x810) = *(undefined8 *)(*plVar8 + 0x18);
      lVar7 = (**(code **)(*(longlong *)*plVar8 + 8))((longlong *)*plVar8,local_248,param_1,param_2)
      ;
      local_res20 = lVar6;
      FUN_140330070((longlong)*(char *)(lVar7 + 0x1f0) + 1,&local_res20,lVar7);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
      switch(local_58) {
      case 1:
        FUN_1402f3da0(local_248);
        break;
      case 5:
        if (local_1f0 != '\0') {
          FUN_140322e90(local_220);
        }
      }
      uVar5 = DAT_140b77e58;
      uVar4 = *param_3;
      FUN_140397ee0(param_1 + 0x428,uVar4,uVar9,DAT_140b77e58,0xffff);
      FUN_140396660(param_1 + 0x428,uVar4,uVar9,uVar5,0xffff);
      FUN_14036b270(param_1 + 0x780,DAT_140aac208,uVar5);
      FUN_14034d9c0(param_1 + 0x8a0);
      FUN_1403e7060(param_1 + 0x890,uVar5);
      FUN_140324750(param_1,lVar6 + 0x1f8);
      FUN_140479530(param_2,param_1,lVar6 + 0x1f8);
      plVar8 = plVar8 + 1;
    } while (plVar8 != plVar3);
  }
  plVar8 = (longlong *)param_3[1];
  if (plVar8 != (longlong *)0x0) {
    LOCK();
    plVar3 = plVar8 + 1;
    lVar6 = *plVar3;
    *(int *)plVar3 = (int)*plVar3 + -1;
    UNLOCK();
    if ((int)lVar6 == 1) {
      (**(code **)*plVar8)(plVar8);
      LOCK();
      piVar1 = (int *)((longlong)plVar8 + 0xc);
      iVar2 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar2 == 1) {
        (**(code **)(*plVar8 + 8))(plVar8);
      }
    }
  }
  return;
}

