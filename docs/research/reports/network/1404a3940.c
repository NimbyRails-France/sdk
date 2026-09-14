// Candidate VA 1404a3940; RVA 0x4a3940
// Ghidra inferred prototype: undefined FUN_1404a3940()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404a3940(longlong *param_1,undefined8 *param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  ulonglong uVar8;
  undefined8 uVar9;
  ulonglong uVar10;
  undefined8 uVar11;
  ulonglong uVar12;
  undefined8 uVar13;
  ulonglong uVar14;
  undefined8 uVar15;
  ulonglong uVar16;
  undefined8 uVar17;
  ulonglong uVar18;
  undefined8 *puVar19;
  void *pvVar20;
  undefined8 *puVar21;
  longlong lVar22;
  int iVar23;
  longlong lVar24;
  double local_res8;
  undefined8 local_d8;
  undefined4 local_d0;
  undefined8 local_cc;
  undefined4 local_c4;
  undefined8 local_c0;
  ulonglong uStack_b8;
  undefined8 local_b0;
  ulonglong uStack_a8;
  
  FUN_1404a0ba0(*param_1);
  uVar10 = _UNK_140aac968;
  uVar9 = _DAT_140aac960;
  uStack_b8 = 0;
  local_b0 = _DAT_140aac960;
  uStack_a8 = _UNK_140aac968;
  local_c0._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
  local_c0 = (void *)(ulonglong)(uint5)local_c0;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  uVar8 = _UNK_140aac958;
  uVar7 = _DAT_140aac950;
  uStack_b8 = 0;
  local_b0 = _DAT_140aac950;
  uStack_a8 = _UNK_140aac958;
  local_c0 = (void *)0x656e694c;
  FUN_1404a13e0(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  uStack_b8 = 0;
  local_b0 = _DAT_140aac930;
  uStack_a8 = _UNK_140aac938;
  local_c0 = (void *)(ulonglong)DAT_140a4d880;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  FUN_1404a1200(*param_1,*param_2);
  uStack_b8 = 0;
  local_b0 = uVar7;
  uStack_a8 = uVar8;
  local_c0 = (void *)(ulonglong)DAT_140a4d7f8;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  FUN_1404a13e0(*param_1,param_2 + 0xb);
  uStack_b8 = 0;
  local_b0 = uVar7;
  uStack_a8 = uVar8;
  local_c0 = (void *)0x65646f63;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  FUN_1404a13e0(*param_1,param_2 + 0xf);
  uStack_b8 = 0;
  local_b0 = uVar9;
  uStack_a8 = uVar10;
  local_c0._0_5_ = CONCAT14(s_color_140a5981c[4],s_color_140a5981c._0_4_);
  local_c0 = (void *)(ulonglong)(uint5)local_c0;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  FUN_1404a1020(*param_1,*(undefined4 *)(param_2 + 0x17));
  uStack_b8 = 0;
  local_b0 = uVar7;
  uStack_a8 = uVar8;
  local_c0 = (void *)0x73676174;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    FUN_140003040(&local_c0,local_c0);
  }
  FUN_1404a0ac0(*param_1);
  puVar19 = (undefined8 *)param_2[0x1a];
  for (puVar21 = (undefined8 *)param_2[0x19]; puVar21 != puVar19; puVar21 = puVar21 + 1) {
    FUN_1404a1200(*param_1,*puVar21);
  }
  puVar19 = (undefined8 *)*param_1;
  puVar19[2] = puVar19[2] + -0xc;
  std::basic_ostream<char,std::char_traits<char>_>::operator<<
            ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
  FUN_1404a1690(puVar19);
  FUN_1402ccc90(*puVar19,&DAT_140a4b2fc);
  uStack_b8 = 0;
  local_b0 = uVar9;
  uStack_a8 = uVar10;
  local_c0._0_5_ = CONCAT14(s_stops_140a75cd0[4],s_stops_140a75cd0._0_4_);
  local_c0 = (void *)(ulonglong)(uint5)local_c0;
  FUN_1404a0c80(*param_1,&local_c0);
  if (0xf < uStack_a8) {
    pvVar20 = local_c0;
    if ((0xfff < uStack_a8 + 1) &&
       (pvVar20 = *(void **)((longlong)local_c0 + -8),
       0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar20);
  }
  FUN_1404a0ac0(*param_1);
  uVar18 = _UNK_140aac9d8;
  uVar17 = _DAT_140aac9d0;
  uVar16 = _UNK_140aac9b8;
  uVar15 = _DAT_140aac9b0;
  uVar14 = _UNK_140aac9a8;
  uVar13 = _DAT_140aac9a0;
  uVar12 = _UNK_140aac988;
  uVar11 = _DAT_140aac980;
  uVar6 = _UNK_140aac948;
  uVar5 = _DAT_140aac940;
  iVar23 = 0;
  lVar24 = param_2[0x23];
  lVar2 = param_2[0x24];
  if (lVar24 != lVar2) {
    local_d8 = 0;
    local_d0 = 1;
    do {
      puVar19 = (undefined8 *)*param_1;
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      FUN_1404b78d0(puVar19 + 1,&local_d8);
      FUN_1402ccc90(*puVar19,&DAT_140a75b4c);
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
      FUN_1404a1690(puVar19);
      uStack_b8 = 0;
      local_b0 = uVar9;
      uStack_a8 = uVar10;
      local_c0._0_5_ = CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
      local_c0 = (void *)(ulonglong)(uint5)local_c0;
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      uStack_b8 = 0;
      local_b0 = uVar7;
      uStack_a8 = uVar8;
      local_c0 = (void *)0x706f7453;
      FUN_1404a13e0(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      uStack_b8 = 0;
      local_b0 = uVar5;
      uStack_a8 = uVar6;
      local_c0 = (void *)(ulonglong)(uint3)DAT_140a4c7c8;
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      puVar19 = (undefined8 *)*param_1;
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,iVar23);
      local_b0 = uVar17;
      uStack_a8 = uVar18;
      local_c0 = (void *)s_leg_distance_140a75cc0._0_8_;
      uStack_b8 = (ulonglong)(uint)s_leg_distance_140a75cc0._8_4_;
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      puVar19 = (undefined8 *)*param_1;
      local_res8 = (double)*(float *)(lVar24 + 0xc4);
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      uVar4 = *puVar19;
      puVar19 = (undefined8 *)FUN_1404b71e0(&local_c0,&local_res8);
      if (0xf < (ulonglong)puVar19[3]) {
        puVar19 = (undefined8 *)*puVar19;
      }
      FUN_1402e46d0(uVar4,puVar19);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      local_b0 = uVar15;
      uStack_a8 = uVar16;
      local_c0 = (void *)s_station_id_140a72588._0_8_;
      uStack_b8 = (ulonglong)(ushort)s_station_id_140a72588._8_2_;
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      FUN_1404a1200(*param_1,*(undefined8 *)(lVar24 + 0x110));
      uStack_b8 = 0;
      local_b0 = uVar11;
      uStack_a8 = uVar12;
      local_c0 = (void *)(ulonglong)
                         CONCAT16(s_arrival_140a75c58[6],
                                  CONCAT24(s_arrival_140a75c58._4_2_,s_arrival_140a75c58._0_4_));
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      puVar19 = (undefined8 *)*param_1;
      iVar1 = *(int *)(lVar24 + 0xb8);
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,iVar1);
      local_b0 = uVar13;
      uStack_a8 = uVar14;
      local_c0 = (void *)s_departure_140a75c48._0_8_;
      uStack_b8 = (ulonglong)(byte)s_departure_140a75c48[8];
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      puVar19 = (undefined8 *)*param_1;
      iVar1 = *(int *)(lVar24 + 0xbc);
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,iVar1);
      uStack_b8 = 0;
      local_b0 = uVar9;
      uStack_a8 = uVar10;
      local_c0._0_5_ = CONCAT14(s_areas_140a75c6c[4],s_areas_140a75c6c._0_4_);
      local_c0 = (void *)(ulonglong)(uint5)local_c0;
      FUN_1404a0c80(*param_1,&local_c0);
      if (0xf < uStack_a8) {
        pvVar20 = local_c0;
        if ((0xfff < uStack_a8 + 1) &&
           (pvVar20 = *(void **)((longlong)local_c0 + -8),
           0x1f < (ulonglong)((longlong)local_c0 + (-8 - (longlong)pvVar20)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar20);
      }
      puVar19 = (undefined8 *)*param_1;
      lVar3 = puVar19[2];
      if (puVar19[1] != lVar3) {
        if ((*(int *)(lVar3 + -0xc) != 0) && (*(int *)(lVar3 + -8) == 0)) {
          FUN_1402ccc90(*puVar19,&DAT_140a4d304);
        }
        *(undefined4 *)(puVar19[2] + -8) = 0;
        *(int *)(puVar19[2] + -0xc) = *(int *)(puVar19[2] + -0xc) + 1;
      }
      local_cc = 0;
      local_c4 = 0;
      FUN_1404b78d0(puVar19 + 1,&local_cc);
      FUN_1402ccc90(*puVar19,&DAT_140a75b50);
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
      FUN_1404a1690(puVar19);
      FUN_1404a2e70(param_1[1],lVar24 + 8);
      lVar3 = *(longlong *)(lVar24 + 0xf8);
      for (lVar22 = *(longlong *)(lVar24 + 0xf0); lVar22 != lVar3; lVar22 = lVar22 + 0xa8) {
        FUN_1404a2e70(param_1[1],lVar22);
      }
      puVar19 = (undefined8 *)*param_1;
      puVar19[2] = puVar19[2] + -0xc;
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
      FUN_1404a1690(puVar19);
      FUN_1402ccc90(*puVar19,&DAT_140a4b2fc);
      puVar19 = (undefined8 *)*param_1;
      puVar19[2] = puVar19[2] + -0xc;
      std::basic_ostream<char,std::char_traits<char>_>::operator<<
                ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
      FUN_1404a1690(puVar19);
      FUN_1402ccc90(*puVar19,&DAT_140a508fc);
      iVar23 = iVar23 + 1;
      lVar24 = lVar24 + 0x158;
    } while (lVar24 != lVar2);
  }
  puVar19 = (undefined8 *)*param_1;
  puVar19[2] = puVar19[2] + -0xc;
  std::basic_ostream<char,std::char_traits<char>_>::operator<<
            ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
  FUN_1404a1690(puVar19);
  FUN_1402ccc90(*puVar19,&DAT_140a4b2fc);
  puVar19 = (undefined8 *)*param_1;
  puVar19[2] = puVar19[2] + -0xc;
  std::basic_ostream<char,std::char_traits<char>_>::operator<<
            ((basic_ostream<char,std::char_traits<char>_> *)*puVar19,FUN_1402cce70);
  FUN_1404a1690(puVar19);
  FUN_1402ccc90(*puVar19,&DAT_140a508fc);
  return;
}


// Incoming references
// 0xc26870 DATA caller none
// 0x4a204f UNCONDITIONAL_CALL caller 1404a1710
