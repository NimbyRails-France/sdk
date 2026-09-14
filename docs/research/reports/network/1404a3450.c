// Candidate VA 1404a3450; RVA 0x4a3450
// Ghidra inferred prototype: undefined FUN_1404a3450()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404a3450(longlong *param_1,longlong *param_2)

{
  undefined8 *puVar1;
  double _X;
  longlong lVar2;
  void *pvVar3;
  longlong *plVar4;
  undefined8 *puVar5;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  ulonglong uStack_30;
  
  if ((*param_2 != 0) && (lVar2 = FUN_1403ca140(*param_1 + 0x430), lVar2 != 0)) {
    _X = exp((double)param_2[3] * DAT_140aab8f0);
    atan(_X);
    FUN_1404a0ba0(param_1[1]);
    uStack_40 = 0;
    local_38 = _DAT_140aac960;
    uStack_30 = _UNK_140aac968;
    local_48 = (void *)(ulonglong)CONCAT14(s_class_140a5b394[4],s_class_140a5b394._0_4_);
    FUN_1404a0c80(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    uStack_40 = 0;
    local_38 = _DAT_140aac980;
    uStack_30 = _UNK_140aac988;
    local_48._0_6_ = CONCAT24(s_Station_140a71ea0._4_2_,s_Station_140a71ea0._0_4_);
    local_48 = (void *)(ulonglong)CONCAT16(s_Station_140a71ea0[6],(uint6)local_48);
    FUN_1404a13e0(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    uStack_40 = 0;
    local_38 = _DAT_140aac930;
    uStack_30 = _UNK_140aac938;
    local_48 = (void *)(ulonglong)DAT_140a4d880;
    FUN_1404a0c80(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    FUN_1404a1200(param_1[1],*param_2);
    uStack_40 = 0;
    local_38 = _DAT_140aac950;
    uStack_30 = _UNK_140aac958;
    local_48 = (void *)(ulonglong)DAT_140a4d7f8;
    FUN_1404a0c80(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    plVar4 = (longlong *)(lVar2 + 8);
    if ((char)param_2[8] == '\0') {
      plVar4 = param_2 + 4;
    }
    FUN_1404a13e0(param_1[1],plVar4);
    uStack_40 = 0;
    local_38 = _DAT_140aac970;
    uStack_30 = _UNK_140aac978;
    local_48._0_6_ = CONCAT24(s_lonlat_140a75cb8._4_2_,s_lonlat_140a75cb8._0_4_);
    local_48 = (void *)(ulonglong)(uint6)local_48;
    FUN_1404a0c80(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    FUN_1404a0ac0(param_1[1]);
    FUN_1404a0f50(param_1[1]);
    FUN_1404a0f50(param_1[1]);
    puVar1 = (undefined8 *)param_1[1];
    puVar1[2] = puVar1[2] + -0xc;
    std::basic_ostream<char,std::char_traits<char>_>::operator<<
              ((basic_ostream<char,std::char_traits<char>_> *)*puVar1,FUN_1402cce70);
    FUN_1404a1690(puVar1);
    FUN_1402ccc90(*puVar1,&DAT_140a4b2fc);
    uStack_40 = 0;
    local_38 = _DAT_140aac950;
    uStack_30 = _UNK_140aac958;
    local_48 = (void *)0x73676174;
    FUN_1404a0c80(param_1[1],&local_48);
    if (0xf < uStack_30) {
      pvVar3 = local_48;
      if ((0xfff < uStack_30 + 1) &&
         (pvVar3 = *(void **)((longlong)local_48 + -8),
         0x1f < (ulonglong)((longlong)local_48 + (-8 - (longlong)pvVar3)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar3);
    }
    FUN_1404a0ac0(param_1[1]);
    puVar1 = (undefined8 *)param_2[10];
    for (puVar5 = (undefined8 *)param_2[9]; puVar5 != puVar1; puVar5 = puVar5 + 1) {
      FUN_1404a1200(param_1[1],*puVar5);
    }
    puVar1 = (undefined8 *)param_1[1];
    puVar1[2] = puVar1[2] + -0xc;
    std::basic_ostream<char,std::char_traits<char>_>::operator<<
              ((basic_ostream<char,std::char_traits<char>_> *)*puVar1,FUN_1402cce70);
    FUN_1404a1690(puVar1);
    FUN_1402ccc90(*puVar1,&DAT_140a4b2fc);
    puVar1 = (undefined8 *)param_1[1];
    puVar1[2] = puVar1[2] + -0xc;
    std::basic_ostream<char,std::char_traits<char>_>::operator<<
              ((basic_ostream<char,std::char_traits<char>_> *)*puVar1,FUN_1402cce70);
    FUN_1404a1690(puVar1);
    FUN_1402ccc90(*puVar1,&DAT_140a508fc);
  }
  return;
}


// Incoming references
// 0xc26864 DATA caller none
// 0x4a1fbe UNCONDITIONAL_CALL caller 1404a1710
