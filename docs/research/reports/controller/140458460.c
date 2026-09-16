
undefined8 FUN_140458460(undefined8 *param_1,ulonglong *param_2,double param_3,double param_4)

{
  double *pdVar1;
  undefined1 auVar2 [16];
  double *pdVar3;
  double *pdVar4;
  longlong *plVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined8 local_68;
  char local_60;
  longlong local_58 [3];
  longlong local_40;

  uVar7 = (undefined4)((ulonglong)param_3 >> 0x20);
  uVar9 = (undefined4)((ulonglong)param_4 >> 0x20);
  uVar8 = SUB84(param_3,0);
  uVar10 = uVar7;
  if (param_4 <= param_3) {
    uVar8 = SUB84(param_4,0);
    uVar10 = uVar9;
  }
  uVar6 = SUB84(param_3,0);
  if (param_3 <= param_4) {
    uVar6 = SUB84(param_4,0);
    uVar7 = uVar9;
  }
  auVar2._8_8_ = 0;
  auVar2._0_8_ = *param_2;
  local_68 = 0;
  local_60 = '\0';
  FUN_14045f860(*param_1,local_58,param_2,
                SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar2,8) +
                SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar2,0),&local_68);
  if (local_60 != '\0') {
    _Mtx_unlock(local_68);
  }
  if (local_58[0] != 0) {
    plVar5 = *(longlong **)(local_40 + 8);
    if (plVar5 != *(longlong **)(local_40 + 0x10)) {
      pdVar4 = (double *)(plVar5 + 2);
      do {
        if (*plVar5 != **(longlong **)param_1[1]) {
          pdVar1 = pdVar4 + 1;
          pdVar3 = pdVar4;
          if (*pdVar4 < *pdVar1) {
            pdVar3 = pdVar1;
          }
          if ((double)CONCAT44(uVar10,uVar8) < *pdVar3) {
            pdVar3 = pdVar4;
            if (*pdVar1 < *pdVar4) {
              pdVar3 = pdVar1;
            }
            if (*pdVar3 <= (double)CONCAT44(uVar7,uVar6) && (double)CONCAT44(uVar7,uVar6) != *pdVar3
               ) {
              *(undefined1 *)param_1[2] = 0;
              return 0;
            }
          }
        }
        plVar5 = plVar5 + 4;
        pdVar4 = pdVar4 + 4;
      } while (plVar5 != *(longlong **)(local_40 + 0x10));
    }
  }
  return 1;
}
