
undefined2 *
FUN_140452480(undefined2 *param_1,undefined8 param_2,undefined8 *param_3,ulonglong *param_4)

{
  undefined8 uVar1;
  undefined1 auVar2 [16];
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  longlong lVar7;
  double *pdVar8;
  undefined8 *puVar9;
  double *pdVar10;
  longlong lVar11;
  double local_res8;
  double local_res18;
  double local_58;
  char local_50;
  longlong local_48 [3];
  longlong local_30;

  *param_1 = 2;
  *(undefined8 *)(param_1 + 4) = 0;
  *(undefined8 *)(param_1 + 8) = 0;
  *(undefined8 *)(param_1 + 0xc) = 0;
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined8 *)(param_1 + 0x14) = 0;
  *(undefined8 *)(param_1 + 0x18) = 0;
  *(undefined8 *)(param_1 + 0x1c) = 0;
  *(undefined8 *)(param_1 + 0x20) = 0;
  *(undefined8 *)(param_1 + 0x24) = 0;
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x2c) = 0;
  *(undefined1 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x34) = 0;
  if ((param_3 != (undefined8 *)0x0) && (param_4 != (ulonglong *)0x0)) {
    lVar11 = param_3[0x1b];
    if ((lVar11 != 0) || (lVar11 = param_3[0x1a], lVar11 != 0)) {
      uVar1 = *param_3;
      *(undefined8 *)(param_1 + 0x2c) = uVar1;
      lVar7 = FUN_14032c420(uVar1,*param_4);
      local_58 = DAT_140aabd08;
      if ((lVar7 != 0) && (0.0 < *(double *)(lVar7 + 0x88))) {
        local_res18 = DAT_140aabd08 / *(double *)(lVar7 + 0x88);
        if (DAT_140aabbf8 <= local_res18) {
          local_res18 = DAT_140aabbf8;
        }
        pdVar10 = &local_res8;
        if (0.0 <= (double)param_4[1] - local_res18) {
          pdVar10 = &local_res18;
        }
        pdVar8 = &local_58;
        if ((double)param_4[1] - local_res18 <= DAT_140aabd08) {
          pdVar8 = pdVar10;
        }
        *(double *)(param_1 + 4) = *pdVar8;
        local_res18 = local_res18 + (double)param_4[1];
        *(undefined1 *)param_1 = 0;
        local_res8 = 0.0;
        pdVar10 = &local_res8;
        if (0.0 <= local_res18) {
          pdVar10 = &local_res18;
        }
        pdVar8 = &local_58;
        if (local_res18 <= local_58) {
          pdVar8 = pdVar10;
        }
        *(double *)(param_1 + 8) = *pdVar8;
        puVar9 = (undefined8 *)FUN_140459ea0(param_3[0xd],local_48,*param_4);
        local_58 = 0.0;
        local_50 = '\0';
        uVar1 = puVar9[1];
        *(undefined8 *)(param_1 + 0x14) = *puVar9;
        *(undefined8 *)(param_1 + 0x18) = uVar1;
        uVar3 = *(undefined4 *)((longlong)puVar9 + 0x14);
        uVar4 = *(undefined4 *)(puVar9 + 3);
        uVar5 = *(undefined4 *)((longlong)puVar9 + 0x1c);
        *(undefined4 *)(param_1 + 0x1c) = *(undefined4 *)(puVar9 + 2);
        *(undefined4 *)(param_1 + 0x1e) = uVar3;
        *(undefined4 *)(param_1 + 0x20) = uVar4;
        *(undefined4 *)(param_1 + 0x22) = uVar5;
        uVar3 = *(undefined4 *)(puVar9 + 4);
        uVar4 = *(undefined4 *)((longlong)puVar9 + 0x24);
        uVar5 = *(undefined4 *)(puVar9 + 5);
        uVar6 = *(undefined4 *)((longlong)puVar9 + 0x2c);
        *(undefined1 *)((longlong)param_1 + 1) = 0;
        *(undefined4 *)(param_1 + 0x24) = uVar3;
        *(undefined4 *)(param_1 + 0x26) = uVar4;
        *(undefined4 *)(param_1 + 0x28) = uVar5;
        *(undefined4 *)(param_1 + 0x2a) = uVar6;
        auVar2._8_8_ = 0;
        auVar2._0_8_ = *param_4;
        FUN_14045f860(lVar11 + 0x880,local_48,param_4,
                      SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar2,8) +
                      SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar2,0),&local_58);
        if (local_50 != '\0') {
          _Mtx_unlock(local_58);
        }
        if (local_48[0] != 0) {
          *(undefined1 *)((longlong)param_1 + 1) = 1;
          *(undefined8 *)(param_1 + 0xc) = *(undefined8 *)(local_30 + 8);
          *(undefined8 *)(param_1 + 0x10) = *(undefined8 *)(local_30 + 0x10);
        }
      }
    }
    return param_1;
  }
  return param_1;
}
