// Candidate VA 140740c40; RVA 0x740c40
// Ghidra inferred prototype: undefined FUN_140740c40()

void FUN_140740c40(longlong param_1)

{
  int *piVar1;
  longlong *plVar2;
  PSRWLOCK SRWLock;
  undefined1 uVar3;
  short sVar4;
  longlong lVar5;
  ulonglong uVar6;
  code *pcVar7;
  void *pvVar8;
  void *pvVar9;
  char cVar10;
  int iVar11;
  longlong lVar12;
  undefined8 *puVar13;
  longlong *plVar14;
  undefined8 uVar15;
  uint *puVar16;
  uint *puVar17;
  uint uVar18;
  longlong lVar19;
  void *pvVar20;
  ulonglong uVar21;
  bool bVar22;
  double dVar23;
  longlong local_res8;
  undefined8 local_res10 [2];
  char *local_e8;
  undefined8 uStack_e0;
  undefined8 local_d8;
  void *local_c8;
  void *local_c0;
  PSRWLOCK local_b8;
  longlong local_b0;
  longlong local_a8;
  PSRWLOCK local_a0;
  void *local_98;
  void *local_90;
  PSRWLOCK local_88;
  void *local_78;
  void *local_70;
  PSRWLOCK local_68;
  uint local_60;
  uint local_5c;
  longlong local_58;
  longlong local_50;
  PSRWLOCK local_48;
  undefined1 local_40;
  
  plVar2 = (longlong *)(param_1 + 0x540);
  SRWLock = (PSRWLOCK)(param_1 + 0xa00);
  AcquireSRWLockShared(SRWLock);
  lVar5 = *(longlong *)(param_1 + 0x5c0);
  lVar19 = *plVar2;
  local_40 = 1;
  local_58 = lVar19;
  local_50 = lVar5;
  local_48 = SRWLock;
  lVar12 = FUN_1407428d0(param_1,&local_c8);
  local_res8 = lVar12;
  if (((*(char *)(param_1 + 0x80) != '\0') &&
      (dVar23 = (double)FUN_140249580(param_1 + 0x58), DAT_140aabe78 < dVar23)) &&
     (lVar12 = local_res8, *(char *)(param_1 + 0x80) != '\0')) {
    FUN_140002d30(param_1 + 0x60);
    *(undefined1 *)(param_1 + 0x80) = 0;
    lVar12 = local_res8;
  }
  FUN_1404e6d80(param_1 + 0x30);
  if (*(char *)(param_1 + 0x88) != '\0') {
    if ((lVar19 != 0) && (lVar5 != 0)) {
      FUN_1404e47c0(param_1 + 0x30,lVar19,lVar5,lVar12);
    }
    FUN_1404e5ea0(param_1 + 0x30);
    FUN_1404e7350(param_1 + 0x30);
  }
  FUN_140002d30(&local_a8);
  ReleaseSRWLockShared(SRWLock);
  lVar5 = *(longlong *)(param_1 + 0x28);
  if (lVar5 != 0) {
    uVar3 = *(undefined1 *)(param_1 + 0x580);
    iVar11 = _Mtx_lock(param_1 + 0xa40);
    if (iVar11 != 0) {
      std::_Throw_Cpp_error(5);
      pcVar7 = (code *)swi(3);
      (*pcVar7)();
      return;
    }
    if (*(int *)(param_1 + 0xa8c) == 0x7fffffff) {
      *(undefined4 *)(param_1 + 0xa8c) = 0x7ffffffe;
      std::_Throw_Cpp_error(6);
      pcVar7 = (code *)swi(3);
      (*pcVar7)();
      return;
    }
    local_e8 = *(char **)(param_1 + 0x9c0);
    *(undefined8 *)(param_1 + 0x9c0) = 0;
    uStack_e0 = *(undefined8 *)(param_1 + 0x9c8);
    local_d8 = *(undefined8 *)(param_1 + 0x9d0);
    *(undefined8 *)(param_1 + 0x9d0) = 0;
    *(undefined8 *)(param_1 + 0x9c8) = 0;
    _Mtx_unlock(param_1 + 0xa40);
    FUN_1404ee3d0(lVar5,&local_c8,&local_e8,uVar3);
    if (local_c8 == local_c0) {
      local_78 = local_98;
      local_98 = (void *)0x0;
      local_70 = local_90;
      local_90 = (void *)0x0;
      local_68 = local_88;
      local_88 = (PSRWLOCK)0x0;
      local_58 = local_b0;
      local_b0 = 0;
      local_50 = local_a8;
      local_a8 = 0;
      local_48 = local_a0;
      local_a0 = (PSRWLOCK)0x0;
      FUN_140348520(plVar2,&local_58,&local_78);
    }
    else {
      puVar13 = (undefined8 *)FUN_140748960(&local_res8);
      uVar15 = *puVar13;
      *puVar13 = 0;
      lVar5 = *(longlong *)(param_1 + 0x328);
      *(undefined8 *)(param_1 + 0x328) = uVar15;
      if (lVar5 != 0) {
        FUN_14073d9d0();
      }
      if (local_res8 != 0) {
        FUN_14073d9d0();
      }
      local_res8 = FUN_140983da8(0x10);
      local_res8 = FUN_14024fa80(local_res8);
      local_res10[0] = 0;
      local_78 = local_c8;
      local_c8 = (void *)0x0;
      local_70 = local_c0;
      local_c0 = (void *)0x0;
      local_68 = local_b8;
      local_b8 = (PSRWLOCK)0x0;
      FUN_1404ad350(*(undefined8 *)(param_1 + 0x328),&local_78,&local_res8);
      local_c0 = local_c8;
      FUN_140742c70(param_1);
    }
    if ((*(char *)(param_1 + 0x582) != '\0') &&
       (lVar5 = *(longlong *)(param_1 + 0x28), *(int *)(lVar5 + 0xf4) != 0)) {
      plVar14 = (longlong *)FUN_1402e7450();
      (**(code **)(*plVar14 + 0x28))(plVar14,*(undefined4 *)(lVar5 + 0xf4));
      *(undefined4 *)(lVar5 + 0xf4) = 0;
    }
    lVar5 = *(longlong *)(param_1 + 0x28);
    if ((*(int *)(lVar5 + 0xf4) == 0) || (*(int *)(lVar5 + 0xf0) < 1)) {
LAB_140740f4f:
      FUN_140348770(plVar2);
      pvVar20 = local_98;
      pvVar8 = local_90;
    }
    else {
      plVar14 = (longlong *)FUN_1402e7450();
      cVar10 = (**(code **)(*plVar14 + 0x38))(plVar14,*(undefined4 *)(lVar5 + 0xf4));
      pvVar20 = local_98;
      pvVar8 = local_90;
      if (cVar10 == '\0') goto LAB_140740f4f;
    }
    for (; pvVar9 = local_90, pvVar20 != local_90; pvVar20 = (void *)((longlong)pvVar20 + 0x70)) {
      local_90 = pvVar8;
      FUN_14034a830(pvVar20);
      pvVar8 = local_90;
      local_90 = pvVar9;
    }
    local_90 = pvVar8;
    if (local_98 != (void *)0x0) {
      free(local_98);
    }
    FUN_14034bee0(&local_b0);
    if (local_c8 != (void *)0x0) {
      free(local_c8);
    }
  }
  if (*(char *)(param_1 + 0x351) == '\0') {
    *(undefined1 *)(param_1 + 0x351) = 1;
    local_e8 = "collections.nrclip";
    uStack_e0 = 0x12;
    FUN_1402603b0(&local_58,&local_e8);
    FUN_14026ce20(&local_c8,&DAT_140b77d10,&local_58);
    FUN_140025470(&local_58);
    plVar14 = (longlong *)FUN_140246a00(&local_e8,&local_c8);
    local_78 = (void *)*plVar14;
    local_70 = (void *)CONCAT44(local_70._4_4_,(int)plVar14[1]);
    if ((int)local_78 == 2) {
      FUN_140247b90(&local_e8,&local_c8);
      FUN_140743970(param_1 + 0x310,&local_e8);
      lVar5 = *(longlong *)(param_1 + 0x318);
      for (lVar19 = *(longlong *)(param_1 + 0x310); lVar19 != lVar5; lVar19 = lVar19 + 0x80) {
        if (*(char *)(lVar19 + 0x38) == '\0') {
          *(undefined1 *)(lVar19 + 0x40) = 1;
        }
        else {
          *(undefined1 *)(lVar19 + 0x40) = 0;
          FUN_140749a70(lVar19 + 0x68);
        }
      }
    }
    FUN_140025470(&local_c8);
  }
  FUN_140744d50(param_1);
  if (*(longlong *)(param_1 + 0x328) == 0) {
    return;
  }
  lVar5 = *(longlong *)(*(longlong *)(param_1 + 0x328) + 0xe8);
  if (lVar5 == 0) {
    return;
  }
  if (1 < (byte)(*(char *)(lVar5 + 8) - 3U)) {
    return;
  }
  FUN_1404ad5b0(*(undefined8 *)(param_1 + 0x328));
  plVar14 = *(longlong **)(param_1 + 0x328);
  if ((char)plVar14[0xe] != '\0') {
    return;
  }
  if (*plVar14 == 0) {
    return;
  }
  if (plVar14[1] == 0) {
    return;
  }
  if (*(char *)(param_1 + 0x330) != '\0') {
    FUN_140742d20(param_1);
    *(undefined1 *)(param_1 + 0x330) = 0;
    lVar5 = *(longlong *)(param_1 + 0x328);
    *(undefined8 *)(param_1 + 0x328) = 0;
    if (lVar5 != 0) {
      FUN_14073d9d0();
    }
    FUN_140742bc0(param_1);
    return;
  }
  FUN_1404abd10(param_1 + 0x368,plVar14 + 2);
  lVar5 = *(longlong *)(param_1 + 0x20);
  if (lVar5 == 0) {
    bVar22 = *(longlong *)(param_1 + 0x28) == 0;
  }
  else {
    bVar22 = false;
    if (*(longlong *)(param_1 + 0x28) == 0) {
      uVar15 = FUN_140019ca0(&local_c8,param_1 + 0x90);
      local_res8 = *(longlong *)(*(longlong *)(param_1 + 0x328) + 8);
      *(undefined8 *)(*(longlong *)(param_1 + 0x328) + 8) = 0;
      local_res10[0] = **(undefined8 **)(param_1 + 0x328);
      **(undefined8 **)(param_1 + 0x328) = 0;
      FUN_1404f2f20(lVar5,local_res10,&local_res8,param_1 + 0x334,uVar15);
      local_res8 = FUN_140983da8(0x1c8);
      uVar15 = FUN_1404ed540(local_res8);
      lVar5 = *(longlong *)(param_1 + 0x28);
      *(undefined8 *)(param_1 + 0x28) = uVar15;
      if (lVar5 != 0) {
        FUN_14073da70();
      }
      lVar5 = *(longlong *)(param_1 + 0x28);
      uVar15 = FUN_1402cf660();
      *(undefined8 *)(lVar5 + 0x50) = uVar15;
      sVar4 = *(short *)(*(longlong *)(param_1 + 0x20) + 400);
      if (sVar4 != 0) {
        lVar5 = *(longlong *)(param_1 + 0x28);
        uVar6 = *(ulonglong *)(lVar5 + 0x70);
        if (uVar6 < 9) {
          uVar21 = 0x7fffffffffffffff;
          if (uVar6 <= 0x7fffffffffffffff - (uVar6 >> 1)) {
            uVar6 = (uVar6 >> 1) + uVar6;
            uVar21 = 0xf;
            if (0xf < uVar6) {
              uVar21 = uVar6;
            }
          }
          puVar13 = (undefined8 *)FUN_140003270(uVar21 + 1);
          *(undefined8 *)(lVar5 + 0x68) = 9;
          *(ulonglong *)(lVar5 + 0x70) = uVar21;
          *puVar13 = DAT_140a7d2b0;
          *(undefined1 *)(puVar13 + 1) = DAT_140a7d2b8;
          *(undefined1 *)((longlong)puVar13 + 9) = 0;
          *(undefined8 **)(lVar5 + 0x58) = puVar13;
          *(short *)(lVar5 + 0x98) = sVar4;
        }
        else {
          pvVar20 = (void *)(lVar5 + 0x58);
          if (0xf < uVar6) {
            pvVar20 = *(void **)(lVar5 + 0x58);
          }
          *(undefined8 *)(lVar5 + 0x68) = 9;
          memmove(pvVar20,&DAT_140a7d2b0,9);
          *(undefined1 *)((longlong)pvVar20 + 9) = 0;
          *(short *)(lVar5 + 0x98) = sVar4;
        }
      }
      goto LAB_140741395;
    }
  }
  local_res8 = *(longlong *)(*(longlong *)(param_1 + 0x328) + 8);
  *(undefined8 *)(*(longlong *)(param_1 + 0x328) + 8) = 0;
  local_res10[0] = **(undefined8 **)(param_1 + 0x328);
  **(undefined8 **)(param_1 + 0x328) = 0;
  if (*(longlong *)(param_1 + 0x18) != 0) {
    LOCK();
    piVar1 = (int *)(*(longlong *)(param_1 + 0x18) + 8);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  local_e8 = *(char **)(param_1 + 0x10);
  uStack_e0 = *(undefined8 *)(param_1 + 0x18);
  FUN_140347b10(plVar2,&local_c8,&local_e8,local_res10,&local_res8,!bVar22);
  if ((*(longlong *)(param_1 + 0x28) != 0) && (local_c8 != (void *)0x0)) {
    uVar18 = DAT_140b77e58 >> 1;
    local_res10[0] = CONCAT44(local_res10[0]._4_4_,uVar18);
    local_60 = DAT_140b77e58;
    local_5c = 1;
    puVar17 = &local_60;
    if ((int)DAT_140b77e58 < 2) {
      puVar17 = &local_5c;
    }
    local_res8 = CONCAT44(local_res8._4_4_,1);
    puVar16 = (uint *)&local_res8;
    if (uVar18 != 0) {
      puVar16 = (uint *)local_res10;
    }
    if ((int)*puVar17 < (int)uVar18) {
      puVar16 = puVar17;
    }
    *(uint *)((longlong)local_c8 + 0x1588) = *puVar16;
  }
  if ((char)local_b0 != '\0') {
    ReleaseSRWLockExclusive(local_b8);
  }
LAB_140741395:
  FUN_140742c70(param_1);
  pvVar20 = *(void **)(param_1 + 0x328);
  *(undefined8 *)(param_1 + 0x328) = 0;
  if (pvVar20 != (void *)0x0) {
    lVar5 = *(longlong *)((longlong)pvVar20 + 0xe8);
    if ((lVar5 != 0) && (1 < (byte)(*(char *)(lVar5 + 8) - 3U))) {
      (**(code **)(*(longlong *)ThreadLocalStoragePointer + 0x20))(lVar5);
    }
    plVar2 = *(longlong **)((longlong)pvVar20 + 0xe8);
    if (plVar2 != (longlong *)0x0) {
      LOCK();
      lVar5 = *plVar2;
      *plVar2 = *plVar2 + -1;
      UNLOCK();
      if (lVar5 == 1) {
        (**(code **)plVar2[3])();
      }
    }
    FUN_140002d30((longlong)pvVar20 + 0x78);
    FUN_1404ac940(pvVar20);
    free(pvVar20);
  }
  return;
}


// Incoming references
// 0xc3722c DATA caller none
// 0x728305 UNCONDITIONAL_CALL caller 1407272d0
