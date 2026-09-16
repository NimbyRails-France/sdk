
void FUN_140450b40(longlong param_1,undefined8 param_2,ulonglong *param_3)

{
  longlong *_Src;
  void *pvVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  code *pcVar4;
  undefined1 auVar5 [16];
  undefined1 auVar6 [16];
  undefined1 auVar7 [16];
  longlong lVar8;
  char cVar9;
  int iVar10;
  longlong *plVar11;
  longlong lVar12;
  longlong lVar13;
  longlong lVar14;
  undefined8 *puVar15;
  longlong *_Dst;
  longlong *plVar16;
  byte bVar17;
  undefined8 *puVar18;
  longlong *plVar19;
  longlong *plVar20;
  longlong *plVar21;
  longlong *local_res10;
  ulonglong *local_res18;
  longlong *local_res20;
  longlong *local_178;
  longlong *plStack_170;
  longlong *local_168;
  longlong *local_160;
  longlong *local_158;
  longlong *local_150;
  longlong *local_148;
  longlong *local_140;
  longlong *local_138;
  longlong *local_130;
  undefined8 local_128;
  char local_120;
  undefined7 uStack_11f;
  longlong local_118;
  void *local_108;
  undefined8 local_100;
  longlong *local_f8;
  longlong *plStack_f0;
  longlong *local_e8;
  longlong *plStack_e0;
  longlong *local_d8;
  undefined8 uStack_d0;
  undefined8 local_c8;
  longlong lStack_c0;
  longlong local_a8;
  void *local_a0;
  undefined8 local_98;
  undefined8 local_90;
  undefined8 uStack_88;
  undefined8 local_80;
  undefined8 *puStack_78;
  undefined8 local_70;
  undefined8 uStack_68;
  undefined8 local_60;
  longlong lStack_58;

  local_108 = (void *)0x0;
  local_100 = 0;
  local_f8 = (longlong *)0x0;
  plStack_f0 = (longlong *)0x0;
  local_e8 = (longlong *)0x0;
  plStack_e0 = (longlong *)0x0;
  local_d8 = (longlong *)0x0;
  uStack_d0 = 0;
  local_c8 = 0;
  lStack_c0 = 0;
  local_res18 = param_3;
  FUN_140461850(&local_108);
  if ((char)param_3[0x96] != '\0') {
    local_a0 = (void *)0x0;
    local_98 = 0;
    local_90 = 0;
    uStack_88 = 0;
    local_80 = 0;
    puStack_78 = (undefined8 *)0x0;
    local_70 = 0;
    uStack_68 = 0;
    local_60 = 0;
    lStack_58 = 0;
    FUN_140461850(&local_a0);
    auVar5._8_8_ = 0;
    auVar5._0_8_ = *param_3;
    lVar12 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,8) +
             SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,0);
    local_128 = 0;
    local_120 = '\0';
    FUN_14045d5a0(param_1,&local_178,lVar12,param_3,&local_128);
    plVar20 = local_168;
    plVar16 = plStack_170;
    lVar14 = local_168[0xb];
    if ((char)local_178 == '\0') {
      lVar14 = (longlong)plStack_170 * 0x60 + lVar14;
      FUN_14045ef00(&local_108,lVar14 + 8);
      FUN_14045f080(lVar14 + 8);
    }
    else {
      *(ulonglong *)((longlong)plStack_170 * 0x60 + lVar14) = *param_3;
      FUN_140461780((longlong)plStack_170 * 0x60 + lVar14 + 8,&local_a0);
      bVar17 = (byte)lVar12 & 0x7f;
      *(byte *)((longlong)plVar16 + plVar20[10]) = bVar17;
      *(byte *)(((ulonglong)(plVar16 + -2) & plVar20[0xd]) + (ulonglong)((uint)plVar20[0xd] & 0xf) +
                1 + plVar20[10]) = bVar17;
    }
    if (local_120 != '\0') {
      _Mtx_unlock(local_128);
    }
    if (local_a0 != (void *)0x0) {
      puVar18 = (undefined8 *)(lStack_58 + 8);
      puVar15 = puStack_78;
      while (puVar15 < puVar18) {
        pvVar1 = (void *)*puVar15;
        puVar15 = puVar15 + 1;
        if (pvVar1 != (void *)0x0) {
          free(pvVar1);
        }
      }
      if (local_a0 != (void *)0x0) {
        free(local_a0);
      }
    }
    local_138 = plStack_f0;
    local_res10 = local_e8;
    local_130 = local_e8;
    local_res20 = plStack_e0;
    plVar16 = local_f8;
    if (local_f8 != local_d8) {
      do {
        if (param_3[0x75] == plVar16[1]) break;
        plVar16 = plVar16 + 4;
        if (plVar16 == local_130) {
          local_res20 = local_res20 + 1;
          plVar16 = (longlong *)*local_res20;
          local_130 = plVar16 + 0x20;
          local_138 = plVar16;
        }
      } while (plVar16 != local_d8);
    }
    if (plVar16 != local_d8) {
      if (local_f8 != plVar16) {
        local_a8 = param_1 + 0x880;
        plVar20 = local_e8;
        plVar19 = local_f8;
        plVar21 = plStack_e0;
        do {
          local_178 = (longlong *)0x0;
          plStack_170 = (longlong *)0x0;
          local_168 = (longlong *)0x0;
          auVar6._8_8_ = 0;
          auVar6._0_8_ = plVar19[1];
          lVar12 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,8) +
                   SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar6,0);
          local_158 = (longlong *)0x0;
          local_150 = (longlong *)((ulonglong)local_150 & 0xffffffffffffff00);
          FUN_14045d710(0,&local_128,lVar12,plVar19 + 1,&local_158);
          lVar14 = *(longlong *)(local_118 + 0x58);
          if ((char)local_128 == '\0') {
            lVar12 = CONCAT71(uStack_11f,local_120) * 0x20;
            _Dst = *(longlong **)(lVar12 + 8 + lVar14);
            plVar11 = *(longlong **)(lVar12 + 0x10 + lVar14);
            plVar20 = local_res10;
            if (_Dst != plVar11) {
              do {
                if ((((*_Dst == *plVar19) && (_Dst[1] == plVar19[1])) &&
                    ((double)plVar19[2] == (double)_Dst[2])) &&
                   ((double)plVar19[3] == (double)_Dst[3])) {
                  _Src = _Dst + 4;
                  if (_Src < plVar11) {
                    memmove(_Dst,_Src,(longlong)plVar11 - (longlong)_Src);
                  }
                  plVar11 = (longlong *)(lVar12 + 0x10 + lVar14);
                  *plVar11 = *plVar11 + -0x20;
                  plVar11 = *(longlong **)(lVar12 + 0x10 + lVar14);
                }
                else {
                  _Dst = _Dst + 4;
                }
              } while (_Dst != plVar11);
            }
          }
          else {
            lVar8 = CONCAT71(uStack_11f,local_120);
            lVar13 = lVar8 * 0x20;
            *(longlong *)(lVar13 + lVar14) = plVar19[1];
            *(undefined8 *)(lVar13 + 8 + lVar14) = 0;
            *(undefined8 *)(lVar13 + 0x10 + lVar14) = 0;
            *(undefined8 *)(lVar13 + 0x18 + lVar14) = 0;
            *(longlong **)(lVar13 + 8 + lVar14) = local_178;
            local_178 = (longlong *)0x0;
            uVar2 = *(undefined8 *)(lVar13 + 0x10 + lVar14);
            *(longlong **)(lVar13 + 0x10 + lVar14) = plStack_170;
            uVar3 = *(undefined8 *)(lVar13 + 0x18 + lVar14);
            *(longlong **)(lVar13 + 0x18 + lVar14) = local_168;
            bVar17 = (byte)lVar12 & 0x7f;
            *(byte *)(lVar8 + *(longlong *)(local_118 + 0x50)) = bVar17;
            *(byte *)((lVar8 - 0x10U & *(ulonglong *)(local_118 + 0x68)) +
                      (ulonglong)((uint)*(ulonglong *)(local_118 + 0x68) & 0xf) + 1 +
                     *(longlong *)(local_118 + 0x50)) = bVar17;
            plStack_170 = (longlong *)uVar2;
            local_168 = (longlong *)uVar3;
          }
          if ((char)local_150 != '\0') {
            _Mtx_unlock(local_158);
          }
          if (local_178 != (longlong *)0x0) {
            free(local_178);
          }
          plVar19 = plVar19 + 4;
          if (plVar19 == plVar20) {
            plVar21 = plVar21 + 1;
            plVar19 = (longlong *)*plVar21;
            plVar20 = plVar19 + 0x20;
            local_res10 = plVar20;
          }
          param_3 = local_res18;
        } while (plVar19 != plVar16);
      }
      plStack_170 = local_138;
      local_168 = local_130;
      local_158 = local_f8;
      local_150 = plStack_f0;
      local_148 = local_e8;
      local_140 = plStack_e0;
      local_178 = plVar16;
      local_160 = local_res20;
      FUN_140458be0(&local_108,&local_128,&local_158,&local_178);
    }
    local_158 = (longlong *)0x0;
    local_150 = (longlong *)((ulonglong)local_150 & 0xffffffffffffff00);
    auVar7._8_8_ = 0;
    auVar7._0_8_ = *param_3;
    lVar12 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,8) +
             SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,0);
    plVar16 = (longlong *)
              ((ulonglong)
               (((uint)((ulonglong)lVar12 >> 0x18) ^ (uint)((ulonglong)lVar12 >> 0x10) ^
                (uint)((ulonglong)lVar12 >> 8)) & 0xf) * 0x88 + param_1);
    iVar10 = _Mtx_lock(plVar16);
    if (iVar10 != 0) {
      std::_Throw_Cpp_error(5);
      pcVar4 = (code *)swi(3);
      (*pcVar4)();
      return;
    }
    if (*(int *)((longlong)plVar16 + 0x4c) == 0x7fffffff) {
      *(undefined4 *)((longlong)plVar16 + 0x4c) = 0x7ffffffe;
      std::_Throw_Cpp_error(6);
      pcVar4 = (code *)swi(3);
      (*pcVar4)();
      return;
    }
    local_150 = (longlong *)CONCAT71(local_150._1_7_,1);
    local_158 = plVar16;
    cVar9 = FUN_14045f780(plVar16 + 10,param_3,lVar12,&local_res18);
    if ((cVar9 != '\0') && (lVar12 = (longlong)local_res18 * 0x60 + plVar16[0xb], lVar12 != 0)) {
      FUN_14045ef00(lVar12 + 8,&local_108);
    }
    _Mtx_unlock(plVar16);
  }
  if (local_108 != (void *)0x0) {
    plVar20 = (longlong *)(lStack_c0 + 8);
    plVar16 = plStack_e0;
    while (plVar16 < plVar20) {
      pvVar1 = (void *)*plVar16;
      plVar16 = plVar16 + 1;
      if (pvVar1 != (void *)0x0) {
        free(pvVar1);
      }
    }
    if (local_108 != (void *)0x0) {
      free(local_108);
    }
  }
  return;
}
