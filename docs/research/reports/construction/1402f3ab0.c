
longlong FUN_1402f3ab0(longlong param_1,longlong param_2,longlong param_3)

{
  undefined1 *puVar1;
  undefined8 local_2c8;
  undefined8 uStack_2c0;
  undefined8 local_2b8;
  undefined8 uStack_2b0;
  undefined8 local_2a8;
  undefined8 uStack_2a0;
  undefined1 local_298;
  undefined1 local_288;
  undefined1 local_278;
  undefined4 local_270;
  undefined1 local_260;
  undefined1 local_250;
  undefined8 **local_248;
  undefined8 **ppuStack_240;
  undefined8 local_238;
  undefined8 uStack_230;
  undefined8 local_228;
  undefined8 **local_218;
  undefined8 **ppuStack_210;
  undefined8 local_208;
  undefined8 uStack_200;
  undefined8 local_1f8;
  undefined8 **local_1e8;
  undefined8 **ppuStack_1e0;
  undefined8 local_1d8;
  undefined8 uStack_1d0;
  undefined8 local_1c8;
  undefined2 local_1b8;
  undefined1 local_1b6;
  undefined1 local_1b0;
  undefined1 local_1a8 [8];
  undefined1 local_1a0 [48];
  undefined1 local_170 [48];
  undefined1 local_140 [48];
  undefined1 local_110 [48];
  char local_e0;
  undefined1 local_d8 [8];
  undefined1 local_d0 [8];
  void *local_c8;
  ulonglong local_c0;
  undefined1 local_a0 [8];
  void *local_98;
  ulonglong local_90;
  undefined1 local_70 [8];
  void *local_68;
  ulonglong local_60;
  undefined1 local_40 [8];
  void *local_38;
  ulonglong local_30;
  
  local_2b8 = 0;
  uStack_2b0 = 0;
  local_2a8 = 0;
  local_2c8 = 0;
  uStack_2c0 = 0;
  uStack_2a0 = 0;
  local_298 = 0;
  local_288 = 0;
  local_278 = 0;
  local_270 = 0;
  local_260 = 0;
  local_250 = 0;
  local_248 = &local_248;
  ppuStack_240 = &local_248;
  local_238 = 0;
  uStack_230 = 0;
  local_228 = 0;
  local_218 = &local_218;
  ppuStack_210 = &local_218;
  local_208 = 0;
  uStack_200 = 0;
  local_1f8 = 0;
  local_1e8 = &local_1e8;
  ppuStack_1e0 = &local_1e8;
  local_1d8 = 0;
  uStack_1d0 = 0;
  local_1c8 = 0;
  local_1b8 = 0;
  local_1b6 = 0;
  local_1b0 = 0;
  local_e0 = '\0';
  puVar1 = (undefined1 *)FUN_1403ad950(param_3 + 0x428,local_d8,param_1 + 0x20);
  if (local_e0 == '\0') {
    FUN_1403344c0(local_1a8,puVar1);
    local_e0 = '\x01';
  }
  else {
    local_1a8[0] = *puVar1;
    FUN_14032c490(local_1a0,puVar1 + 8);
    FUN_14032c490(local_170,puVar1 + 0x38);
    FUN_14032c490(local_140,puVar1 + 0x68);
    FUN_14032c490(local_110,puVar1 + 0x98);
  }
  FUN_140325a40(local_40);
  if (1 < local_30) {
    free(local_38);
  }
  FUN_140325a40(local_70);
  if (1 < local_60) {
    free(local_68);
  }
  FUN_140325a40(local_a0);
  if (1 < local_90) {
    free(local_98);
  }
  FUN_140325a40(local_d0);
  if (1 < local_c0) {
    free(local_c8);
  }
  FUN_1402fabf0(param_2,&local_2c8);
  *(undefined1 *)(param_2 + 0x1f0) = 1;
  FUN_1402f3da0(&local_2c8);
  return param_2;
}

