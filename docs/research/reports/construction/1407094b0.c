
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1407094b0(int *param_1,longlong *param_2,longlong param_3,longlong *param_4)

{
  float *pfVar1;
  undefined8 *puVar2;
  char cVar3;
  int iVar4;
  longlong lVar5;
  int *piVar6;
  float *pfVar7;
  longlong *plVar8;
  undefined8 uVar9;
  longlong *plVar10;
  int iVar11;
  int *piVar12;
  longlong lVar13;
  char *pcVar14;
  undefined8 *****pppppuVar15;
  int iVar16;
  int *piVar17;
  float fVar18;
  int extraout_XMM0_Dc;
  float fVar19;
  float fVar20;
  float local_res18 [4];
  int local_c8 [5];
  float local_b4;
  undefined8 ****local_b0;
  longlong lStack_a8;
  longlong local_a0;
  ulonglong uStack_98;
  char local_90;
  char *local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  ulonglong uStack_70;
  int *local_68;
  
  lVar5 = *(longlong *)(param_3 + 600);
  if (lVar5 == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  piVar17 = param_1 + 0x8a;
  local_68 = piVar17;
  FUN_14058e380(piVar17,param_2,param_4);
  FUN_1405beeb0(param_1 + 0x56,param_2,param_4,"##train_sched_list");
  FUN_1405beeb0(param_1,param_2,param_4);
  cVar3 = (**(code **)(*param_4 + 0x170))(param_4);
  if ((((cVar3 != '\0') && (*(longlong *)(param_1 + 0xe) != 0)) &&
      (*(longlong *)(*(longlong *)(param_1 + 8) + 0x20) != 0)) &&
     (lVar5 = FUN_14033f780(*(longlong *)(lVar5 + 0x890) + 0x200), lVar5 != 0)) {
    lVar5 = *(longlong *)(*(longlong *)(param_1 + 0xbe) + 0x50);
    iVar16 = (int)(*(longlong *)(*(longlong *)(param_1 + 0xbe) + 0x58) - lVar5 >> 3);
    if ((extraout_XMM0_Dc < iVar16) && ((char)param_1[0x94] != '\0')) {
      local_c8[2] = extraout_XMM0_Dc;
      local_c8[4] = iVar16;
      local_c8[0] = 0;
      piVar12 = local_c8;
      if (-1 < extraout_XMM0_Dc) {
        piVar12 = local_c8 + 2;
      }
      piVar6 = local_c8 + 4;
      if (extraout_XMM0_Dc <= iVar16) {
        piVar6 = piVar12;
      }
      iVar16 = (*(int *)(lVar5 + (longlong)*piVar6 * 8) + param_1[0xc0]) % 0x15180;
      local_res18[0] = 0.0;
      pfVar1 = (float *)(param_1 + 0xc1);
      if (iVar16 < 0) {
        iVar16 = iVar16 + 0x15180;
      }
      local_b4 = (float)iVar16 * _DAT_140aab61c * *pfVar1 - DAT_140aac3c0;
      pfVar7 = local_res18;
      if (0.0 <= local_b4) {
        pfVar7 = &local_b4;
      }
      if (*pfVar1 <= local_b4 && local_b4 != *pfVar1) {
        pfVar7 = pfVar1;
      }
      fVar20 = *(float *)(*param_2 + 0x1550);
      if (fVar20 < 0.0) {
        fVar20 = DAT_140aabae4;
      }
      fVar18 = *pfVar7;
      FUN_14050a410(*param_2 + 0x16f8,"##train_sched_list",0);
      local_b0 = (undefined8 *****)0x0;
      lStack_a8 = 0;
      local_a0 = (ulonglong)(uint)(fVar20 * fVar18) << 0x20;
      lVar5 = *param_2;
      local_88 = (char *)0x0;
      uStack_80 = 0;
      local_78 = 0;
      uStack_70 = 0;
      local_88 = (char *)FUN_140003270(0x20);
      uVar9 = s___train_sched_list_140a9a2c0._8_8_;
      local_78 = _DAT_140aaca40;
      uStack_70 = _UNK_140aaca48;
      *(undefined8 *)local_88 = s___train_sched_list_140a9a2c0._0_8_;
      *(undefined8 *)(local_88 + 8) = uVar9;
      *(undefined2 *)(local_88 + 0x10) = s___train_sched_list_140a9a2c0._16_2_;
      local_88[0x12] = '\0';
      FUN_14081dbf0(lVar5,&local_88);
      if (0xf < uStack_70) {
        pcVar14 = local_88;
        if ((0xfff < uStack_70 + 1) &&
           (pcVar14 = *(char **)(local_88 + -8), (char *)0x1f < local_88 + (-8 - (longlong)pcVar14))
           ) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pcVar14);
      }
    }
  }
  fVar20 = local_res18[0];
  local_res18[0] = (float)((uint)local_res18[0] & 0xffffff00);
  fVar19 = DAT_140aac530 / (DAT_140aac3d0 / (float)(1 << ((byte)param_1[0x93] & 0x1f))) +
           DAT_140aac268;
  fVar18 = (float)*(uint *)((longlong)param_2 + 0xa4) - DAT_140aac410;
  *(undefined1 *)(param_4 + 3) = 1;
  if (fVar18 <= fVar19) {
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0x1e0;
  }
  else {
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
    *(float *)((longlong)param_4 + 0x34) = fVar19;
    local_res18[0]._1_3_ = SUB43(fVar20,1);
    if (fVar19 == 0.0) {
      *(undefined1 *)(param_4 + 6) = 0;
      local_res18[0] = (float)CONCAT31(local_res18[0]._1_3_,1);
    }
    else {
      *(undefined1 *)(param_4 + 6) = 1;
      local_res18[0] = (float)CONCAT31(local_res18[0]._1_3_,1);
    }
  }
  *(undefined1 *)(param_4 + 7) = 1;
  *(undefined4 *)((longlong)param_4 + 0x3c) = 0;
  param_4[8] = 0x40800000;
  *(undefined4 *)(param_4 + 9) = 0;
  plVar8 = (longlong *)(**(code **)(*param_4 + 0x30))(param_4,"##train_sched_list",0x800);
  fVar20 = (float)param_1[0xc1];
  fVar18 = (float)param_1[0xc2];
  *(float *)((longlong)plVar8 + 0x2c) = fVar18;
  *(bool *)(plVar8 + 5) = fVar18 != 0.0;
  *(float *)((longlong)plVar8 + 0x34) = fVar20;
  *(bool *)(plVar8 + 6) = fVar20 != 0.0;
  *(undefined1 *)(plVar8 + 4) = 1;
  *(undefined4 *)((longlong)plVar8 + 0x24) = 2;
  (**(code **)(*plVar8 + 8))(plVar8);
  fVar20 = (float)param_1[0xc1];
  *(undefined4 *)((longlong)plVar8 + 0x2c) = 0x3f800000;
  *(undefined1 *)(plVar8 + 5) = 1;
  *(float *)((longlong)plVar8 + 0x34) = fVar20;
  *(bool *)(plVar8 + 6) = fVar20 != 0.0;
  (**(code **)(*plVar8 + 0xa8))(plVar8,&DAT_140a4d470,0x11);
  FUN_1405bf210(param_1 + 0x56,param_2,plVar8);
  fVar20 = (float)param_1[0xc1];
  fVar18 = (float)param_1[0xc2];
  *(float *)((longlong)plVar8 + 0x2c) = fVar18;
  *(bool *)(plVar8 + 5) = fVar18 != 0.0;
  *(float *)((longlong)plVar8 + 0x34) = fVar20;
  *(bool *)(plVar8 + 6) = fVar20 != 0.0;
  *(undefined1 *)(plVar8 + 4) = 1;
  *(undefined4 *)((longlong)plVar8 + 0x24) = 2;
  (**(code **)(*plVar8 + 8))(plVar8);
  *(undefined4 *)((longlong)plVar8 + 0x2c) = 0x42600000;
  *(undefined1 *)(plVar8 + 5) = 1;
  (**(code **)(*plVar8 + 200))(plVar8);
  iVar16 = 0;
  if (0 < param_1[0x8e]) {
    do {
      fVar20 = (float)param_1[0xc1];
      fVar18 = (float)param_1[0x8f];
      *(float *)((longlong)plVar8 + 0x2c) = fVar18;
      *(bool *)(plVar8 + 5) = fVar18 != 0.0;
      *(float *)((longlong)plVar8 + 0x34) = fVar20;
      *(bool *)(plVar8 + 6) = fVar20 != 0.0;
      *(undefined1 *)(plVar8 + 7) = 1;
      *(undefined8 *)((longlong)plVar8 + 0x3c) = 0x3f800000;
      *(undefined8 *)((longlong)plVar8 + 0x44) = 0x3f800000;
      (**(code **)(*plVar8 + 0xa8))(plVar8,&DAT_140a4d470,0x11);
      iVar11 = (param_1[0x8d] + iVar16) % 7;
      if (iVar11 < 0) {
        iVar11 = iVar11 + 7;
      }
      *param_1 = iVar11 * 0x15180 - param_1[0xc0];
      FUN_1405be950(param_1,param_2);
      lVar5 = *(longlong *)(param_1 + 0x3a);
      if (((lVar5 == 0) || ((char)param_1[0x16] == '\0')) ||
         (plVar10 = *(longlong **)(param_1 + 0x40), plVar10 == (longlong *)0x0)) {
        local_90 = '\0';
      }
      else {
        iVar11 = param_1[0x14];
        uVar9 = 0;
        if (iVar11 < (int)(*(longlong *)(lVar5 + 0x58) - *(longlong *)(lVar5 + 0x50) >> 3)) {
          iVar4 = iVar11;
          if (iVar11 < 0) {
            iVar4 = 0;
          }
          lVar13 = (longlong)*(int *)(*(longlong *)(lVar5 + 0x50) + 4 + (longlong)iVar4 * 8) * 0x20;
          puVar2 = (undefined8 *)(lVar13 + 0x10 + *(longlong *)(lVar5 + 0x68));
          local_78 = *puVar2;
          uStack_70 = puVar2[1];
          uVar9 = 0;
          if (*(longlong *)(lVar13 + 8 + *(longlong *)(lVar5 + 0x68)) != 0) {
            uVar9 = FUN_14033f7f0(*plVar10 + 0x180);
          }
        }
        plVar10 = (longlong *)
                  FUN_1405907a0(&local_88,plVar10,uVar9,*(undefined8 *)(param_1 + 0x38),lVar5,
                                param_1[0x42],iVar11,0xffffffff);
        local_b0 = (undefined8 ****)*plVar10;
        lStack_a8 = plVar10[1];
        local_a0 = plVar10[2];
        uStack_98 = plVar10[3];
        *(undefined1 *)plVar10 = 0;
        plVar10[2] = 0;
        plVar10[3] = 0xf;
        local_90 = '\x01';
        if (0xf < uStack_70) {
          pcVar14 = local_88;
          if ((0xfff < uStack_70 + 1) &&
             (pcVar14 = *(char **)(local_88 + -8),
             (char *)0x1f < local_88 + (-8 - (longlong)pcVar14))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pcVar14);
        }
      }
      if (local_90 != '\0') {
        pppppuVar15 = &local_b0;
        if (0xf < uStack_98) {
          pppppuVar15 = (undefined8 *****)local_b0;
        }
        (**(code **)(*plVar8 + 0xc0))(plVar8,pppppuVar15);
      }
      if ((local_90 != '\0') && (0xf < uStack_98)) {
        pppppuVar15 = (undefined8 *****)local_b0;
        if ((0xfff < uStack_98 + 1) &&
           (pppppuVar15 = (undefined8 *****)local_b0[-1],
           0x1f < (ulonglong)((longlong)local_b0 + (-8 - (longlong)pppppuVar15)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppuVar15);
      }
      if ((char)param_1[0x1f] != '\0') {
        *(undefined8 *)(param_1 + 0xa0) = *(undefined8 *)(param_1 + 0x1e);
      }
      iVar16 = iVar16 + 1;
      piVar17 = local_68;
    } while (iVar16 < param_1[0x8e]);
  }
  (**(code **)(*plVar8 + 0x18))(plVar8);
  (**(code **)(*plVar8 + 0x18))(plVar8);
  (**(code **)(*param_4 + 0x38))(param_4);
  if (local_res18[0]._0_1_ != '\0') {
    *(undefined1 *)(param_4 + 3) = 1;
    *(undefined4 *)((longlong)param_4 + 0x1c) = 0x1e0;
    (**(code **)(*param_4 + 200))(param_4);
  }
  *(undefined4 *)((longlong)param_4 + 0x34) = 0x40000000;
  *(undefined1 *)(param_4 + 6) = 1;
  (**(code **)(*param_4 + 200))(param_4);
  FUN_1406aa060(piVar17,param_2,param_4);
  return;
}

