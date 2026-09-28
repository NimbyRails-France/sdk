
longlong FUN_1402fda60(longlong param_1,longlong param_2,longlong param_3)

{
  bool bVar1;
  undefined8 ******ppppppuVar2;
  undefined8 *puVar3;
  longlong *plVar4;
  undefined8 ******ppppppuVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  longlong *plVar9;
  longlong local_res10;
  undefined8 local_228;
  undefined8 uStack_220;
  undefined8 local_218;
  undefined8 uStack_210;
  undefined8 local_208;
  undefined8 uStack_200;
  undefined1 local_1f8 [16];
  undefined1 local_1e8;
  undefined1 local_1d8;
  undefined4 local_1d0;
  undefined1 local_1c0;
  undefined1 local_1b0;
  undefined8 *****local_1a8;
  undefined8 *****pppppuStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 *****local_178;
  undefined8 *****pppppuStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 *****local_148;
  undefined8 *****pppppuStack_140;
  undefined8 *****local_138;
  undefined8 uStack_130;
  longlong local_128;
  undefined2 local_118;
  undefined1 local_116;
  undefined1 local_110;
  undefined1 local_40;
  undefined1 local_38 [16];
  
  lVar7 = 0;
  local_res10 = param_2;
  memset(local_1f8,0,0x1c0);
  local_218 = 0;
  uStack_210 = 0;
  local_208 = 0;
  local_228 = 0;
  uStack_220 = 0;
  uStack_200 = 0;
  local_1f8[0] = 0;
  local_1e8 = 0;
  local_1d8 = 0;
  local_1d0 = 0;
  local_1c0 = 0;
  local_1b0 = 0;
  local_1a8 = &local_1a8;
  pppppuStack_1a0 = &local_1a8;
  local_198 = 0;
  uStack_190 = 0;
  local_188 = 0;
  local_178 = &local_178;
  pppppuStack_170 = &local_178;
  local_168 = 0;
  uStack_160 = 0;
  local_158 = 0;
  local_148 = &local_148;
  pppppuStack_140 = &local_148;
  local_138 = (undefined8 ******)0x0;
  uStack_130 = 0;
  local_128 = 0;
  local_118 = 0;
  local_116 = 0;
  local_110 = 0;
  local_40 = 0;
  plVar9 = (longlong *)(param_3 + 0x428);
  puVar3 = (undefined8 *)FUN_140320c00(*plVar9,*(undefined8 *)(param_1 + 0x60));
  lVar8 = lVar7;
  if (puVar3 != (undefined8 *)0x0) {
    plVar4 = (longlong *)FUN_1403bc9a0(*plVar9 + 0x380);
    lVar8 = *plVar4;
    local_res10 = lVar8;
    FUN_140339140(plVar4,param_1 + 0x20);
    *plVar4 = lVar8;
    FUN_1403a81f0(plVar9,plVar4);
    FUN_14032da20(puVar3 + 0x28,local_38,&local_res10);
    *(longlong *)(param_3 + 0x738) = *(longlong *)(param_3 + 0x738) + 1;
    FUN_1403aaa20(plVar9,*puVar3);
  }
  bVar1 = true;
  ppppppuVar2 = &local_148;
  ppppppuVar5 = (undefined8 ******)local_138;
  while (ppppppuVar5 != (undefined8 ******)0x0) {
    bVar1 = lVar8 < (longlong)ppppppuVar5[4];
    ppppppuVar2 = ppppppuVar5;
    if (lVar8 < (longlong)ppppppuVar5[4]) {
      ppppppuVar5 = (undefined8 ******)ppppppuVar5[1];
    }
    else {
      ppppppuVar5 = (undefined8 ******)*ppppppuVar5;
    }
  }
  ppppppuVar5 = ppppppuVar2;
  if (bVar1) {
    if (ppppppuVar2 != (undefined8 ******)pppppuStack_140) {
      ppppppuVar5 = (undefined8 ******)FUN_14001dcf0(ppppppuVar2);
      goto LAB_1402fdc24;
    }
  }
  else {
LAB_1402fdc24:
    if (lVar8 <= (longlong)ppppppuVar5[4]) goto LAB_1402fdc62;
  }
  lVar6 = thunk_FUN_140983da8(0x28);
  *(longlong *)(lVar6 + 0x20) = lVar8;
  if ((ppppppuVar2 != &local_148) && ((longlong)ppppppuVar2[4] <= lVar8)) {
    lVar7 = 1;
  }
  FUN_14001de10(lVar6,ppppppuVar2,&local_148,lVar7);
  local_128 = local_128 + 1;
LAB_1402fdc62:
  local_118 = CONCAT11(1,(undefined1)local_118);
  FUN_1402fabf0(param_2,&local_228);
  *(undefined1 *)(param_2 + 0x1f0) = 1;
  FUN_1402f3da0(&local_228);
  return param_2;
}

