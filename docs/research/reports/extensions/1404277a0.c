// Candidate VA 1404277a0; RVA 0x4277a0
// Ghidra inferred prototype: undefined FUN_1404277a0()

void FUN_1404277a0(longlong *param_1,longlong param_2,longlong param_3,undefined8 *param_4)

{
  longlong lVar1;
  void *pvVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  longlong *plVar6;
  longlong *plVar7;
  longlong *plVar8;
  longlong *plVar9;
  longlong local_78;
  longlong *local_70;
  longlong local_68;
  undefined8 local_60;
  void *local_58;
  void *pvStack_50;
  undefined8 local_48;
  void *pvStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  longlong local_28;
  undefined4 local_20;

  local_78 = param_2;
  local_70 = param_1;
  FUN_140438e10(param_4,&local_78);
  if ((((((short)((ulonglong)param_3 >> 0x30) == 7) &&
        (lVar5 = FUN_14033f6a0(param_2 + 0x300,param_3), lVar5 != 0)) &&
       (*(char *)(lVar5 + 0xd8) != '\0')) &&
      ((*(longlong *)(lVar5 + 0xe0) == *(longlong *)(lVar5 + 0xe8) &&
       (plVar6 = (longlong *)FUN_1402a2e80(param_1 + 2,param_3), plVar6 != (longlong *)0x0)))) &&
     (*plVar6 == plVar6[1])) {
    plVar9 = (longlong *)0x0;
    plVar8 = (longlong *)plVar6[0x18];
    plVar7 = (longlong *)*plVar8;
    if (plVar7 == plVar8) {
LAB_140427887:
      if (plVar9 != (longlong *)0x0) {
        for (plVar8 = (longlong *)*param_4; plVar8 != (longlong *)param_4[1]; plVar8 = plVar8 + 10)
        {
          if ((*plVar8 == param_3) && (plVar8[8] == plVar9[4])) {
            return;
          }
        }
        lVar1 = plVar9[4];
        lVar5 = FUN_140492140(lVar5,lVar1);
        if (lVar5 != 0) {
          local_60 = 0;
          local_58 = (void *)0x0;
          pvStack_50 = (void *)0x0;
          local_48 = 0;
          pvStack_40 = (void *)0x0;
          local_38 = 0;
          uStack_30 = 0;
          local_20 = 0;
          local_68 = param_3;
          local_28 = lVar1;
          FUN_1404232e0(param_1,plVar6 + 0xe,&local_68,plVar9,lVar5);
          FUN_140422720(param_4,&local_68);
          pvVar2 = local_58;
          pvVar3 = pvStack_50;
          if (pvStack_40 != (void *)0x0) {
            free(pvStack_40);
            pvVar2 = local_58;
            pvVar3 = pvStack_50;
          }
          for (; pvVar4 = pvStack_50, pvVar2 != pvStack_50;
              pvVar2 = (void *)((longlong)pvVar2 + 0x10)) {
            pvStack_50 = pvVar3;
            FUN_1403385b0(pvVar2);
            pvVar3 = pvStack_50;
            pvStack_50 = pvVar4;
          }
          if (local_58 != (void *)0x0) {
            pvStack_50 = pvVar3;
            free(local_58);
          }
        }
      }
    }
    else {
      do {
        plVar9 = plVar7 + 3;
        if (plVar7[0xc] == *(longlong *)(*param_1 + 0x280)) goto LAB_140427887;
        plVar7 = (longlong *)*plVar7;
      } while (plVar7 != plVar8);
    }
  }
  return;
}


// Incoming references
// 0xc22940 DATA caller none
// 0x3ae2e4 UNCONDITIONAL_CALL caller 1403ad950
// 0x3a845c UNCONDITIONAL_CALL caller 1403a81f0
// 0x3b2f60 UNCONDITIONAL_CALL caller 1403b2480
// 0x36f44c UNCONDITIONAL_CALL caller 14036f220
// 0x36d927 UNCONDITIONAL_CALL caller 14036d8d0
// 0x36dad5 UNCONDITIONAL_CALL caller 14036d950
// 0x426d7d UNCONDITIONAL_CALL caller 140426cf0
// 0x426e0d UNCONDITIONAL_CALL caller 140426cf0
// 0x426eb1 UNCONDITIONAL_CALL caller 140426cf0
// 0x426fa9 UNCONDITIONAL_CALL caller 140426cf0
// 0x4270cd UNCONDITIONAL_CALL caller 140426cf0
// 0x42715d UNCONDITIONAL_CALL caller 140426cf0
// 0x4271ed UNCONDITIONAL_CALL caller 140426cf0
