
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14046cbd0(longlong *param_1,undefined8 *param_2,longlong param_3,longlong param_4,
                  longlong param_5)

{
  int *piVar1;
  void *pvVar2;
  void *pvVar3;
  undefined1 (*pauVar4) [16];
  longlong **pplVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;
  longlong *plVar10;
  longlong *plVar11;
  longlong lVar12;
  char cVar13;
  char cVar14;
  char cVar15;
  char cVar16;
  char cVar17;
  char cVar18;
  char cVar19;
  char cVar20;
  char cVar21;
  char cVar22;
  char cVar23;
  char cVar24;
  char cVar25;
  char cVar26;
  char cVar27;
  char cVar28;
  undefined1 auVar29 [8];
  longlong **pplVar30;
  char cVar31;
  uint uVar32;
  undefined8 uVar33;
  longlong *plVar34;
  longlong *plVar35;
  longlong lVar36;
  ulonglong *puVar37;
  longlong lVar38;
  void *pvVar39;
  void *pvVar40;
  undefined8 *puVar41;
  ulonglong uVar42;
  undefined8 *puVar43;
  longlong lVar44;
  longlong lVar45;
  longlong *plVar46;
  undefined1 (*_Memory) [16];
  uint uVar47;
  int iVar48;
  longlong *plVar49;
  ulonglong uVar50;
  longlong *plVar51;
  longlong *plVar52;
  undefined1 auVar53 [16];
  undefined1 auVar54 [16];
  longlong *local_1c8;
  uint local_1c0;
  longlong *local_1b8;
  undefined8 local_1b0;
  undefined1 local_1a8 [8];
  longlong **pplStack_1a0;
  longlong local_198;
  longlong *plStack_190;
  undefined8 local_178;
  longlong *local_168;
  longlong *plStack_160;
  longlong *local_158;
  longlong *plStack_150;
  longlong *local_148;
  undefined4 local_140;
  int local_13c;
  longlong *local_138;
  longlong *local_130;
  undefined1 (*local_128) [16];
  longlong **pplStack_120;
  undefined8 *local_118;
  longlong **local_110;
  longlong *local_108;
  longlong *plStack_100;
  undefined8 local_f8;
  longlong local_f0;
  longlong local_e8;
  longlong local_e0;
  longlong local_d8;
  longlong local_d0;
  longlong *local_c8;
  longlong *local_c0;
  longlong *local_b8;
  undefined4 local_a8 [2];
  undefined8 local_a0;
  undefined8 local_98;
  undefined8 local_90;
  undefined8 uStack_88;
  undefined8 local_80;
  undefined8 uStack_78;
  undefined8 local_70;
  undefined8 uStack_68;
  undefined4 local_60;
  undefined4 local_5c;
  
  local_1b0 = (longlong *)((ulonglong)local_1b0 & 0xffffffff00000000);
  local_1c0 = 0;
  param_5._0_4_ = (float)((double)param_5 * DAT_140aabb00);
  if (DAT_140b77e58 < 4) {
    uVar47 = 2;
  }
  else if (*(uint *)(param_4 + 0x2218) < 2) {
    uVar47 = DAT_140b77e58 - 1;
  }
  else {
    uVar32 = *(uint *)(param_4 + 0x2218) - 1;
    uVar47 = 2;
    if (2 < uVar32) {
      uVar47 = uVar32;
    }
  }
  local_108 = (longlong *)0x0;
  plStack_100 = (longlong *)0x0;
  local_f8 = 0;
  FUN_1404707a0(&local_108,uVar47);
  FUN_1404705e0(param_1 + 0xd,uVar47);
  local_e8 = param_3 + 0x400;
  local_1b8 = *(longlong **)(param_3 + 0x410);
  local_f0 = param_4 + 0x40;
  FUN_14046e3b0(param_1,*param_2,param_3);
  local_158 = (longlong *)0x0;
  plStack_150 = (longlong *)0x0;
  local_148 = (longlong *)0x0;
  local_140 = 0;
  local_13c = (int)(*(longlong *)(param_3 + 0xa0) - *(longlong *)(param_3 + 0x98) >> 3) *
              *(int *)(param_3 + 0x88);
  plVar46 = (longlong *)(*(longlong *)(param_4 + 0x2200) + 0x458);
  if (plVar46 != param_1 + 0x10) {
    FUN_1403506e0(plVar46);
    pplStack_1a0 = (longlong **)param_1[0x11];
    plStack_160 = (longlong *)(pplStack_1a0 + param_1[0x12]);
    local_168 = (longlong *)*plStack_160;
    local_1a8 = (undefined1  [8])*pplStack_1a0;
    if (*pplStack_1a0 == (longlong *)0x0) {
      pplStack_1a0 = pplStack_1a0 + 1;
      lVar44 = (longlong)*pplStack_1a0;
      while (lVar44 == 0) {
        pplStack_1a0 = pplStack_1a0 + 1;
        lVar44 = (longlong)*pplStack_1a0;
      }
      local_1a8 = (undefined1  [8])*pplStack_1a0;
    }
    FUN_140472b20(plVar46,local_1a8,&local_168);
  }
  if (*(char *)(param_4 + 0x2348) != '\0') {
    plVar46 = (longlong *)param_1[0x11];
    pvVar39 = (void *)*plVar46;
    if (pvVar39 == (void *)0x0) {
      plVar46 = plVar46 + 1;
      pvVar39 = (void *)*plVar46;
      while (pvVar39 == (void *)0x0) {
        plVar46 = plVar46 + 1;
        pvVar39 = (void *)*plVar46;
      }
    }
    if (pvVar39 != *(void **)(param_1[0x11] + param_1[0x12] * 8)) {
      do {
        lVar44 = 0;
        puVar37 = *(ulonglong **)
                   (*(longlong *)(param_4 + 0x2320) +
                   (*(ulonglong *)((longlong)pvVar39 + 8) % (ulonglong)*(uint *)(param_4 + 9000)) *
                   8);
        plVar34 = plVar46;
        if (puVar37 == (ulonglong *)0x0) {
LAB_14046ce71:
          pvVar40 = *(void **)((longlong)pvVar39 + 0x80);
          while (pvVar40 == (void *)0x0) {
            plVar34 = plVar34 + 1;
            pvVar40 = (void *)*plVar34;
          }
          pvVar2 = (void *)*plVar46;
          pvVar3 = *(void **)((longlong)pvVar2 + 0x80);
          if (pvVar2 == pvVar39) {
            *plVar46 = (longlong)pvVar3;
          }
          else {
            for (; pvVar3 != pvVar39; pvVar3 = *(void **)((longlong)pvVar3 + 0x80)) {
              pvVar2 = pvVar3;
            }
            *(undefined8 *)((longlong)pvVar2 + 0x80) = *(undefined8 *)((longlong)pvVar3 + 0x80);
          }
          FUN_14034b770((longlong)pvVar39 + 0x48);
          FUN_14034b770((longlong)pvVar39 + 0x10);
          free(pvVar39);
          param_1[0x13] = param_1[0x13] + -1;
        }
        else {
          do {
            lVar36 = lVar44 + 1;
            if (*(ulonglong *)((longlong)pvVar39 + 8) != *puVar37) {
              lVar36 = lVar44;
            }
            puVar37 = (ulonglong *)puVar37[1];
            lVar44 = lVar36;
          } while (puVar37 != (ulonglong *)0x0);
          if (lVar36 == 0) goto LAB_14046ce71;
          pvVar40 = *(void **)((longlong)pvVar39 + 0x80);
          while (pvVar40 == (void *)0x0) {
            plVar34 = plVar34 + 1;
            pvVar40 = (void *)*plVar34;
          }
        }
        pvVar39 = pvVar40;
        plVar46 = plVar34;
      } while (pvVar40 != *(void **)(param_1[0x11] + param_1[0x12] * 8));
    }
    plVar46 = *(longlong **)(param_4 + 0x2320);
    puVar41 = (undefined8 *)*plVar46;
    if (puVar41 == (undefined8 *)0x0) {
      plVar46 = plVar46 + 1;
      puVar41 = (undefined8 *)*plVar46;
      while (puVar41 == (undefined8 *)0x0) {
        plVar46 = plVar46 + 1;
        puVar41 = (undefined8 *)*plVar46;
      }
    }
    puVar43 = *(undefined8 **)(*(longlong *)(param_4 + 0x2320) + *(longlong *)(param_4 + 9000) * 8);
    while (puVar41 != puVar43) {
      FUN_1404735f0(param_1 + 0x10,local_1a8);
      *(undefined8 *)((longlong)local_1a8 + 8) = *puVar41;
      puVar41 = (undefined8 *)puVar41[1];
      while (puVar41 == (undefined8 *)0x0) {
        plVar46 = plVar46 + 1;
        puVar41 = (undefined8 *)*plVar46;
      }
    }
  }
  uVar42 = param_1[0x25];
  if ((ulonglong)param_1[0x25] < (ulonglong)param_1[0x18]) {
    uVar42 = param_1[0x18];
  }
  param_1[0x18] = uVar42;
  FUN_14045a3f0(param_1 + 0x19);
  uVar42 = param_1[0x38];
  if ((ulonglong)param_1[0x38] < (ulonglong)param_1[0x31]) {
    uVar42 = param_1[0x31];
  }
  param_1[0x31] = uVar42;
  FUN_140470b60(param_1 + 0x32);
  if (param_1 + 0x44 != param_1 + 0x154) {
    plVar46 = param_1 + 0x4e;
    do {
      FUN_1400020a0(plVar46 + -10);
      if (plVar46[2] != 0) {
        if ((ulonglong)plVar46[3] < 0x80) {
          if (plVar46[3] != 0) {
            lVar36 = 0;
            lVar44 = 0;
            do {
              if (-1 < *(char *)(lVar36 + *plVar46)) {
                FUN_140353b70(plVar46[1] + 0x18 + lVar44);
              }
              lVar36 = lVar36 + 1;
              lVar44 = lVar44 + 0x50;
              lVar38 = plVar46[3];
            } while (lVar36 != lVar38);
            plVar46[2] = 0;
            memset((void *)*plVar46,-0x80,lVar38 + 0x10);
            *(undefined1 *)(lVar38 + *plVar46) = 0xff;
            plVar46[6] = (plVar46[3] - plVar46[2]) - ((ulonglong)plVar46[3] >> 3);
          }
        }
        else {
          FUN_1403537d0(plVar46);
        }
      }
      _Mtx_unlock(plVar46 + -10);
      plVar34 = plVar46 + 7;
      plVar46 = plVar46 + 0x11;
    } while (plVar34 != param_1 + 0x154);
  }
  local_1c8 = (longlong *)0x0;
  local_128 = (undefined1 (*) [16])local_1b8;
  local_118 = &param_5;
  local_110 = &local_1c8;
  local_138 = param_1;
  local_130 = (longlong *)(param_4 + 0x40);
  pplStack_120 = (longlong **)param_4;
  FUN_14046fb80(uVar47,(*(longlong *)(param_1[4] + 0xe0) - *(longlong *)(param_1[4] + 0xd8)) / 0x90,
                &local_138);
  cVar28 = UNK_140aad76f;
  cVar27 = UNK_140aad76e;
  cVar26 = UNK_140aad76d;
  cVar25 = UNK_140aad76c;
  cVar24 = UNK_140aad76b;
  cVar23 = UNK_140aad76a;
  cVar22 = UNK_140aad769;
  cVar21 = UNK_140aad768;
  cVar20 = UNK_140aad767;
  cVar19 = UNK_140aad766;
  cVar18 = UNK_140aad765;
  cVar17 = UNK_140aad764;
  cVar16 = UNK_140aad763;
  cVar15 = UNK_140aad762;
  cVar14 = UNK_140aad761;
  cVar13 = DAT_140aad760;
  local_128 = (undefined1 (*) [16])param_1[0x4e];
  pplStack_120 = (longlong **)param_1[0x4f];
  cVar31 = (*local_128)[0];
  while (cVar31 < -1) {
    local_1c0 = 0;
    auVar53[0] = -((char)(*local_128)[0] < DAT_140aad760);
    auVar53[1] = -((char)(*local_128)[1] < UNK_140aad761);
    auVar53[2] = -((char)(*local_128)[2] < UNK_140aad762);
    auVar53[3] = -((char)(*local_128)[3] < UNK_140aad763);
    auVar53[4] = -((char)(*local_128)[4] < UNK_140aad764);
    auVar53[5] = -((char)(*local_128)[5] < UNK_140aad765);
    auVar53[6] = -((char)(*local_128)[6] < UNK_140aad766);
    auVar53[7] = -((char)(*local_128)[7] < UNK_140aad767);
    auVar53[8] = -((char)(*local_128)[8] < UNK_140aad768);
    auVar53[9] = -((char)(*local_128)[9] < UNK_140aad769);
    auVar53[10] = -((char)(*local_128)[10] < UNK_140aad76a);
    auVar53[0xb] = -((char)(*local_128)[0xb] < UNK_140aad76b);
    auVar53[0xc] = -((char)(*local_128)[0xc] < UNK_140aad76c);
    auVar53[0xd] = -((char)(*local_128)[0xd] < UNK_140aad76d);
    auVar53[0xe] = -((char)(*local_128)[0xe] < UNK_140aad76e);
    auVar53[0xf] = -((char)(*local_128)[0xf] < UNK_140aad76f);
    uVar32 = (ushort)((ushort)(SUB161(auVar53 >> 7,0) & 1) |
                      (ushort)(SUB161(auVar53 >> 0xf,0) & 1) << 1 |
                      (ushort)(SUB161(auVar53 >> 0x17,0) & 1) << 2 |
                      (ushort)(SUB161(auVar53 >> 0x1f,0) & 1) << 3 |
                      (ushort)(SUB161(auVar53 >> 0x27,0) & 1) << 4 |
                      (ushort)(SUB161(auVar53 >> 0x2f,0) & 1) << 5 |
                      (ushort)(SUB161(auVar53 >> 0x37,0) & 1) << 6 |
                      (ushort)(SUB161(auVar53 >> 0x3f,0) & 1) << 7 |
                      (ushort)(SUB161(auVar53 >> 0x47,0) & 1) << 8 |
                      (ushort)(SUB161(auVar53 >> 0x4f,0) & 1) << 9 |
                      (ushort)(SUB161(auVar53 >> 0x57,0) & 1) << 10 |
                      (ushort)(SUB161(auVar53 >> 0x5f,0) & 1) << 0xb |
                      (ushort)(SUB161(auVar53 >> 0x67,0) & 1) << 0xc |
                      (ushort)(SUB161(auVar53 >> 0x6f,0) & 1) << 0xd |
                      (ushort)(SUB161(auVar53 >> 0x77,0) & 1) << 0xe |
                     (ushort)(auVar53[0xf] >> 7) << 0xf) + 1;
    uVar47 = 0;
    if (uVar32 != 0) {
      for (; (uVar32 >> uVar47 & 1) == 0; uVar47 = uVar47 + 1) {
      }
    }
    local_128 = (undefined1 (*) [16])(*local_128 + uVar47);
    pplStack_120 = pplStack_120 + (ulonglong)uVar47 * 10;
    cVar31 = (*local_128)[0];
  }
  local_130 = param_1 + 0x154;
  local_118 = (undefined8 *)0x0;
  if (param_1 + 0x44 != (longlong *)0x0) {
    local_118 = (undefined8 *)(param_1[0x51] + param_1[0x4e]);
    local_110 = pplStack_120;
  }
  local_138 = param_1 + 0x44;
  local_1a8 = (undefined1  [8])local_128;
  pplStack_1a0 = pplStack_120;
  FUN_140472030(&local_138);
  while (pplVar30 = pplStack_120, local_138 != (longlong *)0x0) {
    local_1b8 = *pplStack_120;
    FUN_140486b80(param_4 + 0x70,local_1a8);
    auVar29 = local_1a8;
    *(longlong **)((undefined1 *)local_1a8 + 8) = pplVar30[1];
    *(longlong **)*(undefined1 (*) [16])((longlong)local_1a8 + 0x10) = pplVar30[2];
    FUN_140472160(local_1a8,pplVar30 + 3);
    pauVar4 = *(undefined1 (**) [16])(*(undefined1 (*) [16])((longlong)auVar29 + 0x10) + 8);
    *(undefined1 (*) [8])(*(undefined1 (*) [16])((longlong)auVar29 + 0x10) + 8) = local_1a8;
    pplVar5 = *(longlong ***)*(undefined1 (*) [16])((longlong)auVar29 + 0x20);
    *(longlong ***)*(undefined1 (*) [16])((longlong)auVar29 + 0x20) = pplStack_1a0;
    lVar44 = *(longlong *)(*(undefined1 (*) [16])((longlong)auVar29 + 0x20) + 8);
    *(longlong *)(*(undefined1 (*) [16])((longlong)auVar29 + 0x20) + 8) = local_198;
    plVar46 = *(longlong **)*(undefined1 (*) [16])((longlong)auVar29 + 0x30);
    *(longlong **)*(undefined1 (*) [16])((longlong)auVar29 + 0x30) = plStack_190;
    uVar33 = *(undefined8 *)(*(undefined1 (*) [16])((longlong)auVar29 + 0x40) + 8);
    *(undefined8 *)(*(undefined1 (*) [16])((longlong)auVar29 + 0x40) + 8) = local_178;
    local_198 = lVar44;
    plStack_190 = plVar46;
    local_178 = uVar33;
    local_1a8 = (undefined1  [8])pauVar4;
    pplStack_1a0 = pplVar5;
    if (plVar46 != (longlong *)0x0) {
      _Memory = pauVar4;
      if ((0xfff < (longlong)plVar46 * 0x20 + ((ulonglong)(plVar46 + 3) & 0xfffffffffffffff8)) &&
         (_Memory = *(undefined1 (**) [16])(pauVar4[-1] + 8),
         (undefined1 *)0x1f < (undefined1 *)((longlong)pauVar4 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(_Memory);
      local_1a8 = (undefined1  [8])&DAT_140a6d430;
      pplStack_1a0 = (longlong **)0x0;
      local_198 = 0;
      plStack_190 = (longlong *)0x0;
      local_178 = 0;
    }
    local_128 = (undefined1 (*) [16])(*local_128 + 1);
    pplStack_120 = pplVar30 + 10;
    cVar31 = (*local_128)[0];
    while (cVar31 < -1) {
      local_1c0 = 0;
      auVar53 = *local_128;
      auVar54[0] = -(auVar53[0] < cVar13);
      auVar54[1] = -(auVar53[1] < cVar14);
      auVar54[2] = -(auVar53[2] < cVar15);
      auVar54[3] = -(auVar53[3] < cVar16);
      auVar54[4] = -(auVar53[4] < cVar17);
      auVar54[5] = -(auVar53[5] < cVar18);
      auVar54[6] = -(auVar53[6] < cVar19);
      auVar54[7] = -(auVar53[7] < cVar20);
      auVar54[8] = -(auVar53[8] < cVar21);
      auVar54[9] = -(auVar53[9] < cVar22);
      auVar54[10] = -(auVar53[10] < cVar23);
      auVar54[0xb] = -(auVar53[0xb] < cVar24);
      auVar54[0xc] = -(auVar53[0xc] < cVar25);
      auVar54[0xd] = -(auVar53[0xd] < cVar26);
      auVar54[0xe] = -(auVar53[0xe] < cVar27);
      auVar54[0xf] = -(auVar53[0xf] < cVar28);
      uVar32 = (ushort)((ushort)(SUB161(auVar54 >> 7,0) & 1) |
                        (ushort)(SUB161(auVar54 >> 0xf,0) & 1) << 1 |
                        (ushort)(SUB161(auVar54 >> 0x17,0) & 1) << 2 |
                        (ushort)(SUB161(auVar54 >> 0x1f,0) & 1) << 3 |
                        (ushort)(SUB161(auVar54 >> 0x27,0) & 1) << 4 |
                        (ushort)(SUB161(auVar54 >> 0x2f,0) & 1) << 5 |
                        (ushort)(SUB161(auVar54 >> 0x37,0) & 1) << 6 |
                        (ushort)(SUB161(auVar54 >> 0x3f,0) & 1) << 7 |
                        (ushort)(SUB161(auVar54 >> 0x47,0) & 1) << 8 |
                        (ushort)(SUB161(auVar54 >> 0x4f,0) & 1) << 9 |
                        (ushort)(SUB161(auVar54 >> 0x57,0) & 1) << 10 |
                        (ushort)(SUB161(auVar54 >> 0x5f,0) & 1) << 0xb |
                        (ushort)(SUB161(auVar54 >> 0x67,0) & 1) << 0xc |
                        (ushort)(SUB161(auVar54 >> 0x6f,0) & 1) << 0xd |
                        (ushort)(SUB161(auVar54 >> 0x77,0) & 1) << 0xe |
                       (ushort)(auVar54[0xf] >> 7) << 0xf) + 1;
      uVar47 = 0;
      if (uVar32 != 0) {
        for (; (uVar32 >> uVar47 & 1) == 0; uVar47 = uVar47 + 1) {
        }
      }
      local_128 = (undefined1 (*) [16])(*local_128 + uVar47);
      pplStack_120 = pplStack_120 + (ulonglong)uVar47 * 10;
      cVar31 = (*local_128)[0];
    }
    FUN_140472030(&local_138);
  }
  *(undefined4 *)((longlong)param_1 + 0x44) = local_1c8._0_4_;
  *(undefined4 *)(param_1 + 9) = local_1c8._4_4_;
  lVar36 = _UNK_140aacc78;
  lVar44 = _DAT_140aacc70;
  local_90 = 0;
  uStack_88 = 0;
  local_80 = 0;
  uStack_78 = 0;
  local_70 = 0;
  uStack_68 = 0;
  local_a0 = 0;
  local_98 = 0;
  local_60 = 0xffffffff;
  local_a8[0] = 2;
  local_5c = 0;
  local_1c8 = local_108;
  plVar46 = local_1c8;
  if (local_108 != plStack_100) {
    local_e0 = param_4 + 0xa0;
    local_d8 = param_4 + 0x40;
    local_d0 = param_4 + 0x1658;
    local_c8 = param_1 + 0x10;
    local_c0 = param_1 + 0x19;
    local_b8 = param_1 + 0x32;
    local_1b8 = (longlong *)0x0;
    do {
      local_1c8 = plVar46;
      lVar38 = *(longlong *)(param_4 + 0x2108);
      lVar6 = *(longlong *)(param_4 + 0x28);
      iVar48 = *(int *)(param_4 + 0x20f8) % 0x93a80;
      if (iVar48 < 0) {
        iVar48 = iVar48 + 0x93a80;
      }
      lVar7 = *(longlong *)(param_4 + 0x2128);
      lVar45 = param_1[0xd] + (longlong)local_1b8;
      lVar8 = param_1[4];
      lVar9 = *param_1;
      local_168 = (longlong *)FUN_140983da8(0x130);
      plVar46 = local_1c8;
      *local_168 = local_e8;
      local_168[1] = param_4;
      local_168[2] = *(longlong *)(param_4 + 0x2200);
      local_168[3] = *(longlong *)(param_4 + 0x2208);
      local_168[4] = param_4 + 0x23e0;
      local_168[5] = *(longlong *)(local_e8 + 0x10);
      lVar12 = *(longlong *)(param_4 + 0x28);
      local_168[6] = *(longlong *)(param_4 + 0x20);
      local_168[7] = lVar12;
      local_168[8] = local_e8 + 0x28;
      local_168[9] = local_e8 + 0x380;
      local_168[10] = *local_168 + lVar44;
      local_168[0xb] = local_168[1] + lVar36;
      local_168[0xc] = local_e0;
      local_168[0xd] = local_d8;
      local_168[0xe] = local_d0;
      local_168[0xf] = (longlong)local_c8;
      local_168[0x10] = lVar9;
      local_168[0x11] = lVar8;
      local_168[0x12] = lVar45;
      *(float *)(local_168 + 0x13) = (float)param_5;
      local_168[0x14] = lVar7;
      local_168[0x15] = 0;
      local_168[0x16] = 0;
      local_168[0x17] = 0;
      local_168[0x18] = 0;
      local_168[0x19] = 0;
      local_168[0x1a] = 0;
      local_168[0x1b] = 0;
      local_168[0x1c] = 0;
      local_168[0x1d] = 0;
      *(int *)(local_168 + 0x1e) = iVar48;
      local_168[0x1f] = lVar6 * 10000;
      local_168[0x20] = lVar38;
      local_168[0x23] = (longlong)local_c0;
      local_168[0x24] = (longlong)local_b8;
      local_168[0x25] = (longlong)local_a8;
      local_168[0x22] = lVar38 - iVar48;
      lVar38 = SUB168(SEXT816(-0x7777777777777777) * SEXT816(lVar38),8) + lVar38;
      local_168[0x21] = (lVar38 >> 5) - (lVar38 >> 0x3f);
      uVar47 = (uint)local_1b0;
      pvVar39 = (void *)*local_1c8;
      *local_1c8 = (longlong)local_168;
      if (pvVar39 != (void *)0x0) {
        if (*(void **)((longlong)pvVar39 + 0xd8) != (void *)0x0) {
          free(*(void **)((longlong)pvVar39 + 0xd8));
        }
        if (*(void **)((longlong)pvVar39 + 0xc0) != (void *)0x0) {
          free(*(void **)((longlong)pvVar39 + 0xc0));
        }
        if (*(void **)((longlong)pvVar39 + 0xa8) != (void *)0x0) {
          free(*(void **)((longlong)pvVar39 + 0xa8));
        }
        free(pvVar39);
      }
      local_1b8 = local_1b8 + 0x20;
      local_1a8 = (undefined1  [8])&local_140;
      pplStack_1a0 = (longlong **)&local_13c;
      plStack_190 = plVar46;
      local_198 = param_3;
      uVar33 = FUN_1402cbb70();
      local_1c0 = uVar47 | 6;
      plVar34 = _aligned_malloc(0x80,0x40);
      if (plVar34 == (longlong *)0x0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      *plVar34 = 1;
      *(undefined1 *)(plVar34 + 1) = 0;
      plVar34[2] = 0;
      plVar35 = (longlong *)0x0;
      plVar34[2] = 0;
      plVar34[3] = (longlong)&PTR_FUN_140a02e60;
      plVar34[8] = (longlong)local_1a8;
      plVar34[9] = (longlong)pplStack_1a0;
      plVar34[10] = local_198;
      plVar34[0xb] = (longlong)plStack_190;
      *plVar34 = *plVar34 + 1;
      local_1a8 = (undefined1  [8])&local_1c8;
      local_1c8 = (longlong *)0x0;
      local_1b0 = plVar34;
      local_168 = plVar34;
      FUN_140004680(uVar33,&local_1b0);
      if (local_1c8 != (longlong *)0x0) {
        LOCK();
        lVar38 = *local_1c8;
        *local_1c8 = *local_1c8 + -1;
        UNLOCK();
        if (lVar38 == 1) {
          (**(code **)local_1c8[3])();
        }
      }
      plVar51 = plStack_150;
      plVar10 = local_158;
      local_1b0 = (longlong *)((CONCAT44(local_1b0._4_4_,uVar47) | 6) & 0xfffffffffffffffd | 1);
      if (plStack_150 < local_148) {
        local_1c8 = plStack_150 + 1;
        *plStack_150 = (longlong)plVar34;
      }
      else {
        lVar38 = (longlong)plStack_150 - (longlong)local_158 >> 3;
        plVar49 = plVar10;
        if (lVar38 == 0) {
          lVar38 = 1;
LAB_14046d764:
          plVar35 = (longlong *)thunk_FUN_140983da8(lVar38 * 8);
          plVar52 = plVar35;
        }
        else {
          lVar38 = lVar38 * 2;
          plVar52 = plVar35;
          if (lVar38 != 0) goto LAB_14046d764;
        }
        for (; plVar49 != plVar51; plVar49 = plVar49 + 1) {
          *plVar35 = *plVar49;
          *plVar49 = 0;
          plVar35 = plVar35 + 1;
        }
        *plVar35 = (longlong)plVar34;
        local_168 = (longlong *)0x0;
        local_1c8 = plVar35 + 1;
        for (plVar34 = plVar10; plVar34 != plVar51; plVar34 = plVar34 + 1) {
          plVar35 = (longlong *)*plVar34;
          if (plVar35 != (longlong *)0x0) {
            LOCK();
            lVar6 = *plVar35;
            *plVar35 = *plVar35 + -1;
            UNLOCK();
            if (lVar6 == 1) {
              (**(code **)plVar35[3])();
            }
          }
        }
        if (plVar10 != (longlong *)0x0) {
          free(plVar10);
        }
        local_148 = plVar52 + lVar38;
        local_158 = plVar52;
      }
      plVar46 = plVar46 + 1;
      plStack_150 = local_1c8;
    } while (plVar46 != plStack_100);
  }
  local_1c8 = plVar46;
  plVar34 = plStack_150;
  plVar46 = local_158;
  uVar42 = 0;
  plVar35 = local_158;
  while( true ) {
    lVar44 = local_f0;
    if (plVar35 == plVar34) {
      uVar50 = uVar42;
      if (param_1[0x38] != 0) {
        do {
          plVar46 = (longlong *)0x0;
          if (uVar42 < (ulonglong)param_1[0x38]) {
            lVar36 = param_1[0x33] - param_1[0x32] >> 3;
            if (uVar42 < (ulonglong)(lVar36 * 0x6db6db6db6db6db7)) {
              plVar46 = (longlong *)(param_1[0x32] + uVar50);
            }
            else {
              plVar46 = (longlong *)((uVar42 + lVar36 * -0x6db6db6db6db6db7) * 0x38 + param_1[0x35])
              ;
            }
          }
          if ((*plVar46 != 0) && (lVar36 = FUN_140486a40(lVar44), lVar36 != 0)) {
            FUN_140465330(lVar36 + 8,plVar46 + 1);
          }
          uVar42 = uVar42 + 1;
          plVar34 = plStack_150;
          plVar46 = local_158;
          uVar50 = uVar50 + 0x38;
        } while (uVar42 < (ulonglong)param_1[0x38]);
      }
      lVar44 = local_f0;
      plVar51 = local_108;
      plVar35 = local_108;
      plVar10 = plStack_100;
      plVar49 = plVar46;
      if (local_108 != plStack_100) {
        do {
          puVar41 = *(undefined8 **)(*plVar51 + 200);
          for (puVar43 = *(undefined8 **)(*plVar51 + 0xc0); puVar43 != puVar41;
              puVar43 = puVar43 + 1) {
            FUN_1404863d0(lVar44,*puVar43);
          }
          puVar41 = *(undefined8 **)(*plVar51 + 0xe0);
          for (puVar43 = *(undefined8 **)(*plVar51 + 0xd8); puVar43 != puVar41;
              puVar43 = puVar43 + 1) {
            FUN_1404594d0(param_4 + 0xd0,*puVar43);
          }
          plVar51 = plVar51 + 1;
          plVar46 = local_158;
          plVar35 = local_108;
          plVar10 = plStack_100;
          plVar49 = local_158;
          plVar34 = plStack_150;
        } while (plVar51 != plStack_100);
      }
      for (; plVar52 = plStack_100, plVar51 = local_108, local_108 = plVar35, plStack_100 = plVar10,
          plVar46 != plVar34; plVar46 = plVar46 + 1) {
        plVar11 = (longlong *)*plVar46;
        if (plVar11 != (longlong *)0x0) {
          LOCK();
          lVar44 = *plVar11;
          *plVar11 = *plVar11 + -1;
          UNLOCK();
          if (lVar44 == 1) {
            (**(code **)plVar11[3])();
          }
        }
        plVar35 = local_108;
        plVar10 = plStack_100;
        plStack_100 = plVar52;
        local_108 = plVar51;
      }
      plVar46 = plVar51;
      if (plVar49 != (longlong *)0x0) {
        free(plVar49);
      }
      for (; plVar46 != plVar52; plVar46 = plVar46 + 1) {
        FUN_140470960(plVar46);
      }
      if (plVar51 != (longlong *)0x0) {
        free(plVar51);
      }
      plVar46 = (longlong *)param_2[1];
      if (plVar46 != (longlong *)0x0) {
        LOCK();
        plVar34 = plVar46 + 1;
        lVar44 = *plVar34;
        *(int *)plVar34 = (int)*plVar34 + -1;
        UNLOCK();
        if ((int)lVar44 == 1) {
          (**(code **)*plVar46)(plVar46);
          LOCK();
          piVar1 = (int *)((longlong)plVar46 + 0xc);
          iVar48 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar48 == 1) {
            (**(code **)(*plVar46 + 8))(plVar46);
          }
        }
      }
      return;
    }
    lVar44 = *plVar35;
    cVar31 = *(char *)(lVar44 + 8);
    if (cVar31 != '\x03') {
      if (cVar31 == '\x04') break;
      (**(code **)(*(longlong *)ThreadLocalStoragePointer + 0x20))(lVar44);
      cVar31 = *(char *)(lVar44 + 8);
    }
    if (cVar31 == '\x04') break;
    plVar10 = (longlong *)*plVar35;
    *plVar35 = 0;
    if (plVar10 != (longlong *)0x0) {
      LOCK();
      lVar44 = *plVar10;
      *plVar10 = *plVar10 + -1;
      UNLOCK();
      if (lVar44 == 1) {
        (**(code **)plVar10[3])();
      }
    }
    plVar35 = plVar35 + 1;
  }
                    /* WARNING: Subroutine does not return */
  terminate();
}

