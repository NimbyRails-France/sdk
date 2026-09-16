
void FUN_1404512d0(longlong param_1,ulonglong param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  void *_Memory;
  code *pcVar3;
  undefined1 auVar4 [16];
  undefined1 auVar5 [16];
  longlong lVar6;
  char cVar7;
  int iVar8;
  ulonglong *puVar9;
  undefined8 *puVar10;
  byte bVar11;
  longlong lVar12;
  ulonglong *_Dst;
  ulonglong *_Src;
  longlong *plVar13;
  longlong *plVar14;
  longlong lVar15;
  longlong lVar16;
  longlong local_res8;
  ulonglong local_res10;
  longlong local_d8;
  char local_d0;
  void *local_c8;
  undefined8 uStack_c0;
  undefined8 local_b8;
  char local_b0 [8];
  longlong local_a8;
  longlong local_a0;
  void *local_98;
  undefined8 local_90;
  longlong local_88;
  undefined8 uStack_80;
  longlong local_78;
  longlong *plStack_70;
  longlong local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  longlong lStack_50;

  local_98 = (void *)0x0;
  local_90 = 0;
  local_88 = 0;
  uStack_80 = 0;
  local_78 = 0;
  plStack_70 = (longlong *)0x0;
  local_68 = 0;
  uStack_60 = 0;
  local_58 = 0;
  lStack_50 = 0;
  local_res10 = param_2;
  FUN_140461850(&local_98);
  local_d8 = 0;
  local_d0 = 0;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = local_res10;
  lVar12 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar4,8) +
           SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar4,0);
  lVar15 = (ulonglong)
           (((uint)((ulonglong)lVar12 >> 0x18) ^ (uint)((ulonglong)lVar12 >> 0x10) ^
            (uint)((ulonglong)lVar12 >> 8)) & 0xf) * 0x88 + param_1;
  iVar8 = _Mtx_lock(lVar15);
  if (iVar8 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar3 = (code *)swi(3);
    (*pcVar3)();
    return;
  }
  if (*(int *)(lVar15 + 0x4c) != 0x7fffffff) {
    local_d0 = '\x01';
    local_d8 = lVar15;
    cVar7 = FUN_14045f780(lVar15 + 0x50,&local_res10,lVar12,&local_res8);
    if ((cVar7 != '\0') && (lVar12 = local_res8 * 0x60 + *(longlong *)(lVar15 + 0x58), lVar12 != 0))
    {
      FUN_14045ef00(&local_98,lVar12 + 8);
      FUN_14045f080(lVar12 + 8);
    }
    _Mtx_unlock(lVar15);
    FUN_140457bb0(param_1,&local_res10);
    lVar6 = local_68;
    lVar15 = local_88;
    lVar12 = local_78;
    plVar14 = plStack_70;
    while (lVar15 != lVar6) {
      local_c8 = (void *)0x0;
      uStack_c0 = 0;
      local_b8 = 0;
      auVar5._8_8_ = 0;
      auVar5._0_8_ = *(ulonglong *)(lVar15 + 8);
      lVar16 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,8) +
               SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,0);
      local_d8 = 0;
      local_d0 = '\0';
      FUN_14045d710(param_1 + 0x880,local_b0,lVar16,lVar15 + 8,&local_d8);
      if (local_b0[0] == '\0') {
        lVar16 = local_a8 * 0x20 + *(longlong *)(local_a0 + 0x58);
        puVar9 = *(ulonglong **)(lVar16 + 0x10);
        _Dst = *(ulonglong **)(lVar16 + 8);
        if (*(ulonglong **)(lVar16 + 8) != puVar9) {
          do {
            _Src = _Dst + 4;
            if (*_Dst == local_res10) {
              if (_Src < puVar9) {
                memmove(_Dst,_Src,(longlong)puVar9 - (longlong)_Src);
              }
              *(longlong *)(lVar16 + 0x10) = *(longlong *)(lVar16 + 0x10) + -0x20;
              puVar9 = *(ulonglong **)(lVar16 + 0x10);
              _Src = _Dst;
            }
            _Dst = _Src;
          } while (_Src != puVar9);
        }
      }
      else {
        puVar10 = (undefined8 *)(local_a8 * 0x20 + *(longlong *)(local_a0 + 0x58));
        *puVar10 = *(undefined8 *)(lVar15 + 8);
        puVar10[1] = 0;
        puVar10[2] = 0;
        puVar10[3] = 0;
        puVar10[1] = local_c8;
        local_c8 = (void *)0x0;
        uVar1 = puVar10[2];
        puVar10[2] = uStack_c0;
        uVar2 = puVar10[3];
        puVar10[3] = local_b8;
        bVar11 = (byte)lVar16 & 0x7f;
        *(byte *)(local_a8 + *(longlong *)(local_a0 + 0x50)) = bVar11;
        *(byte *)((local_a8 - 0x10U & *(ulonglong *)(local_a0 + 0x68)) +
                  (ulonglong)((uint)*(ulonglong *)(local_a0 + 0x68) & 0xf) + 1 +
                 *(longlong *)(local_a0 + 0x50)) = bVar11;
        uStack_c0 = uVar1;
        local_b8 = uVar2;
      }
      if (local_d0 != '\0') {
        _Mtx_unlock(local_d8);
      }
      if (local_c8 != (void *)0x0) {
        free(local_c8);
      }
      lVar15 = lVar15 + 0x20;
      if (lVar15 == lVar12) {
        plVar14 = plVar14 + 1;
        lVar15 = *plVar14;
        lVar12 = lVar15 + 0x100;
      }
    }
    if (local_98 != (void *)0x0) {
      plVar13 = (longlong *)(lStack_50 + 8);
      plVar14 = plStack_70;
      while (plVar14 < plVar13) {
        _Memory = (void *)*plVar14;
        plVar14 = plVar14 + 1;
        if (_Memory != (void *)0x0) {
          free(_Memory);
        }
      }
      if (local_98 != (void *)0x0) {
        free(local_98);
      }
    }
    return;
  }
  *(undefined4 *)(lVar15 + 0x4c) = 0x7ffffffe;
  std::_Throw_Cpp_error(6);
  pcVar3 = (code *)swi(3);
  (*pcVar3)();
  return;
}
