// Candidate VA 14049f050; RVA 0x49f050
// Ghidra inferred prototype: undefined FUN_14049f050()

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 *
FUN_14049f050(undefined8 *param_1,basic_ostream<char,std::char_traits<char>_> *param_2,
             longlong param_3)

{
  longlong lVar1;
  int iVar2;
  longlong lVar3;
  char *******pppppppcVar4;
  char *******pppppppcVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  uint uVar8;
  size_t _Size;
  longlong lVar9;
  undefined8 *_Src;
  undefined1 local_res20 [8];
  char *******local_6f8;
  undefined8 uStack_6f0;
  longlong local_6e8;
  ulonglong uStack_6e0;
  undefined8 *local_6d8;
  undefined4 local_6d0;
  undefined2 local_6cc;
  undefined2 local_6ca;
  undefined2 local_6c8;
  undefined2 local_6c6;
  undefined4 local_6c4;
  undefined4 local_6c0;
  undefined **local_6b0;
  undefined4 local_6a8;
  basic_ostream<char,std::char_traits<char>_> *local_6a0;
  undefined1 local_698;
  undefined7 uStack_697;
  undefined8 uStack_690;
  longlong local_688;
  ulonglong uStack_680;
  char local_678;
  undefined **local_668 [4];
  longlong *local_648;
  longlong *local_628;
  undefined8 local_600;
  undefined4 local_5f8;
  longlong local_5e8;
  longlong local_5e0;
  int local_5d8;
  int local_5d4;
  int local_5d0;
  int local_5cc;
  int local_5c8;
  undefined4 local_5c4;
  undefined4 local_5c0;
  undefined4 local_5bc;
  undefined4 local_5b8;
  undefined4 local_5b4;
  undefined4 local_5b0;
  undefined4 local_5ac;
  undefined4 local_5a8;
  undefined8 local_5a4;
  undefined8 uStack_59c;
  undefined8 local_594;
  undefined8 uStack_58c;
  undefined8 local_584;
  undefined8 uStack_57c;
  undefined8 local_574;
  undefined8 uStack_56c;
  undefined8 local_564;
  undefined8 uStack_55c;
  undefined8 local_554;
  undefined8 uStack_54c;
  undefined8 local_544;
  undefined8 uStack_53c;
  undefined8 local_534;
  undefined8 uStack_52c;
  undefined1 local_524 [1024];
  undefined8 local_124;
  undefined8 uStack_11c;
  undefined8 local_114;
  undefined8 uStack_10c;
  undefined8 local_104;
  undefined8 uStack_fc;
  undefined8 local_f4;
  undefined8 uStack_ec;
  undefined8 local_e4;
  undefined8 uStack_dc;
  undefined8 local_d4;
  undefined8 uStack_cc;
  undefined2 local_c4;
  undefined1 local_c2;
  undefined1 local_c1;
  undefined8 local_c0;
  undefined8 uStack_b8;
  undefined8 local_b0;
  undefined8 uStack_a8;
  longlong local_a0;
  basic_ostream<char,std::char_traits<char>_> local_98 [112];

  uStack_6f0 = 0;
  local_6e8 = _DAT_140aac910;
  uStack_6e0 = _UNK_140aac918;
  local_6f8 = (char *******)0x0;
  std::basic_streambuf<char,std::char_traits<char>_>::basic_streambuf<char,std::char_traits<char>_>
            ((basic_streambuf<char,std::char_traits<char>_> *)local_668);
  local_600 = 0;
  local_5f8 = 0;
  local_668[0] = `class_std::optional<std::basic_string<char,std::char_traits<char>,std::allocator<char>_>_>___cdecl_nimby::sync::saves::save_ostream(std::basic_ostream<char,std::char_traits<char>_>&___ptr64,nimby::sync::saves::Game_const&___ptr64)'
                 ::__l3::my_stringbuf::vftable;
  std::basic_ostream<char,std::char_traits<char>_>::basic_ostream<char,std::char_traits<char>_>
            (local_98,(basic_streambuf<char,std::char_traits<char>_> *)local_668,false);
  local_6b0 = serde::SerializerOStream::vftable;
  local_6a0 = local_98;
  local_6a8 = 0xe6;
  FUN_1404b5ea0(param_3,&local_6b0);
  if ((code *)local_6b0[2] != _guard_check_icall) {
    (*(code *)local_6b0[2])(&local_6b0);
  }
  lVar3 = *local_648;
  lVar9 = *local_628;
  FUN_1404b7020(&local_698);
  if (local_678 == '\0') {
    lVar3 = FUN_1402d82e0("error_saving_internal","Internal error while saving game");
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    lVar9 = -1;
    do {
      lVar9 = lVar9 + 1;
    } while (*(char *)(lVar3 + lVar9) != '\0');
    FUN_140002c00(param_1,lVar3);
    *(undefined1 *)(param_1 + 4) = 1;
    if (local_678 != '\0') {
      FUN_140002d30(&local_698);
    }
    std::basic_ostream<char,std::char_traits<char>_>::_vbase_destructor_(local_98);
    local_668[0] = std::basic_stringbuf<char,std::char_traits<char>,std::allocator<char>_>::vftable;
    FUN_140279b20(local_668);
    std::basic_streambuf<char,std::char_traits<char>_>::
    ~basic_streambuf<char,std::char_traits<char>_>
              ((basic_streambuf<char,std::char_traits<char>_> *)local_668);
    if (uStack_6e0 < 0x10) {
      return param_1;
    }
    pppppppcVar5 = local_6f8;
    if ((0xfff < uStack_6e0 + 1) &&
       (pppppppcVar5 = (char *******)local_6f8[-1],
       (char *)0x1f < (char *)((longlong)local_6f8 + (-8 - (longlong)pppppppcVar5)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
    if (0xf < uStack_6e0) {
      pppppppcVar5 = local_6f8;
      if ((0xfff < uStack_6e0 + 1) &&
         (pppppppcVar5 = (char *******)local_6f8[-1],
         (char *)0x1f < (char *)((longlong)local_6f8 + (-8 - (longlong)pppppppcVar5)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pppppppcVar5);
    }
    local_6f8 = (char *******)CONCAT71(uStack_697,local_698);
    uStack_6f0 = uStack_690;
    local_6e8 = local_688;
    uStack_6e0 = uStack_680;
    local_688 = _DAT_140aac910;
    uStack_680 = _UNK_140aac918;
    local_698 = 0;
    if (local_678 != '\0') {
      FUN_140002d30(&local_698);
    }
    std::basic_ostream<char,std::char_traits<char>_>::_vbase_destructor_(local_98);
    local_668[0] = std::basic_stringbuf<char,std::char_traits<char>,std::allocator<char>_>::vftable;
    FUN_140279b20(local_668);
    std::basic_streambuf<char,std::char_traits<char>_>::
    ~basic_streambuf<char,std::char_traits<char>_>
              ((basic_streambuf<char,std::char_traits<char>_> *)local_668);
    local_6d0 = 0x59424d4e;
    local_6cc = 2;
    local_6ca = (undefined2)DAT_140b77ca0;
    local_6c8 = DAT_140b77ca4;
    local_6c6 = DAT_140b77ca8;
    local_6c4 = DAT_140b77cac;
    local_6c0 = 0xe6;
    std::basic_ostream<char,std::char_traits<char>_>::write(param_2,(char *)&local_6d0,0x14);
    local_c1 = 0;
    local_5a4 = 0;
    uStack_59c = 0;
    local_594 = 0;
    uStack_58c = 0;
    local_584 = 0;
    uStack_57c = 0;
    local_574 = 0;
    uStack_56c = 0;
    local_564 = 0;
    uStack_55c = 0;
    local_554 = 0;
    uStack_54c = 0;
    local_544 = 0;
    uStack_53c = 0;
    local_534 = 0;
    uStack_52c = 0;
    memset(local_524,0,0x400);
    local_c0 = 0;
    uStack_b8 = 0;
    local_b0 = 0;
    uStack_a8 = 0;
    local_5e8 = (*(longlong *)(param_3 + 0x3b8) * 10000) / 1000000 + *(longlong *)(param_3 + 0x3b0);
    if (*(char *)(param_3 + 0x3c0) == '\0') {
      local_5e0 = (longlong)*(double *)(param_3 + 0x4f0);
    }
    else {
      local_5e0 = 0;
    }
    local_5d8 = (int)(*(longlong *)(param_3 + 0xb0) - *(longlong *)(param_3 + 0xa8) >> 3) *
                0x2c0685b5;
    local_5d4 = (int)(*(longlong *)(param_3 + 0x110) - *(longlong *)(param_3 + 0x108) >> 3) *
                0x4f72c235;
    local_5d0 = (int)(*(longlong *)(param_3 + 0xe0) - *(longlong *)(param_3 + 0xd8) >> 3) *
                0x26e978d5;
    local_5cc = (int)(*(longlong *)(param_3 + 0x170) - *(longlong *)(param_3 + 0x168) >> 3) *
                0x677d46cf;
    local_5c8 = (int)(*(longlong *)(param_3 + 0x140) - *(longlong *)(param_3 + 0x138) >> 7) *
                -0x33333333;
    local_5c4 = *(undefined4 *)(param_3 + 0xe28);
    local_5c0 = *(undefined4 *)(param_3 + 0xe2c);
    local_5bc = *(undefined4 *)(param_3 + 0xe30);
    local_5b8 = *(undefined4 *)(param_3 + 0xe34);
    local_124 = *(undefined8 *)(param_3 + 0xb58);
    uStack_11c = *(undefined8 *)(param_3 + 0xb60);
    local_114 = *(undefined8 *)(param_3 + 0xb68);
    uStack_10c = *(undefined8 *)(param_3 + 0xb70);
    lVar1 = *(longlong *)(param_3 + 0xb80);
    if (*(longlong *)(param_3 + 0xb78) == lVar1) {
      local_104 = 0;
      uStack_fc = 0;
      local_f4 = 0;
      uStack_ec = 0;
    }
    else {
      local_104 = *(undefined8 *)(lVar1 + -0x20);
      uStack_fc = *(undefined8 *)(lVar1 + -0x18);
      local_f4 = *(undefined8 *)(lVar1 + -0x10);
      uStack_ec = *(undefined8 *)(lVar1 + -8);
    }
    local_e4 = *(undefined8 *)(param_3 + 0xe38);
    uStack_dc = *(undefined8 *)(param_3 + 0xe40);
    local_d4 = *(undefined8 *)(param_3 + 0xe48);
    uStack_cc = *(undefined8 *)(param_3 + 0xe50);
    local_c4 = *(undefined2 *)(param_3 + 0xe58);
    local_c2 = *(undefined1 *)(param_3 + 0xe5a);
    local_5b4 = *(undefined4 *)(param_3 + 0x350);
    local_5b0 = *(undefined4 *)(param_3 + 0x354);
    local_5ac = *(undefined4 *)(param_3 + 0x358);
    local_5a8 = *(undefined4 *)(param_3 + 0x35c);
    local_5a4 = 0;
    uStack_59c = 0;
    local_594 = 0;
    uStack_58c = 0;
    local_584 = 0;
    uStack_57c = 0;
    local_574 = 0;
    uStack_56c = 0;
    local_564 = 0;
    uStack_55c = 0;
    local_554 = 0;
    uStack_54c = 0;
    local_544 = 0;
    uStack_53c = 0;
    local_534 = 0;
    uStack_52c = 0;
    _Src = (undefined8 *)(param_3 + 0x3e0);
    if (*(ulonglong *)(param_3 + 0x3f0) < 0x80) {
      if (0xf < *(ulonglong *)(param_3 + 0x3f8)) {
        _Src = (undefined8 *)*_Src;
      }
      memcpy(&local_5a4,_Src,*(ulonglong *)(param_3 + 0x3f0));
    }
    else {
      _Size = 0x7f;
      puVar7 = _Src;
      if (0xf < *(ulonglong *)(param_3 + 0x3f8)) {
        puVar7 = (undefined8 *)*_Src;
      }
      do {
        puVar6 = (undefined8 *)(_Size + (longlong)puVar7);
        local_6d8 = puVar7;
        do {
          if (local_6d8 == puVar6) break;
          iVar2 = FUN_1404c4ca0(&local_6d8,puVar6,local_res20);
        } while (iVar2 == 0);
        if (local_6d8 == puVar6) {
          if (0xf < *(ulonglong *)(param_3 + 0x3f8)) {
            _Src = (undefined8 *)*_Src;
          }
          memcpy(&local_5a4,_Src,_Size);
          break;
        }
        uVar8 = (int)_Size - 1;
        _Size = (size_t)uVar8;
      } while (0 < (int)uVar8);
    }
    memset(local_524,0,0x400);
    pppppppcVar5 = (char *******)&local_6f8;
    if (0xf < uStack_6e0) {
      pppppppcVar5 = local_6f8;
    }
    pppppppcVar4 = (char *******)&local_6f8;
    if (0xf < uStack_6e0) {
      pppppppcVar4 = local_6f8;
    }
    local_a0 = lVar9 - lVar3;
    FUN_1404b8ee0(pppppppcVar4,(char *)((longlong)pppppppcVar5 + local_6e8),&local_c0);
    std::basic_ostream<char,std::char_traits<char>_>::write(param_2,(char *)&local_5e8,0x550);
    pppppppcVar5 = (char *******)&local_6f8;
    if (0xf < uStack_6e0) {
      pppppppcVar5 = local_6f8;
    }
    std::basic_ostream<char,std::char_traits<char>_>::write(param_2,(char *)pppppppcVar5,local_6e8);
    std::basic_ostream<char,std::char_traits<char>_>::flush(param_2);
    *(undefined1 *)(param_1 + 4) = 0;
    if (uStack_6e0 < 0x10) {
      return param_1;
    }
    pppppppcVar5 = local_6f8;
    if ((0xfff < uStack_6e0 + 1) &&
       (pppppppcVar5 = (char *******)local_6f8[-1],
       (char *)0x1f < (char *)((longlong)local_6f8 + (-8 - (longlong)local_6f8[-1])))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  free(pppppppcVar5);
  return param_1;
}


// Incoming references
// 0xc26774 DATA caller none
// 0x4e4a29 UNCONDITIONAL_CALL caller 1404e47c0
// 0x4a06a4 UNCONDITIONAL_CALL caller 1404a00f0
