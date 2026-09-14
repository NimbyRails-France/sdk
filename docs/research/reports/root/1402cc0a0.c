
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_1402cc0a0(undefined8 param_1)

{
  longlong lVar1;
  void *pvVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  ulonglong uVar5;
  uint uVar6;
  char cVar7;
  int iVar8;
  HRESULT HVar9;
  undefined8 uVar10;
  longlong **pplVar11;
  char *pcVar12;
  undefined8 *puVar13;
  basic_ostream<char,std::char_traits<char>_> *this;
  uintptr_t uVar14;
  longlong *plVar15;
  void *pvVar16;
  undefined8 uVar17;
  undefined1 *_Memory;
  ulonglong uVar18;
  char **ppcVar19;
  char **ppcVar20;
  undefined8 **_Dst;
  ulonglong uVar21;
  undefined8 **ppuVar22;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Da_00;
  undefined4 uVar23;
  longlong *local_res18;
  longlong **local_res20;
  undefined8 local_2c8;
  undefined8 uStack_2c0;
  undefined8 local_2b8;
  ulonglong uStack_2b0;
  longlong **local_2a8;
  undefined8 uStack_2a0;
  undefined8 **local_298;
  undefined8 local_290;
  undefined4 local_288;
  undefined8 local_284;
  undefined4 local_27c;
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [520];
  
  uVar17 = 0;
  uVar10 = FUN_1402cbb70();
  pplVar11 = _aligned_malloc(0x40,0x40);
  if (pplVar11 == (longlong **)0x0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  *pplVar11 = (longlong *)0x1;
  *(undefined1 *)(pplVar11 + 1) = 0;
  pplVar11[2] = (longlong *)0x0;
  pplVar11[3] = (longlong *)&PTR_FUN_140a02680;
  *pplVar11 = (longlong *)((longlong)*pplVar11 + 1);
  local_2c8 = &local_res18;
  local_res18 = (longlong *)0x0;
  local_res20 = pplVar11;
  local_2a8 = pplVar11;
  FUN_140004680(uVar10,&local_res20);
  if (local_res18 != (longlong *)0x0) {
    LOCK();
    lVar1 = *local_res18;
    *local_res18 = *local_res18 + -1;
    UNLOCK();
    if (lVar1 == 1) {
      (**(code **)local_res18[3])();
    }
  }
  LOCK();
  plVar15 = *pplVar11;
  *pplVar11 = (longlong *)((longlong)*pplVar11 + -1);
  UNLOCK();
  if (plVar15 == (longlong *)0x1) {
    (*(code *)*pplVar11[3])(pplVar11);
  }
  uVar10 = FUN_1402cbf90();
  pplVar11 = _aligned_malloc(0x40,0x40);
  if (pplVar11 == (longlong **)0x0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  *pplVar11 = (longlong *)0x1;
  *(undefined1 *)(pplVar11 + 1) = 0;
  pplVar11[2] = (longlong *)0x0;
  pplVar11[3] = (longlong *)&PTR_FUN_140a026a0;
  local_2c8._0_4_ = SUB84(pplVar11,0);
  local_2c8._4_4_ = (undefined4)((ulonglong)pplVar11 >> 0x20);
  *pplVar11 = (longlong *)((longlong)*pplVar11 + 1);
  local_2a8 = &local_res18;
  local_res18 = (longlong *)0x0;
  local_res20 = pplVar11;
  FUN_140004680(uVar10,&local_res20);
  if (local_res18 != (longlong *)0x0) {
    LOCK();
    lVar1 = *local_res18;
    *local_res18 = *local_res18 + -1;
    UNLOCK();
    if (lVar1 == 1) {
      (**(code **)local_res18[3])();
    }
  }
  LOCK();
  plVar15 = *pplVar11;
  *pplVar11 = (longlong *)((longlong)*pplVar11 + -1);
  UNLOCK();
  if (plVar15 == (longlong *)0x1) {
    (*(code *)*pplVar11[3])(pplVar11);
  }
  uVar5 = DAT_140b77bd8;
  if (DAT_140b77bd8 < 0xd) {
    if (0x7fffffffffffffff - (DAT_140b77bd8 >> 1) < DAT_140b77bd8) {
      uVar21 = 0x7fffffffffffffff;
    }
    else {
      uVar18 = DAT_140b77bd8 + (DAT_140b77bd8 >> 1);
      uVar21 = 0xf;
      if (0xf < uVar18) {
        uVar21 = uVar18;
      }
    }
    pcVar12 = (char *)FUN_140003270(uVar21 + 1);
    DAT_140b77bd0 = 0xd;
    DAT_140b77bd8 = uVar21;
    *(undefined8 *)pcVar12 = s_Weird_and_Wry_140a5e330._0_8_;
    *(undefined4 *)(pcVar12 + 8) = s_Weird_and_Wry_140a5e330._8_4_;
    pcVar12[0xc] = s_Weird_and_Wry_140a5e330[0xc];
    pcVar12[0xd] = '\0';
    if (0xf < uVar5) {
      FUN_140003040(&DAT_140b77bc0,DAT_140b77bc0,uVar5);
    }
  }
  else {
    ppcVar19 = &DAT_140b77bc0;
    if (0xf < DAT_140b77bd8) {
      ppcVar19 = (char **)DAT_140b77bc0;
    }
    DAT_140b77bd0 = 0xd;
    memmove(ppcVar19,"Weird and Wry",0xd);
    *(char *)((longlong)ppcVar19 + 0xd) = '\0';
    pcVar12 = DAT_140b77bc0;
  }
  DAT_140b77bc0 = pcVar12;
  uVar5 = DAT_140b77bf8;
  ppcVar19 = &DAT_140b77be0;
  if (DAT_140b77bf8 < 0xb) {
    if (0x7fffffffffffffff - (DAT_140b77bf8 >> 1) < DAT_140b77bf8) {
      uVar21 = 0x7fffffffffffffff;
    }
    else {
      uVar18 = (DAT_140b77bf8 >> 1) + DAT_140b77bf8;
      uVar21 = 0xf;
      if (0xf < uVar18) {
        uVar21 = uVar18;
      }
    }
    pcVar12 = (char *)FUN_140003270(uVar21 + 1);
    DAT_140b77bf0 = 0xb;
    DAT_140b77bf8 = uVar21;
    *(undefined8 *)pcVar12 = s_NIMBY_Rails_140a5e3b0._0_8_;
    *(undefined2 *)(pcVar12 + 8) = s_NIMBY_Rails_140a5e3b0._8_2_;
    pcVar12[10] = s_NIMBY_Rails_140a5e3b0[10];
    pcVar12[0xb] = '\0';
    if (0xf < uVar5) {
      FUN_140003040(&DAT_140b77be0,DAT_140b77be0,uVar5);
    }
  }
  else {
    ppcVar20 = &DAT_140b77be0;
    if (0xf < DAT_140b77bf8) {
      ppcVar20 = (char **)DAT_140b77be0;
    }
    DAT_140b77bf0 = 0xb;
    memmove(ppcVar20,"NIMBY Rails",0xb);
    *(char *)((longlong)ppcVar20 + 0xb) = '\0';
    pcVar12 = DAT_140b77be0;
  }
  DAT_140b77be0 = pcVar12;
  ppcVar20 = &DAT_140b77bc0;
  if (0xf < DAT_140b77bd8) {
    ppcVar20 = (char **)DAT_140b77bc0;
  }
  FUN_140030630(&DAT_140b77c00,ppcVar20,DAT_140b77bd0);
  if (0xf < DAT_140b77bf8) {
    ppcVar19 = (char **)DAT_140b77be0;
  }
  FUN_140030630(&DAT_140b77c20,ppcVar19,DAT_140b77bf0);
  uVar5 = DAT_140b77c58;
  DAT_140b77ca0 = 1;
  _DAT_140b77ca4 = 0x13;
  _DAT_140b77ca8 = 10;
  DAT_140b77cac = 0x5bfaea3;
  ppuVar22 = &DAT_140b77c40;
  if (DAT_140b77c58 < 7) {
    uVar18 = 0x7fffffffffffffff;
    if (DAT_140b77c58 <= 0x7fffffffffffffff - (DAT_140b77c58 >> 1)) {
      uVar21 = DAT_140b77c58 + (DAT_140b77c58 >> 1);
      uVar18 = 0xf;
      if (0xf < uVar21) {
        uVar18 = uVar21;
      }
    }
    puVar13 = (undefined8 *)FUN_140003270(uVar18 + 1);
    DAT_140b77c50 = 7;
    DAT_140b77c58 = uVar18;
    *(undefined4 *)puVar13 = _DAT_140a51db0;
    *(undefined2 *)((longlong)puVar13 + 4) = _DAT_140a51db4;
    *(undefined1 *)((longlong)puVar13 + 6) = (undefined1)u_0NIMBYRails_140a51db6[0];
    *(undefined1 *)((longlong)puVar13 + 7) = 0;
    if (0xf < uVar5) {
      FUN_140003040(&DAT_140b77c40,DAT_140b77c40,uVar5);
    }
  }
  else {
    _Dst = &DAT_140b77c40;
    if (0xf < DAT_140b77c58) {
      _Dst = (undefined8 **)DAT_140b77c40;
    }
    DAT_140b77c50 = 7;
    memmove(_Dst,&DAT_140a51db0,7);
    *(undefined1 *)((longlong)_Dst + 7) = 0;
    puVar13 = DAT_140b77c40;
  }
  DAT_140b77c40 = puVar13;
  if (0xf < DAT_140b77c58) {
    ppuVar22 = (undefined8 **)DAT_140b77c40;
  }
  local_290 = DAT_140b77c50;
  local_288 = DAT_140b77cac;
  local_284 = CONCAT44((uint)uStack_2c0,local_2c8._4_4_);
  local_27c = uStack_2c0._4_4_;
  local_2c8._0_4_ = 0x2d;
  local_2c8._4_4_ = 0;
  uStack_2c0 = &local_298;
  local_258 = 0;
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_260 = local_248;
  local_250 = 500;
  local_2a8 = (longlong **)&DAT_140a51da8;
  uStack_2a0 = 7;
  local_298 = ppuVar22;
  FUN_140022830(&local_268,&local_2a8,&local_2c8);
  local_2c8._0_4_ = 0;
  local_2c8._4_4_ = 0;
  uStack_2c0._0_4_ = 0;
  uStack_2c0._4_4_ = 0;
  local_2b8 = 0;
  uStack_2b0 = 0;
  FUN_140002c00(&local_2c8,local_260);
  local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_260 != local_248) {
    _Memory = local_260;
    if ((0xfff < local_250) &&
       (_Memory = *(undefined1 **)(local_260 + -8),
       (undefined1 *)0x1f < local_260 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  if (0xf < DAT_140b77c78) {
    pvVar2 = (void *)CONCAT44(DAT_140b77c60._4_4_,(undefined4)DAT_140b77c60);
    pvVar16 = pvVar2;
    if ((0xfff < DAT_140b77c78 + 1) &&
       (pvVar16 = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar16)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar16);
  }
  DAT_140b77c60._0_4_ = (undefined4)local_2c8;
  DAT_140b77c60._4_4_ = local_2c8._4_4_;
  uRam0000000140b77c68 = (uint)uStack_2c0;
  uRam0000000140b77c6c = uStack_2c0._4_4_;
  DAT_140b77c70 = local_2b8;
  DAT_140b77c78 = uStack_2b0;
  FUN_140030630(&DAT_140b77c80,"Release",7);
  DAT_140b77e58 = 1;
  uVar10 = FUN_140247b90(&local_298,param_1);
  cVar7 = FUN_1402f0a40(&DAT_140b77bc0,uVar10);
  if (cVar7 != '\0') {
    iVar8 = FUN_1400d13e0(0x11,1);
    if (iVar8 != 0) {
      local_2c8._0_4_ = 0;
      local_2c8._4_4_ = 0;
      uStack_2c0._0_4_ = 0;
      uStack_2c0._4_4_ = 0;
      local_2b8 = 0;
      uStack_2b0 = 0;
      pcVar12 = (char *)FUN_140003270(0x20);
      uVar4 = s_Cannot_configure_SQLite_140a5e3c8._12_4_;
      uVar3 = s_Cannot_configure_SQLite_140a5e3c8._8_4_;
      uVar23 = s_Cannot_configure_SQLite_140a5e3c8._4_4_;
      local_2c8._0_4_ = SUB84(pcVar12,0);
      local_2c8._4_4_ = (undefined4)((ulonglong)pcVar12 >> 0x20);
      local_2b8 = 0x17;
      uStack_2b0 = 0x1f;
      *(undefined4 *)pcVar12 = s_Cannot_configure_SQLite_140a5e3c8._0_4_;
      *(undefined4 *)(pcVar12 + 4) = uVar23;
      *(undefined4 *)(pcVar12 + 8) = uVar3;
      *(undefined4 *)(pcVar12 + 0xc) = uVar4;
      *(undefined4 *)(pcVar12 + 0x10) = s_Cannot_configure_SQLite_140a5e3c8._16_4_;
      *(undefined2 *)(pcVar12 + 0x14) = s_Cannot_configure_SQLite_140a5e3c8._20_2_;
      pcVar12[0x16] = s_Cannot_configure_SQLite_140a5e3c8[0x16];
      pcVar12[0x17] = '\0';
      FUN_1402eecf0(&local_2c8);
      if (0xf < uStack_2b0) {
        FUN_140003040(&local_2c8,CONCAT44(local_2c8._4_4_,(undefined4)local_2c8));
      }
    }
    iVar8 = FUN_1400d13e0(2,1);
    if (iVar8 != 0) {
      local_2c8._0_4_ = 0;
      local_2c8._4_4_ = 0;
      uStack_2c0._0_4_ = 0;
      uStack_2c0._4_4_ = 0;
      local_2b8 = 0;
      uStack_2b0 = 0;
      pcVar12 = (char *)FUN_140003270(0x30);
      uVar4 = s_Cannot_enable_SQLite_multithread_140a5e3e0._12_4_;
      uVar3 = s_Cannot_enable_SQLite_multithread_140a5e3e0._8_4_;
      uVar23 = s_Cannot_enable_SQLite_multithread_140a5e3e0._4_4_;
      local_2c8._0_4_ = SUB84(pcVar12,0);
      local_2c8._4_4_ = (undefined4)((ulonglong)pcVar12 >> 0x20);
      local_2b8 = 0x23;
      uStack_2b0 = 0x2f;
      *(undefined4 *)pcVar12 = s_Cannot_enable_SQLite_multithread_140a5e3e0._0_4_;
      *(undefined4 *)(pcVar12 + 4) = uVar23;
      *(undefined4 *)(pcVar12 + 8) = uVar3;
      *(undefined4 *)(pcVar12 + 0xc) = uVar4;
      uVar10 = s_Cannot_enable_SQLite_multithread_140a5e3e0._24_8_;
      *(undefined8 *)(pcVar12 + 0x10) = s_Cannot_enable_SQLite_multithread_140a5e3e0._16_8_;
      *(undefined8 *)(pcVar12 + 0x18) = uVar10;
      *(undefined2 *)(pcVar12 + 0x20) = s_Cannot_enable_SQLite_multithread_140a5e3e0._32_2_;
      pcVar12[0x22] = s_Cannot_enable_SQLite_multithread_140a5e3e0[0x22];
      pcVar12[0x23] = '\0';
      FUN_1402eecf0(&local_2c8);
      if (0xf < uStack_2b0) {
        FUN_140003040(&local_2c8,CONCAT44(local_2c8._4_4_,(undefined4)local_2c8));
      }
    }
    iVar8 = FUN_1400d0f90();
    if (iVar8 != 0) {
      local_2c8._0_4_ = 0;
      local_2c8._4_4_ = 0;
      uStack_2c0._0_4_ = 0;
      uStack_2c0._4_4_ = 0;
      local_2b8 = 0;
      uStack_2b0 = 0;
      pcVar12 = (char *)FUN_140003270(0x20);
      uVar10 = s_Cannot_initialize_SQLite_140a5e340._8_8_;
      local_2c8._0_4_ = SUB84(pcVar12,0);
      local_2c8._4_4_ = (undefined4)((ulonglong)pcVar12 >> 0x20);
      local_2b8 = 0x18;
      uStack_2b0 = 0x1f;
      *(undefined8 *)pcVar12 = s_Cannot_initialize_SQLite_140a5e340._0_8_;
      *(undefined8 *)(pcVar12 + 8) = uVar10;
      *(undefined8 *)(pcVar12 + 0x10) = s_Cannot_initialize_SQLite_140a5e340._16_8_;
      pcVar12[0x18] = '\0';
      FUN_1402eecf0(&local_2c8);
      if (0xf < uStack_2b0) {
        FUN_140003040(&local_2c8,CONCAT44(local_2c8._4_4_,(undefined4)local_2c8));
      }
    }
    FUN_1402eee70();
    FUN_1402abd40();
    FUN_14022b8f0();
    FUN_1407293b0(&DAT_140b77db0);
    uVar10 = FUN_140247b90(&local_298,param_1);
    cVar7 = FUN_1402cf2c0(uVar10);
    if (cVar7 != '\0') {
      puVar13 = (undefined8 *)FUN_1402e7450();
      iVar8 = (**(code **)*puVar13)(puVar13);
      if (iVar8 != 0) {
        this = (basic_ostream<char,std::char_traits<char>_> *)
               FUN_1402ccc90(cerr_exref,"Cannot initialize net engine");
        std::basic_ostream<char,std::char_traits<char>_>::operator<<(this,FUN_1402cce70);
      }
      DAT_140b77ab0 = SDL_CreateAsyncIOQueue();
      puVar13 = (undefined8 *)FUN_140983da8(0x10);
      *puVar13 = &DAT_140b77ab0;
      puVar13[1] = FUN_140246b40;
      uVar14 = _beginthreadex((void *)0x0,0,FUN_1402459b0,puVar13,0,(uint *)&uStack_2c0);
      uVar23 = uStack_2c0._4_4_;
      uVar6 = (uint)uStack_2c0;
      local_2c8._0_4_ = (undefined4)uVar14;
      local_2c8._4_4_ = (undefined4)(uVar14 >> 0x20);
      if (uVar14 == 0) {
        uStack_2c0._0_4_ = 0;
        std::_Throw_Cpp_error(6);
      }
      else {
        if (DAT_140b77b30 != 0) {
                    /* WARNING: Subroutine does not return */
          terminate();
        }
        local_2c8._0_4_ = 0;
        local_2c8._4_4_ = 0;
        uStack_2c0._0_4_ = 0;
        uStack_2c0._4_4_ = 0;
        DAT_140b77b30 = uVar6;
        _DAT_140b77b34 = uVar23;
        _DAT_140b77b28 = uVar14;
        FUN_1408d24b0(3);
        FUN_140243c00(&DAT_140b77aa0);
        cVar7 = FUN_1402e00b0();
        if (cVar7 == '\0') {
          FUN_140247530();
          FUN_1402433f0(DAT_140b77aa0);
          FUN_1408d2450();
          plVar15 = (longlong *)FUN_1402e7450();
          (**(code **)(*plVar15 + 8))(plVar15);
          goto LAB_1402cca3e;
        }
        FUN_1402d5d40(&DAT_140b77db0);
        FUN_1402d5490();
        HVar9 = CoInitializeEx((LPVOID)0x0,6);
        if (HVar9 < 0) {
          if (HVar9 == -0x7ffefefa) {
            DAT_140b77a32 = '\0';
          }
        }
        else {
          DAT_140b77a32 = '\x01';
        }
        FUN_1407255f0();
        if (DAT_140b819d0 != '\0') {
          uVar10 = FUN_1402ed700(&DAT_140b819a0);
          FUN_1405f5f00(&local_298,uVar10);
          FUN_14029ac90(&local_298);
          FUN_1402e0680();
          FUN_1402e0960();
          FUN_1402d8e30();
          FUN_14030e2d0();
          FUN_140415f70();
          FUN_140416120();
          pvVar16 = (void *)FUN_140984250(0xb40);
          uVar23 = extraout_XMM0_Da;
          if (pvVar16 != (void *)0x0) {
            memset(pvVar16,0,0xb40);
            uVar17 = FUN_14073ae40(pvVar16);
            uVar23 = extraout_XMM0_Da_00;
          }
          FUN_14073ac80(uVar23,uVar17);
          lVar1 = DAT_140b81998;
          uVar10 = FUN_1404f8380(&local_2c8);
          FUN_140232230(lVar1 + 0x10,uVar10);
          if (CONCAT44(uStack_2c0._4_4_,(uint)uStack_2c0) != 0) {
            FUN_140001c10();
          }
          FUN_14024d8e0(**(undefined8 **)(lVar1 + 0x10));
          FUN_140742bc0(DAT_140b81998);
          FUN_1402d4f40();
          uVar23 = FUN_14072ef10();
          uVar23 = FUN_14073ac40(uVar23,0);
          FUN_14073ac80(uVar23,0);
          if (DAT_140b77a32 != '\0') {
            CoUninitialize();
          }
          if (DAT_140b77aa8 != '\0') {
            DAT_140b77b39 = 1;
            FUN_140831fa0();
            DAT_140b77aa8 = '\0';
          }
          if (DAT_140b77a34 != '\0') {
            SDL_DestroyWindow(DAT_140b7b600);
            SDL_Quit();
            DAT_140b77a34 = '\0';
          }
          FUN_140247530();
          FUN_1402433f0(DAT_140b77aa0);
          FUN_1408d2450();
          plVar15 = (longlong *)FUN_1402e7450();
          (**(code **)(*plVar15 + 8))(plVar15);
          puVar13 = DAT_140b7b620;
          DAT_140b7b620 = (undefined8 *)0x0;
          if (puVar13 != (undefined8 *)0x0) {
            (**(code **)*puVar13)(puVar13,1);
          }
          puVar13 = DAT_140b7b5a0;
          DAT_140b7b5a0 = (undefined8 *)0x0;
          if (puVar13 != (undefined8 *)0x0) {
            (**(code **)*puVar13)(puVar13,1);
          }
          SteamAPI_Shutdown();
          if (DAT_140b7b628 != (HANDLE)0x0) {
            ReleaseMutex(DAT_140b7b628);
            CloseHandle(DAT_140b7b628);
          }
          FUN_140025470(param_1);
          return 0;
        }
      }
                    /* WARNING: Subroutine does not return */
      FUN_1402f2020();
    }
  }
LAB_1402cca3e:
  FUN_140025470(param_1);
  return 0xffffffff;
}

