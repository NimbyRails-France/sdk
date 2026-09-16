
void FUN_1403489d0(ulonglong *param_1)

{
  int *piVar1;
  PSRWLOCK pRVar2;
  ulonglong uVar3;
  longlong *plVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  code *pcVar7;
  double dVar8;
  bool bVar9;
  undefined8 uVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  DWORD DVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  undefined1 (*pauVar18) [16];
  longlong lVar19;
  LARGE_INTEGER LVar20;
  ulonglong uVar21;
  ulonglong **ppuVar22;
  char cVar23;
  ulonglong *puVar24;
  longlong lVar25;
  undefined8 *puVar26;
  ulonglong *puVar27;
  ulonglong *puVar28;
  LARGE_INTEGER LVar29;
  undefined8 uVar30;
  LARGE_INTEGER LVar31;
  longlong lVar32;
  ulonglong uVar33;
  ulonglong *puVar34;
  ulonglong *puVar35;
  undefined1 uVar36;
  ulonglong *puVar37;
  undefined1 auVar38 [16];
  undefined4 uVar39;
  undefined4 uVar40;
  undefined4 uVar41;
  undefined4 uVar42;
  undefined4 uVar43;
  undefined4 uVar44;
  undefined4 uVar45;
  undefined4 uVar46;
  LARGE_INTEGER local_res10;
  ulonglong local_res18;
  uint local_res20;
  ulonglong *local_660;
  ulonglong *local_658;
  ulonglong *puStack_650;
  ulonglong *local_648;
  undefined8 local_640;
  undefined8 uStack_638;
  undefined8 local_630;
  undefined8 uStack_628;
  undefined8 local_618;
  undefined8 uStack_610;
  undefined8 local_608;
  undefined8 uStack_600;
  ulonglong *local_5f8;
  undefined8 local_5e8;
  undefined8 uStack_5e0;
  undefined8 local_5d8;
  undefined4 uStack_5d0;
  undefined4 uStack_5cc;
  undefined8 local_5c8;
  undefined4 local_5b8;
  LARGE_INTEGER local_5b0;
  LARGE_INTEGER local_5a8;
  LARGE_INTEGER local_5a0;
  LARGE_INTEGER local_598;
  LARGE_INTEGER local_590;
  LARGE_INTEGER local_588;
  ulonglong *local_580;
  ulonglong *local_578;
  undefined8 local_570;
  undefined4 local_568;
  undefined8 local_560;
  ulonglong *local_558;
  ulonglong *puStack_550;
  ulonglong *local_548;
  ulonglong *local_540;
  ulonglong *puStack_538;
  ulonglong *local_530;
  ulonglong *puStack_528;
  ulonglong *local_520;
  ulonglong *puStack_518;
  ulonglong *local_510;
  ulonglong *local_508;
  ulonglong *local_500;
  undefined8 local_4f8;
  undefined8 uStack_4f0;
  undefined8 local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  undefined8 uStack_4d0;
  ulonglong *local_4c8;
  ulonglong *puStack_4c0;
  ulonglong *local_4b8;
  LARGE_INTEGER local_4b0;
  LARGE_INTEGER local_4a8;
  PSRWLOCK local_4a0;
  LARGE_INTEGER local_498 [2];
  undefined1 local_488;
  char local_487;
  undefined1 local_480 [8];
  undefined8 *local_478;
  ulonglong local_470;
  undefined8 local_468;
  undefined4 local_460;
  undefined8 local_45c;
  undefined1 local_450 [8];
  undefined8 *local_448;
  ulonglong local_440;
  undefined8 local_438;
  undefined4 local_430;
  undefined8 local_42c;
  undefined1 local_40c;
  void *local_408;
  undefined8 uStack_400;
  undefined8 local_3f8;
  void *local_3f0;
  undefined8 local_3e8;
  undefined8 uStack_3e0;
  undefined8 local_3d8;
  undefined8 uStack_3d0;
  undefined8 local_3c8;
  undefined8 local_3c0;
  undefined8 local_3b8;
  undefined8 uStack_3b0;
  void *local_3a8;
  undefined8 uStack_3a0;
  undefined8 local_398;
  undefined1 local_390 [8];
  void *local_388;
  ulonglong local_380;
  char local_360;
  undefined1 local_358 [8];
  undefined8 *local_350;
  ulonglong local_348;
  undefined8 local_340;
  undefined4 local_338;
  undefined8 local_334;
  ulonglong local_328;
  ulonglong local_320;
  ulonglong local_318;
  ulonglong local_310;
  ulonglong local_308;
  ulonglong local_300;
  ulonglong local_2f8;
  ulonglong local_2f0;
  ulonglong local_2e8;
  ulonglong local_2e0;
  ulonglong local_2d8;
  ulonglong local_2d0;
  ulonglong local_2c8;
  ulonglong local_2c0;
  undefined8 local_2b8;
  undefined8 local_2b0;
  undefined8 uStack_2a8;
  undefined8 local_2a0;
  undefined8 uStack_298;
  undefined1 local_288 [368];
  undefined8 *local_118;
  undefined8 *local_110;
  undefined1 local_f8 [32];
  undefined1 local_d8 [152];

  local_res20 = 0;
  DVar14 = GetCurrentThreadId();
  FUN_1402f1b60(DVar14);
  FUN_1401901c0("SimRunner::run");
  local_res10.QuadPart = 0;
  QueryPerformanceCounter(&local_res10);
  LVar29 = local_res10;
  local_res10.QuadPart = 0;
  QueryPerformanceCounter(&local_res10);
  LVar31 = local_res10;
  local_res10.QuadPart = 0;
  QueryPerformanceCounter(&local_res10);
  local_res10.QuadPart = 0;
  QueryPerformanceCounter(&local_res10);
  dVar13 = DAT_140aabc88;
  dVar12 = DAT_140aabb00;
  dVar11 = DAT_140aaba78;
  local_2b8 = 0;
  local_2b0 = 0;
  uStack_2a8 = 0;
  local_2a0 = 0;
  uStack_298 = 0;
  cVar23 = (char)param_1[1];
  do {
    if (cVar23 != '\0') {
      FUN_14034bc60(&uStack_2a8);
      return;
    }
    if (*(char *)(*param_1 + 0x40) == '\0') {
      local_res10.QuadPart = 100;
      FUN_14026ea10(&local_res10);
      local_5b0.QuadPart = 0;
      QueryPerformanceCounter(&local_5b0);
      LVar29 = local_5b0;
    }
    else {
      if (*(char *)(*param_1 + 0x1c0) != '\0') {
        local_res10.QuadPart = 5;
        FUN_14026ea10(&local_res10);
      }
      local_487 = '\0';
      local_460 = 0x3f800000;
      local_45c = 0x40000000;
      local_470 = 1;
      local_478 = &DAT_140b5abc0;
      local_468 = 0;
      local_430 = 0x3f800000;
      local_42c = 0x40000000;
      local_440 = 1;
      local_448 = &DAT_140b5abc0;
      local_438 = 0;
      local_40c = 0;
      local_408 = (void *)0x0;
      uStack_400 = 0;
      local_3f8 = 0;
      local_3f0 = (void *)0x0;
      local_3e8 = 0;
      uStack_3e0 = 0;
      local_3d8 = 0;
      uStack_3d0 = 0;
      local_3c8 = 0;
      local_3c0 = 0;
      local_3b8 = 0;
      uStack_3b0 = 0;
      local_3a8 = (void *)0x0;
      uStack_3a0 = 0;
      local_398 = 0;
      local_360 = '\0';
      local_338 = 0x3f800000;
      local_334 = 0x40000000;
      local_348 = 1;
      local_350 = &DAT_140b5abc0;
      local_340 = 0;
      local_558 = (ulonglong *)0x0;
      puStack_550 = (ulonglong *)0x0;
      local_548 = (ulonglong *)0x0;
      local_4c8 = (ulonglong *)0x0;
      puStack_4c0 = (ulonglong *)0x0;
      local_4b8 = (ulonglong *)0x0;
      if ((*(char *)(*param_1 + 0x40) == '\0') || (*(char *)(*param_1 + 0x41) == '\0')) {
        uVar33 = *param_1;
LAB_140348cad:
        cVar23 = '\0';
      }
      else {
        uVar33 = *param_1;
        if (*(char *)(*(longlong *)(uVar33 + 0x140) + 0x2114) == '\0') goto LAB_140348cad;
        cVar23 = '\x01';
      }
      local_res10.s.LowPart._0_1_ = cVar23;
      if ((*(char *)(uVar33 + 0x40) == '\0') || (*(char *)(uVar33 + 0x41) == '\0')) {
        local_660 = (ulonglong *)0x0;
        local_5f8 = (ulonglong *)0x0;
      }
      else {
        if (cVar23 == '\0') {
          uVar33 = *(ulonglong *)(*(longlong *)(*param_1 + 0x140) + 0x2128);
        }
        else {
          uVar33 = 0xffffffffffffffff;
        }
        uVar3 = *param_1;
        uVar21 = uVar3 + 0x500;
        local_res18 = uVar21;
        FUN_1400020a0(uVar21);
        local_658 = (ulonglong *)0x0;
        puStack_650 = (ulonglong *)0x0;
        puVar24 = (ulonglong *)0x0;
        local_648 = (ulonglong *)0x0;
        local_res20 = local_res20 | 0x10;
        lVar25 = *(longlong *)(uVar3 + 0x3e0);
        uStack_638 = *(undefined8 *)(uVar3 + 1000);
        local_630 = *(undefined8 *)(uVar3 + 0x3f0);
        uStack_628 = *(undefined8 *)(uVar3 + 0x3f8);
        local_640 = lVar25;
        if (lVar25 == *(longlong *)(uVar3 + 0x400)) {
          local_660 = (ulonglong *)0x0;
        }
        else {
          local_660 = (ulonglong *)0x0;
          local_630._4_4_ = (undefined4)((ulonglong)local_630 >> 0x20);
          uStack_628._4_4_ = (undefined4)((ulonglong)uStack_628 >> 0x20);
          local_640._0_4_ = (undefined4)lVar25;
          local_640._4_4_ = (undefined4)((ulonglong)lVar25 >> 0x20);
          uStack_638._4_4_ = (undefined4)((ulonglong)uStack_638 >> 0x20);
          uVar39 = (undefined4)local_640;
          uVar40 = local_640._4_4_;
          uVar41 = (undefined4)uStack_638;
          uVar42 = uStack_638._4_4_;
          uVar43 = (undefined4)local_630;
          uVar44 = local_630._4_4_;
          uVar45 = (undefined4)uStack_628;
          uVar46 = uStack_628._4_4_;
          do {
            if (uVar33 < *(ulonglong *)(lVar25 + 8)) break;
            if (local_660 < puVar24) {
              FUN_14034c8b0(local_660);
              puStack_650 = local_660 + 0xb;
            }
            else {
              FUN_14034cb10(&local_658);
              puVar24 = local_648;
            }
            local_660 = puStack_650;
            local_5e8 = (ulonglong *)CONCAT44(uVar40,uVar39);
            uStack_5e0 = (ulonglong *)CONCAT44(uVar42,uVar41);
            local_5d8 = (PSRWLOCK)CONCAT44(uVar44,uVar43);
            uStack_5d0 = uVar45;
            uStack_5cc = uVar46;
            pauVar18 = (undefined1 (*) [16])FUN_14034c050(uVar3 + 0x3d0,local_f8,&local_5e8);
            auVar38 = *pauVar18;
            uVar39 = auVar38._0_4_;
            uVar40 = auVar38._4_4_;
            uVar41 = auVar38._8_4_;
            uVar42 = auVar38._12_4_;
            uVar43 = *(undefined4 *)pauVar18[1];
            uVar44 = *(undefined4 *)(pauVar18[1] + 4);
            uVar45 = *(undefined4 *)(pauVar18[1] + 8);
            uVar46 = *(undefined4 *)(pauVar18[1] + 0xc);
            lVar25 = auVar38._0_8_;
          } while (lVar25 != *(longlong *)(uVar3 + 0x400));
        }
        _Mtx_unlock(uVar21);
        puVar28 = local_658;
        local_5f8 = local_658;
        local_4c8 = local_658;
        puStack_4c0 = local_660;
        local_res20 = local_res20 & 0xffffffef;
        local_4b8 = puVar24;
        FUN_14034ce20(0,0);
        if (puVar28 != local_660) {
          plVar4 = (longlong *)*param_1;
          pRVar2 = (PSRWLOCK)(plVar4 + 0x98);
          AcquireSRWLockExclusive(pRVar2);
          lVar25 = plVar4[0x28];
          lVar19 = *plVar4;
          uStack_600 = CONCAT71(uStack_600._1_7_,1);
          local_618 = lVar19;
          uStack_610 = lVar25;
          local_608 = pRVar2;
          if ((lVar19 != 0) && (lVar25 != 0)) {
            puVar24 = local_5f8 + 6;
            do {
              if ((char)puVar24[4] != '\0') {
                uVar33 = *param_1;
                if (*(longlong *)(uVar33 + 0x228) != 0) {
                  LOCK();
                  piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
                  *piVar1 = *piVar1 + 1;
                  UNLOCK();
                }
                local_640 = *(longlong *)(uVar33 + 0x220);
                uStack_638 = *(undefined8 *)(uVar33 + 0x228);
                FUN_14030e020(lVar19,lVar25,&local_640,puVar24,&local_558);
              }
              puVar28 = puVar24 + 5;
              puVar24 = puVar24 + 0xb;
            } while (puVar28 != local_660);
          }
          ReleaseSRWLockExclusive(pRVar2);
        }
      }
      if ((*(char *)(*param_1 + 0x40) == '\0') || (*(char *)(*param_1 + 0x41) == '\0')) {
        bVar9 = false;
      }
      else {
        bVar9 = true;
      }
      if (bVar9) {
        FUN_140347520(*param_1,local_288);
        FUN_140348270(&local_488,local_288);
        if (local_487 != '\0') {
          LOCK();
          *(undefined1 *)(*param_1 + 0x200) = local_488;
          UNLOCK();
        }
        local_5a8.QuadPart = 0;
        QueryPerformanceCounter(&local_5a8);
        lVar25 = local_5a8.QuadPart - (longlong)LVar31;
        if (DAT_140b5ebf0.QuadPart == -1) {
          QueryPerformanceFrequency(&local_4b0);
          DAT_140b5ebf0 = local_4b0;
        }
        puVar5 = local_110;
        uVar36 = local_res10.s.LowPart._0_1_;
        if ((dVar13 <= (double)lVar25 / (double)DAT_140b5ebf0.QuadPart) || (local_110 != local_118))
        {
          local_658 = (ulonglong *)0x0;
          puStack_650 = (ulonglong *)0x0;
          local_648 = (ulonglong *)0x0;
          local_5e8 = (ulonglong *)0x0;
          uStack_5e0 = (ulonglong *)0x0;
          local_5d8 = (PSRWLOCK)0x0;
          local_5c8 = 0;
          uVar10 = local_5d8;
          uVar30 = local_5c8;
          for (puVar26 = local_118; local_5d8 = (PSRWLOCK)uVar10, local_5c8 = uVar30,
              puVar26 != puVar5; puVar26 = puVar26 + 1) {
            iVar15 = (**(code **)(*(longlong *)*puVar26 + 0x28))();
            if ((iVar15 == 1) ||
               ((iVar15 = (**(code **)(*(longlong *)*puVar26 + 0x28))(), iVar15 == 2 &&
                (uVar36 != '\0')))) {
              ppuVar22 = &local_658;
            }
            else {
              ppuVar22 = (ulonglong **)&local_5e8;
            }
            FUN_14034af80(ppuVar22,puVar26);
            uVar10 = local_5d8;
            uVar30 = local_5c8;
          }
          plVar4 = (longlong *)*param_1;
          AcquireSRWLockShared((PSRWLOCK)(plVar4 + 0x98));
          if (*plVar4 != 0) {
            uVar30 = *(undefined8 *)(plVar4[0x10] + 0x2128);
            local_5c8 = uVar30;
          }
          ReleaseSRWLockShared((PSRWLOCK)(plVar4 + 0x98));
          uVar21 = *param_1;
          local_580 = local_5e8;
          local_5e8 = (ulonglong *)0x0;
          local_578 = uStack_5e0;
          uStack_5e0 = (ulonglong *)0x0;
          local_5d8 = (PSRWLOCK)0x0;
          local_568 = uStack_5d0;
          local_res10.QuadPart = (LONGLONG)&local_580;
          uVar33 = uVar21 + 0x500;
          local_res18 = uVar33;
          local_570 = uVar10;
          local_560 = uVar30;
          iVar15 = _Mtx_lock(uVar33);
          if (iVar15 != 0) {
            std::_Throw_Cpp_error(5);
LAB_14034a238:
            *(undefined4 *)(uVar33 + 0x5cc) = 0x7ffffffe;
            std::_Throw_Cpp_error(6);
            pcVar7 = (code *)swi(3);
            (*pcVar7)();
            return;
          }
          if (*(int *)(uVar21 + 0x54c) == 0x7fffffff) {
            *(undefined4 *)(uVar21 + 0x54c) = 0x7ffffffe;
            std::_Throw_Cpp_error(6);
            pcVar7 = (code *)swi(3);
            (*pcVar7)();
            return;
          }
          puVar5 = *(undefined8 **)(uVar21 + 0x488);
          if (puVar5 < *(undefined8 **)(uVar21 + 0x490)) {
            *puVar5 = 0;
            puVar5[1] = 0;
            puVar5[2] = 0;
            *puVar5 = local_580;
            local_580 = (ulonglong *)0x0;
            puVar24 = (ulonglong *)puVar5[1];
            puVar5[1] = local_578;
            uVar30 = puVar5[2];
            puVar5[2] = local_570;
            *(undefined4 *)(puVar5 + 3) = local_568;
            puVar5[4] = local_560;
            *(longlong *)(uVar21 + 0x488) = *(longlong *)(uVar21 + 0x488) + 0x28;
            local_578 = puVar24;
            local_570 = uVar30;
          }
          else {
            FUN_14034c990(uVar21 + 0x480,&local_580);
          }
          _Mtx_unlock(uVar33);
          puVar28 = local_578;
          for (puVar24 = local_580; puVar24 != puVar28; puVar24 = puVar24 + 1) {
            puVar5 = (undefined8 *)*puVar24;
            if (puVar5 != (undefined8 *)0x0) {
              (**(code **)*puVar5)(puVar5,1);
            }
          }
          if (local_580 != (ulonglong *)0x0) {
            free(local_580);
          }
          local_5a0.QuadPart = 0;
          QueryPerformanceCounter(&local_5a0);
          LVar31 = local_5a0;
          puVar28 = puStack_650;
          puVar24 = local_658;
          if (local_658 != puStack_650) {
            plVar4 = (longlong *)*param_1;
            pRVar2 = (PSRWLOCK)(plVar4 + 0x98);
            AcquireSRWLockExclusive(pRVar2);
            uStack_610 = plVar4[0x28];
            local_618 = *plVar4;
            uStack_600 = CONCAT71(uStack_600._1_7_,1);
            local_608 = pRVar2;
            if ((local_618 != 0) && (uStack_610 != 0)) {
              uVar33 = *param_1;
              if (*(longlong *)(uVar33 + 0x228) != 0) {
                LOCK();
                piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
                *piVar1 = *piVar1 + 1;
                UNLOCK();
              }
              local_640 = *(longlong *)(uVar33 + 0x220);
              uStack_638 = *(undefined8 *)(uVar33 + 0x228);
              FUN_14030e020(local_618,uStack_610,&local_640,&local_658,&local_558);
              puVar28 = puStack_650;
              puVar24 = local_658;
            }
            ReleaseSRWLockExclusive(pRVar2);
          }
          FUN_14034bc60(&local_5e8);
          for (puVar35 = puVar24; puVar35 != puVar28; puVar35 = puVar35 + 1) {
            puVar5 = (undefined8 *)*puVar35;
            if (puVar5 != (undefined8 *)0x0) {
              (**(code **)*puVar5)(puVar5,1);
            }
          }
          if (puVar24 != (ulonglong *)0x0) {
            free(puVar24);
          }
        }
        FUN_14034bc60(&local_118);
        FUN_140347150(local_288);
      }
      else {
        FUN_140347520(*param_1,local_288);
        FUN_140348270(&local_488,local_288);
        if (local_487 != '\0') {
          LOCK();
          *(undefined1 *)(*param_1 + 0x200) = local_488;
          UNLOCK();
        }
        if (local_118 != local_110) {
          plVar4 = (longlong *)*param_1;
          pRVar2 = (PSRWLOCK)(plVar4 + 0x98);
          AcquireSRWLockExclusive(pRVar2);
          uStack_610 = plVar4[0x28];
          local_618 = *plVar4;
          uStack_600 = CONCAT71(uStack_600._1_7_,1);
          local_608 = pRVar2;
          if ((local_618 != 0) && (uStack_610 != 0)) {
            uVar33 = *param_1;
            if (*(longlong *)(uVar33 + 0x228) != 0) {
              LOCK();
              piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
              *piVar1 = *piVar1 + 1;
              UNLOCK();
            }
            local_640 = *(longlong *)(uVar33 + 0x220);
            uStack_638 = *(undefined8 *)(uVar33 + 0x228);
            FUN_14030e020(local_618,uStack_610,&local_640,&local_118,&local_558);
          }
          ReleaseSRWLockExclusive(pRVar2);
        }
        FUN_14034bc60(&local_118);
        FUN_140347150(local_288);
        uVar36 = local_res10.s.LowPart._0_1_;
      }
      local_500 = local_558;
      local_508 = puStack_550;
      if (local_558 != puStack_550) {
        local_658 = local_558;
        local_500 = (ulonglong *)0x0;
        local_558 = (ulonglong *)0x0;
        puStack_650 = puStack_550;
        local_508 = (ulonglong *)0x0;
        puStack_550 = (ulonglong *)0x0;
        local_648 = local_548;
        local_548 = (ulonglong *)0x0;
        FUN_140347680(*param_1,&local_658);
      }
      local_588.QuadPart = 0;
      QueryPerformanceCounter(&local_588);
      local_res18 = local_res18 & 0xffffffffffffff00;
      plVar4 = (longlong *)*param_1;
      pRVar2 = (PSRWLOCK)(plVar4 + 0x98);
      local_4a0 = pRVar2;
      AcquireSRWLockShared(pRVar2);
      uStack_5e0 = (ulonglong *)plVar4[0x10];
      puVar24 = (ulonglong *)*plVar4;
      uStack_5d0 = CONCAT31(uStack_5d0._1_3_,1);
      lVar25 = *(longlong *)(*param_1 + 0x140);
      local_5e8 = puVar24;
      local_5d8 = pRVar2;
      local_510 = puVar24;
      if ((lVar25 != 0) && (puVar24 != (ulonglong *)0x0)) {
        lVar19 = *(longlong *)((longlong)uStack_5e0 + 0x20) * 100 +
                 *(longlong *)((longlong)uStack_5e0 + 0x28);
        lVar32 = lVar19 * 10000;
        iVar15 = (int)(lVar32 / 86400000000);
        if (((longlong)iVar15 * 86400000000 + lVar19 * -10000 != 0) &&
           (lVar32 <= (longlong)iVar15 * 86400000000)) {
          iVar15 = iVar15 + -1;
        }
        if (*(int *)((longlong)puVar24 + 0x158c) != iVar15) {
          local_5b8 = FUN_1404777c0((longlong)uStack_5e0 + 0x20,lVar32 % 86400000000);
          local_res18 = CONCAT71(local_res18._1_7_,1);
        }
        LVar20.QuadPart = FUN_14034a2a0(local_288,&local_488);
        local_res10.QuadPart = LVar20.QuadPart;
        FUN_140481aa0(lVar25 + 0x2220);
        FUN_140347150(LVar20.QuadPart);
        if (((*(char *)(*param_1 + 0x40) == '\0') || (*(char *)(*param_1 + 0x41) == '\0')) ||
           (uVar36 != '\0')) {
          iVar15 = *(int *)(*(longlong *)(*param_1 + 0x140) + 0x2118);
          iVar16 = 1;
          if (1 < iVar15) {
            iVar16 = iVar15;
          }
          if (DAT_140b5ebf0.QuadPart == -1) {
            QueryPerformanceFrequency(&local_4a8);
            DAT_140b5ebf0 = local_4a8;
          }
          dVar8 = (double)DAT_140b5ebf0.QuadPart;
          local_598.QuadPart = 0;
          QueryPerformanceCounter(&local_598);
          auVar38._0_8_ =
               ((double)(local_598.QuadPart - (longlong)LVar29) / dVar8) / (dVar12 / (double)iVar16)
          ;
          lVar25 = (longlong)auVar38._0_8_;
          if ((lVar25 != -0x8000000000000000) && ((double)lVar25 != auVar38._0_8_)) {
            auVar38._8_8_ = auVar38._0_8_;
            uVar17 = movmskpd(0,auVar38);
            auVar38._0_8_ = (double)(longlong)(lVar25 - (ulonglong)(uVar17 & 1));
          }
          if (auVar38._0_8_ <= 0.0) {
            auVar38._0_8_ = 0.0;
          }
          LVar29.QuadPart =
               LVar29.QuadPart + (longlong)((dVar12 / (double)iVar16) * auVar38._0_8_ * dVar8);
          uVar33 = *param_1;
          lVar25 = *(longlong *)(uVar33 + 0x140);
          local_658 = *(ulonglong **)(lVar25 + 0x2128);
          uVar39 = *(undefined4 *)(lVar25 + 0x2118);
          puStack_650 = (ulonglong *)CONCAT44((int)((ulonglong)puStack_650 >> 0x20),uVar39);
          if ((*(int *)(lVar25 + 0x2118) == 0) ||
             (local_648 = (ulonglong *)(longlong)auVar38._0_8_, (longlong)local_648 < 1)) {
            local_648 = (ulonglong *)0x0;
          }
          else {
            local_658 = (ulonglong *)((longlong)local_658 + 1);
            if (0xa6 < (longlong)local_648) {
              local_648 = (ulonglong *)0xa6;
            }
          }
          lVar25 = *(longlong *)(uVar33 + 0x140);
          if ((longlong)local_648 < 1) {
            *(ulonglong **)(lVar25 + 0x2128) = local_658;
            *(undefined4 *)(lVar25 + 0x2118) = uVar39;
          }
          else {
            if (*(longlong *)(uVar33 + 0x228) != 0) {
              LOCK();
              piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
              *piVar1 = *piVar1 + 1;
              UNLOCK();
            }
            local_640 = *(longlong *)(uVar33 + 0x220);
            uStack_638 = *(undefined8 *)(uVar33 + 0x228);
            FUN_140478000(lVar25,&local_640,puVar24,&local_658);
          }
          uVar30 = *(undefined8 *)(*param_1 + 0x140);
          uVar36 = *(undefined1 *)(*param_1 + 0x200);
          uVar33 = *param_1;
          if (*(longlong *)(uVar33 + 0x228) != 0) {
            LOCK();
            piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
            *piVar1 = *piVar1 + 1;
            UNLOCK();
          }
          local_4d8 = *(undefined8 *)(uVar33 + 0x220);
          uStack_4d0 = *(undefined8 *)(uVar33 + 0x228);
          FUN_140478f40(uVar30,&local_4d8,puVar24,uVar36);
          FUN_14047a5f0(*(undefined8 *)(*param_1 + 0x140),puVar24,&local_658);
          uVar33 = *param_1;
          FUN_1400020a0(uVar33 + 0x580);
          uVar21 = *param_1;
          lVar25 = *(longlong *)(uVar21 + 0x140);
          uVar30 = *(undefined8 *)(lVar25 + 0x2200);
          *(undefined8 *)(lVar25 + 0x2200) = *(undefined8 *)(uVar21 + 0xc0);
          *(undefined8 *)(uVar21 + 0xc0) = uVar30;
          uVar30 = *(undefined8 *)(lVar25 + 0x2208);
          *(undefined8 *)(lVar25 + 0x2208) = *(undefined8 *)(uVar21 + 0x100);
          *(undefined8 *)(uVar21 + 0x100) = uVar30;
          LOCK();
          *(undefined1 *)(*param_1 + 0x180) = 1;
          UNLOCK();
          _Mtx_unlock(uVar33 + 0x580);
        }
        else {
          local_res10.QuadPart = local_res10.QuadPart & 0xffffffffffffff00;
          if (local_5f8 != local_660) {
            puVar28 = local_5f8 + 2;
            do {
              if ((char)puVar28[3] != '\0') {
                uVar33 = *param_1;
                lVar25 = *(longlong *)(uVar33 + 0x140);
                if ((*(ulonglong *)(lVar25 + 0x2128) < *puVar28) && (0 < (longlong)puVar28[2])) {
                  if (*(longlong *)(uVar33 + 0x228) != 0) {
                    LOCK();
                    piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
                    *piVar1 = *piVar1 + 1;
                    UNLOCK();
                  }
                  local_640 = *(longlong *)(uVar33 + 0x220);
                  uStack_638 = *(undefined8 *)(uVar33 + 0x228);
                  FUN_140478000(lVar25,&local_640,puVar24,puVar28);
                }
                else {
                  *(ulonglong *)(lVar25 + 0x2128) = *puVar28;
                  *(int *)(lVar25 + 0x2118) = (int)puVar28[1];
                }
                uVar33 = *param_1;
                uVar30 = *(undefined8 *)(uVar33 + 0x140);
                if (*(longlong *)(uVar33 + 0x228) != 0) {
                  LOCK();
                  piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
                  *piVar1 = *piVar1 + 1;
                  UNLOCK();
                }
                local_4f8 = *(undefined8 *)(uVar33 + 0x220);
                uStack_4f0 = *(undefined8 *)(uVar33 + 0x228);
                FUN_140478f40(uVar30,&local_4f8,puVar24,0);
                local_res10.s.LowPart._0_1_ = 1;
                uVar33 = *param_1;
                uVar21 = *(ulonglong *)(*(longlong *)(uVar33 + 0x140) + 0x2128);
                lVar25 = uVar33 + 0x500;
                local_640 = lVar25;
                FUN_1400020a0(lVar25);
                local_658 = (ulonglong *)0x0;
                puStack_650 = (ulonglong *)0x0;
                local_648 = (ulonglong *)0x0;
                local_res20 = local_res20 | 0x20;
                lVar19 = *(longlong *)(uVar33 + 0x438);
                uStack_610 = *(longlong *)(uVar33 + 0x440);
                local_608 = *(PSRWLOCK *)(uVar33 + 0x448);
                uStack_600 = *(undefined8 *)(uVar33 + 0x450);
                local_618 = lVar19;
                if (lVar19 != *(longlong *)(uVar33 + 0x458)) {
                  local_608._4_4_ = (undefined4)((ulonglong)local_608 >> 0x20);
                  uStack_600._4_4_ = (undefined4)((ulonglong)uStack_600 >> 0x20);
                  local_618._0_4_ = (undefined4)lVar19;
                  local_618._4_4_ = (undefined4)((ulonglong)lVar19 >> 0x20);
                  uStack_610._4_4_ = (undefined4)((ulonglong)uStack_610 >> 0x20);
                  uVar39 = (undefined4)local_618;
                  uVar40 = local_618._4_4_;
                  uVar41 = (undefined4)uStack_610;
                  uVar42 = uStack_610._4_4_;
                  uVar43 = (undefined4)local_608;
                  uVar44 = local_608._4_4_;
                  uVar45 = (undefined4)uStack_600;
                  uVar46 = uStack_600._4_4_;
                  do {
                    if (*(ulonglong *)(lVar19 + 8) < uVar21) {
                      local_618 = CONCAT44(uVar40,uVar39);
                      uStack_610 = CONCAT44(uVar42,uVar41);
                      local_608 = (PSRWLOCK)CONCAT44(uVar44,uVar43);
                      uStack_600 = CONCAT44(uVar46,uVar45);
                      pauVar18 = (undefined1 (*) [16])
                                 FUN_14034c350(uVar33 + 0x428,&local_580,&local_618);
                    }
                    else {
                      if (*(ulonglong *)(lVar19 + 8) != uVar21) break;
                      FUN_14034b450(&local_658);
                      local_618 = CONCAT44(uVar40,uVar39);
                      uStack_610 = CONCAT44(uVar42,uVar41);
                      local_608 = (PSRWLOCK)CONCAT44(uVar44,uVar43);
                      uStack_600 = CONCAT44(uVar46,uVar45);
                      pauVar18 = (undefined1 (*) [16])
                                 FUN_14034c350(uVar33 + 0x428,local_d8,&local_618);
                    }
                    uVar43 = *(undefined4 *)pauVar18[1];
                    uVar44 = *(undefined4 *)(pauVar18[1] + 4);
                    uVar45 = *(undefined4 *)(pauVar18[1] + 8);
                    uVar46 = *(undefined4 *)(pauVar18[1] + 0xc);
                    auVar38 = *pauVar18;
                    uVar39 = auVar38._0_4_;
                    uVar40 = auVar38._4_4_;
                    uVar41 = auVar38._8_4_;
                    uVar42 = auVar38._12_4_;
                    lVar19 = auVar38._0_8_;
                  } while (lVar19 != *(longlong *)(uVar33 + 0x458));
                }
                _Mtx_unlock(lVar25);
                puVar35 = puStack_650;
                puVar24 = local_658;
                if (local_658 != puStack_650) {
                  puVar27 = local_658 + 4;
                  do {
                    uVar30 = *(undefined8 *)(*param_1 + 0x140);
                    local_328 = puVar27[-4];
                    local_320 = puVar27[-3];
                    local_318 = puVar27[-2];
                    puVar27[-2] = 0;
                    local_310 = puVar27[-1];
                    puVar27[-1] = 0;
                    local_308 = *puVar27;
                    *puVar27 = 0;
                    local_300 = puVar27[1];
                    puVar27[1] = 0;
                    local_2f8 = puVar27[2];
                    puVar27[2] = 0;
                    local_2f0 = puVar27[3];
                    puVar27[3] = 0;
                    local_2e8 = puVar27[4];
                    puVar27[4] = 0;
                    local_2e0 = puVar27[5];
                    puVar27[5] = 0;
                    local_2d8 = puVar27[6];
                    puVar27[6] = 0;
                    local_2d0 = puVar27[7];
                    puVar27[7] = 0;
                    local_2c8 = puVar27[8];
                    puVar27[8] = 0;
                    local_2c0 = puVar27[9];
                    puVar27[9] = 0;
                    FUN_140478b20(uVar30);
                    puVar37 = puVar27 + 10;
                    puVar27 = puVar27 + 0xe;
                  } while (puVar37 != puVar35);
                }
                local_res20 = local_res20 & 0xffffffdf;
                for (puVar27 = puVar24; puVar27 != puVar35; puVar27 = puVar27 + 0xe) {
                  FUN_14034a830(puVar27);
                }
                if (puVar24 != (ulonglong *)0x0) {
                  free(puVar24);
                }
                puVar24 = local_510;
                FUN_14047a5f0(*(undefined8 *)(*param_1 + 0x140),local_510,puVar28);
                uVar33 = *param_1;
                iVar15 = _Mtx_lock(uVar33 + 0x580);
                if (iVar15 != 0) {
                  std::_Throw_Cpp_error(5);
                  pcVar7 = (code *)swi(3);
                  (*pcVar7)();
                  return;
                }
                if (*(int *)(uVar33 + 0x5cc) == 0x7fffffff) goto LAB_14034a238;
                uVar21 = *param_1;
                lVar25 = *(longlong *)(uVar21 + 0x140);
                uVar30 = *(undefined8 *)(lVar25 + 0x2200);
                *(undefined8 *)(lVar25 + 0x2200) = *(undefined8 *)(uVar21 + 0xc0);
                *(undefined8 *)(uVar21 + 0xc0) = uVar30;
                uVar30 = *(undefined8 *)(lVar25 + 0x2208);
                *(undefined8 *)(lVar25 + 0x2208) = *(undefined8 *)(uVar21 + 0x100);
                *(undefined8 *)(uVar21 + 0x100) = uVar30;
                LOCK();
                *(undefined1 *)(*param_1 + 0x180) = 1;
                UNLOCK();
                _Mtx_unlock(uVar33 + 0x580);
              }
              puVar35 = puVar28 + 9;
              puVar28 = puVar28 + 0xb;
            } while (puVar35 != local_660);
            if (local_res10.s.LowPart._0_1_ != '\0') goto LAB_140349c5c;
          }
          uVar33 = *param_1;
          uVar30 = *(undefined8 *)(uVar33 + 0x140);
          if (*(longlong *)(uVar33 + 0x228) != 0) {
            LOCK();
            piVar1 = (int *)(*(longlong *)(uVar33 + 0x228) + 8);
            *piVar1 = *piVar1 + 1;
            UNLOCK();
          }
          local_4e8 = *(undefined8 *)(uVar33 + 0x220);
          uStack_4e0 = *(undefined8 *)(uVar33 + 0x228);
          FUN_140478f40(uVar30);
        }
LAB_140349c5c:
        ppuVar22 = *(ulonglong ***)(puVar24[0x2a3] + 0x48);
        local_res10.QuadPart = (LONGLONG)(ppuVar22 + 7);
        iVar15 = _Mtx_lock(local_res10.QuadPart);
        uVar17 = local_res20;
        if (iVar15 != 0) {
          std::_Throw_Cpp_error(5);
          pcVar7 = (code *)swi(3);
          (*pcVar7)();
          return;
        }
        if (*(int *)((longlong)ppuVar22 + 0x84) == 0x7fffffff) {
          *(undefined4 *)((longlong)ppuVar22 + 0x84) = 0x7ffffffe;
          std::_Throw_Cpp_error(6);
          pcVar7 = (code *)swi(3);
          (*pcVar7)();
          return;
        }
        local_540 = (ulonglong *)0x0;
        puStack_538 = (ulonglong *)0x0;
        local_530 = (ulonglong *)0x0;
        puStack_528 = (ulonglong *)0x0;
        local_520 = (ulonglong *)0x0;
        puStack_518 = (ulonglong *)0x0;
        if (&local_540 != ppuVar22) {
          FUN_1402a3ab0(&local_540);
        }
        if (&puStack_528 != ppuVar22 + 3) {
          FUN_1402a3990(&puStack_528);
        }
        if (*ppuVar22 != ppuVar22[1]) {
          ppuVar22[1] = *ppuVar22;
        }
        puVar24 = ppuVar22[3];
        if (puVar24 != ppuVar22[4]) {
          ppuVar22[4] = puVar24;
        }
        _Mtx_unlock();
        puVar28 = puStack_538;
        puVar24 = local_540;
        local_res20 = uVar17 & 0xfffffff7 | 4;
        puVar35 = puStack_528;
        puVar27 = puStack_518;
        puVar37 = local_530;
        if (local_540 != puStack_538) {
          local_res10.QuadPart = *param_1 + 0x580;
          FUN_1400020a0(local_res10.QuadPart);
          uVar33 = *param_1;
          puVar5 = *(undefined8 **)(uVar33 + 0x210);
          if (puVar5 < *(undefined8 **)(uVar33 + 0x218)) {
            *puVar5 = 0;
            puVar5[1] = 0;
            puVar5[2] = 0;
            puVar5[3] = 0;
            puVar5[4] = 0;
            puVar5[5] = 0;
            puVar5[6] = 0;
            puVar5[7] = 0;
            puVar5[8] = 0;
            *(longlong *)(uVar33 + 0x210) = *(longlong *)(uVar33 + 0x210) + 0x48;
          }
          else {
            FUN_14034cc90(uVar33 + 0x208);
          }
          puVar34 = (ulonglong *)0x0;
          LVar20 = (LARGE_INTEGER)((LARGE_INTEGER *)(uVar33 + 0x210))->QuadPart;
          ppuVar22 = (ulonglong **)(LVar20.QuadPart + -0x30);
          puVar37 = local_530;
          local_res10 = LVar20;
          if (ppuVar22 != &local_540) {
            FUN_140015e70(ppuVar22);
            *ppuVar22 = puVar24;
            *(ulonglong **)(LVar20.QuadPart + -0x28) = puVar28;
            *(ulonglong **)(LVar20.QuadPart + -0x20) = local_530;
            puVar24 = puVar34;
            puVar37 = puVar34;
          }
          LVar20 = local_res10;
          ppuVar22 = (ulonglong **)(local_res10.QuadPart + -0x18);
          puVar35 = puStack_528;
          puVar27 = puStack_518;
          if (ppuVar22 != &puStack_528) {
            FUN_140015c60(ppuVar22);
            *ppuVar22 = puStack_528;
            *(ulonglong **)(LVar20.QuadPart + -0x10) = local_520;
            *(ulonglong **)(LVar20.QuadPart + -8) = puStack_518;
            puVar35 = puVar34;
            puVar27 = (ulonglong *)0x0;
          }
          *(ulonglong **)(local_res10.QuadPart + -0x48) =
               *(ulonglong **)(*(longlong *)(*param_1 + 0x140) + 0x2128);
          *(ulonglong **)(local_res10.QuadPart + -0x40) =
               (ulonglong *)(*(longlong *)(*(longlong *)(*param_1 + 0x140) + 0x28) * 10000);
          *(undefined4 *)(local_res10.QuadPart + -0x38) =
               *(undefined4 *)(*(longlong *)(*param_1 + 0x140) + 0x2118);
          _Mtx_unlock();
        }
        if (puVar35 != (ulonglong *)0x0) {
          puVar28 = puVar35;
          if ((0xfff < ((longlong)puVar27 - (longlong)puVar35 & 0xfffffffffffffff0U)) &&
             (puVar28 = (ulonglong *)puVar35[-1],
             0x1f < (ulonglong)((longlong)puVar35 + (-8 - (longlong)puVar28)))) goto LAB_14034a25a;
          free(puVar28);
        }
        if (puVar24 != (ulonglong *)0x0) {
          puVar28 = puVar24;
          if ((0xfff < ((longlong)puVar37 - (longlong)puVar24 & 0xffffffffffffffe0U)) &&
             (puVar28 = (ulonglong *)puVar24[-1],
             0x1f < (ulonglong)((longlong)puVar24 + (-8 - (longlong)puVar28)))) {
LAB_14034a25a:
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar28);
        }
        uVar33 = local_510[0x2b0];
        uVar21 = *(ulonglong *)(uVar33 + 0x40);
        if (*(ulonglong *)(uVar33 + 0x40) < *(ulonglong *)(uVar33 + 0x38)) {
          uVar21 = *(ulonglong *)(uVar33 + 0x38);
        }
        *(ulonglong *)(uVar33 + 0x40) = uVar21;
        LOCK();
        *(undefined8 *)(uVar33 + 0x38) = 0;
        UNLOCK();
        FUN_140324fb0(uVar33 + 8,*(undefined8 *)(uVar33 + 0x40));
        puVar5 = (undefined8 *)(uVar33 + 0x20);
        puVar26 = (undefined8 *)*puVar5;
        while (puVar26 != puVar5) {
          puVar6 = (undefined8 *)*puVar26;
          if ((void *)puVar26[2] != (void *)0x0) {
            free((void *)puVar26[2]);
          }
          free(puVar26);
          puVar26 = puVar6;
        }
        *puVar5 = puVar5;
        *(undefined8 **)(uVar33 + 0x28) = puVar5;
        *(undefined8 *)(uVar33 + 0x30) = 0;
      }
      ReleaseSRWLockShared(local_4a0);
      if ((char)local_res18 != '\0') {
        plVar4 = (longlong *)*param_1;
        AcquireSRWLockExclusive((PSRWLOCK)(plVar4 + 0x98));
        if (*plVar4 != 0) {
          *(undefined4 *)(*plVar4 + 0x158c) = local_5b8;
        }
        ReleaseSRWLockExclusive((PSRWLOCK)(plVar4 + 0x98));
      }
      local_590.QuadPart = 0;
      QueryPerformanceCounter(&local_590);
      lVar25 = local_590.QuadPart - (longlong)local_588;
      if (DAT_140b5ebf0.QuadPart == -1) {
        QueryPerformanceFrequency(local_498);
        DAT_140b5ebf0 = local_498[0];
      }
      if (((*(char *)(*param_1 + 0x40) != '\0') &&
          (*(int *)(*(longlong *)(*param_1 + 0x140) + 0x2118) < 0x65)) &&
         ((double)lVar25 / (double)DAT_140b5ebf0.QuadPart - dVar11 < 0.0)) {
        local_res10.QuadPart = llround();
        FUN_14034b570(&local_res10);
      }
      puVar24 = local_5f8;
      if (local_5f8 != local_660) {
        puVar28 = local_5f8 + 6;
        do {
          if ((char)puVar28[4] != '\0') {
            FUN_14034bc60(puVar28);
          }
          puVar35 = puVar28 + 5;
          puVar28 = puVar28 + 0xb;
        } while (puVar35 != local_660);
      }
      if (puVar24 != (ulonglong *)0x0) {
        free(puVar24);
      }
      puVar24 = local_500;
      FUN_14032a340(local_500);
      if (puVar24 != (ulonglong *)0x0) {
        free(puVar24);
      }
      FUN_1402531c0(local_358);
      if (1 < local_348) {
        free(local_350);
      }
      if ((local_360 != '\0') && (FUN_1402531c0(local_390), 1 < local_380)) {
        free(local_388);
      }
      if (local_3a8 != (void *)0x0) {
        free(local_3a8);
      }
      FUN_140002cc0(&local_3c0);
      FUN_140015c60(&local_3d8);
      if (local_3f0 != (void *)0x0) {
        free(local_3f0);
      }
      if (local_408 != (void *)0x0) {
        free(local_408);
      }
      FUN_1402531c0(local_450);
      if (1 < local_440) {
        free(local_448);
      }
      FUN_1402531c0(local_480);
      if (1 < local_470) {
        free(local_478);
      }
    }
    cVar23 = (char)param_1[1];
  } while( true );
}


// ref 140c1ae4c caller none
// ref 140346f99 caller 140346cb0
// ref 140346fa0 caller 140346cb0
// ref 1403488e1 caller 1403488c0
// ref 1403488e8 caller 1403488c0
