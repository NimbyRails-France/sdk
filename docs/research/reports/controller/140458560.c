
bool FUN_140458560(undefined8 *param_1,longlong *param_2)

{
  double *pdVar1;
  double dVar2;
  double dVar3;
  longlong *plVar4;
  char cVar5;
  double *pdVar6;
  longlong lVar7;
  longlong lVar8;
  double *pdVar9;
  longlong *plVar10;
  double dVar11;

  if (*(longlong *)*param_1 != 0) {
    FUN_1403bbf00();
  }
  if ((((int)param_2[4] == 6) && ((longlong *)param_2[5] != (longlong *)0x0)) &&
     (*(longlong *)param_2[5] == *(longlong *)param_1[1])) {
    *(undefined1 *)param_1[2] = 1;
    return true;
  }
  if (*(char *)param_1[2] == '\0') {
    return true;
  }
  pdVar9 = (double *)(param_2 + 1);
  pdVar1 = (double *)(param_2 + 2);
  cVar5 = FUN_1404582d0(param_1[3],*param_2,param_2[1],param_2[2]);
  if ((cVar5 != '\0') && (cVar5 = FUN_140458460(param_1[4],*param_2,*pdVar9,*pdVar1), cVar5 != '\0')
     ) {
    pdVar6 = pdVar9;
    if (*pdVar1 < *pdVar9) {
      pdVar6 = pdVar1;
    }
    dVar2 = *pdVar6;
    if (*pdVar9 < *pdVar1) {
      pdVar9 = pdVar1;
    }
    plVar4 = *(longlong **)(*param_2 + 0x100);
    dVar3 = *pdVar9;
    for (plVar10 = *(longlong **)(*param_2 + 0xf8); plVar10 != plVar4; plVar10 = plVar10 + 6) {
      if ((((double)*(float *)(plVar10 + 3) <= dVar3) &&
          (dVar2 <= (double)*(float *)((longlong)plVar10 + 0x1c))) &&
         ((*plVar10 != 0 && (lVar7 = FUN_14032c420(*(undefined8 *)(param_1[5] + 0x428)), lVar7 != 0)
          ))) {
        dVar11 = (double)*(float *)(plVar10 + 4);
        cVar5 = FUN_1404582d0(param_1[3],lVar7,dVar11,(double)*(float *)((longlong)plVar10 + 0x24));
        if (cVar5 == '\0') {
          return false;
        }
        cVar5 = FUN_140458460(param_1[4],lVar7,dVar11);
        if (cVar5 == '\0') {
          return false;
        }
      }
    }
    if (((((int)param_2[4] != 7) ||
         (lVar7 = FUN_1404276c0(param_1[5] + 0x1508,param_1[6],param_2[5]), lVar7 == 0)) ||
        (lVar8 = FUN_14033f780(param_1[5] + 0x200,lVar7), lVar8 == 0)) ||
       (lVar7 == **(longlong **)param_1[7])) {
      return (int)param_2[4] != 6;
    }
    *(undefined1 *)param_1[8] = 0;
  }
  return false;
}
