
undefined8 FUN_140443200(longlong param_1,undefined8 *param_2,undefined8 *param_3,longlong param_4)

{
  byte *pbVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  char cVar5;
  int iVar6;
  longlong lVar7;
  undefined8 uVar8;
  undefined8 *puVar9;
  longlong *plVar10;
  longlong *plVar11;
  int iVar12;
  longlong *plVar13;
  longlong lVar14;
  longlong lVar15;
  longlong lVar16;
  longlong lVar17;
  double dVar18;
  undefined4 uVar19;
  undefined8 in_stack_fffffffffffffec8;
  undefined4 uVar20;
  undefined8 local_128;
  undefined8 uStack_120;
  undefined8 local_118;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined1 uStack_100;
  undefined2 uStack_ff;
  undefined1 uStack_fd;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  undefined1 uStack_f4;
  undefined2 uStack_f3;
  undefined1 uStack_f1;
  undefined8 uStack_f0;
  int local_e8;
  undefined8 *local_e0;
  undefined8 local_d8;
  undefined8 local_d0;
  longlong local_c8;
  longlong local_c0;
  void *local_b8 [4];
  void *local_98;

  uVar20 = (undefined4)((ulonglong)in_stack_fffffffffffffec8 >> 0x20);
  if ((((param_2 == (undefined8 *)0x0) || (param_3 == (undefined8 *)0x0)) ||
      (*(char *)(param_3 + 0x3a) == '\0')) ||
     ((*(char *)(param_3 + 0xc1) == '\0' || (*(char *)(param_3 + 0xba) == '\0')))) {
    param_3[0x15] = 1;
    if (*(char *)(param_3 + 0x16) == '\0') {
      *(undefined1 *)(param_3 + 0x16) = 1;
    }
  }
  else {
    local_c0 = param_3[0xc0];
    uVar19 = *(undefined4 *)(param_3 + 0xbf);
    *(undefined1 *)(param_3 + 0xc1) = 0;
    if (((param_3[0x17] == 0) ||
        (lVar7 = FUN_14032c420(**(undefined8 **)(param_1 + 0x28)), lVar7 == 0)) ||
       ((param_3[0xb5] == 0 ||
        (local_e0 = (undefined8 *)FUN_14033f7f0(**(longlong **)(param_1 + 0x30) + 0x180),
        local_e0 == (undefined8 *)0x0)))) {
      param_3[0x15] = 1;
      if (*(char *)(param_3 + 0x16) == '\0') {
        *(undefined1 *)(param_3 + 0x16) = 1;
        return 0;
      }
    }
    else {
      local_e8 = *(int *)(param_3 + 0xb9);
      lVar14 = local_e0[0x23];
      lVar17 = 0;
      local_128 = param_3[0xb4];
      uStack_120 = param_3[0xb5];
      lVar15 = param_3[0xb8];
      local_118 = param_3[0xb6];
      uStack_110 = param_3[0xb7];
      iVar12 = (int)(local_e0[0x24] - lVar14 >> 3) * 0x2fa0be83;
      iVar6 = 0;
      if (0 < local_e8) {
        iVar6 = local_e8;
      }
      if (0 < iVar12) {
        lVar17 = (longlong)((iVar12 + iVar6 % iVar12) % iVar12) * 0x158 + lVar14;
      }
      if (*(char *)(param_3 + 0xbe) == '\0') {
        local_d8 = 0;
        local_d0 = 0;
      }
      else {
        local_d8 = param_3[0xbb];
        local_d0 = param_3[0xbc];
      }
      if (lVar17 != 0) {
        iVar6 = FUN_140355610(&local_128,local_e0 + 0x23,local_e8);
        local_c8 = iVar6 + lVar15;
        lVar15 = *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x2108) + 10;
        lVar14 = local_c8;
        if (local_c8 < lVar15) {
          lVar14 = lVar15;
        }
        if (((*(longlong *)(lVar17 + 0x78) == 0) || (lVar15 = FUN_14032c420(), lVar15 == 0)) ||
           (lVar16 = *(longlong *)(lVar15 + 0xd0), *(longlong *)(lVar15 + 0xd0) == 0)) {
          lVar16 = 0;
          if (*(longlong *)(lVar7 + 0xd0) != 0) {
            lVar16 = *(longlong *)(lVar7 + 0xd0);
          }
        }
        dVar18 = (double)FUN_140442ba0(param_1,*param_3,*local_e0,lVar16,CONCAT44(uVar20,uVar19));
        puVar2 = param_3 + 0x17;
        if (*(int *)((longlong)local_e0 + 0xfc) == 1) {
          if (*(int *)(local_e0 + 0x20) == 1) {
            if (*(longlong *)(lVar17 + 0xf0) == *(longlong *)(lVar17 + 0xf8)) {
              cVar5 = '\x01';
            }
            else {
              local_128 = *puVar2;
              uStack_120 = param_3[0x18];
              local_118._0_1_ = (char)param_3[0x19];
              local_118 = CONCAT71((int7)((ulonglong)param_3[0x19] >> 8),-(char)local_118);
              uVar8 = FUN_140377010(local_b8,*(undefined8 *)(param_1 + 0x28),&local_128,
                                    (double)*(float *)(param_3 + 8));
              cVar5 = FUN_140385760(lVar17 + 8,uVar8);
              if ((local_b8[0] != (void *)0x0) && (local_b8[0] != local_98)) {
                free(local_b8[0]);
              }
            }
          }
          else {
            cVar5 = '\0';
          }
        }
        else {
          cVar5 = '\0';
        }
        if (0 < *(int *)(lVar17 + 0xc0) + *(int *)(lVar17 + 0xe0)) {
          lVar7 = *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x20);
          param_3[0x97] = 0;
          param_3[0x98] = 0;
          local_118 = 0;
          param_3[0x99] = 0;
          if (*(char *)(param_3 + 0x9a) == '\0') {
            *(undefined1 *)(param_3 + 0x9a) = 1;
          }
          param_3[0x97] = local_c8;
          param_3[0x98] = (lVar14 - lVar7) * 1000000;
          param_3[0x1a] = 0;
        }
        iVar6 = 0;
        if (*(char *)(param_3 + 0x9a) == '\0') {
          if (DAT_140aabd08 <= dVar18) {
            uStack_fc = 0;
            uStack_f8 = 0;
            local_128 = 0;
            uStack_120 = 0;
            local_118 = 0;
            uStack_110 = 0;
            uStack_ff = 0;
            param_3[0xa3] = 0;
            param_3[0xa4] = 0;
            uStack_fd = 0;
            param_3[0xa5] = 0;
            param_3[0xa6] = 0;
            local_108 = 0;
            uStack_100 = 0;
            uStack_f3 = 0;
            uStack_f1 = 0;
            uStack_f0 = 0;
            uStack_f4 = 0;
            param_3[0xa7] = 0;
            param_3[0xa8] = 0;
            param_3[0xa9] = 0;
            param_3[0xaa] = 0;
            if (*(char *)(param_3 + 0xab) == '\0') {
              *(undefined1 *)(param_3 + 0xab) = 1;
            }
            lVar7 = *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x28);
            *(float *)(param_3 + 0xaa) = (float)dVar18;
            *(undefined1 *)((longlong)param_3 + 0x54c) = 1;
            param_3[0xa3] = lVar7 * 10000;
            *(undefined4 *)((longlong)param_3 + 0x534) = 0;
            *(undefined1 *)(param_3 + 0xa8) = 1;
          }
          FUN_140442530(param_1,local_e0,lVar17,param_2,param_3,0);
          FUN_140444030(param_1);
        }
        else {
          param_3[0x9f] = 0;
          param_3[0xa0] = 0;
          param_3[0xa1] = 0;
          if (*(char *)(param_3 + 0xa2) == '\0') {
            *(undefined1 *)(param_3 + 0xa2) = 1;
          }
          if (0 < *(int *)(lVar17 + 0xc0) + *(int *)(lVar17 + 0xe0)) {
            param_3[0xa0] = *local_e0;
            *(int *)(param_3 + 0xa1) = local_e8;
          }
          param_3[0x9f] = lVar16;
          *(float *)((longlong)param_3 + 0x50c) = (float)dVar18;
          if (*(char *)(param_3 + 0xbe) != '\0') {
            param_3[0x9b] = 0;
            param_3[0x9c] = 0;
            local_118 = 0;
            param_3[0x9d] = 0;
            if (*(char *)(param_3 + 0x9e) == '\0') {
              *(undefined1 *)(param_3 + 0x9e) = 1;
            }
            param_3[0x9b] = local_d8;
            param_3[0x9c] = local_d0;
            *(undefined4 *)(param_3 + 0x9d) = *(undefined4 *)(param_3 + 0xbd);
          }
          if ((lVar16 != 0) &&
             (puVar9 = (undefined8 *)FUN_14032c3b0(**(longlong **)(param_1 + 0x28) + 0x80,lVar16),
             puVar9 != (undefined8 *)0x0)) {
            uStack_fc = 0;
            uStack_f8 = 0;
            local_128 = 0;
            uStack_120 = 0;
            local_118 = 0;
            uStack_110 = 0;
            uStack_ff = 0;
            param_3[0xa3] = 0;
            param_3[0xa4] = 0;
            uStack_fd = 0;
            param_3[0xa5] = 0;
            param_3[0xa6] = 0;
            local_108 = 0;
            uStack_100 = 0;
            uStack_f3 = 0;
            uStack_f1 = 0;
            uStack_f0 = 0;
            uStack_f4 = 0;
            param_3[0xa7] = 0;
            param_3[0xa8] = 0;
            *(undefined4 *)(param_3 + 0xa9) = 0;
            *(undefined4 *)((longlong)param_3 + 0x54c) = 0;
            *(undefined4 *)(param_3 + 0xaa) = 0;
            *(undefined4 *)((longlong)param_3 + 0x554) = 0;
            if (*(char *)(param_3 + 0xab) == '\0') {
              *(undefined1 *)(param_3 + 0xab) = 1;
            }
            lVar7 = param_3[0x9f];
            param_3[0xa3] = *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x28) * 10000;
            *(undefined4 *)((longlong)param_3 + 0x534) = *(undefined4 *)(param_4 + 0x38);
            plVar10 = *(longlong **)(param_4 + 0x18);
            plVar13 = (longlong *)(param_4 + 8);
            if (plVar10 != (longlong *)0x0) {
              do {
                plVar11 = plVar10;
                if (lVar7 <= plVar10[4]) {
                  plVar11 = plVar10 + 1;
                  plVar13 = plVar10;
                }
                plVar10 = (longlong *)*plVar11;
              } while (plVar10 != (longlong *)0x0);
              if ((plVar13 != (longlong *)(param_4 + 8)) && (plVar13[4] <= lVar7)) {
                lVar14 = plVar13[5];
                if (lVar14 != plVar13[6]) {
                  do {
                    pbVar1 = (byte *)(lVar14 + 0x23);
                    lVar14 = lVar14 + 0x28;
                    iVar6 = iVar6 + (uint)*pbVar1;
                  } while (lVar14 != plVar13[6]);
                  if (iVar6 != 0) {
                    *(int *)((longlong)param_3 + 0x534) = iVar6;
                  }
                }
              }
            }
            *(float *)(param_3 + 0xaa) =
                 *(float *)((longlong)param_3 + 0x50c) + *(float *)(param_3 + 0xaa);
            if ((lVar7 != 0) &&
               (lVar7 = FUN_140486a40(*(undefined8 *)(param_1 + 0x40),lVar7), lVar7 != 0)) {
              *(undefined4 *)((longlong)param_3 + 0x544) = *(undefined4 *)(lVar7 + 0x98);
            }
            if ((10000000 < local_c0) && (0 < *(int *)(lVar17 + 0xc0) + *(int *)(lVar17 + 0xe0))) {
              dVar18 = (double)local_c0 * DAT_140aab938;
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*puVar9,0x14,dVar18);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,0x14,dVar18);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),param_3[0xb5],0x14,dVar18);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),0,0x14,dVar18);
            }
            dVar18 = DAT_140aabd08;
            if (((param_4 != 0) && (0 < *(int *)(param_3 + 9))) &&
               (*(int *)(param_3 + 9) <= *(int *)(param_4 + 0x38))) {
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*puVar9,0x2b,DAT_140aabd08);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,0x2b,dVar18);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),param_3[0xb5],0x2b,dVar18);
              FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),0,0x2b,dVar18);
            }
          }
          FUN_140442530(param_1,local_e0,lVar17,param_2,param_3,0);
          uVar8 = uStack_110;
          if (cVar5 != '\0') {
            uVar3 = param_3[0x19];
            uStack_110 = CONCAT71(uStack_110._1_7_,*(undefined1 *)(param_3 + 0x1b));
            uVar4 = uStack_110;
            local_118._0_4_ = (undefined4)uVar3;
            local_118._4_4_ = (undefined4)((ulonglong)uVar3 >> 0x20);
            uStack_110._4_4_ = SUB84(uVar8,4);
            *(undefined4 *)(param_3 + 0x3f) = *(undefined4 *)puVar2;
            *(undefined4 *)((longlong)param_3 + 0x1fc) = *(undefined4 *)((longlong)param_3 + 0xbc);
            *(undefined4 *)(param_3 + 0x40) = *(undefined4 *)(param_3 + 0x18);
            *(undefined4 *)((longlong)param_3 + 0x204) = *(undefined4 *)((longlong)param_3 + 0xc4);
            *(undefined4 *)(param_3 + 0x41) = (undefined4)local_118;
            *(undefined4 *)((longlong)param_3 + 0x20c) = local_118._4_4_;
            *(undefined4 *)(param_3 + 0x42) = (undefined4)uStack_110;
            *(undefined4 *)((longlong)param_3 + 0x214) = uStack_110._4_4_;
            if (*(char *)(param_3 + 0x43) == '\0') {
              *(undefined1 *)(param_3 + 0x43) = 1;
            }
            local_118 = uVar3;
            uStack_110 = uVar4;
            FUN_14045f560(param_3 + 0x52);
            FUN_14045a6e0(puVar2);
            return 0;
          }
        }
        return 1;
      }
      param_3[0x15] = 1;
      if (*(char *)(param_3 + 0x16) == '\0') {
        *(undefined1 *)(param_3 + 0x16) = 1;
        return 0;
      }
    }
  }
  return 0;
}
