
longlong FUN_1404515c0(longlong param_1,undefined8 param_2,longlong param_3,ulonglong param_4,
                      double param_5,double param_6)

{
  undefined1 auVar1 [16];
  double *pdVar2;
  double *pdVar3;
  longlong lVar4;
  double *pdVar5;
  longlong *plVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  ulonglong local_res20;
  undefined8 local_68;
  char local_60;
  longlong *local_58;
  longlong local_50;
  ulonglong local_48;
  longlong *local_40;
  longlong *local_38;

  uVar8 = (undefined4)((ulonglong)param_5 >> 0x20);
  uVar10 = (undefined4)((ulonglong)param_6 >> 0x20);
  uVar9 = SUB84(param_5,0);
  uVar11 = uVar8;
  if (param_6 <= param_5) {
    uVar9 = SUB84(param_6,0);
    uVar11 = uVar10;
  }
  uVar7 = SUB84(param_5,0);
  if (param_5 <= param_6) {
    uVar7 = SUB84(param_6,0);
    uVar8 = uVar10;
  }
  auVar1._8_8_ = 0;
  auVar1._0_8_ = param_4;
  local_68 = 0;
  local_60 = '\0';
  local_res20 = param_4;
  FUN_14045f860(param_1 + 0x880,&local_58,&local_res20,
                SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar1,8) +
                SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar1,0),&local_68);
  if (local_60 != '\0') {
    _Mtx_unlock(local_68);
  }
  if (local_58 != (longlong *)0x0) {
    plVar6 = (longlong *)local_40[1];
    if (plVar6 != (longlong *)local_40[2]) {
      pdVar3 = (double *)(plVar6 + 2);
      do {
        if (*plVar6 != param_3) {
          pdVar5 = pdVar3 + 1;
          pdVar2 = pdVar3;
          if (*pdVar3 < *pdVar5) {
            pdVar2 = pdVar5;
          }
          if ((double)CONCAT44(uVar11,uVar9) < *pdVar2) {
            pdVar2 = pdVar3;
            if (*pdVar5 < *pdVar3) {
              pdVar2 = pdVar5;
            }
            if (*pdVar2 <= (double)CONCAT44(uVar8,uVar7) && (double)CONCAT44(uVar8,uVar7) != *pdVar2
               ) {
              return *plVar6;
            }
          }
        }
        plVar6 = plVar6 + 4;
        pdVar3 = pdVar3 + 4;
      } while (plVar6 != (longlong *)local_40[2]);
    }
  }
  FUN_140459ea0(param_2,&local_58,local_res20);
  do {
    if ((local_58 == (longlong *)0x0) ||
       (((*local_58 == local_58[1] || (*(longlong *)(*local_58 + local_48 * 8) == 0)) &&
        ((local_40 == (longlong *)0x0 || (local_40 == local_38)))))) {
      return 0;
    }
    plVar6 = local_40;
    if ((*local_58 != local_58[1]) && (*(longlong *)(*local_58 + local_48 * 8) != 0)) {
      plVar6 = (longlong *)(local_48 * 0x20 + local_58[3]);
    }
    lVar4 = *local_58;
    if (local_58[1] - lVar4 >> 3 != 0) {
      while (*(longlong *)(lVar4 + local_48 * 8) != 0) {
        local_48 = (local_58[1] - *local_58 >> 3) - 1U & local_48 + 1;
        lVar4 = *(longlong *)(*local_58 + local_48 * 8);
        if (((lVar4 == local_50) || (lVar4 == 0)) || (lVar4 = *local_58, local_58[1] == lVar4))
        break;
      }
    }
    if (((*local_58 == local_58[1]) || (*(longlong *)(*local_58 + local_48 * 8) == 0)) &&
       (local_40 < local_38)) {
      local_40 = local_40 + 4;
    }
    if (plVar6 == (longlong *)0x0) {
      return 0;
    }
    if (*plVar6 != param_3) {
      pdVar3 = (double *)(plVar6 + 2);
      pdVar2 = (double *)(plVar6 + 1);
      pdVar5 = pdVar2;
      if (*pdVar2 < *pdVar3) {
        pdVar5 = pdVar3;
      }
      if ((double)CONCAT44(uVar11,uVar9) < *pdVar5 || (double)CONCAT44(uVar11,uVar9) == *pdVar5) {
        if (*pdVar3 < *pdVar2) {
          pdVar2 = pdVar3;
        }
        if (*pdVar2 <= (double)CONCAT44(uVar8,uVar7)) {
          return *plVar6;
        }
      }
    }
  } while( true );
}
