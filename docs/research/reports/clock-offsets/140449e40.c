
void FUN_140449e40(longlong param_1,undefined8 *param_2,longlong param_3,longlong param_4,
                  longlong *param_5)

{
  ulonglong *puVar1;
  undefined8 *puVar2;
  longlong *plVar3;
  ulonglong uVar4;
  double *pdVar5;
  code *pcVar6;
  double dVar7;
  undefined8 uVar8;
  int iVar9;
  ulonglong *puVar10;
  ulonglong uVar11;
  longlong lVar12;
  double dVar13;
  double dVar14;
  longlong lVar15;
  undefined4 uVar16;
  ulonglong *puVar17;
  bool bVar18;
  undefined4 local_res18;
  undefined8 local_78;
  undefined8 uStack_70;
  longlong local_68 [6];

  if (*(char *)(param_3 + 0x4d0) != '\0') {
    puVar17 = (ulonglong *)0x0;
    local_res18 = 0;
    puVar10 = puVar17;
    if (*(char *)(param_3 + 0x510) != '\0') {
      if (*(longlong *)(param_3 + 0x4f8) != 0) {
        puVar10 = (ulonglong *)FUN_14032c3b0(**(longlong **)(param_1 + 0x28) + 0x80);
      }
      puVar17 = *(ulonglong **)(param_3 + 0x500);
      local_res18 = *(undefined4 *)(param_3 + 0x508);
    }
    if (*(char *)(param_3 + 0x1d0) != '\0') {
      *(undefined8 *)(param_3 + 0xd0) = 0;
    }
    dVar7 = DAT_140aabb00;
    lVar15 = *(longlong *)(param_3 + 0x4c0) +
             *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x28) * -10000;
    if (lVar15 < 1) {
      lVar15 = SUB168(SEXT816(-0x346dc5d63886594b) * SEXT816(lVar15),8);
      *param_5 = (lVar15 >> 0xb) - (lVar15 >> 0x3f);
      if (puVar10 != (ulonglong *)0x0) {
        plVar3 = *(longlong **)(param_1 + 0x90);
        local_78 = *param_2;
        uStack_70 = *(undefined8 *)(param_3 + 0x4b8);
        uVar4 = *puVar10;
        LOCK();
        puVar1 = (ulonglong *)(plVar3 + 0xc);
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + 1;
        UNLOCK();
        if (uVar11 < (ulonglong)plVar3[0xd]) {
          uVar11 = (uVar4 >> 0x21 ^ uVar4) * -0xae502812aa7333;
          uVar11 = (uVar11 >> 0x21 ^ uVar11) * -0x3b314601e57a13ad;
          uVar11 = uVar11 >> 0x21 ^ uVar11;
          lVar15 = plVar3[1];
          lVar12 = *plVar3;
          if (lVar15 - lVar12 >> 3 != 0) {
            do {
              uVar11 = uVar11 & (lVar15 - lVar12 >> 3) - 1U;
              puVar1 = (ulonglong *)(lVar12 + uVar11 * 8);
              LOCK();
              bVar18 = *puVar1 == 0;
              if (bVar18) {
                *puVar1 = uVar4;
              }
              UNLOCK();
              if (bVar18) {
                puVar2 = (undefined8 *)(plVar3[3] + uVar11 * 0x10);
                *puVar2 = local_78;
                puVar2[1] = uStack_70;
                break;
              }
              uVar11 = uVar11 + 1;
              lVar15 = plVar3[1];
              lVar12 = *plVar3;
            } while (lVar15 != lVar12);
          }
        }
        else {
          iVar9 = _Mtx_lock(plVar3 + 0xe);
          if (iVar9 != 0) {
            std::_Throw_Cpp_error(5);
            pcVar6 = (code *)swi(3);
            (*pcVar6)();
            return;
          }
          if (*(int *)((longlong)plVar3 + 0xbc) == 0x7fffffff) {
            *(undefined4 *)((longlong)plVar3 + 0xbc) = 0x7ffffffe;
            std::_Throw_Cpp_error(6);
          }
          FUN_140364d10(plVar3 + 6,local_68);
          FUN_140290980(local_68[0] + 8,&local_78);
          _Mtx_unlock(plVar3 + 0xe);
        }
      }
      uVar8 = DAT_140aabd08;
      if ((param_4 == 0) || (*(int *)(param_4 + 0x38) < *(int *)(param_3 + 0x48))) {
        bVar18 = false;
      }
      else {
        bVar18 = true;
      }
      if ((puVar10 != (ulonglong *)0x0) &&
         (FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*puVar10,0x11,DAT_140aabd08), bVar18)) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*puVar10,0x28,uVar8);
      }
      FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,0x11,uVar8);
      if (puVar17 != (ulonglong *)0x0) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),puVar17,0x11,uVar8);
      }
      FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),0,0x11,uVar8);
      if (bVar18) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,0x28,uVar8);
        if (puVar17 != (ulonglong *)0x0) {
          FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),puVar17,0x28,uVar8);
        }
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),0,0x28,uVar8);
      }
      if ((*(char *)(param_3 + 0x558) != '\0') && (*(char *)(param_3 + 0x54c) == '\0')) {
        *(undefined1 *)(param_3 + 0x54c) = 1;
        *(longlong *)(param_3 + 0x518) = *(longlong *)(*(longlong *)(param_1 + 0x18) + 0x28) * 10000
        ;
      }
      if (((puVar17 != (ulonglong *)0x0) &&
          (lVar15 = FUN_14033f7f0(**(longlong **)(param_1 + 0x30) + 0x180,puVar17), lVar15 != 0)) &&
         (lVar12 = FUN_1404225e0(lVar15,local_res18), lVar12 != 0)) {
        FUN_140442530(param_1,lVar15,lVar12,param_2,param_3,1);
      }
      *(undefined1 *)(param_3 + 0x4d0) = 0;
      *(undefined1 *)(param_3 + 0x510) = 0;
      *(undefined1 *)(param_3 + 0x4f0) = 0;
      if (*(char *)(param_3 + 0x611) == '\0') {
        *(undefined1 *)(param_3 + 0x611) = 1;
      }
    }
    else {
      uVar16 = 0x10;
      if (*(char *)(param_3 + 0x558) != '\0') {
        uVar16 = 0x10;
        if (0.0 < *(float *)(param_3 + 0x548)) {
          uVar16 = 0x12;
        }
      }
      if (puVar10 != (ulonglong *)0x0) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*puVar10,uVar16,
                      (double)*param_5 * DAT_140aabb00);
      }
      FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,uVar16,(double)*param_5 * dVar7);
      if (puVar17 != (ulonglong *)0x0) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),puVar17,uVar16,(double)*param_5 * dVar7);
      }
      FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),0,uVar16,(double)*param_5 * dVar7);
      pdVar5 = *(double **)(param_1 + 0x20);
      dVar7 = (double)*param_5 * *(double *)(param_3 + 0x68) * DAT_140aac648;
      dVar13 = *pdVar5;
      do {
        LOCK();
        dVar14 = *pdVar5;
        bVar18 = dVar13 == dVar14;
        if (bVar18) {
          *pdVar5 = dVar13 + dVar7;
          dVar14 = dVar13;
        }
        UNLOCK();
        dVar13 = dVar14;
      } while (!bVar18);
      FUN_140380bb0(pdVar5,0,0xc,dVar7);
      FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),*param_2,0xc,dVar7);
      if (puVar17 != (ulonglong *)0x0) {
        FUN_140380bb0(*(undefined8 *)(param_1 + 0x20),puVar17,0xc,dVar7);
      }
      *param_5 = 0;
    }
  }
  return;
}
