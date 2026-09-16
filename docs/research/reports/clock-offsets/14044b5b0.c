
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14044b5b0(longlong param_1,ulonglong *param_2,undefined8 *param_3)

{
  ulonglong *puVar1;
  undefined4 *puVar2;
  char cVar3;
  undefined8 *puVar4;
  longlong lVar5;
  longlong *plVar6;
  code *pcVar7;
  undefined1 auVar8 [16];
  longlong lVar9;
  int iVar10;
  longlong *plVar11;
  char cVar12;
  longlong lVar13;
  ulonglong uVar14;
  longlong lVar15;
  bool bVar16;
  undefined4 uVar17;
  double dVar18;
  ulonglong local_res18;
  double local_res20;
  ulonglong local_108;
  undefined1 local_100 [16];
  ulonglong *local_f0;
  double dStack_e8;
  undefined1 *local_c8;
  undefined8 local_c0;
  longlong local_b8;
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  longlong local_a8 [4];
  ulonglong local_88;
  undefined1 local_80;
  undefined1 local_7f;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined1 local_68 [16];
  undefined4 local_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined8 local_48;

  if (*(char *)(param_3 + 0x3a) == '\0') {
    return;
  }
  if (*(char *)(param_3 + 0x3e) != '\0') {
    return;
  }
  puVar4 = *(undefined8 **)(param_1 + 0x28);
  lVar5 = param_3[0x17];
  if (lVar5 == 0) {
    return;
  }
  plVar11 = (longlong *)FUN_14032c420(*puVar4,lVar5);
  if (plVar11 == (longlong *)0x0) {
    return;
  }
  local_c0 = 0;
  cVar3 = *(char *)((longlong)param_3 + 0xc9);
  if (cVar3 == '\0') {
    local_res20 = (double)param_3[0x18];
  }
  else {
    local_res20 = (double)param_3[0x18];
    if ((*plVar11 == lVar5) && (*(char *)((longlong)plVar11 + 0x2c) != cVar3)) {
      local_res20 = DAT_140aabd08 - local_res20;
    }
    if (*plVar11 == lVar5) {
      cVar12 = *(char *)(param_3 + 0x19);
      if (*(char *)((longlong)plVar11 + 0x2c) != cVar3) {
        cVar12 = -*(char *)(param_3 + 0x19);
      }
      goto LAB_14044b650;
    }
  }
  cVar12 = *(char *)(param_3 + 0x19);
LAB_14044b650:
  local_res18 = CONCAT44(local_res18._4_4_,(int)cVar12);
  puVar1 = param_3 + 0x1c;
  local_108 = *puVar1;
  local_100 = ZEXT816(0);
  local_f0 = (ulonglong *)0x0;
  dStack_e8 = 0.0;
  local_b8 = lVar5;
  FUN_14043f670(local_a8,puVar4,*param_3,(double)*(float *)(param_3 + 8),&local_108,param_3[0x1d],
                &local_b8,&local_c0,&local_res20,&local_res18,local_100,
                *(undefined8 *)(param_1 + 0x70),(float)(double)param_3[0x1a]);
  if ((local_108 != param_3[0x1d]) && (local_108 + 0x10 < (ulonglong)param_3[0x1d])) {
    FUN_1404589a0(puVar1);
  }
  if (100 < (ulonglong)((longlong)(param_3[0x1d] - *puVar1) >> 4)) {
    FUN_1404589a0(puVar1,*puVar1 + 0x630);
  }
  if ((0.0 < (double)local_f0) && (0.0 < dStack_e8)) {
    dVar18 = dStack_e8;
    if ((0.0 < (double)local_f0) && (0.0 < dStack_e8)) {
      dVar18 = (double)local_100._8_8_ + _UNK_140aad668;
      local_100._8_4_ = SUB84(dVar18,0);
      local_100._0_8_ = (double)local_100._0_8_ + _DAT_140aad660;
      local_100._12_4_ = (int)((ulonglong)dVar18 >> 0x20);
      local_f0 = (ulonglong *)((double)local_f0 + _DAT_140aabf50);
      dVar18 = dStack_e8 + _DAT_140aabf50;
    }
    param_3[0x29] = local_100._0_8_;
    param_3[0x2a] = local_100._8_8_;
    uVar17 = (undefined4)((ulonglong)dVar18 >> 0x20);
    auVar8._8_4_ = SUB84(dVar18,0);
    auVar8._0_8_ = local_f0;
    auVar8._12_4_ = uVar17;
    *(undefined1 (*) [16])(param_3 + 0x2b) = auVar8;
    local_88 = *param_2;
    local_78 = local_100._0_8_;
    uStack_70 = local_100._8_8_;
    local_68._8_4_ = SUB84(dVar18,0);
    local_68._0_8_ = local_f0;
    local_68._12_4_ = uVar17;
    local_58 = *(undefined4 *)(param_3 + 0x17);
    uStack_54 = *(undefined4 *)((longlong)param_3 + 0xbc);
    uStack_50 = *(undefined4 *)(param_3 + 0x18);
    uStack_4c = *(undefined4 *)((longlong)param_3 + 0xc4);
    local_48 = param_3[0x19];
    local_80 = *(undefined1 *)(param_3 + 0x1b);
    uVar14 = (longlong)(param_3[0x1d] - *puVar1) >> 4;
    local_7f = 100;
    if (uVar14 < 100) {
      local_7f = (char)uVar14;
    }
    local_100._8_8_ = *(undefined8 *)(param_1 + 0x80);
    local_100._0_8_ =
         std::
         _Func_impl_no_alloc<`public:_void___cdecl_nimby::model::TrainHitMap::rasterize(nimby::model::TrainHitMap::AABB_const&___ptr64)___ptr64'::`2'::<lambda_1>,void,__int64>
         ::vftable;
    local_f0 = &local_88;
    local_c8 = local_100;
    FUN_14043f350(local_58,&local_88,local_100);
    plVar11 = (longlong *)*puVar1;
    plVar6 = (longlong *)param_3[0x1d];
    while (plVar11 != plVar6) {
      lVar9 = *plVar11;
      uVar17 = *(undefined4 *)((longlong)plVar11 + 4);
      local_b8 = *plVar11;
      uStack_b0 = (undefined4)plVar11[1];
      uStack_ac = *(undefined4 *)((longlong)plVar11 + 0xc);
      lVar5 = *(longlong *)(param_1 + 0x80);
      local_res18 = *param_2;
      LOCK();
      puVar1 = (ulonglong *)(lVar5 + 0x120);
      uVar14 = *puVar1;
      *puVar1 = *puVar1 + 1;
      UNLOCK();
      if (uVar14 < *(ulonglong *)(lVar5 + 0x128)) {
        uVar14 = (local_res18 >> 0x21 ^ local_res18) * -0xae502812aa7333;
        uVar14 = (uVar14 >> 0x21 ^ uVar14) * -0x3b314601e57a13ad;
        uVar14 = uVar14 >> 0x21 ^ uVar14;
        lVar13 = *(longlong *)(lVar5 + 200);
        lVar15 = *(longlong *)(lVar5 + 0xc0);
        if (lVar13 - lVar15 >> 3 == 0) {
LAB_14044b9ba:
          plVar11 = plVar11 + 2;
        }
        else {
          do {
            uVar14 = uVar14 & (lVar13 - lVar15 >> 3) - 1U;
            puVar1 = (ulonglong *)(lVar15 + uVar14 * 8);
            LOCK();
            bVar16 = *puVar1 == 0;
            if (bVar16) {
              *puVar1 = local_res18;
            }
            UNLOCK();
            if (bVar16) {
              puVar2 = (undefined4 *)(*(longlong *)(lVar5 + 0xd8) + uVar14 * 0x10);
              *puVar2 = (int)lVar9;
              puVar2[1] = uVar17;
              puVar2[2] = uStack_b0;
              puVar2[3] = uStack_ac;
              goto LAB_14044b9ba;
            }
            uVar14 = uVar14 + 1;
            lVar13 = *(longlong *)(lVar5 + 200);
            lVar15 = *(longlong *)(lVar5 + 0xc0);
          } while (lVar13 != lVar15);
          plVar11 = plVar11 + 2;
        }
      }
      else {
        dVar18 = (double)(lVar5 + 0x130);
        local_res20 = dVar18;
        iVar10 = _Mtx_lock(dVar18);
        if (iVar10 != 0) {
          std::_Throw_Cpp_error(5);
          pcVar7 = (code *)swi(3);
          (*pcVar7)();
          return;
        }
        if (*(int *)(lVar5 + 0x17c) == 0x7fffffff) {
          *(undefined4 *)(lVar5 + 0x17c) = 0x7ffffffe;
          std::_Throw_Cpp_error(6);
          pcVar7 = (code *)swi(3);
          (*pcVar7)();
          return;
        }
        FUN_140364d10(lVar5 + 0xf0,local_a8);
        FUN_140290980(local_a8[0] + 8,&local_b8);
        _Mtx_unlock(dVar18);
        plVar11 = plVar11 + 2;
      }
    }
  }
  return;
}
