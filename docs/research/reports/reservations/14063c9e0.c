// Candidate VA 14063c9e0; RVA 0x63c9e0
// Ghidra inferred prototype: undefined FUN_14063c9e0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14063c9e0(longlong param_1,longlong *param_2,undefined8 ******param_3,ulonglong param_4,
                  longlong *param_5,undefined8 param_6,undefined8 param_7,undefined8 param_8,
                  longlong *param_9,longlong param_10)

{
  void *pvVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  undefined8 uVar6;
  ulonglong uVar7;
  float *pfVar8;
  longlong lVar9;
  longlong lVar10;
  double *pdVar11;
  undefined8 ******ppppppuVar12;
  char cVar13;
  float *pfVar14;
  undefined1 *_Memory;
  undefined8 *****_Memory_00;
  uint uVar15;
  float *pfVar16;
  longlong lVar17;
  longlong *plVar18;
  longlong *plVar19;
  longlong lVar20;
  longlong *plVar21;
  float *pfVar22;
  double dVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  ulonglong local_res20;
  undefined8 in_stack_fffffffffffff768;
  undefined4 uVar28;
  undefined8 uVar27;
  undefined4 uVar29;
  undefined8 *****local_868;
  undefined8 uStack_860;
  float **local_858;
  longlong *plStack_850;
  undefined2 local_848 [2];
  undefined4 local_844;
  undefined4 local_840;
  undefined4 local_83c;
  undefined8 local_838;
  undefined8 *****local_830;
  undefined8 uStack_828;
  float **local_820;
  longlong *plStack_818;
  undefined1 *local_810;
  longlong local_808;
  ulonglong *local_800;
  float local_7f8;
  float local_7f4;
  float local_7f0;
  float local_7ec;
  longlong *local_7e8;
  undefined8 uStack_7e0;
  undefined8 local_7d8;
  undefined8 uStack_7d0;
  longlong *local_7c8;
  undefined8 uStack_7c0;
  longlong *local_7b8;
  longlong lStack_7b0;
  undefined4 local_7a8;
  undefined4 uStack_7a4;
  undefined8 uStack_7a0;
  longlong *local_798;
  longlong lStack_790;
  float *local_788;
  float *pfStack_780;
  undefined8 local_778;
  longlong *local_768;
  longlong lStack_760;
  undefined8 local_758;
  longlong **pplStack_750;
  longlong *local_748;
  longlong lStack_740;
  undefined1 local_738 [16];
  undefined **local_728;
  undefined1 *local_720;
  undefined8 local_718;
  ulonglong local_710;
  undefined1 local_708 [512];
  undefined **local_508;
  undefined1 *local_500;
  undefined8 local_4f8;
  ulonglong local_4f0;
  undefined1 local_4e8 [512];
  undefined1 local_2e8 [40];
  void *local_2c0;
  void *local_2b8;
  undefined1 local_2a8 [8];
  void *local_2a0;
  ulonglong local_298;
  undefined1 local_278 [404];
  undefined4 local_e4;
  undefined2 local_e0;
  
  lVar17 = 0;
  lVar20 = *param_2 + 0x970;
  local_res20 = param_4;
  FUN_140556590(lVar20,&local_7c8,param_1 + 0x2b8);
  FUN_140556590(lVar20,&local_7d8,param_1 + 0x2e0);
  FUN_140556590(lVar20,&local_7e8,param_1 + 0x308);
  uStack_7a0 = (longlong **)CONCAT44(uStack_7a0._4_4_,(undefined4)uStack_7a0);
  if (((uStack_7c0._4_1_ != '\0') &&
      (uStack_7a0 = (longlong **)CONCAT44(uStack_7a0._4_4_,(undefined4)uStack_7a0),
      uStack_7d0._4_1_ != '\0')) &&
     (uStack_7a0 = (longlong **)CONCAT44(uStack_7a0._4_4_,(undefined4)uStack_7a0),
     uStack_7e0._4_1_ != '\0')) {
    FUN_14056de90(local_2e8,param_2 + 0x23);
    local_e0 = 8;
    local_e4 = 3;
    dVar23 = (double)FUN_140249580(param_1 + 0x4a0);
    if (DAT_140aabd08 < dVar23) {
      FUN_140241680(param_1 + 0x4a0);
    }
    dVar23 = (double)FUN_140249580(param_1 + 0x4a0);
    uVar28 = (undefined4)((ulonglong)in_stack_fffffffffffff768 >> 0x20);
    uVar15 = (int)(dVar23 * DAT_140aabe08) & 0x80000003;
    if ((int)uVar15 < 0) {
      uVar15 = (uVar15 - 1 | 0xfffffffc) + 1;
    }
    cVar13 = (char)uVar15;
    if ((int)uVar15 < 0) {
      cVar13 = (char)uVar15 + '\x04';
    }
    local_res20 = CONCAT71(local_res20._1_7_,cVar13) & 0xffffffffffffff01;
    local_788 = (float *)0x0;
    pfStack_780 = (float *)0x0;
    local_778 = 0;
    local_820 = &local_788;
    local_810 = local_2e8;
    local_800 = &local_res20;
    plVar19 = *(longlong **)(param_10 + 8);
    plVar18 = (longlong *)*plVar19;
    plVar21 = plVar19;
    if (plVar18 == (longlong *)0x0) {
      plVar21 = plVar19 + 1;
      plVar18 = (longlong *)*plVar21;
      while (plVar18 == (longlong *)0x0) {
        plVar21 = plVar21 + 1;
        plVar18 = (longlong *)*plVar21;
      }
    }
    plVar19 = (longlong *)plVar19[*(longlong *)(param_10 + 0x10)];
    local_830 = param_3;
    local_808 = param_1;
    uStack_828 = param_7;
    plStack_818 = param_5;
    while (plVar18 != plVar19) {
      if (((*plVar18 != 0) && (lVar20 = FUN_14045f300(param_8), lVar20 != 0)) &&
         ((*(char *)(lVar20 + 0x1d0) != '\0' &&
          ((*(char *)(lVar20 + 0x4b0) != '\0' && (*(longlong *)(lVar20 + 0x458) != 0)))))) {
        lVar20 = *param_5;
        lVar9 = FUN_14033f630(lVar20 + 0x380);
        if (lVar9 != 0) {
          lVar10 = lVar17;
          if (*(longlong *)(lVar9 + 0x40) != 0) {
            lVar10 = FUN_14032c420(lVar20);
          }
          FUN_14063d390(&local_830,lVar10,lVar9);
        }
      }
      uVar28 = (undefined4)((ulonglong)in_stack_fffffffffffff768 >> 0x20);
      plVar18 = (longlong *)plVar18[1];
      while (plVar18 == (longlong *)0x0) {
        plVar21 = plVar21 + 1;
        plVar18 = (longlong *)*plVar21;
      }
    }
    FUN_14056e900(local_2e8,param_2,*param_2 + 0x970);
    fVar2 = DAT_140aabae4;
    lVar20 = *param_2;
    plVar21 = *(longlong **)(lVar20 + 0x1580);
    local_848[0] = *(undefined2 *)(lVar20 + 0x15b8);
    local_844 = 0x41600000;
    local_840 = 0x12;
    local_83c = 0xff000000;
    local_838 = 0;
    fVar24 = *(float *)(lVar20 + 0x1550);
    if (*(float *)(lVar20 + 0x1550) < 0.0) {
      fVar24 = DAT_140aabae4;
    }
    uVar27 = CONCAT44(uVar28,fVar24 * DAT_140aabb98);
    FUN_1401b2200(plVar21,0x17,(short)param_2[0x14],*(undefined2 *)((longlong)param_2 + 0xa4),uVar27
                 );
    pfVar8 = pfStack_780;
    uVar7 = _UNK_140aac918;
    uVar6 = _DAT_140aac910;
    uVar28 = DAT_140aac424;
    fVar5 = DAT_140aac0a0;
    fVar4 = DAT_140aabf18;
    fVar3 = DAT_140aabd78;
    fVar24 = DAT_140aab9dc;
    if (local_788 != pfStack_780) {
      pfVar22 = local_788 + 5;
      do {
        lVar20 = lVar17;
        if (*(longlong *)(pfVar22 + -3) != 0) {
          lVar20 = FUN_14032c420(*param_5);
        }
        lVar9 = lVar17;
        if (*(longlong *)(pfVar22 + -5) != 0) {
          lVar9 = FUN_14033f780(*param_9 + 0x200);
        }
        uVar29 = (undefined4)((ulonglong)uVar27 >> 0x20);
        if ((lVar20 != 0) && (lVar9 != 0)) {
          pfVar16 = pfVar22 + -1;
          pfVar14 = pfVar16;
          if (*pfVar22 < *pfVar16) {
            pfVar14 = pfVar22;
          }
          if (*pfVar16 < *pfVar22) {
            pfVar16 = pfVar22;
          }
          pdVar11 = (double *)
                    FUN_1403898d0(lVar20,local_738,
                                  (double)((*pfVar16 - *pfVar14) * fVar24 + *pfVar14),0);
          fVar25 = (float)(pdVar11[1] * (double)param_2[0x2e] + *pdVar11 * (double)param_2[0x2b] +
                          (double)param_2[0x31]);
          fVar26 = (float)(pdVar11[1] * (double)param_2[0x2f] + *pdVar11 * (double)param_2[0x2c] +
                          (double)param_2[0x32]);
          uStack_860 = 0;
          local_858 = (float **)uVar6;
          plStack_850 = (longlong *)uVar7;
          local_868 = (undefined8 *****)0x0;
          plVar19 = (longlong *)(lVar9 + 0x10);
          if (pfVar22[1] == 2.8026e-45) {
            local_798 = (longlong *)FUN_1402d82e0("signal_check_hit_res","Reserved by {}");
            lStack_790 = -1;
            do {
              lStack_790 = lStack_790 + 1;
            } while (*(char *)((longlong)local_798 + lStack_790) != '\0');
            if (0xf < *(ulonglong *)(lVar9 + 0x28)) {
              plVar19 = (longlong *)*plVar19;
            }
            lStack_7b0 = *(longlong *)(lVar9 + 0x20);
            local_758 = 0xd;
            local_718 = 0;
            local_728 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            local_720 = local_708;
            local_710 = 500;
            local_7a8 = 0xd;
            uStack_7a4 = 0;
            local_7b8 = plVar19;
            local_768 = plVar19;
            lStack_760 = lStack_7b0;
            pplStack_750 = &local_7b8;
            local_748 = local_798;
            lStack_740 = lStack_790;
            uStack_7a0 = &local_7b8;
            FUN_140022830(&local_728,&local_798,&local_7a8);
            local_830 = (undefined8 ******)0x0;
            uStack_828 = 0;
            local_820 = (float **)0x0;
            plStack_818 = (longlong *)0x0;
            FUN_140002c00(&local_830,local_720);
            local_728 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            if (local_720 != local_708) {
              _Memory = local_720;
              if ((0xfff < local_710) &&
                 (_Memory = *(undefined1 **)(local_720 + -8),
                 (undefined1 *)0x1f < local_720 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
LAB_14063d0d4:
              free(_Memory);
            }
          }
          else {
            local_7c8 = (longlong *)FUN_1402d82e0("signal_check_hit_occ","Occupied by {}");
            uStack_7c0 = -1;
            do {
              uStack_7c0 = uStack_7c0 + 1;
            } while (*(char *)((longlong)local_7c8 + uStack_7c0) != '\0');
            if (0xf < *(ulonglong *)(lVar9 + 0x28)) {
              plVar19 = (longlong *)*plVar19;
            }
            uStack_7e0 = *(longlong *)(lVar9 + 0x20);
            local_7d8 = 0xd;
            uStack_7d0 = &local_798;
            local_4f8 = 0;
            local_508 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            local_500 = local_4e8;
            local_4f0 = 500;
            local_7a8 = 0xd;
            uStack_7a4 = 0;
            local_7e8 = plVar19;
            local_7b8 = local_7c8;
            lStack_7b0 = uStack_7c0;
            local_798 = plVar19;
            lStack_790 = uStack_7e0;
            uStack_7a0 = uStack_7d0;
            FUN_140022830(&local_508,&local_7b8,&local_7a8);
            local_830 = (undefined8 ******)0x0;
            uStack_828 = 0;
            local_820 = (float **)0x0;
            plStack_818 = (longlong *)0x0;
            FUN_140002c00(&local_830,local_500);
            local_508 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
            if (local_500 != local_4e8) {
              _Memory = local_500;
              if ((0xfff < local_4f0) &&
                 (_Memory = *(undefined1 **)(local_500 + -8),
                 (undefined1 *)0x1f < local_500 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              goto LAB_14063d0d4;
            }
          }
          if (0xf < plStack_850) {
            _Memory_00 = local_868;
            if ((0xfff < (longlong)plStack_850 + 1U) &&
               (_Memory_00 = (undefined8 *****)local_868[-1],
               0x1f < (ulonglong)((longlong)local_868 + (-8 - (longlong)_Memory_00)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(_Memory_00);
          }
          local_858 = local_820;
          plStack_850 = plStack_818;
          local_868 = local_830;
          uStack_860 = uStack_828;
          ppppppuVar12 = &local_868;
          if ((longlong *)0xf < plStack_818) {
            ppppppuVar12 = (undefined8 ******)local_830;
          }
          pfVar16 = &local_7f8;
          uVar27 = CONCAT44(uVar29,uVar28);
          FUN_1401b32a0(plVar21,local_848,fVar25,fVar26,uVar27,ppppppuVar12);
          uVar29 = (undefined4)((ulonglong)uVar27 >> 0x20);
          (**(code **)*plVar21)(plVar21);
          uVar27 = CONCAT44(uVar29,(local_7ec - local_7f4) + fVar4);
          (**(code **)(*plVar21 + 0x38))
                    (plVar21,local_7f8 - fVar3,local_7f4 - fVar2,(local_7f0 - local_7f8) + fVar5,
                     uVar27);
          uVar29 = (undefined4)((ulonglong)uVar27 >> 0x20);
          (**(code **)(*plVar21 + 0x70))(plVar21,0xffeeeeff,0);
          ppppppuVar12 = &local_868;
          if ((longlong *)0xf < plStack_850) {
            ppppppuVar12 = (undefined8 ******)local_868;
          }
          uVar27 = CONCAT44(uVar29,uVar28);
          (**(code **)(*plVar21 + 0x140))
                    (plVar21,local_848,fVar25,fVar26,uVar27,ppppppuVar12,0,
                     (ulonglong)pfVar16 & 0xffffffff00000000);
          if ((longlong *)0xf < plStack_850) {
            ppppppuVar12 = (undefined8 ******)local_868;
            if ((0xfff < (longlong)plStack_850 + 1U) &&
               (ppppppuVar12 = (undefined8 ******)local_868[-1],
               0x1f < (ulonglong)((longlong)local_868 + (-8 - (longlong)ppppppuVar12)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppppuVar12);
          }
        }
        pfVar16 = pfVar22 + 3;
        pfVar22 = pfVar22 + 8;
      } while (pfVar16 != pfVar8);
    }
    FUN_1401b2400(plVar21);
    if (local_788 != (float *)0x0) {
      free(local_788);
    }
    FUN_14056de10(local_278);
    FUN_140325a40(local_2a8);
    pvVar1 = local_2c0;
    if (1 < local_298) {
      free(local_2a0);
      pvVar1 = local_2c0;
    }
    for (; pvVar1 != local_2b8; pvVar1 = (void *)((longlong)pvVar1 + 0x28)) {
      FUN_140025470(pvVar1);
    }
    if (local_2c0 != (void *)0x0) {
      free(local_2c0);
    }
  }
  FUN_1402531c0(param_10);
  if (1 < *(ulonglong *)(param_10 + 0x10)) {
    free(*(void **)(param_10 + 8));
  }
  return;
}


// Incoming references
// 0xc31a30 DATA caller none
// 0x63f4e8 UNCONDITIONAL_CALL caller 14063f070
