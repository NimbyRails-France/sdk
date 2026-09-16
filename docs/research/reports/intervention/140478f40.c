
double FUN_140478f40(double *param_1,undefined8 *param_2,longlong param_3,undefined1 param_4)

{
  int *piVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  longlong *plVar5;
  double dVar6;
  undefined8 local_28;
  undefined8 uStack_20;

  if (param_2[1] != 0) {
    LOCK();
    piVar1 = (int *)(param_2[1] + 8);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  local_28 = *param_2;
  uStack_20 = param_2[1];
  FUN_14046e5e0(param_1 + 0x2cb,&local_28,param_3,param_1,param_4);
  FUN_14047d6d0(param_1 + 0x47c);
  if (*(char *)((longlong)param_1 + 0x211c) != '\0') {
    FUN_14047eae0(param_1,param_3 + 0x400);
  }
  *param_1 = param_1[0x47c] + *param_1;
  LOCK();
  dVar6 = param_1[0x47c];
  param_1[0x47c] = 0.0;
  UNLOCK();
  *(undefined1 *)((longlong)param_1 + 0x211c) = 0;
  plVar5 = (longlong *)param_2[1];
  if (plVar5 != (longlong *)0x0) {
    LOCK();
    puVar2 = (uint *)(plVar5 + 1);
    uVar3 = *puVar2;
    dVar6 = (double)(ulonglong)uVar3;
    *puVar2 = *puVar2 - 1;
    UNLOCK();
    if (uVar3 == 1) {
      dVar6 = (double)(**(code **)*plVar5)(plVar5);
      LOCK();
      piVar1 = (int *)((longlong)plVar5 + 0xc);
      iVar4 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar4 == 1) {
        dVar6 = (double)(**(code **)(*plVar5 + 8))(plVar5);
      }
    }
  }
  return dVar6;
}


// ref 140c250b8 caller none
// ref 140349660 caller 1403489d0
// ref 1403499d5 caller 1403489d0
// ref 140349bd0 caller 1403489d0
// ref 1404f3bb8 caller 1404f3520
