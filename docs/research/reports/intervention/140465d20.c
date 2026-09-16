
void FUN_140465d20(undefined8 param_1,longlong param_2,longlong param_3,longlong param_4,
                  undefined8 *param_5)

{
  undefined4 *puVar1;
  uint uVar2;
  longlong lVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;
  undefined8 uVar6;
  undefined8 *puVar7;
  longlong lVar8;
  undefined4 *puVar9;
  undefined4 uVar10;
  longlong *local_118;
  longlong local_f8;
  longlong lStack_f0;
  longlong local_e8;
  longlong lStack_e0;
  undefined1 local_d8 [32];
  undefined8 local_b8;
  longlong local_b0;
  longlong local_a8;
  longlong local_a0;
  longlong local_98;
  ulonglong local_90;
  undefined4 local_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined4 uStack_7c;
  int local_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined8 local_68;

  if (param_5 != (undefined8 *)0x0) {
    puVar7 = (undefined8 *)param_5[2];
    while (puVar7 != param_5 + 1) {
      if (puVar7[4] == 0) {
        local_118 = (longlong *)0x0;
      }
      else {
        local_118 = (longlong *)FUN_140486a40();
      }
      lVar8 = 0;
      if ((param_4 != 0) &&
         (lVar3 = FUN_14033f7f0(*(longlong *)(param_2 + 0x380) + 0x180,param_4), lVar3 != 0)) {
        lVar8 = *(longlong *)(lVar3 + 0x10);
      }
      puVar1 = (undefined4 *)puVar7[6];
      puVar9 = (undefined4 *)puVar7[5];
      if (local_118 == (longlong *)0x0) {
        for (; puVar9 != puVar1; puVar9 = puVar9 + 10) {
          FUN_14046f140();
        }
      }
      else if (puVar9 != puVar1) {
        lVar3 = param_3 + 0x23e0;
        do {
          uVar10 = SUB84((double)*(byte *)((longlong)puVar9 + 0x23),0);
          if (*(longlong *)(puVar9 + 2) == *local_118) {
            FUN_140380bb0(lVar3,0,0x1a,uVar10);
            FUN_140380bb0(lVar3,*param_5,0x1a,uVar10);
            FUN_140380bb0(lVar3,*local_118,0x1a,uVar10);
            if (param_4 != 0) {
              FUN_140380bb0(lVar3,param_4,0x1a,uVar10);
            }
            if (lVar8 != 0) {
              uVar6 = 0x1a;
LAB_140466038:
              FUN_140380bb0(lVar3,lVar8,uVar6,uVar10);
            }
          }
          else {
            local_78 = puVar9[4];
            uStack_74 = puVar9[5];
            uStack_70 = puVar9[6];
            uStack_6c = puVar9[7];
            local_88 = *puVar9;
            uStack_84 = puVar9[1];
            uStack_80 = puVar9[2];
            uStack_7c = puVar9[3];
            local_b8._0_4_ = (uint)*(undefined8 *)(puVar9 + 8);
            uVar2 = (uint)local_b8;
            local_90 = (*(longlong *)(param_3 + 0x2108) + 300) * 0x100 | 2;
            local_b8 = CONCAT44((int)((ulonglong)*(undefined8 *)(puVar9 + 8) >> 0x20),
                                (int)*(longlong *)(param_3 + 0x2108) - local_78 & 0xffffffU |
                                (uint)local_b8 & 0xff000000);
            *(uint *)(local_118 + 0xc) = (int)local_118[0xc] + (uVar2 >> 0x18);
            local_b0 = local_118[7];
            local_a8 = local_118[8];
            local_a0 = local_118[9];
            local_98 = local_118[10];
            local_f8 = local_118[3];
            lStack_f0 = local_118[4];
            local_e8 = local_118[5];
            lStack_e0 = local_118[6];
            local_68 = local_b8;
            FUN_14046ef60(local_d8,&local_f8,&local_b0,&local_90);
            FUN_1404716d0(local_118 + 1,local_d8,&local_f8,&local_90);
            uVar10 = SUB84((double)(uVar2 >> 0x18),0);
            FUN_140380bb0(lVar3,0,0x1f,uVar10);
            FUN_140380bb0(lVar3,*param_5,0x1f,uVar10);
            FUN_140380bb0(lVar3,*local_118,0x1f,uVar10);
            if (param_4 != 0) {
              FUN_140380bb0(lVar3,param_4,0x1f,uVar10);
            }
            if (lVar8 != 0) {
              uVar6 = 0x1f;
              goto LAB_140466038;
            }
          }
          puVar9 = puVar9 + 10;
        } while (puVar9 != puVar1);
      }
      puVar4 = (undefined8 *)*puVar7;
      if (puVar4 == (undefined8 *)0x0) {
        puVar4 = (undefined8 *)puVar7[2];
        puVar5 = (undefined8 *)0x0;
        if (puVar7 == (undefined8 *)*puVar4) {
          do {
            puVar7 = puVar4;
            puVar4 = (undefined8 *)puVar7[2];
          } while (puVar7 == (undefined8 *)*puVar4);
          puVar5 = (undefined8 *)*puVar7;
        }
        if (puVar5 != puVar4) {
          puVar7 = puVar4;
        }
      }
      else {
        for (puVar5 = (undefined8 *)puVar4[1]; puVar7 = puVar4, puVar5 != (undefined8 *)0x0;
            puVar5 = (undefined8 *)puVar5[1]) {
          puVar4 = puVar5;
        }
      }
    }
  }
  return;
}


// ref 140c24560 caller none
// ref 140b09b8c caller none
// ref 140b09bd0 caller none
// ref 14044c94f caller 14044c810
// ref 14044e13c caller 14044d6b0
