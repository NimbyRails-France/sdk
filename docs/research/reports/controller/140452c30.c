
/* WARNING: Type propagation algorithm not settling */

undefined8 FUN_140452c30(undefined8 param_1,longlong *param_2,ulonglong *param_3,longlong param_4)

{
  longlong lVar1;
  ulonglong uVar2;
  undefined8 *puVar3;
  undefined1 auVar4 [16];
  char cVar5;
  longlong lVar6;
  longlong *plVar7;
  double *pdVar8;
  double *pdVar9;
  longlong lVar10;
  undefined8 *puVar11;
  double dVar12;
  double dVar13;
  undefined8 local_98;
  char local_90;
  double local_88 [3];
  double local_70 [3];
  longlong local_58 [3];
  longlong local_40;

  cVar5 = FUN_140452aa0();
  if (cVar5 != '\0') {
    return 1;
  }
  if ((((param_2 != (longlong *)0x0) && (param_3 != (ulonglong *)0x0)) && (param_4 != 0)) &&
     ((lVar10 = param_2[0x1b], lVar10 != 0 || (lVar10 = param_2[0x1a], lVar10 != 0)))) {
    lVar1 = *param_2;
    uVar2 = *param_3;
    lVar6 = FUN_14032c420(lVar1,uVar2);
    if ((lVar6 != 0) && (0.0 < *(double *)(lVar6 + 0x88))) {
      local_98 = 0;
      auVar4._8_8_ = 0;
      auVar4._0_8_ = uVar2;
      local_90 = '\0';
      FUN_14045f860(lVar10 + 0x880,local_58,param_3,
                    SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar4,8) +
                    SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar4,0),&local_98);
      if (local_90 != '\0') {
        _Mtx_unlock(local_98);
      }
      if (local_58[0] != 0) {
        local_88[2] = DAT_140aabd08;
        local_70[1] = DAT_140aabd08 / *(double *)(lVar6 + 0x88);
        local_88[0] = 0.0;
        if (DAT_140aabbf8 <= local_70[1]) {
          local_70[1] = DAT_140aabbf8;
        }
        local_70[2] = DAT_140aabd08;
        local_70[0] = 0.0;
        local_88[1] = (double)param_3[1] - local_70[1];
        local_70[1] = (double)param_3[1] + local_70[1];
        pdVar9 = local_88;
        if (0.0 <= local_88[1]) {
          pdVar9 = local_88 + 1;
        }
        pdVar8 = local_88 + 2;
        if (local_88[1] <= DAT_140aabd08) {
          pdVar8 = pdVar9;
        }
        dVar13 = *pdVar8;
        pdVar9 = local_70;
        if (0.0 <= local_70[1]) {
          pdVar9 = local_70 + 1;
        }
        pdVar8 = local_70 + 2;
        if (local_70[1] <= DAT_140aabd08) {
          pdVar8 = pdVar9;
        }
        dVar12 = *pdVar8;
        puVar3 = *(undefined8 **)(local_40 + 0x10);
        for (puVar11 = *(undefined8 **)(local_40 + 8); puVar11 != puVar3; puVar11 = puVar11 + 4) {
          plVar7 = (longlong *)FUN_14033f780(lVar1 + 0x200,*puVar11);
          if ((plVar7 != (longlong *)0x0) && (*plVar7 == param_4)) {
            pdVar9 = (double *)(puVar11 + 3);
            pdVar8 = (double *)(puVar11 + 2);
            if ((double)puVar11[2] < *pdVar9) {
              pdVar8 = pdVar9;
            }
            if (*pdVar8 <= dVar13 && dVar13 != *pdVar8) {
              return 0;
            }
            pdVar8 = (double *)(puVar11 + 2);
            if (*pdVar9 < (double)puVar11[2]) {
              pdVar8 = pdVar9;
            }
            if (dVar12 < *pdVar8) {
              return 0;
            }
            return 1;
          }
        }
      }
    }
  }
  return 0;
}
