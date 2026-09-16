
longlong * FUN_140452860(undefined8 param_1,longlong *param_2,ulonglong *param_3,longlong param_4)

{
  longlong lVar1;
  undefined8 *puVar2;
  undefined1 auVar3 [16];
  longlong *plVar4;
  longlong lVar5;
  double *pdVar6;
  double *pdVar7;
  longlong lVar8;
  undefined8 *puVar9;
  double dVar10;
  double dVar11;
  undefined8 local_88;
  char local_80;
  double local_78 [3];
  double local_60 [3];
  longlong local_48 [3];
  longlong local_30;

  plVar4 = (longlong *)FUN_1404526b0();
  if (plVar4 == (longlong *)0x0) {
    if (((param_2 != (longlong *)0x0) && (param_3 != (ulonglong *)0x0)) &&
       ((lVar8 = param_2[0x1b], lVar8 != 0 || (lVar8 = param_2[0x1a], lVar8 != 0)))) {
      lVar1 = *param_2;
      lVar5 = FUN_14032c420(lVar1,*param_3);
      if ((lVar5 != 0) && (0.0 < *(double *)(lVar5 + 0x88))) {
        local_88 = 0;
        auVar3._8_8_ = 0;
        auVar3._0_8_ = *param_3;
        local_80 = '\0';
        FUN_14045f860(lVar8 + 0x880,local_48,param_3,
                      SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar3,8) +
                      SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar3,0),&local_88);
        if (local_80 != '\0') {
          _Mtx_unlock(local_88);
        }
        if (local_48[0] != 0) {
          local_78[2] = DAT_140aabd08;
          local_60[1] = DAT_140aabd08 / *(double *)(lVar5 + 0x88);
          local_78[0] = 0.0;
          if (DAT_140aabbf8 <= local_60[1]) {
            local_60[1] = DAT_140aabbf8;
          }
          local_60[2] = DAT_140aabd08;
          local_60[0] = 0.0;
          local_78[1] = (double)param_3[1] - local_60[1];
          local_60[1] = (double)param_3[1] + local_60[1];
          pdVar7 = local_78;
          if (0.0 <= local_78[1]) {
            pdVar7 = local_78 + 1;
          }
          pdVar6 = local_78 + 2;
          if (local_78[1] <= DAT_140aabd08) {
            pdVar6 = pdVar7;
          }
          dVar11 = *pdVar6;
          pdVar7 = local_60;
          if (0.0 <= local_60[1]) {
            pdVar7 = local_60 + 1;
          }
          pdVar6 = local_60 + 2;
          if (local_60[1] <= DAT_140aabd08) {
            pdVar6 = pdVar7;
          }
          dVar10 = *pdVar6;
          puVar9 = *(undefined8 **)(local_30 + 8);
          puVar2 = *(undefined8 **)(local_30 + 0x10);
          if (puVar9 != puVar2) {
            pdVar7 = (double *)(puVar9 + 2);
            do {
              plVar4 = (longlong *)FUN_14033f780(lVar1 + 0x200,*puVar9);
              if ((plVar4 != (longlong *)0x0) && (*plVar4 != param_4)) {
                pdVar6 = pdVar7;
                if (*pdVar7 < pdVar7[1]) {
                  pdVar6 = pdVar7 + 1;
                }
                if (dVar11 < *pdVar6 || dVar11 == *pdVar6) {
                  pdVar6 = pdVar7;
                  if (pdVar7[1] < *pdVar7) {
                    pdVar6 = pdVar7 + 1;
                  }
                  if (*pdVar6 <= dVar10) {
                    return plVar4;
                  }
                }
              }
              puVar9 = puVar9 + 4;
              pdVar7 = pdVar7 + 4;
            } while (puVar9 != puVar2);
          }
        }
      }
    }
    plVar4 = (longlong *)0x0;
  }
  return plVar4;
}
