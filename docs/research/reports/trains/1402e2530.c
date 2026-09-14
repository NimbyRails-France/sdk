// Candidate VA 1402e2530; RVA 0x2e2530
// Ghidra inferred prototype: undefined FUN_1402e2530()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 *
FUN_1402e2530(longlong param_1,undefined8 *param_2,longlong *param_3,undefined8 *param_4)

{
  char cVar1;
  code *pcVar2;
  longlong *plVar3;
  longlong *plVar4;
  undefined4 uVar5;
  longlong lVar6;
  char *pcVar7;
  undefined8 *puVar8;
  longlong *plVar9;
  void *pvVar10;
  undefined8 ****ppppuVar11;
  undefined8 ****ppppuVar12;
  longlong lVar13;
  undefined8 uVar14;
  ulonglong uVar15;
  longlong local_res8;
  undefined8 *local_res10;
  longlong *local_res18;
  undefined8 *local_res20;
  void *local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  ulonglong uStack_80;
  undefined8 ***local_78 [2];
  longlong local_68;
  ulonglong local_60;
  
  local_res8 = param_1;
  local_res10 = param_2;
  local_res18 = param_3;
  local_res20 = param_4;
  if ((*(char *)(param_1 + 0xa0) == '\0') || (*param_3 == *(longlong *)(param_1 + 0xa8))) {
    *(undefined1 *)(param_1 + 0xa0) = 1;
    FUN_1402e1b50((longlong *)(param_1 + 0xa8),param_3);
    FUN_1402e3bf0(param_1 + 0x1a0,param_4);
    if (*param_3 == 0) {
      *param_2 = 0;
      param_2[1] = 0;
      param_2[2] = 0;
      param_2[3] = 0;
      pcVar7 = (char *)FUN_140003270(0x40);
      *param_2 = pcVar7;
      param_2[2] = 0x36;
      param_2[3] = 0x3f;
      uVar14 = s_ERROR__cannot_update_UGCMod_with_140a5f118._8_8_;
      *(undefined8 *)pcVar7 = s_ERROR__cannot_update_UGCMod_with_140a5f118._0_8_;
      *(undefined8 *)(pcVar7 + 8) = uVar14;
      uVar14 = s_ERROR__cannot_update_UGCMod_with_140a5f118._24_8_;
      *(undefined8 *)(pcVar7 + 0x10) = s_ERROR__cannot_update_UGCMod_with_140a5f118._16_8_;
      *(undefined8 *)(pcVar7 + 0x18) = uVar14;
      uVar14 = s_ERROR__cannot_update_UGCMod_with_140a5f118._40_8_;
      *(undefined8 *)(pcVar7 + 0x20) = s_ERROR__cannot_update_UGCMod_with_140a5f118._32_8_;
      *(undefined8 *)(pcVar7 + 0x28) = uVar14;
      *(undefined4 *)(pcVar7 + 0x30) = s_ERROR__cannot_update_UGCMod_with_140a5f118._48_4_;
      *(undefined2 *)(pcVar7 + 0x34) = s_ERROR__cannot_update_UGCMod_with_140a5f118._52_2_;
      pcVar7[0x36] = '\0';
      *(undefined1 *)(param_2 + 4) = 1;
    }
    else {
      puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
      plVar9 = (longlong *)*puVar8;
      pcVar2 = *(code **)(*plVar9 + 0x168);
      lVar6 = *param_3;
      puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5eb98);
      uVar5 = (**(code **)(*(longlong *)*puVar8 + 0x48))();
      lVar6 = (*pcVar2)(plVar9,uVar5,lVar6);
      if (lVar6 == -1) {
        *param_2 = 0;
        param_2[1] = 0;
        param_2[2] = 0;
        param_2[3] = 0;
        pcVar7 = (char *)FUN_140003270(0x50);
        *param_2 = pcVar7;
        param_2[2] = 0x43;
        param_2[3] = 0x4f;
        uVar14 = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._8_8_;
        *(undefined8 *)pcVar7 = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._0_8_;
        *(undefined8 *)(pcVar7 + 8) = uVar14;
        uVar14 = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._24_8_;
        *(undefined8 *)(pcVar7 + 0x10) = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._16_8_;
        *(undefined8 *)(pcVar7 + 0x18) = uVar14;
        uVar14 = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._40_8_;
        *(undefined8 *)(pcVar7 + 0x20) = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._32_8_;
        *(undefined8 *)(pcVar7 + 0x28) = uVar14;
        uVar14 = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._56_8_;
        *(undefined8 *)(pcVar7 + 0x30) = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._48_8_;
        *(undefined8 *)(pcVar7 + 0x38) = uVar14;
        *(undefined2 *)(pcVar7 + 0x40) = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150._64_2_;
        pcVar7[0x42] = s_ERROR_SteamUGC___>StartItemUpdat_140a5f150[0x42];
        pcVar7[0x43] = '\0';
        *(undefined1 *)(param_2 + 4) = 1;
      }
      else {
        puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
        uVar14 = 2;
        if ((char)param_3[1] != '\0') {
          uVar14 = 0;
        }
        (**(code **)(*(longlong *)*puVar8 + 400))((longlong *)*puVar8,lVar6,uVar14);
        puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
        plVar9 = param_3 + 2;
        if (0xf < (ulonglong)param_3[5]) {
          plVar9 = (longlong *)*plVar9;
        }
        (**(code **)(*(longlong *)*puVar8 + 0x170))((longlong *)*puVar8,lVar6,plVar9);
        if (*(char *)((longlong)param_3 + 0xf1) == '\0') {
          puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
          plVar9 = param_3 + 6;
          if (0xf < (ulonglong)param_3[9]) {
            plVar9 = (longlong *)*plVar9;
          }
          (**(code **)(*(longlong *)*puVar8 + 0x178))((longlong *)*puVar8,lVar6,plVar9);
        }
        local_98 = malloc(param_4[1] << 3);
        uStack_90 = CONCAT44(uStack_90._4_4_,*(undefined4 *)(param_4 + 1));
        plVar9 = *(longlong **)*param_4;
        if (*(char *)((longlong)plVar9 + 0x19) == '\0') {
          lVar13 = 0;
          do {
            switch(*(undefined4 *)((longlong)plVar9 + 0x1c)) {
            case 0:
              *(char **)(lVar13 + (longlong)local_98) = "Translation";
              break;
            case 1:
              *(char **)(lVar13 + (longlong)local_98) = "Trains";
              break;
            case 2:
              *(char **)(lVar13 + (longlong)local_98) = "Buildings";
              break;
            case 3:
              *(char **)(lVar13 + (longlong)local_98) = "Tracks";
              break;
            case 4:
              *(char **)(lVar13 + (longlong)local_98) = "Blueprints";
              break;
            case 5:
              *(char **)(lVar13 + (longlong)local_98) = "Locomotives";
              break;
            case 6:
              *(char **)(lVar13 + (longlong)local_98) = "Carriages";
              break;
            case 7:
              *(char **)(lVar13 + (longlong)local_98) = "Map styles";
              break;
            case 8:
              pcVar7 = "POIs and demand";
              goto LAB_1402e288f;
            case 9:
              pcVar7 = "Scripts";
              goto LAB_1402e288f;
            case 10:
              pcVar7 = "Signals";
LAB_1402e288f:
              *(char **)(lVar13 + (longlong)local_98) = pcVar7;
            }
            lVar13 = lVar13 + 8;
            plVar3 = (longlong *)plVar9[2];
            if (*(char *)((longlong)plVar3 + 0x19) == '\0') {
              cVar1 = *(char *)(*plVar3 + 0x19);
              plVar9 = plVar3;
              plVar3 = (longlong *)*plVar3;
              while (cVar1 == '\0') {
                cVar1 = *(char *)(*plVar3 + 0x19);
                plVar9 = plVar3;
                plVar3 = (longlong *)*plVar3;
              }
            }
            else {
              cVar1 = *(char *)(plVar9[1] + 0x19);
              plVar4 = (longlong *)plVar9[1];
              plVar3 = plVar9;
              while ((plVar9 = plVar4, cVar1 == '\0' && (plVar3 == (longlong *)plVar9[2]))) {
                cVar1 = *(char *)(plVar9[1] + 0x19);
                plVar4 = (longlong *)plVar9[1];
                plVar3 = plVar9;
              }
            }
            param_3 = local_res18;
            param_1 = local_res8;
            param_2 = local_res10;
            param_4 = local_res20;
          } while (*(char *)((longlong)plVar9 + 0x19) == '\0');
        }
        puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
        (**(code **)(*(longlong *)*puVar8 + 0x198))((longlong *)*puVar8,lVar6,&local_98,0);
        free(local_98);
        if (param_3[0x10] != 0) {
          FUN_140247b90(local_78);
          uVar15 = _UNK_140aac918;
          uVar14 = _DAT_140aac910;
          uStack_90 = 0;
          local_88 = _DAT_140aac910;
          uStack_80 = _UNK_140aac918;
          local_98 = (void *)0x0;
          ppppuVar11 = local_78;
          if (7 < local_60) {
            ppppuVar11 = (undefined8 ****)local_78[0];
          }
          ppppuVar12 = local_78;
          if (7 < local_60) {
            ppppuVar12 = (undefined8 ****)local_78[0];
          }
          FUN_140247700(&local_res8,ppppuVar12,(void *)((longlong)ppppuVar11 + local_68 * 2));
          puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
          (**(code **)(*(longlong *)*puVar8 + 0x1a0))((longlong *)*puVar8,lVar6);
          if (0xf < uStack_80) {
            pvVar10 = local_98;
            if ((0xfff < uStack_80 + 1) &&
               (pvVar10 = *(void **)((longlong)local_98 + -8),
               0x1f < (ulonglong)((longlong)local_98 + (-8 - (longlong)pvVar10)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(pvVar10);
            uVar14 = _DAT_140aac910;
            uVar15 = _UNK_140aac918;
          }
          local_98 = (void *)((ulonglong)local_98 & 0xffffffffffffff00);
          local_88 = uVar14;
          uStack_80 = uVar15;
          if (7 < local_60) {
            ppppuVar11 = (undefined8 ****)local_78[0];
            if ((0xfff < local_60 * 2 + 2) &&
               (ppppuVar11 = (undefined8 ****)local_78[0][-1],
               0x1f < (ulonglong)((longlong)local_78[0] + (-8 - (longlong)ppppuVar11)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppuVar11);
          }
        }
        if (((char)param_3[0x1e] == '\0') && (param_3[0xc] != 0)) {
          FUN_140247b90(local_78);
          uStack_90 = 0;
          local_88 = _DAT_140aac910;
          uStack_80 = _UNK_140aac918;
          local_98 = (void *)0x0;
          ppppuVar11 = local_78;
          if (7 < local_60) {
            ppppuVar11 = (undefined8 ****)local_78[0];
          }
          ppppuVar12 = local_78;
          if (7 < local_60) {
            ppppuVar12 = (undefined8 ****)local_78[0];
          }
          FUN_140247700(&local_res8,ppppuVar12,(void *)((longlong)ppppuVar11 + local_68 * 2));
          puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
          (**(code **)(*(longlong *)*puVar8 + 0x1a8))((longlong *)*puVar8,lVar6);
          if (0xf < uStack_80) {
            pvVar10 = local_98;
            if ((0xfff < uStack_80 + 1) &&
               (pvVar10 = *(void **)((longlong)local_98 + -8),
               0x1f < (ulonglong)((longlong)local_98 + (-8 - (longlong)pvVar10)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(pvVar10);
          }
          local_88 = _DAT_140aac910;
          uStack_80 = _UNK_140aac918;
          local_98 = (void *)((ulonglong)local_98 & 0xffffffffffffff00);
          if (7 < local_60) {
            ppppuVar11 = (undefined8 ****)local_78[0];
            if ((0xfff < local_60 * 2 + 2) &&
               (ppppuVar11 = (undefined8 ****)local_78[0][-1],
               0x1f < (ulonglong)((longlong)local_78[0] + (-8 - (longlong)ppppuVar11)))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(ppppuVar11);
          }
        }
        puVar8 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5ebc0);
        plVar9 = param_3 + 0x12;
        if (0xf < (ulonglong)param_3[0x15]) {
          plVar9 = (longlong *)*plVar9;
        }
        lVar6 = (**(code **)(*(longlong *)*puVar8 + 0x210))((longlong *)*puVar8,lVar6,plVar9);
        if (lVar6 == 0) {
          *param_2 = 0;
          param_2[1] = 0;
          param_2[2] = 0;
          param_2[3] = 0;
          pcVar7 = (char *)FUN_140003270(0x40);
          *param_2 = pcVar7;
          param_2[2] = 0x3d;
          param_2[3] = 0x3f;
          uVar14 = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._8_8_;
          *(undefined8 *)pcVar7 = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._0_8_;
          *(undefined8 *)(pcVar7 + 8) = uVar14;
          uVar14 = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._24_8_;
          *(undefined8 *)(pcVar7 + 0x10) = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._16_8_;
          *(undefined8 *)(pcVar7 + 0x18) = uVar14;
          uVar14 = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._40_8_;
          *(undefined8 *)(pcVar7 + 0x20) = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._32_8_;
          *(undefined8 *)(pcVar7 + 0x28) = uVar14;
          *(undefined8 *)(pcVar7 + 0x30) = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._48_8_;
          *(undefined4 *)(pcVar7 + 0x38) = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448._56_4_;
          pcVar7[0x3c] = s_ERROR_SteamUGC___>SubmitItemUpda_140a5f448[0x3c];
          pcVar7[0x3d] = '\0';
          *(undefined1 *)(param_2 + 4) = 1;
        }
        else {
          if (*(longlong *)(param_1 + 0x50) != 0) {
            SteamAPI_UnregisterCallResult(param_1 + 0x40);
          }
          *(longlong *)(param_1 + 0x50) = lVar6;
          *(longlong *)(param_1 + 0x58) = param_1;
          *(code **)(param_1 + 0x60) = FUN_1402e18e0;
          SteamAPI_RegisterCallResult(param_1 + 0x40,lVar6);
          *(undefined1 *)(param_2 + 4) = 0;
        }
      }
    }
  }
  else {
    lVar6 = FUN_1402d82e0("ugc_warn_already",
                          "A Mod is still being created and/or updated, please wait");
    *param_2 = 0;
    param_2[1] = 0;
    param_2[2] = 0;
    param_2[3] = 0;
    lVar13 = -1;
    do {
      lVar13 = lVar13 + 1;
    } while (*(char *)(lVar6 + lVar13) != '\0');
    FUN_140002c00(param_2,lVar6);
    *(undefined1 *)(param_2 + 4) = 1;
  }
  FUN_1402c5be0(param_4,param_4);
  return param_2;
}


// Incoming references
// 0xc15cb0 DATA caller none
// 0xa5f8d0 DATA caller none
