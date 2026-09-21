// Candidate VA 14049b990; RVA 0x49b990
// Ghidra inferred prototype: undefined FUN_14049b990()

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_14049b990(undefined2 *param_1,basic_istream<char,std::char_traits<char>_> *param_2)

{
  ulonglong uVar1;
  ulonglong uVar2;
  bool bVar3;
  undefined8 uVar4;
  undefined2 *puVar5;
  longlong lVar6;
  longlong lVar7;
  short *psVar8;
  undefined8 *******pppppppuVar9;
  longlong lVar10;
  undefined8 *******pppppppuVar11;
  ulonglong local_res18;
  undefined **local_868;
  int local_860;
  basic_istream<char,std::char_traits<char>_> *local_858;
  undefined8 *******local_848;
  undefined8 uStack_840;
  ulonglong local_838;
  ulonglong local_830;
  undefined8 *******local_828;
  undefined8 uStack_820;
  ulonglong local_818;
  ulonglong uStack_810;
  undefined8 *******local_808;
  undefined8 uStack_800;
  ulonglong local_7f8;
  ulonglong uStack_7f0;
  undefined8 local_7e8 [2];
  int local_7d8;
  short local_7d4;
  short local_7d2;
  undefined2 uStack_7d0;
  undefined2 uStack_7ce;
  undefined2 uStack_7cc;
  undefined2 uStack_7ca;
  undefined2 uStack_7c8;
  undefined2 local_7c6;
  undefined8 local_7b8;
  undefined8 uStack_7b0;
  undefined8 local_7a8;
  int local_798;
  int local_794;
  undefined1 local_790 [8];
  undefined1 local_788 [8];
  undefined1 local_780 [8];
  short local_778 [32];
  short local_738 [32];
  short local_6f8 [178];
  short local_594;
  undefined2 local_592;
  undefined2 local_590;
  undefined4 local_58c;
  undefined8 local_588;
  undefined8 local_580;
  undefined4 local_578;
  undefined4 local_574;
  undefined4 local_570;
  undefined4 local_56c;
  undefined4 local_568;
  undefined4 local_564;
  undefined4 local_560;
  undefined4 local_55c;
  undefined4 local_558;
  undefined4 local_554;
  undefined4 local_550;
  undefined4 local_54c;
  undefined4 local_548;
  char local_544 [127];
  char local_4c5;
  char local_4c4 [1023];
  char local_c5;
  undefined8 local_c4;
  undefined8 uStack_bc;
  undefined8 local_b4;
  undefined8 uStack_ac;
  undefined8 local_a4;
  undefined8 uStack_9c;
  undefined8 local_94;
  undefined8 uStack_8c;
  undefined8 local_84;
  undefined8 uStack_7c;
  undefined8 local_74;
  undefined8 uStack_6c;
  undefined2 local_64;
  undefined1 local_62;
  undefined8 local_60;
  undefined8 uStack_58;
  undefined8 local_50;
  undefined8 uStack_48;

  local_7d8 = 0;
  local_7d4 = 0;
  local_7d2 = 0;
  uStack_7d0 = 0;
  uStack_7ce = 0;
  uStack_7cc = 0;
  uStack_7ca = 0;
  uStack_7c8 = 0;
  local_7c6 = 0;
  std::basic_istream<char,std::char_traits<char>_>::read(param_2,(char *)&local_7d8,0x14);
  if ((local_7d8 == 0x59424d4e) && (local_7d2 == 1)) {
    *param_1 = 1;
    param_1[1] = uStack_7d0;
    param_1[2] = uStack_7ce;
    *(uint *)(param_1 + 4) = CONCAT22(uStack_7ca,uStack_7cc);
    *(uint *)(param_1 + 6) = CONCAT22(local_7c6,uStack_7c8);
    if (local_7d4 == 2) {
      memset(&local_588,0,0x550);
      std::basic_istream<char,std::char_traits<char>_>::read(param_2,(char *)&local_588,0x550);
      *(undefined8 *)(param_1 + 8) = local_588;
      *(undefined8 *)(param_1 + 0xc) = local_580;
      *(undefined4 *)(param_1 + 0x10) = local_578;
      *(undefined4 *)(param_1 + 0x12) = local_574;
      *(undefined4 *)(param_1 + 0x14) = local_570;
      *(undefined4 *)(param_1 + 0x16) = local_56c;
      *(undefined4 *)(param_1 + 0x18) = local_568;
      *(undefined4 *)(param_1 + 0x1a) = local_564;
      *(undefined4 *)(param_1 + 0x1c) = local_560;
      *(undefined4 *)(param_1 + 0x1e) = local_55c;
      *(undefined4 *)(param_1 + 0x20) = local_558;
      *(undefined4 *)(param_1 + 0x22) = local_554;
      *(undefined4 *)(param_1 + 0x24) = local_550;
      *(undefined4 *)(param_1 + 0x26) = local_54c;
      *(undefined4 *)(param_1 + 0x28) = local_548;
      lVar6 = -1;
      if (local_4c5 == '\0') {
        lVar10 = -1;
        do {
          lVar7 = lVar10 + 1;
          lVar10 = lVar10 + 1;
        } while (local_544[lVar7] != '\0');
        FUN_140030630(param_1 + 0x2c,local_544);
      }
      if (local_c5 == '\0') {
        do {
          lVar7 = lVar6 + 1;
          lVar10 = lVar6 + 1;
          lVar6 = lVar7;
        } while (local_4c4[lVar10] != '\0');
        FUN_140030630(param_1 + 0x3c,local_4c4,lVar7);
      }
      *(undefined8 *)(param_1 + 0x4c) = local_c4;
      *(undefined8 *)(param_1 + 0x50) = uStack_bc;
      *(undefined8 *)(param_1 + 0x54) = local_b4;
      *(undefined8 *)(param_1 + 0x58) = uStack_ac;
      *(undefined8 *)(param_1 + 0x5c) = local_a4;
      *(undefined8 *)(param_1 + 0x60) = uStack_9c;
      *(undefined8 *)(param_1 + 100) = local_94;
      *(undefined8 *)(param_1 + 0x68) = uStack_8c;
      *(undefined8 *)(param_1 + 0x6c) = local_84;
      *(undefined8 *)(param_1 + 0x70) = uStack_7c;
      *(undefined8 *)(param_1 + 0x74) = local_74;
      *(undefined8 *)(param_1 + 0x78) = uStack_6c;
      param_1[0x7c] = local_64;
      *(undefined1 *)(param_1 + 0x7d) = local_62;
      *(undefined8 *)((longlong)param_1 + 0xfb) = local_60;
      *(undefined8 *)((longlong)param_1 + 0x103) = uStack_58;
      *(undefined8 *)((longlong)param_1 + 0x10b) = local_50;
      *(undefined8 *)((longlong)param_1 + 0x113) = uStack_48;
      return 1;
    }
LAB_14049c51f:
    uVar4 = 1;
  }
  else {
    local_7b8 = 0;
    uStack_7b0 = 0;
    local_7a8 = 0;
    std::basic_istream<char,std::char_traits<char>_>::seekg(param_2,&local_7b8);
    local_868 = serde::DeserializerIStream::vftable;
    local_860 = 0;
    local_798 = 0;
    local_858 = param_2;
    FUN_140498d30(&local_868,&local_798,4,1);
    if ((code *)local_868[3] == FUN_140498d30) {
      FUN_140498d30();
    }
    else {
      (*(code *)local_868[3])(&local_868,&local_794,4,1);
    }
    if ((code *)local_868[3] == FUN_140498d30) {
      FUN_140498d30();
    }
    else {
      (*(code *)local_868[3])(&local_868,local_790,4,1);
    }
    if ((code *)local_868[3] == FUN_140498d30) {
      FUN_140498d30();
    }
    else {
      (*(code *)local_868[3])(&local_868,local_788,8,1);
    }
    if ((code *)local_868[3] == FUN_140498d30) {
      FUN_140498d30();
    }
    else {
      (*(code *)local_868[3])(&local_868,local_780,8,1);
    }
    if ((code *)local_868[1] != _guard_check_icall) {
      (*(code *)local_868[1])
                (&local_868,
                 "??$visit@E$0EA@@serde@@YAXPEAV?$array@E$0EA@@std@@PEAUDeserializer@0@@Z");
    }
    psVar8 = local_778;
    do {
      if ((code *)local_868[3] == FUN_140498d30) {
        FUN_140498d30();
      }
      else {
        (*(code *)local_868[3])(&local_868,psVar8,1,1);
      }
      psVar8 = (short *)((longlong)psVar8 + 1);
    } while (psVar8 != local_738);
    if ((code *)local_868[2] != _guard_check_icall) {
      (*(code *)local_868[2])(&local_868);
    }
    if ((code *)local_868[1] != _guard_check_icall) {
      (*(code *)local_868[1])
                (&local_868,
                 "??$visit@E$0EA@@serde@@YAXPEAV?$array@E$0EA@@std@@PEAUDeserializer@0@@Z");
    }
    psVar8 = local_738;
    do {
      if ((code *)local_868[3] == FUN_140498d30) {
        FUN_140498d30();
      }
      else {
        (*(code *)local_868[3])(&local_868,psVar8,1,1);
      }
      psVar8 = (short *)((longlong)psVar8 + 1);
    } while (psVar8 != local_6f8);
    if ((code *)local_868[2] != _guard_check_icall) {
      (*(code *)local_868[2])(&local_868);
    }
    if ((code *)local_868[1] != _guard_check_icall) {
      (*(code *)local_868[1])
                (&local_868,
                 "??$visit@E$0BGE@@serde@@YAXPEAV?$array@E$0BGE@@std@@PEAUDeserializer@0@@Z");
    }
    psVar8 = local_6f8;
    do {
      if ((code *)local_868[3] == FUN_140498d30) {
        FUN_140498d30();
      }
      else {
        (*(code *)local_868[3])(&local_868,psVar8,1);
      }
      psVar8 = (short *)((longlong)psVar8 + 1);
    } while (psVar8 != &local_594);
    if ((code *)local_868[2] != _guard_check_icall) {
      (*(code *)local_868[2])(&local_868);
    }
    if ((code *)local_868[2] != _guard_check_icall) {
      (*(code *)local_868[2])(&local_868);
    }
    bVar3 = std::ios_base::eof((ios_base *)(param_2 + *(int *)(*(longlong *)param_2 + 4)));
    uVar2 = _UNK_140aac918;
    uVar1 = _DAT_140aac910;
    if ((!bVar3) && (local_860 = local_794, local_798 == 0x59424d4e)) {
      uStack_840 = 0;
      local_838 = 0;
      local_830 = 0xf;
      local_848 = (undefined8 *******)0x0;
      uStack_820 = 0;
      local_818 = _DAT_140aac910;
      uStack_810 = _UNK_140aac918;
      local_828 = (undefined8 *******)0x0;
      uStack_800 = 0;
      local_7f8 = _DAT_140aac910;
      uStack_7f0 = _UNK_140aac918;
      local_808 = (undefined8 *******)0x0;
      if ((code *)local_868[1] != _guard_check_icall) {
        (*(code *)local_868[1])(&local_868,"nimby::sync::saves::Preview1");
      }
      if (0 < local_860) {
        if ((code *)local_868[1] != _guard_check_icall) {
          (*(code *)local_868[1])
                    (&local_868,
                     "?visit@serde@@YAXPEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAUDeserializer@1@@Z"
                    );
        }
        local_res18 = local_838;
        if ((code *)local_868[3] == FUN_140498d30) {
          FUN_140498d30();
        }
        else {
          (*(code *)local_868[3])(&local_868,&local_res18,8);
        }
        if (local_838 != local_res18) {
          if (local_838 < local_res18) {
            FUN_1402457c0(&local_848,local_res18 - local_838,0);
          }
          else {
            local_838 = local_res18;
            pppppppuVar9 = &local_848;
            if (0xf < local_830) {
              pppppppuVar9 = local_848;
            }
            *(undefined1 *)((longlong)pppppppuVar9 + local_res18) = 0;
          }
        }
        pppppppuVar9 = &local_848;
        if (0xf < local_830) {
          pppppppuVar9 = local_848;
        }
        pppppppuVar11 = (undefined8 *******)((longlong)pppppppuVar9 + local_838);
        for (; pppppppuVar9 != pppppppuVar11;
            pppppppuVar9 = (undefined8 *******)((longlong)pppppppuVar9 + 1)) {
          if ((code *)local_868[3] == FUN_140498d30) {
            FUN_140498d30();
          }
          else {
            (*(code *)local_868[3])(&local_868,pppppppuVar9);
          }
        }
        if ((code *)local_868[2] != _guard_check_icall) {
          (*(code *)local_868[2])(&local_868);
        }
        if (0 < local_860) {
          if ((code *)local_868[1] != _guard_check_icall) {
            (*(code *)local_868[1])
                      (&local_868,
                       "?visit@serde@@YAXPEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAUDeserializer@1@@Z"
                      );
          }
          local_res18 = local_818;
          if ((code *)local_868[3] == FUN_140498d30) {
            FUN_140498d30();
          }
          else {
            (*(code *)local_868[3])(&local_868,&local_res18,8);
          }
          if (local_818 != local_res18) {
            if (local_818 < local_res18) {
              FUN_1402457c0(&local_828,local_res18 - local_818,0);
            }
            else {
              local_818 = local_res18;
              pppppppuVar9 = &local_828;
              if (0xf < uStack_810) {
                pppppppuVar9 = local_828;
              }
              *(undefined1 *)((longlong)pppppppuVar9 + local_res18) = 0;
            }
          }
          pppppppuVar9 = &local_828;
          if (0xf < uStack_810) {
            pppppppuVar9 = local_828;
          }
          pppppppuVar11 = (undefined8 *******)((longlong)pppppppuVar9 + local_818);
          for (; pppppppuVar9 != pppppppuVar11;
              pppppppuVar9 = (undefined8 *******)((longlong)pppppppuVar9 + 1)) {
            if ((code *)local_868[3] == FUN_140498d30) {
              FUN_140498d30();
            }
            else {
              (*(code *)local_868[3])(&local_868,pppppppuVar9,1);
            }
          }
          if ((code *)local_868[2] != _guard_check_icall) {
            (*(code *)local_868[2])(&local_868);
          }
          if (0 < local_860) {
            if ((code *)local_868[1] != _guard_check_icall) {
              (*(code *)local_868[1])
                        (&local_868,
                         "?visit@serde@@YAXPEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAUDeserializer@1@@Z"
                        );
            }
            local_res18 = local_7f8;
            if ((code *)local_868[3] == FUN_140498d30) {
              FUN_140498d30();
            }
            else {
              (*(code *)local_868[3])(&local_868,&local_res18,8);
            }
            if (local_7f8 != local_res18) {
              if (local_7f8 < local_res18) {
                FUN_1402457c0(&local_808,local_res18 - local_7f8,0);
              }
              else {
                local_7f8 = local_res18;
                pppppppuVar9 = &local_808;
                if (0xf < uStack_7f0) {
                  pppppppuVar9 = local_808;
                }
                *(undefined1 *)((longlong)pppppppuVar9 + local_res18) = 0;
              }
            }
            pppppppuVar9 = &local_808;
            if (0xf < uStack_7f0) {
              pppppppuVar9 = local_808;
            }
            pppppppuVar11 = (undefined8 *******)((longlong)pppppppuVar9 + local_7f8);
            for (; pppppppuVar9 != pppppppuVar11;
                pppppppuVar9 = (undefined8 *******)((longlong)pppppppuVar9 + 1)) {
              if ((code *)local_868[3] == FUN_140498d30) {
                FUN_140498d30();
              }
              else {
                (*(code *)local_868[3])(&local_868,pppppppuVar9);
              }
            }
            if ((code *)local_868[2] != _guard_check_icall) {
              (*(code *)local_868[2])(&local_868);
            }
            if (0 < local_860) {
              if ((code *)local_868[3] == FUN_140498d30) {
                FUN_140498d30();
              }
              else {
                (*(code *)local_868[3])(&local_868,local_7e8);
              }
            }
          }
        }
      }
      if ((code *)local_868[2] != _guard_check_icall) {
        (*(code *)local_868[2])(&local_868);
      }
      bVar3 = std::ios_base::eof((ios_base *)(param_2 + *(int *)(*(longlong *)param_2 + 4)));
      if ((!bVar3) && (FUN_14049c560(&local_798), local_594 == 1)) {
        *param_1 = 1;
        param_1[1] = local_592;
        param_1[2] = local_590;
        *(undefined4 *)(param_1 + 4) = local_58c;
        *(int *)(param_1 + 6) = local_794;
        *(undefined8 *)(param_1 + 8) = local_7e8[0];
        *(undefined8 *)(param_1 + 0xc) = 0;
        *(undefined8 *)(param_1 + 0x10) = 0;
        *(undefined8 *)(param_1 + 0x14) = 0;
        *(undefined8 *)(param_1 + 0x18) = 0;
        *(undefined8 *)(param_1 + 0x1c) = 0;
        *(undefined8 *)(param_1 + 0x20) = 0;
        *(undefined8 *)(param_1 + 0x24) = 0;
        *(undefined4 *)(param_1 + 0x28) = 0;
        if ((undefined8 ********)(param_1 + 0x2c) != &local_848) {
          pppppppuVar9 = &local_848;
          if (0xf < local_830) {
            pppppppuVar9 = local_848;
          }
          FUN_140030630(param_1 + 0x2c,pppppppuVar9);
        }
        puVar5 = param_1 + 0x3c;
        if (0xf < *(ulonglong *)(param_1 + 0x48)) {
          puVar5 = *(undefined2 **)(param_1 + 0x3c);
        }
        *(undefined8 *)(param_1 + 0x44) = 0;
        *(undefined1 *)puVar5 = 0;
        if (0xf < uStack_7f0) {
          pppppppuVar9 = local_808;
          if ((0xfff < uStack_7f0 + 1) &&
             (pppppppuVar9 = (undefined8 *******)local_808[-1],
             0x1f < (ulonglong)((longlong)local_808 + (-8 - (longlong)pppppppuVar9))))
          goto LAB_14049c504;
          free(pppppppuVar9);
        }
        local_7f8 = uVar1;
        uStack_7f0 = uVar2;
        local_808 = (undefined8 *******)((ulonglong)local_808 & 0xffffffffffffff00);
        if (0xf < uStack_810) {
          pppppppuVar9 = local_828;
          if ((0xfff < uStack_810 + 1) &&
             (pppppppuVar9 = (undefined8 *******)local_828[-1],
             0x1f < (ulonglong)((longlong)local_828 + (-8 - (longlong)pppppppuVar9))))
          goto LAB_14049c504;
          free(pppppppuVar9);
        }
        local_818 = uVar1;
        uStack_810 = uVar2;
        local_828 = (undefined8 *******)((ulonglong)local_828 & 0xffffffffffffff00);
        if (0xf < local_830) {
          pppppppuVar9 = local_848;
          if ((0xfff < local_830 + 1) &&
             (pppppppuVar9 = (undefined8 *******)local_848[-1],
             0x1f < (ulonglong)((longlong)local_848 + (-8 - (longlong)pppppppuVar9))))
          goto LAB_14049c504;
          free(pppppppuVar9);
        }
        goto LAB_14049c51f;
      }
      if (0xf < uStack_7f0) {
        pppppppuVar9 = local_808;
        if ((0xfff < uStack_7f0 + 1) &&
           (pppppppuVar9 = (undefined8 *******)local_808[-1],
           0x1f < (ulonglong)((longlong)local_808 + (-8 - (longlong)pppppppuVar9))))
        goto LAB_14049c504;
        free(pppppppuVar9);
      }
      local_808 = (undefined8 *******)((ulonglong)local_808 & 0xffffffffffffff00);
      local_7f8 = uVar1;
      uStack_7f0 = uVar2;
      if (0xf < uStack_810) {
        pppppppuVar9 = local_828;
        if ((0xfff < uStack_810 + 1) &&
           (pppppppuVar9 = (undefined8 *******)local_828[-1],
           0x1f < (ulonglong)((longlong)local_828 + (-8 - (longlong)pppppppuVar9))))
        goto LAB_14049c504;
        free(pppppppuVar9);
      }
      local_828 = (undefined8 *******)((ulonglong)local_828 & 0xffffffffffffff00);
      local_818 = uVar1;
      uStack_810 = uVar2;
      if (0xf < local_830) {
        pppppppuVar9 = local_848;
        if ((0xfff < local_830 + 1) &&
           (pppppppuVar9 = (undefined8 *******)local_848[-1],
           0x1f < (ulonglong)((longlong)local_848 + (-8 - (longlong)pppppppuVar9)))) {
LAB_14049c504:
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pppppppuVar9);
      }
    }
    uVar4 = 0;
  }
  return uVar4;
}


// Incoming references
// 0xc266e4 DATA caller none
// 0x4e641e UNCONDITIONAL_CALL caller 1404e5ea0
// 0x49c893 UNCONDITIONAL_CALL caller 14049c7e0
// 0x49db1f UNCONDITIONAL_CALL caller 14049d9a0
