
longlong FUN_1402f6ee0(longlong param_1,longlong param_2,longlong param_3)

{
  undefined1 uVar1;
  undefined1 uVar2;
  longlong *plVar3;
  undefined8 **ppuVar4;
  undefined1 *puVar5;
  ulonglong uVar6;
  undefined ***pppuVar7;
  longlong lVar8;
  undefined8 uVar9;
  undefined8 *puVar10;
  longlong *plVar11;
  undefined8 *puVar12;
  longlong lVar13;
  undefined8 ***pppuVar14;
  ulonglong uVar15;
  longlong *plVar16;
  undefined8 *puVar17;
  undefined8 *puVar18;
  undefined1 *puVar19;
  undefined8 ****_Src;
  undefined **ppuVar20;
  undefined4 uVar21;
  undefined **local_2e8;
  undefined8 ***local_2e0;
  undefined8 ***pppuStack_2d8;
  undefined8 *local_2d0;
  ulonglong uStack_2c8;
  ulonglong local_2c0;
  undefined ***local_2a8;
  undefined ***pppuStack_2a0;
  undefined8 **local_288;
  undefined8 **ppuStack_280;
  ulonglong local_278;
  ulonglong local_270;
  undefined ***local_250;
  longlong *local_240;
  longlong *local_238;
  undefined *local_228;
  undefined8 uStack_220;
  undefined8 local_218;
  undefined8 uStack_210;
  undefined8 local_208;
  undefined8 uStack_200;
  uint local_1f8;
  undefined8 local_1f0;
  ulonglong local_1e8;
  undefined8 uStack_1e0;
  ulonglong local_1d8;
  undefined4 local_1d0;
  undefined4 local_1cc;
  undefined4 local_1c8;
  undefined1 local_1c4;
  uint local_1c0;
  undefined1 local_1bc;
  undefined1 local_1bb;
  undefined4 local_1b8;
  undefined1 local_1b0 [8];
  undefined8 ***local_1a8;
  undefined8 ***pppuStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 ***local_178;
  undefined8 ***pppuStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 ***local_148;
  undefined8 ***pppuStack_140;
  undefined8 local_138;
  undefined8 uStack_130;
  undefined8 local_128;
  undefined1 local_118;
  undefined1 local_117;
  undefined1 local_116;
  undefined1 local_110;
  undefined1 local_40;
  
  plVar11 = *(longlong **)(param_1 + 0x20);
  plVar3 = *(longlong **)(param_1 + 0x28);
  puVar18 = (undefined8 *)(param_3 + 0x428);
  if (plVar11 != plVar3) {
    plVar16 = plVar11 + 1;
    do {
      ppuVar4 = (undefined8 **)*plVar11;
      lVar8 = FUN_140320c00(*puVar18,ppuVar4);
      if (lVar8 != 0) {
        lVar13 = plVar16[1];
        *(longlong *)(lVar8 + 0x30) = *plVar16;
        *(longlong *)(lVar8 + 0x38) = lVar13;
        *(longlong *)(param_3 + 0x738) = *(longlong *)(param_3 + 0x738) + 1;
        local_2e8 = (undefined **)ppuVar4;
        FUN_1403c67c0(&local_2e0,&local_2e8,&local_2e0);
        FUN_140393cb0(puVar18,&local_2e0);
      }
      plVar11 = plVar11 + 3;
      plVar16 = plVar16 + 3;
    } while (plVar11 != plVar3);
  }
  puVar10 = *(undefined8 **)(param_1 + 0x40);
  puVar12 = *(undefined8 **)(param_1 + 0x38);
  if (puVar12 != puVar10) {
    puVar17 = puVar12 + 1;
    do {
      local_2a8 = (undefined ***)*puVar17;
      pppuStack_2a0 = (undefined ***)puVar17[1];
      FUN_14039b2f0(puVar18,*puVar12,&local_2a8);
      puVar12 = puVar12 + 3;
      puVar17 = puVar17 + 3;
    } while (puVar12 != puVar10);
  }
  puVar10 = *(undefined8 **)(param_1 + 0x58);
  for (puVar12 = *(undefined8 **)(param_1 + 0x50); puVar12 != puVar10; puVar12 = puVar12 + 2) {
    FUN_14039b220(puVar18,*puVar12,puVar12[1]);
  }
  puVar10 = *(undefined8 **)(param_1 + 0x70);
  for (puVar12 = *(undefined8 **)(param_1 + 0x68); puVar12 != puVar10; puVar12 = puVar12 + 2) {
    FUN_14039b470(puVar18,*puVar12,*(undefined4 *)(puVar12 + 1));
  }
  puVar10 = *(undefined8 **)(param_1 + 0x88);
  for (puVar12 = *(undefined8 **)(param_1 + 0x80); puVar12 != puVar10; puVar12 = puVar12 + 3) {
    FUN_14039d2e0(puVar18,puVar12[2],puVar12[1],*puVar12);
  }
  lVar8 = *(longlong *)(param_1 + 0xa0);
  for (lVar13 = *(longlong *)(param_1 + 0x98); lVar13 != lVar8; lVar13 = lVar13 + 0xe8) {
    FUN_1403a87f0(puVar18,lVar13);
  }
  puVar12 = *(undefined8 **)(param_1 + 0x118);
  for (puVar10 = *(undefined8 **)(param_1 + 0x110); puVar10 != puVar12; puVar10 = puVar10 + 0x19) {
    local_2e8 = &local_228;
    local_228 = (undefined *)*puVar10;
    uVar6 = (ulonglong)uStack_220 >> 8;
    uStack_220 = CONCAT71((int7)uVar6,*(undefined1 *)(puVar10 + 1));
    FUN_140019ca0(&local_218,puVar10 + 2);
    local_1f8 = *(uint *)(puVar10 + 6);
    local_1f0 = puVar10[7];
    local_1e8 = puVar10[8];
    uStack_1e0 = puVar10[9];
    local_1d8 = puVar10[10];
    local_1d0 = *(undefined4 *)(puVar10 + 0xb);
    local_1cc = *(undefined4 *)((longlong)puVar10 + 0x5c);
    local_1c8 = *(undefined4 *)(puVar10 + 0xc);
    local_1c4 = *(undefined1 *)((longlong)puVar10 + 100);
    local_1c0 = *(uint *)(puVar10 + 0xd);
    local_1bc = *(undefined1 *)((longlong)puVar10 + 0x6c);
    local_1bb = *(undefined1 *)((longlong)puVar10 + 0x6d);
    local_1b8 = *(undefined4 *)(puVar10 + 0xe);
    FUN_14031f8f0(local_1b0,puVar10 + 0xf);
    FUN_1402f7920(&local_198,puVar10 + 0x12);
    FUN_1403a84e0(puVar18,&local_228);
  }
  puVar10 = *(undefined8 **)(param_1 + 0xb8);
  for (puVar12 = *(undefined8 **)(param_1 + 0xb0); puVar12 != puVar10; puVar12 = puVar12 + 2) {
    FUN_14039fa80(puVar18,*puVar12,puVar12[1]);
  }
  puVar10 = *(undefined8 **)(param_1 + 0xd0);
  for (puVar12 = *(undefined8 **)(param_1 + 200); puVar12 != puVar10; puVar12 = puVar12 + 2) {
    FUN_1403a4080(puVar18,*puVar12,puVar12[1]);
  }
  if (*(longlong *)(param_1 + 0xe8) != *(longlong *)(param_1 + 0xe0)) {
    uVar9 = FUN_14031f8f0(&local_240);
    FUN_1403a4290(puVar18,uVar9);
  }
  plVar3 = *(longlong **)(param_1 + 0x100);
  for (plVar11 = *(longlong **)(param_1 + 0xf8); plVar11 != plVar3; plVar11 = plVar11 + 1) {
    ppuVar4 = (undefined8 **)*plVar11;
    if (ppuVar4 == (undefined8 **)0x0) {
      puVar10 = (undefined8 *)0x0;
    }
    else {
      puVar10 = (undefined8 *)FUN_14032c420(*puVar18,ppuVar4);
    }
    if (puVar10[0x1a] != 0) {
      FUN_14038ef20(puVar18,puVar10[0x1a],*puVar10);
      FUN_140391290(puVar18,puVar10[0x1a]);
      *(longlong *)(param_3 + 0x738) = *(longlong *)(param_3 + 0x738) + 1;
      local_2e8 = (undefined **)ppuVar4;
      FUN_1403c67c0(&local_2e0,&local_2e8,&local_2e0);
      FUN_140393cb0(puVar18,&local_2e0);
    }
  }
  plVar3 = *(longlong **)(param_1 + 0x130);
  for (plVar11 = *(longlong **)(param_1 + 0x128); plVar11 != plVar3; plVar11 = plVar11 + 2) {
    ppuVar4 = (undefined8 **)*plVar11;
    FUN_1403a5550(puVar18,&local_2a8,ppuVar4,plVar11[1]);
    *(longlong *)(param_3 + 0x738) = *(longlong *)(param_3 + 0x738) + 1;
    local_2e8 = (undefined **)ppuVar4;
    FUN_1403c67c0(&local_2e0,&local_2e8,&local_2e0);
    FUN_140393cb0(puVar18,&local_2e0);
    pppuVar7 = pppuStack_2a0;
    for (pppuVar14 = (undefined8 ***)local_2a8; pppuVar14 != (undefined8 ***)pppuVar7;
        pppuVar14 = pppuVar14 + 1) {
      local_2e8 = (undefined **)*pppuVar14;
      FUN_1403c67c0(&local_2e0,&local_2e8,&local_2e0);
      FUN_140393cb0(puVar18,&local_2e0);
    }
    if ((undefined8 ***)local_2a8 != (undefined8 ***)0x0) {
      free(local_2a8);
    }
  }
  puVar5 = *(undefined1 **)(param_1 + 0x148);
  for (puVar19 = *(undefined1 **)(param_1 + 0x140); puVar19 != puVar5; puVar19 = puVar19 + 0x10) {
    uVar1 = *puVar19;
    uVar2 = puVar19[1];
    ppuVar4 = *(undefined8 ***)(puVar19 + 8);
    lVar8 = FUN_140320c00(*(undefined8 *)(param_3 + 0x428),ppuVar4);
    if (lVar8 != 0) {
      *(undefined1 *)(lVar8 + 0xda) = uVar2;
      *(undefined1 *)(lVar8 + 0xdb) = uVar1;
      *(longlong *)(param_3 + 0x738) = *(longlong *)(param_3 + 0x738) + 1;
      local_2e8 = (undefined **)ppuVar4;
      FUN_1403c67c0(&local_2e0,&local_2e8,&local_2e0);
      FUN_140393cb0(param_3 + 0x428,&local_2e0);
      FUN_1403aaa20(param_3 + 0x428,ppuVar4);
    }
  }
  ppuVar20 = *(undefined ***)(param_1 + 0x158);
  local_2e8 = *(undefined ***)(param_1 + 0x160);
  if (ppuVar20 != local_2e8) {
    do {
      local_2e0 = (undefined8 ***)*ppuVar20;
      FUN_140019ca0(&pppuStack_2d8,ppuVar20 + 1);
      uVar6 = uStack_2c8;
      local_288 = (undefined8 **)0x0;
      ppuStack_280 = (undefined8 ***)0x0;
      local_278 = 0;
      local_270 = 0;
      _Src = &pppuStack_2d8;
      if (0xf < local_2c0) {
        _Src = (undefined8 ****)pppuStack_2d8;
      }
      if (0x7fffffffffffffff < uStack_2c8) {
                    /* WARNING: Subroutine does not return */
        FUN_140001c70();
      }
      if (uStack_2c8 < 0x10) {
        local_278 = uStack_2c8;
        local_270 = 0xf;
        local_288 = *_Src;
        ppuStack_280 = _Src[1];
      }
      else {
        uVar15 = uStack_2c8 | 0xf;
        if (uVar15 < 0x8000000000000000) {
          if (uVar15 < 0x16) {
            uVar15 = 0x16;
          }
        }
        else {
          uVar15 = 0x7fffffffffffffff;
        }
        local_288 = (undefined8 **)FUN_140003270(uVar15 + 1);
        local_278 = uVar6;
        local_270 = uVar15;
        memcpy(local_288,_Src,uVar6 + 1);
      }
      local_2a8 = (undefined ***)&local_288;
      FUN_1403a45e0(param_3 + 0x428,&local_240,local_2e0);
      plVar16 = local_238;
      plVar3 = local_240;
      for (plVar11 = local_240; plVar11 != plVar16; plVar11 = plVar11 + 1) {
        lVar8 = *plVar11;
        if ((lVar8 != 0) &&
           (lVar13 = FUN_14032c420(*(undefined8 *)(param_3 + 0x428),lVar8), lVar13 != 0)) {
          FUN_1403a7220(param_3 + 0x428,lVar8,0,&local_288);
        }
      }
      if (plVar3 != (longlong *)0x0) {
        free(plVar3);
      }
      FUN_140002d30(&local_288);
      FUN_140002d30(&pppuStack_2d8);
      ppuVar20 = ppuVar20 + 5;
    } while (ppuVar20 != local_2e8);
  }
  plVar3 = *(longlong **)(param_1 + 0x178);
  for (plVar11 = *(longlong **)(param_1 + 0x170); plVar11 != plVar3; plVar11 = plVar11 + 2) {
    lVar8 = *plVar11;
    uVar21 = (undefined4)plVar11[1];
    if ((lVar8 != 0) &&
       (lVar13 = FUN_14032c420(*(undefined8 *)(param_3 + 0x428),lVar8), lVar13 != 0)) {
      local_2e0 = &local_2e0;
      pppuStack_2d8 = &local_2e0;
      local_2d0 = (undefined8 *)0x0;
      uStack_2c8 = 0;
      local_2c0 = 0;
      local_288 = (undefined8 **)
                  std::
                  _Func_impl_no_alloc<`public:_void___cdecl_nimby::model::TrackNetwork::track_change_layer(__int64,int)___ptr64'::`2'::<lambda_1>,void,nimby::model::Track*___ptr64>
                  ::vftable;
      ppuStack_280 = (undefined8 **)CONCAT44(ppuStack_280._4_4_,uVar21);
      local_250 = (undefined ***)&local_288;
      FUN_1403a4980(param_3 + 0x428,lVar8,&local_2e0,&local_288);
      puVar18 = local_2d0;
      while (puVar18 != (undefined8 *)0x0) {
        FUN_1402450d0(&local_2e0,*puVar18);
        puVar10 = (undefined8 *)puVar18[1];
        free(puVar18);
        puVar18 = puVar10;
      }
    }
  }
  puVar18 = *(undefined8 **)(param_1 + 0x188);
  puVar10 = *(undefined8 **)(param_1 + 400);
  if (puVar18 != puVar10) {
    do {
      local_2e0 = &local_2e0;
      pppuStack_2d8 = &local_2e0;
      local_2d0 = (undefined8 *)0x0;
      uStack_2c8 = 0;
      local_2c0 = 0;
      local_288 = (undefined8 **)
                  std::
                  _Func_impl_no_alloc<`public:_void___cdecl_nimby::model::TrackNetwork::track_change_kind(__int64,int)___ptr64'::`2'::<lambda_1>,void,nimby::model::Track*___ptr64>
                  ::vftable;
      ppuStack_280 = (undefined8 **)CONCAT44(ppuStack_280._4_4_,(int)puVar18[1]);
      local_250 = (undefined ***)&local_288;
      FUN_1403a4980(param_3 + 0x428,*puVar18,&local_2e0,&local_288);
      puVar12 = local_2d0;
      while (puVar12 != (undefined8 *)0x0) {
        FUN_1402450d0(&local_2e0,*puVar12);
        puVar17 = (undefined8 *)puVar12[1];
        free(puVar12);
        puVar12 = puVar17;
      }
      puVar18 = puVar18 + 2;
    } while (puVar18 != puVar10);
  }
  local_218 = 0;
  uStack_210 = 0;
  local_208 = 0;
  local_228 = (undefined *)0x0;
  uStack_220 = 0;
  uStack_200 = 0;
  local_1f8 = local_1f8 & 0xffffff00;
  local_1e8 = local_1e8 & 0xffffffffffffff00;
  local_1d8 = local_1d8 & 0xffffffffffffff00;
  local_1d0 = 0;
  local_1c0 = local_1c0 & 0xffffff00;
  local_1b0[0] = 0;
  local_1a8 = &local_1a8;
  pppuStack_1a0 = &local_1a8;
  local_198 = 0;
  uStack_190 = 0;
  local_188 = 0;
  local_178 = &local_178;
  pppuStack_170 = &local_178;
  local_168 = 0;
  uStack_160 = 0;
  local_158 = 0;
  local_148 = &local_148;
  pppuStack_140 = &local_148;
  local_138 = 0;
  uStack_130 = 0;
  local_128 = 0;
  local_118 = 0;
  local_117 = *(undefined1 *)(param_1 + 0x1a0);
  local_116 = 0;
  local_110 = 0;
  local_40 = 0;
  FUN_1402fabf0(param_2,&local_228);
  *(undefined1 *)(param_2 + 0x1f0) = 1;
  FUN_1402f3da0(&local_228);
  return param_2;
}

