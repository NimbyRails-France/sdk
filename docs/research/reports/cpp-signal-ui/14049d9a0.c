// Candidate VA 14049d9a0; RVA 0x49d9a0
// Ghidra inferred prototype: undefined FUN_14049d9a0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 *
FUN_14049d9a0(undefined8 *param_1,basic_istream<char,std::char_traits<char>_> *param_2,
             undefined8 *param_3,ushort *param_4,undefined8 param_5,undefined8 ****param_6)

{
  code *pcVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char cVar5;
  bool bVar6;
  int iVar7;
  longlong lVar8;
  undefined8 *puVar9;
  uint *puVar10;
  void *pvVar11;
  undefined **_Dst;
  basic_istream<char,struct_std::char_traits<char>_> *pbVar12;
  undefined8 uVar13;
  undefined4 *puVar14;
  undefined8 ****ppppuVar15;
  undefined1 *_Memory;
  undefined8 ****ppppuVar16;
  char ****ppppcVar17;
  ulonglong _Size;
  ulonglong uVar18;
  longlong lVar19;
  undefined8 local_4a8;
  undefined8 uStack_4a0;
  uint *local_498;
  ulonglong uStack_490;
  uint *local_488;
  ulonglong uStack_480;
  undefined4 local_478;
  undefined4 uStack_474;
  undefined8 uStack_470;
  undefined4 local_468;
  uint *local_458;
  ulonglong uStack_450;
  undefined8 local_448;
  ulonglong uStack_440;
  undefined8 ***local_438;
  undefined8 uStack_430;
  undefined8 local_428;
  ulonglong local_420;
  undefined8 ***local_418;
  undefined8 uStack_410;
  undefined8 local_408;
  ulonglong local_400;
  char ***local_3f8 [3];
  ulonglong local_3e0;
  undefined1 local_3d8 [32];
  char local_3b8;
  undefined4 local_3b0;
  undefined4 uStack_3ac;
  undefined8 uStack_3a8;
  undefined4 local_3a0;
  undefined4 uStack_39c;
  undefined8 uStack_398;
  undefined4 local_390;
  undefined4 uStack_38c;
  undefined8 uStack_388;
  longlong local_380;
  longlong local_378;
  longlong local_368;
  longlong local_360;
  int iStack_34c;
  uint local_348;
  undefined4 uStack_344;
  undefined4 local_33c;
  undefined8 local_338 [2];
  undefined8 local_328;
  ulonglong uStack_320;
  undefined8 local_318;
  undefined8 uStack_310;
  undefined8 local_308;
  undefined8 uStack_300;
  undefined8 local_2f8;
  undefined8 uStack_2f0;
  uint *local_2e8;
  ulonglong uStack_2e0;
  undefined *local_2d8;
  undefined8 uStack_2d0;
  basic_ios<char,std::char_traits<char>_> local_2b8 [96];
  undefined **local_258;
  undefined1 *local_250;
  undefined8 local_248;
  ulonglong local_240;
  undefined1 local_238 [512];

  local_468 = 0;
  lVar8 = FUN_1402d82e0("loader_parsing_header","Parsing header");
  local_438 = (undefined8 ****)0x0;
  uStack_430 = 0;
  local_428 = 0;
  local_420 = 0;
  _Size = 0xffffffffffffffff;
  lVar19 = -1;
  do {
    lVar19 = lVar19 + 1;
  } while (*(char *)(lVar8 + lVar19) != '\0');
  FUN_140002c00(&local_438,lVar8);
  ppppuVar16 = param_6 + 4;
  iVar7 = _Mtx_lock(ppppuVar16);
  if (iVar7 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar1 = (code *)swi(3);
    puVar9 = (undefined8 *)(*pcVar1)();
    return puVar9;
  }
  if (*(int *)((longlong)param_6 + 0x6c) == 0x7fffffff) {
    *(undefined4 *)((longlong)param_6 + 0x6c) = 0x7ffffffe;
    std::_Throw_Cpp_error(6);
  }
  if (param_6 != &local_438) {
    ppppuVar15 = &local_438;
    if (0xf < local_420) {
      ppppuVar15 = (undefined8 ****)local_438;
    }
    FUN_140030630(param_6,ppppuVar15);
  }
  _Mtx_unlock(ppppuVar16);
  if (0xf < local_420) {
    ppppuVar15 = (undefined8 ****)local_438;
    if ((0xfff < local_420 + 1) &&
       (ppppuVar15 = (undefined8 ****)local_438[-1],
       0x1f < (ulonglong)((longlong)local_438 + (-8 - (longlong)ppppuVar15)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppuVar15);
  }
  std::basic_istream<char,std::char_traits<char>_>::seekg(param_2,0,2);
  std::basic_istream<char,std::char_traits<char>_>::tellg(param_2);
  std::basic_istream<char,std::char_traits<char>_>::seekg(param_2,0,0);
  cVar5 = FUN_14049b990(param_4,param_2);
  bVar6 = std::ios_base::eof((ios_base *)(param_2 + *(int *)(*(longlong *)param_2 + 4)));
  if ((bVar6) || (cVar5 == '\0')) {
    pvVar11 = (void *)FUN_1402d82e0("error_corrupted_file","Corrupted file: ");
    do {
      _Size = _Size + 1;
    } while (*(char *)((longlong)pvVar11 + _Size) != '\0');
    lVar8 = param_3[2];
    local_478 = (undefined4)lVar8;
    uStack_474 = (undefined4)((ulonglong)lVar8 >> 0x20);
    if (0x7fffffffffffffffU - lVar8 < _Size) {
LAB_14049e373:
                    /* WARNING: Subroutine does not return */
      FUN_140001c70();
    }
    param_6 = (undefined8 ****)param_3;
    if (0xf < (ulonglong)param_3[3]) {
      param_6 = (undefined8 ****)*param_3;
    }
    local_4a8 = (undefined **)0x0;
    uStack_4a0 = (uint *)0x0;
    local_488 = (uint *)(lVar8 + _Size);
    uVar18 = 0xf;
    _Dst = (undefined **)&local_4a8;
    if ((uint *)0xf < local_488) {
      uVar18 = (ulonglong)local_488 | 0xf;
      if (uVar18 < 0x8000000000000000) {
        if (uVar18 < 0x16) {
          uVar18 = 0x16;
        }
      }
      else {
        uVar18 = 0x7fffffffffffffff;
      }
      _Dst = (undefined **)FUN_140003270(uVar18 + 1);
      local_4a8 = _Dst;
    }
    local_498 = local_488;
    uStack_490 = uVar18;
    memcpy(_Dst,pvVar11,_Size);
    pvVar11 = (void *)(_Size + (longlong)_Dst);
LAB_14049e31a:
    memcpy(pvVar11,param_6,CONCAT44(uStack_474,local_478));
    *(undefined1 *)((longlong)local_488 + (longlong)_Dst) = 0;
LAB_14049e335:
    *(undefined4 *)param_1 = (undefined4)local_4a8;
    *(undefined4 *)((longlong)param_1 + 4) = local_4a8._4_4_;
    *(undefined4 *)(param_1 + 1) = (undefined4)uStack_4a0;
    *(undefined4 *)((longlong)param_1 + 0xc) = uStack_4a0._4_4_;
    param_1[2] = local_498;
    param_1[3] = uStack_490;
  }
  else {
    if (100 < *(uint *)(param_4 + 6)) {
      if (*(uint *)(param_4 + 6) < 0xe7) {
        std::basic_istream<char,std::char_traits<char>_>::tellg(param_2);
        if (local_360 + local_368 < local_378 + local_380) {
          lVar8 = ((local_378 - local_360) - local_368) + local_380;
          FUN_140023a60(local_3f8,lVar8,0);
          ppppcVar17 = local_3f8;
          if (0xf < local_3e0) {
            ppppcVar17 = (char ****)local_3f8[0];
          }
          pbVar12 = std::basic_istream<char,std::char_traits<char>_>::read
                              (param_2,(char *)ppppcVar17,lVar8);
          bVar6 = std::ios_base::operator_bool
                            ((ios_base *)(pbVar12 + *(int *)(*(longlong *)pbVar12 + 4)));
          if (bVar6) {
            lVar8 = FUN_1402d82e0("loader_decompressing","Decompressing");
            local_418 = (undefined8 ****)0x0;
            uStack_410 = 0;
            local_408 = 0;
            local_400 = 0;
            lVar19 = -1;
            do {
              lVar19 = lVar19 + 1;
            } while (*(char *)(lVar8 + lVar19) != '\0');
            FUN_140002c00(&local_418,lVar8);
            iVar7 = _Mtx_lock(ppppuVar16);
            if (iVar7 != 0) {
              std::_Throw_Cpp_error(5);
              pcVar1 = (code *)swi(3);
              puVar9 = (undefined8 *)(*pcVar1)();
              return puVar9;
            }
            if (*(int *)((longlong)param_6 + 0x6c) == 0x7fffffff) {
              *(undefined4 *)((longlong)param_6 + 0x6c) = 0x7ffffffe;
              std::_Throw_Cpp_error(6);
            }
            if (param_6 != &local_418) {
              ppppuVar15 = &local_418;
              if (0xf < local_400) {
                ppppuVar15 = (undefined8 ****)local_418;
              }
              FUN_140030630(param_6,ppppuVar15);
            }
            _Mtx_unlock(ppppuVar16);
            if (0xf < local_400) {
              ppppuVar16 = (undefined8 ****)local_418;
              if ((0xfff < local_400 + 1) &&
                 (ppppuVar16 = (undefined8 ****)local_418[-1],
                 0x1f < (ulonglong)((longlong)local_418 + (-8 - (longlong)ppppuVar16)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(ppppuVar16);
            }
            FUN_1404b6e80(local_3d8,local_3f8);
            if (local_3b8 == '\0') {
              uVar13 = FUN_1402d82e0("error_corrupted_file","Corrupted file: ");
              puVar14 = (undefined4 *)FUN_14022cde0(&local_4a8,uVar13,param_3);
              *param_1 = 0;
              param_1[1] = 0;
              param_1[2] = 0;
              param_1[3] = 0;
              uVar2 = puVar14[1];
              uVar3 = puVar14[2];
              uVar4 = puVar14[3];
              *(undefined4 *)param_1 = *puVar14;
              *(undefined4 *)((longlong)param_1 + 4) = uVar2;
              *(undefined4 *)(param_1 + 1) = uVar3;
              *(undefined4 *)((longlong)param_1 + 0xc) = uVar4;
              uVar2 = puVar14[5];
              uVar3 = puVar14[6];
              uVar4 = puVar14[7];
              *(undefined4 *)(param_1 + 2) = puVar14[4];
              *(undefined4 *)((longlong)param_1 + 0x14) = uVar2;
              *(undefined4 *)(param_1 + 3) = uVar3;
              *(undefined4 *)((longlong)param_1 + 0x1c) = uVar4;
              *(undefined1 *)puVar14 = 0;
              *(undefined8 *)(puVar14 + 4) = 0;
              *(undefined8 *)(puVar14 + 6) = 0xf;
              *(undefined1 *)(param_1 + 4) = 1;
              FUN_140002d30(&local_4a8);
              if (local_3b8 != '\0') {
                FUN_140002d30(local_3d8);
              }
              FUN_140002d30(local_3f8);
            }
            else {
              lVar8 = FUN_1402d82e0("loader_deserializing","Deserializing");
              local_458 = (uint *)0x0;
              uStack_450 = 0;
              local_448 = 0;
              uStack_440 = 0;
              do {
                _Size = _Size + 1;
              } while (*(char *)(lVar8 + _Size) != '\0');
              FUN_140002c00(&local_458,lVar8,_Size);
              FUN_14049d910(param_6,&local_458);
              if (0xf < uStack_440) {
                FUN_140003040(&local_458,local_458);
              }
              local_348 = 0x40a749c0;
              uStack_344 = 1;
              std::basic_ios<char,std::char_traits<char>_>::basic_ios<char,std::char_traits<char>_>
                        (local_2b8);
              local_468 = 0x40;
              std::basic_istream<char,std::char_traits<char>_>::
              basic_istream<char,std::char_traits<char>_>
                        ((basic_istream<char,std::char_traits<char>_> *)&local_348,
                         (basic_streambuf<char,std::char_traits<char>_> *)local_338,false);
              *(undefined ***)
               ((longlong)&local_348 + (longlong)*(int *)(CONCAT44(uStack_344,local_348) + 4)) =
                   std::basic_istringstream<char,std::char_traits<char>,std::allocator<char>_>::
                   vftable;
              iVar7 = *(int *)(CONCAT44(uStack_344,local_348) + 4);
              *(int *)((longlong)&iStack_34c + (longlong)iVar7) = iVar7 + -0x90;
              FUN_14041b810(local_338,local_3d8,1);
              local_4a8 = serde::DeserializerIStream::vftable;
              local_498 = &local_348;
              uStack_4a0 = (uint *)CONCAT44(uStack_4a0._4_4_,*(undefined4 *)(param_4 + 6));
              _guard_check_icall(&local_4a8,"nimby::sync::saves::Game");
              FUN_1404b6700(param_5,&local_4a8);
              (*(code *)local_4a8[2])(&local_4a8);
              FUN_14049e380(&local_348);
              if (local_3b8 != '\0') {
                FUN_140002d30(local_3d8);
              }
              *(undefined1 *)(param_1 + 4) = 0;
              FUN_140002d30(local_3f8);
            }
          }
          else {
            uVar13 = FUN_1402d82e0("error_corrupted_file","Corrupted file: ");
            puVar14 = (undefined4 *)FUN_14022cde0(&local_458,uVar13,param_3);
            *param_1 = 0;
            param_1[1] = 0;
            param_1[2] = 0;
            param_1[3] = 0;
            uVar2 = puVar14[1];
            uVar3 = puVar14[2];
            uVar4 = puVar14[3];
            *(undefined4 *)param_1 = *puVar14;
            *(undefined4 *)((longlong)param_1 + 4) = uVar2;
            *(undefined4 *)(param_1 + 1) = uVar3;
            *(undefined4 *)((longlong)param_1 + 0xc) = uVar4;
            uVar2 = puVar14[5];
            uVar3 = puVar14[6];
            uVar4 = puVar14[7];
            *(undefined4 *)(param_1 + 2) = puVar14[4];
            *(undefined4 *)((longlong)param_1 + 0x14) = uVar2;
            *(undefined4 *)(param_1 + 3) = uVar3;
            *(undefined4 *)((longlong)param_1 + 0x1c) = uVar4;
            *(undefined1 *)puVar14 = 0;
            *(undefined8 *)(puVar14 + 4) = 0;
            *(undefined8 *)(puVar14 + 6) = 0xf;
            *(undefined1 *)(param_1 + 4) = 1;
            FUN_140002d30(&local_458);
            FUN_140002d30(local_3f8);
          }
          goto LAB_14049e34d;
        }
        pvVar11 = (void *)FUN_1402d82e0("error_corrupted_file","Corrupted file: ");
        do {
          _Size = _Size + 1;
        } while (*(char *)((longlong)pvVar11 + _Size) != '\0');
        lVar8 = param_3[2];
        local_478 = (undefined4)lVar8;
        uStack_474 = (undefined4)((ulonglong)lVar8 >> 0x20);
        if (0x7fffffffffffffffU - lVar8 < _Size) goto LAB_14049e373;
        param_6 = (undefined8 ****)param_3;
        if (0xf < (ulonglong)param_3[3]) {
          param_6 = (undefined8 ****)*param_3;
        }
        local_4a8 = (undefined **)0x0;
        uStack_4a0 = (uint *)0x0;
        local_488 = (uint *)(lVar8 + _Size);
        uVar18 = 0xf;
        _Dst = (undefined **)&local_4a8;
        if ((uint *)0xf < local_488) {
          uVar18 = (ulonglong)local_488 | 0xf;
          if (uVar18 < 0x8000000000000000) {
            if (uVar18 < 0x16) {
              uVar18 = 0x16;
            }
          }
          else {
            uVar18 = 0x7fffffffffffffff;
          }
          _Dst = (undefined **)FUN_140003270(uVar18 + 1);
          local_4a8 = _Dst;
        }
        local_498 = local_488;
        uStack_490 = uVar18;
        memcpy(_Dst,pvVar11,_Size);
        pvVar11 = (void *)(_Size + (longlong)_Dst);
        goto LAB_14049e31a;
      }
      puVar10 = (uint *)FUN_1402d82e0("error_too_new_save_v",
                                      "The game version ({}.{}.{}.{:x}) that created this save file is newer than your game version ({}.{}.{}.{:x}). Update the game to load this file."
                                     );
      do {
        _Size = _Size + 1;
      } while (*(char *)((longlong)puVar10 + _Size) != '\0');
      local_488 = (uint *)CONCAT44(local_488._4_4_,(uint)param_4[2]);
      local_3b0 = *(undefined4 *)(param_4 + 4);
      local_3a0 = DAT_140b77ca0;
      local_390 = _DAT_140b77ca4;
      local_458 = (uint *)CONCAT44(local_458._4_4_,_DAT_140b77ca8);
      local_4a8 = (undefined **)CONCAT44(local_4a8._4_4_,DAT_140b77cac);
      local_348 = (uint)*param_4;
      uStack_344 = uStack_474;
      local_33c = uStack_470._4_4_;
      local_338[0] = CONCAT44(uStack_474,(uint)param_4[1]);
      local_328 = local_488;
      uStack_320 = uStack_480;
      local_318 = CONCAT44(uStack_3ac,local_3b0);
      uStack_310 = uStack_3a8;
      local_308 = CONCAT44(uStack_39c,DAT_140b77ca0);
      uStack_300 = uStack_398;
      local_2f8 = CONCAT44(uStack_38c,_DAT_140b77ca4);
      uStack_2f0 = uStack_388;
      local_2e8 = local_458;
      uStack_2e0 = uStack_450;
      local_2d8 = (undefined *)local_4a8;
      uStack_2d0 = uStack_4a0;
      local_478 = 0x21112222;
      uStack_474 = 0;
      uStack_4a0 = &local_348;
      local_248 = 0;
      local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_250 = local_238;
      local_240 = 500;
      local_4a8 = (undefined **)0x21112222;
      local_488 = puVar10;
      uStack_480 = _Size;
      local_458 = puVar10;
      uStack_450 = _Size;
      uStack_470 = uStack_4a0;
      FUN_140022830(&local_258,&local_458,&local_4a8);
      local_4a8 = (undefined **)0x0;
      uStack_4a0 = (uint *)0x0;
      local_498 = (uint *)0x0;
      uStack_490 = 0;
      FUN_140002c00(&local_4a8,local_250);
      local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_250 != local_238) {
        _Memory = local_250;
        if ((0xfff < local_240) &&
           (_Memory = *(undefined1 **)(local_250 + -8),
           (undefined1 *)0x1f < local_250 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      goto LAB_14049e335;
    }
    lVar8 = FUN_1402d82e0("error_too_old_save",
                          "The game version that created this save file is too old and it cannot be loaded. Try the v1.6 or newer legacy branches in Steam."
                         );
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    do {
      _Size = _Size + 1;
    } while (*(char *)(lVar8 + _Size) != '\0');
    FUN_140002c00(param_1,lVar8,_Size);
  }
  *(undefined1 *)(param_1 + 4) = 1;
LAB_14049e34d:
  FUN_140002d30(param_3);
  return param_1;
}


// Incoming references
// 0xc26750 DATA caller none
// 0x49e918 UNCONDITIONAL_CALL caller 14049e400
// 0x4aac27 UNCONDITIONAL_CALL caller 1404aa730
