// Candidate VA 140661740; RVA 0x661740
// Ghidra inferred prototype: undefined FUN_140661740()

void FUN_140661740(longlong *param_1,undefined8 *param_2,longlong param_3,longlong *param_4)

{
  longlong lVar1;
  code *pcVar2;
  bool bVar3;
  char cVar4;
  int iVar5;
  undefined8 uVar6;
  longlong *plVar7;
  undefined8 uVar8;
  int iVar9;
  longlong lVar10;
  int *piVar11;
  undefined8 **ppuVar12;
  longlong *plVar13;
  undefined1 *puVar14;
  int *piVar15;
  ulonglong uVar16;
  int *piVar17;
  longlong *local_res18;
  undefined8 in_stack_fffffffffffffea8;
  undefined4 uVar18;
  undefined8 **local_148 [2];
  int local_138;
  ulonglong local_130;
  undefined4 local_128;
  longlong local_120;
  int local_118;
  undefined4 uStack_114;
  undefined1 local_110 [8];
  size_t local_108;
  ulonglong local_100;
  char local_f0;
  ulonglong local_e8;
  ulonglong local_e0;
  ulonglong local_d8;
  ulonglong local_d0;
  ulonglong local_c8;
  longlong local_c0;
  undefined **local_b8 [7];
  undefined ***local_80;
  undefined **local_78 [7];
  undefined ***local_40;

  uVar18 = (undefined4)((ulonglong)in_stack_fffffffffffffea8 >> 0x20);
  lVar10 = *(longlong *)(param_3 + 600);
  if (lVar10 != 0) {
    local_e8 = (ulonglong)*(uint *)(lVar10 + 0x30);
    local_e0 = (ulonglong)*(uint *)(lVar10 + 0x130);
    local_d8 = (ulonglong)*(uint *)(lVar10 + 0xb0);
    local_d0 = (ulonglong)*(uint *)(lVar10 + 0x1b0);
    local_c8 = (ulonglong)*(uint *)(lVar10 + 0x230);
    local_c0 = *(longlong *)(*(longlong *)(param_3 + 0x250) + 0x28);
    if ((*(longlong *)(*(longlong *)(param_3 + 0x250) + 0x20) == 0) && (local_c0 != 0)) {
      bVar3 = false;
    }
    else {
      bVar3 = true;
    }
    local_120 = 0;
    lVar10 = *(longlong *)(param_3 + 0x260);
    if (lVar10 != 0) {
      lVar1 = *(longlong *)(lVar10 + 0x1678);
      if (*(longlong *)(lVar10 + 0x1680) != 0) {
        LOCK();
        piVar15 = (int *)(*(longlong *)(lVar10 + 0x1680) + 8);
        *piVar15 = *piVar15 + 1;
        UNLOCK();
      }
      plVar7 = *(longlong **)(lVar10 + 0x1680);
      if (lVar1 != 0) {
        lVar10 = *(longlong *)(lVar1 + 0xe0) - *(longlong *)(lVar1 + 0xd8);
        lVar10 = lVar10 / 0x12 + (lVar10 >> 0x3f);
        local_120 = (lVar10 >> 3) - (lVar10 >> 0x3f);
      }
      if (plVar7 != (longlong *)0x0) {
        LOCK();
        plVar13 = plVar7 + 1;
        lVar10 = *plVar13;
        *(int *)plVar13 = (int)*plVar13 + -1;
        UNLOCK();
        if ((int)lVar10 == 1) {
          (**(code **)*plVar7)(plVar7);
          LOCK();
          piVar15 = (int *)((longlong)plVar7 + 0xc);
          iVar5 = *piVar15;
          *piVar15 = *piVar15 + -1;
          UNLOCK();
          if (iVar5 == 1) {
            (**(code **)(*plVar7 + 8))(plVar7);
          }
        }
      }
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8df60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("option_company_name","Company name:");
      (*pcVar2)(param_4,uVar6,0x14);
      if (bVar3) {
        FUN_140019ca0(&local_118,param_1);
        FUN_14055b8e0(param_4,&DAT_140b8e0e0);
        (**(code **)(*param_4 + 0xe0))(param_4,param_1,200,0,CONCAT44(uVar18,0x260));
        plVar7 = param_1;
        if (0xf < (ulonglong)param_1[3]) {
          plVar7 = (longlong *)*param_1;
        }
        piVar15 = (int *)CONCAT44(uStack_114,local_118);
        piVar11 = &local_118;
        if (0xf < local_100) {
          piVar11 = piVar15;
        }
        if ((local_108 != param_1[2]) ||
           ((local_108 != 0 && (iVar5 = memcmp(piVar11,plVar7,local_108), iVar5 != 0)))) {
          plVar7 = (longlong *)FUN_14068f8f0(param_3 + 0x228);
          lVar10 = *(longlong *)(param_3 + 0x250);
          local_128 = (**(code **)(*plVar7 + 0x28))(plVar7);
          FUN_14074c5a0(lVar10 + 0x4e8,local_148);
          local_148[0][1] = (undefined8 *)((longlong)local_148[0][1] + 1);
          plVar7[3] = (longlong)local_148[0][1];
          if (plVar7 + 4 != param_1) {
            plVar13 = param_1;
            if (0xf < (ulonglong)param_1[3]) {
              plVar13 = (longlong *)*param_1;
            }
            FUN_140030630(plVar7 + 4,plVar13);
          }
        }
        if (0xf < local_100) {
          piVar11 = piVar15;
          if ((0xfff < local_100 + 1) &&
             (piVar11 = *(int **)(piVar15 + -2),
             0x1f < (ulonglong)((longlong)piVar15 + (-8 - (longlong)piVar11)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(piVar11);
        }
      }
      else {
        FUN_14055b8e0(param_4,&DAT_140b8e0e0);
        (**(code **)(*param_4 + 0xa0))(param_4,param_1,0x11);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_tracks","Track segments:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_e8);
      if (0 < local_138 + -3) {
        lVar10 = (longlong)(local_138 + -3);
        uVar16 = (ulonglong)((local_138 - 4U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_buildings","Buildings:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_e0);
      if (0 < local_138 + -3) {
        lVar10 = (longlong)(local_138 + -3);
        uVar16 = (ulonglong)((local_138 - 4U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_stations","Stations:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_d8);
      if (0 < local_138 + -3) {
        lVar10 = (longlong)(local_138 + -3);
        uVar16 = (ulonglong)((local_138 - 4U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_active_tiles","Active pax tiles:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      lVar10 = local_120;
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_120);
      iVar9 = local_138 + -3;
      iVar5 = (int)(lVar10 >> 0x3f);
      if (-iVar5 < iVar9) {
        lVar10 = (longlong)iVar9;
        uVar16 = (ulonglong)(((iVar9 + iVar5) - 1U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_lines","Lines:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_d0);
      if (0 < local_138 + -3) {
        lVar10 = (longlong)(local_138 + -3);
        uVar16 = (ulonglong)((local_138 - 4U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8dfe0);
      FUN_14055b8e0(param_4,&DAT_140b8dd60);
      (**(code **)(*param_4 + 8))(param_4);
      FUN_14055b8e0(param_4,&DAT_140b8e060);
      pcVar2 = *(code **)(*param_4 + 0xa8);
      uVar6 = FUN_1402d82e0("info_company_trains","Trains:");
      (*pcVar2)(param_4,uVar6,0x14);
      FUN_14055b8e0(param_4,&DAT_140b8e0e0);
      pcVar2 = *(code **)(*param_4 + 0xa0);
      FUN_14028fc90(local_148,local_c8);
      if (0 < local_138 + -3) {
        lVar10 = (longlong)(local_138 + -3);
        uVar16 = (ulonglong)((local_138 - 4U) / 3 + 1);
        do {
          FUN_140587330(local_148,lVar10);
          lVar10 = lVar10 + -3;
          uVar16 = uVar16 - 1;
        } while (uVar16 != 0);
      }
      (*pcVar2)(param_4,local_148);
      if (0xf < local_130) {
        ppuVar12 = local_148[0];
        if ((0xfff < local_130 + 1) &&
           (ppuVar12 = (undefined8 **)local_148[0][-1],
           0x1f < (ulonglong)((longlong)local_148[0] + (-8 - (longlong)ppuVar12)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(ppuVar12);
      }
      (**(code **)(*param_4 + 0x18))(param_4);
      if (local_c0 != 0) {
LAB_140662a48:
        cVar4 = (**(code **)(*param_4 + 0x170))(param_4);
        if (cVar4 != '\0') {
          piVar15 = *(int **)(param_3 + 0x180);
          piVar11 = *(int **)(param_3 + 0x188);
          if (piVar15 != piVar11) {
            piVar17 = piVar15 + 2;
            do {
              local_118 = *piVar15;
              local_f0 = -1;
              cVar4 = (char)piVar17[8];
              if (cVar4 != -1) {
                if (cVar4 == '\0') {
                  FUN_140019ca0(local_110,piVar17);
                }
                else {
                  FUN_140247b90();
                }
                local_f0 = cVar4 != '\0';
              }
              puVar14 = local_110;
              if (local_f0 != '\x01') {
                puVar14 = (undefined1 *)0x0;
              }
              if ((local_118 == 1) && (puVar14 != (undefined1 *)0x0)) {
                if ((char)param_1[0xad] == '\0') {
                  FUN_140247b90(param_1 + 0xa9);
                  *(undefined1 *)(param_1 + 0xad) = 1;
                }
                else {
                  FUN_140246270();
                }
                uVar6 = *param_2;
                uVar8 = FUN_1402d82e0("file_export_title","Exported to file");
                FUN_14081d6d0(uVar6,uVar8,0,1);
              }
              if (local_f0 != -1) {
                if (local_f0 == '\0') {
                  FUN_140002d30(local_110);
                }
                else {
                  FUN_140025470();
                }
              }
              piVar15 = piVar15 + 0xc;
              piVar17 = piVar17 + 0xc;
            } while (piVar15 != piVar11);
          }
        }
        return;
      }
      if (*(longlong *)(param_3 + 600) != 0) {
        if (**(char **)(*(longlong *)(param_3 + 600) + 0x410) == '\0') {
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("option_company_switch_no_cash","Switch to unlimited money");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("confirm_set_no_cash","Confirm switching to unlimited money");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
        }
        if (*(longlong *)(param_3 + 600) != 0) {
          if (*(char *)(*(longlong *)(*(longlong *)(param_3 + 600) + 0x410) + 1) == '\0') {
            FUN_14055b8e0(param_4,&DAT_140b8dfe0);
            (**(code **)(*param_4 + 8))(param_4);
            *(undefined1 *)(param_4 + 3) = 1;
            *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
            pcVar2 = *(code **)(*param_4 + 0x90);
            uVar6 = FUN_1402d82e0("option_company_switch_no_couplers",
                                  "Switch to unrestricted coupling");
            cVar4 = (*pcVar2)(param_4,uVar6,0);
            if (cVar4 != '\0') {
              uVar6 = *param_2;
              uVar8 = FUN_1402d82e0("confirm_set_no_couplers",
                                    "Confirm switching to unrestricted coupling");
              FUN_14081d6d0(uVar6,uVar8,0,1);
            }
            (**(code **)(*param_4 + 0x18))(param_4);
          }
          local_res18 = param_4 + 3;
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("option_company_acc_export","Export accounting data");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            local_b8[0] = std::
                          _Func_impl_no_alloc<`public:_void___cdecl_nimby::shell::UITxn::export_accounting_now(void)___ptr64'::`2'::<lambda_1>,void,nimby::shell::UIState&___ptr64>
                          ::vftable;
            local_80 = local_b8;
            if (*(ulonglong *)(param_3 + 0x170) < *(ulonglong *)(param_3 + 0x178)) {
              *(ulonglong *)(param_3 + 0x170) = *(ulonglong *)(param_3 + 0x170) + 0x40;
              FUN_140004b80();
            }
            else {
              FUN_14074c320(param_3 + 0x168,local_b8);
            }
            if (local_80 != (undefined ***)0x0) {
              (*(code *)(*local_80)[4])(local_80,local_80 != local_b8);
            }
            *(undefined1 *)(param_3 + 0x198) = 1;
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)local_res18 = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("option_company_tt_export","Export timetable data");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            local_78[0] = std::
                          _Func_impl_no_alloc<`public:_void___cdecl_nimby::shell::UITxn::export_timetable_now(void)___ptr64'::`2'::<lambda_1>,void,nimby::shell::UIState&___ptr64>
                          ::vftable;
            local_40 = local_78;
            if (*(ulonglong *)(param_3 + 0x170) < *(ulonglong *)(param_3 + 0x178)) {
              *(ulonglong *)(param_3 + 0x170) = *(ulonglong *)(param_3 + 0x170) + 0x40;
              FUN_140004b80();
            }
            else {
              FUN_14074c320(param_3 + 0x168,local_78);
            }
            if (local_40 != (undefined ***)0x0) {
              (*(code *)(*local_40)[4])(local_40,local_40 != local_78);
            }
            *(undefined1 *)(param_3 + 0x198) = 1;
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("option_company_geo_export","Export GeoJSON");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            if (*(longlong *)(param_3 + 600) == 0) goto LAB_140662b85;
            FUN_140661040(param_1);
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("geo_export_title","Exported GeoJSON");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          *(undefined4 *)((longlong)param_4 + 0x34) = 0x41000000;
          *(undefined1 *)(param_4 + 6) = 1;
          (**(code **)(*param_4 + 200))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_recalc_all_lines_but","Refresh all line timings");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            lVar10 = FUN_140687af0(param_3);
            *(undefined1 *)(lVar10 + 0x20) = 0;
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_intervention_stale_trains_but","All stale trains intervention"
                               );
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("mass_intervention_stale_trains_title",
                                  "Confirm all stale trains intervention");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          plVar7 = param_4 + 3;
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)plVar7 = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_intervention_trains_but","All trains intervention");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("mass_intervention_trains_title","Confirm all trains intervention"
                                 );
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)plVar7 = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("full_simulation_reset","Full simulation reset");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("full_simulation_reset","Full simulation reset");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)plVar7 = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_del_trains_but","Sell all trains");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("mass_del_trains_title","Sell all trains");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)plVar7 = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_del_lines_but","Delete all lines");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("mass_del_lines_title","Delete all lines");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("mass_del_schedules_but","Delete all schedules");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            uVar6 = *param_2;
            uVar8 = FUN_1402d82e0("mass_del_schedules_title","Delete all schedules");
            FUN_14081d6d0(uVar6,uVar8,0,1);
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          FUN_14055b8e0(param_4,&DAT_140b8dfe0);
          (**(code **)(*param_4 + 8))(param_4);
          *(undefined1 *)(param_4 + 3) = 1;
          *(undefined4 *)((longlong)param_4 + 0x1c) = 0xa0;
          pcVar2 = *(code **)(*param_4 + 0x90);
          uVar6 = FUN_1402d82e0("bulk_edit_all_stations","Bulk edit all stations");
          cVar4 = (*pcVar2)(param_4,uVar6,0);
          if (cVar4 != '\0') {
            *(undefined1 *)(param_1 + 0x93) = 1;
          }
          (**(code **)(*param_4 + 0x18))(param_4);
          goto LAB_140662a48;
        }
      }
    }
  }
LAB_140662b85:
                    /* WARNING: Subroutine does not return */
  abort();
}


// Incoming references
// 0xc32738 DATA caller none
// 0x667893 UNCONDITIONAL_CALL caller 140667240
