
void FUN_14047a370(longlong param_1)

{
  longlong *plVar1;
  int *piVar2;
  int iVar3;
  longlong *plVar4;
  ulonglong uVar5;
  longlong lVar6;
  longlong lVar7;

  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  plVar4 = *(longlong **)(param_1 + 0x70);
  *(undefined8 *)(param_1 + 0x68) = 0;
  *(undefined8 *)(param_1 + 0x70) = 0;
  if (plVar4 != (longlong *)0x0) {
    LOCK();
    plVar1 = plVar4 + 1;
    lVar6 = *plVar1;
    *(int *)plVar1 = (int)*plVar1 + -1;
    UNLOCK();
    if ((int)lVar6 == 1) {
      (**(code **)*plVar4)(plVar4);
      LOCK();
      piVar2 = (int *)((longlong)plVar4 + 0xc);
      iVar3 = *piVar2;
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (iVar3 == 1) {
        (**(code **)(*plVar4 + 8))(plVar4);
      }
    }
  }
  plVar4 = *(longlong **)(param_1 + 0x80);
  *(undefined8 *)(param_1 + 0x78) = 0;
  *(undefined8 *)(param_1 + 0x80) = 0;
  if (plVar4 != (longlong *)0x0) {
    LOCK();
    plVar1 = plVar4 + 1;
    lVar6 = *plVar1;
    *(int *)plVar1 = (int)*plVar1 + -1;
    UNLOCK();
    if ((int)lVar6 == 1) {
      (**(code **)*plVar4)(plVar4);
      LOCK();
      piVar2 = (int *)((longlong)plVar4 + 0xc);
      iVar3 = *piVar2;
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (iVar3 == 1) {
        (**(code **)(*plVar4 + 8))(plVar4);
      }
    }
  }
  FUN_140350500(param_1 + 0x318);
  FUN_1403505a0(param_1 + 0x348);
  plVar4 = (longlong *)(param_1 + 0x378);
  if (*(longlong *)(param_1 + 0x388) != 0) {
    uVar5 = *(ulonglong *)(param_1 + 0x390);
    if (uVar5 < 0x80) {
      if (uVar5 != 0) {
        *(undefined8 *)(param_1 + 0x388) = 0;
        memset((void *)*plVar4,-0x80,uVar5 + 0x10);
        *(undefined1 *)(uVar5 + *plVar4) = 0xff;
        *(ulonglong *)(param_1 + 0x3a8) =
             (*(ulonglong *)(param_1 + 0x390) - (*(ulonglong *)(param_1 + 0x390) >> 3)) -
             *(longlong *)(param_1 + 0x388);
      }
    }
    else {
      FUN_14034dfe0(plVar4);
    }
  }
  FUN_140325a40(param_1 + 0x3b0);
  FUN_140482470(param_1 + 0x3e0);
  FUN_1404824f0(param_1 + 0x3f8);
  lVar6 = *(longlong *)(param_1 + 0x418);
  lVar7 = *(longlong *)(param_1 + 0x410);
  if (lVar7 != lVar6) {
    do {
      FUN_140354b10(lVar7 + 0x10);
      lVar7 = lVar7 + 0x40;
    } while (lVar7 != lVar6);
    *(undefined8 *)(param_1 + 0x418) = *(undefined8 *)(param_1 + 0x410);
  }
  FUN_140350790(param_1 + 0x488);
  FUN_140350640(param_1 + 0x428);
  FUN_1403506e0(param_1 + 0x458);
  FUN_1402531c0(param_1 + 0x4b8);
  return;
}
