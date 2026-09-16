
undefined8 *
FUN_14044cc70(undefined8 param_1,undefined8 *param_2,longlong param_3,undefined8 param_4,
             char param_5)

{
  longlong *plVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  longlong lVar5;
  undefined1 *puVar6;
  longlong *plVar7;
  ulonglong uVar8;
  longlong *plVar9;
  undefined4 extraout_XMM0_Da;
  undefined1 local_70 [8];
  undefined8 local_68;
  undefined1 local_60 [16];
  undefined1 local_50 [24];

  *param_2 = 0;
  param_2[1] = 0;
  param_2[2] = 0;
  param_2[3] = 0;
  *param_2 = param_2;
  param_2[1] = param_2;
  param_2[2] = 0;
  *(undefined1 *)(param_2 + 3) = 0;
  param_2[4] = 0;
  plVar1 = *(longlong **)(param_3 + 0x220);
  plVar9 = *(longlong **)(param_3 + 0x218);
  do {
    if (plVar9 == plVar1) {
      return param_2;
    }
    plVar7 = (longlong *)*plVar9;
    if ((plVar7 != (longlong *)0x0) && (*(uint *)(param_3 + 0x208) != 0)) {
      uVar8 = (ulonglong)*(uint *)(param_3 + 0x208);
      do {
        sVar3 = (short)((ulonglong)*plVar7 >> 0x30);
        if (((sVar3 != -1) && (sVar3 != 0)) && ((int)plVar7[0x17] != 2)) {
          if (param_5 == '\0') {
            puVar6 = local_50;
LAB_14044cd96:
            FUN_140329d50(param_2,puVar6);
          }
          else if ((*plVar7 != 0) && (lVar5 = FUN_14045f300(param_1), lVar5 != 0)) {
            FUN_14043f150(local_70,param_3 + 0x890,lVar5);
            iVar4 = FUN_14043e130(lVar5);
            cVar2 = FUN_14043e1f0(extraout_XMM0_Da,local_68);
            if ((cVar2 != '\0') || (iVar4 - 3U < 2)) {
              puVar6 = local_60;
              goto LAB_14044cd96;
            }
          }
        }
        plVar7 = plVar7 + 0x2f;
        uVar8 = uVar8 - 1;
      } while (uVar8 != 0);
    }
    plVar9 = plVar9 + 1;
  } while( true );
}


// ref 140c2375c caller none
// ref 140303677 caller 140303620
