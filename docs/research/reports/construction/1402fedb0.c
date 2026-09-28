
longlong FUN_1402fedb0(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  undefined8 *****pppppuVar5;
  undefined8 *****pppppuVar6;
  undefined8 ****local_2f8;
  undefined8 ****local_2f0;
  undefined8 *local_2e8;
  undefined8 local_2d8;
  undefined8 ****local_2c8;
  undefined8 ****local_2c0;
  undefined8 *local_2b8;
  undefined8 local_2a8;
  undefined8 local_288;
  undefined8 uStack_280;
  undefined8 local_278;
  undefined8 uStack_270;
  undefined8 local_268;
  undefined8 uStack_260;
  undefined1 local_258 [16];
  undefined1 local_248;
  undefined1 local_238;
  undefined4 local_230;
  undefined1 local_220;
  undefined1 local_210;
  undefined8 ****local_208;
  undefined8 ****ppppuStack_200;
  undefined8 *local_1f8;
  ulonglong uStack_1f0;
  undefined8 local_1e8;
  undefined8 ****local_1d8;
  undefined8 ****ppppuStack_1d0;
  undefined8 *local_1c8;
  ulonglong uStack_1c0;
  undefined8 local_1b8;
  undefined8 ****local_1a8;
  undefined8 ****ppppuStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined2 local_178;
  undefined1 local_176;
  undefined1 local_170;
  undefined1 local_a0;
  undefined1 local_98 [24];
  undefined1 local_80 [24];
  undefined1 local_68 [24];
  undefined1 local_50 [24];
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  
  memset(local_258,0,0x1c0);
  local_278 = 0;
  uStack_270 = 0;
  local_268 = 0;
  local_288 = 0;
  uStack_280 = 0;
  uStack_260 = 0;
  local_258[0] = 0;
  local_248 = 0;
  local_238 = 0;
  local_230 = 0;
  local_220 = 0;
  local_210 = 0;
  local_208 = &local_208;
  ppppuStack_200 = &local_208;
  local_1f8 = (undefined8 *)0x0;
  uStack_1f0 = 0;
  local_1e8 = 0;
  local_1d8 = &local_1d8;
  ppppuStack_1d0 = &local_1d8;
  local_1c8 = (undefined8 *)0x0;
  uStack_1c0 = 0;
  local_1b8 = 0;
  local_1a8 = &local_1a8;
  ppppuStack_1a0 = &local_1a8;
  local_198 = 0;
  uStack_190 = 0;
  local_188 = 0;
  local_178 = 0;
  local_176 = 0;
  local_170 = 0;
  local_a0 = 0;
  uVar1 = *(undefined8 *)(param_1 + 0xa0);
  FUN_1403205c0(local_98,param_1 + 0x20);
  FUN_1403206d0(local_80,param_1 + 0x38);
  FUN_1403207c0(local_68,param_1 + 0x50);
  FUN_140320090(local_50,param_1 + 0x68);
  local_38 = *(undefined4 *)(param_1 + 0x80);
  uStack_34 = *(undefined4 *)(param_1 + 0x84);
  uStack_30 = *(undefined4 *)(param_1 + 0x88);
  uStack_2c = *(undefined4 *)(param_1 + 0x8c);
  FUN_1403abf70(param_3 + 0x428,&local_2f8,local_98,param_1 + 0x90,uVar1);
  puVar3 = local_2e8;
  puVar4 = local_1c8;
  while (puVar4 != (undefined8 *)0x0) {
    local_2e8 = puVar3;
    FUN_1402450d0(&local_1d8,*puVar4);
    puVar2 = (undefined8 *)puVar4[1];
    free(puVar4);
    puVar3 = local_2e8;
    puVar4 = puVar2;
  }
  local_1d8 = &local_1d8;
  ppppuStack_1d0 = &local_1d8;
  local_1c8 = (undefined8 *)0x0;
  uStack_1c0 = uStack_1c0 & 0xffffffffffffff00;
  local_2d8 = 0;
  local_2e8 = (undefined8 *)0x0;
  pppppuVar5 = (undefined8 *****)local_2f8;
  pppppuVar6 = (undefined8 *****)local_2f0;
  puVar4 = local_2b8;
  puVar2 = local_1f8;
  if (puVar3 != (undefined8 *)0x0) {
    *(undefined8 ******)((longlong)puVar3 + 0x10) = &local_1d8;
    local_2e8 = (undefined8 *)0x0;
    pppppuVar5 = &local_2f8;
    pppppuVar6 = &local_2f8;
    local_1d8 = local_2f8;
    ppppuStack_1d0 = local_2f0;
    local_1c8 = puVar3;
  }
  while (local_2f0 = pppppuVar6, local_2f8 = pppppuVar5, local_2b8 = puVar4,
        puVar2 != (undefined8 *)0x0) {
    FUN_1402450d0(&local_208,*puVar2);
    puVar3 = (undefined8 *)puVar2[1];
    free(puVar2);
    pppppuVar5 = (undefined8 *****)local_2f8;
    pppppuVar6 = (undefined8 *****)local_2f0;
    puVar4 = local_2b8;
    puVar2 = puVar3;
  }
  local_208 = &local_208;
  ppppuStack_200 = &local_208;
  local_1f8 = (undefined8 *)0x0;
  uStack_1f0 = uStack_1f0 & 0xffffffffffffff00;
  local_2a8 = 0;
  pppppuVar5 = (undefined8 *****)local_2c8;
  pppppuVar6 = (undefined8 *****)local_2c0;
  if (puVar4 != (undefined8 *)0x0) {
    puVar4[2] = &local_208;
    local_2b8 = (undefined8 *)0x0;
    pppppuVar5 = &local_2c8;
    pppppuVar6 = &local_2c8;
    local_208 = local_2c8;
    ppppuStack_200 = local_2c0;
    local_1f8 = puVar4;
  }
  local_2c0 = pppppuVar6;
  local_2c8 = pppppuVar5;
  local_178 = CONCAT11(1,(undefined1)local_178);
  FUN_1402fabf0(param_2,&local_288);
  *(undefined1 *)(param_2 + 0x1f0) = 1;
  puVar3 = local_2b8;
  while (puVar4 = local_2e8, puVar3 != (undefined8 *)0x0) {
    FUN_1402450d0(&local_2c8,*puVar3);
    puVar4 = (undefined8 *)puVar3[1];
    free(puVar3);
    puVar3 = puVar4;
  }
  while (puVar4 != (undefined8 *)0x0) {
    FUN_1402450d0(&local_2f8,*puVar4);
    puVar3 = (undefined8 *)puVar4[1];
    free(puVar4);
    puVar4 = puVar3;
  }
  FUN_1402f3da0(&local_288);
  return param_2;
}

