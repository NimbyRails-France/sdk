
undefined8 * FUN_14045ac60(undefined8 *param_1,undefined8 *param_2)

{
  void *_Src;
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  longlong lVar7;
  ulonglong _Size;
  undefined4 *puVar8;

  *param_1 = *param_2;
  FUN_140343580(param_1 + 1,param_2 + 1);
  param_1[0xf] = param_2[0xf];
  *(undefined4 *)(param_1 + 0x10) = *(undefined4 *)(param_2 + 0x10);
  lVar7 = (longlong)(param_2[0x12] - param_2[0x11]) >> 5;
  puVar6 = (undefined4 *)0x0;
  puVar5 = puVar6;
  if (lVar7 != 0) {
    puVar5 = (undefined4 *)thunk_FUN_140983da8(lVar7 << 5);
  }
  param_1[0x11] = puVar5;
  param_1[0x12] = puVar5;
  param_1[0x13] = puVar5 + lVar7 * 8;
  _Src = (void *)param_2[0x11];
  if (_Src != (void *)param_2[0x12]) {
    _Size = (longlong)param_2[0x12] - (longlong)_Src & 0xffffffffffffffe0;
    memmove(puVar5,_Src,_Size);
    puVar5 = (undefined4 *)((longlong)puVar5 + _Size);
  }
  param_1[0x12] = puVar5;
  param_1[0x14] = param_2[0x14];
  param_1[0x15] = param_2[0x15];
  *(undefined4 *)(param_1 + 0x16) = *(undefined4 *)(param_2 + 0x16);
  *(undefined1 *)(param_1 + 0x3a) = 0;
  if (*(char *)(param_2 + 0x3a) != '\0') {
    FUN_140462f40(param_1 + 0x17);
    *(undefined1 *)(param_1 + 0x3a) = 1;
  }
  uVar4 = param_2[0x3c];
  param_1[0x3b] = param_2[0x3b];
  param_1[0x3c] = uVar4;
  uVar4 = param_2[0x3e];
  param_1[0x3d] = param_2[0x3d];
  param_1[0x3e] = uVar4;
  uVar4 = param_2[0x40];
  param_1[0x3f] = param_2[0x3f];
  param_1[0x40] = uVar4;
  uVar4 = param_2[0x42];
  param_1[0x41] = param_2[0x41];
  param_1[0x42] = uVar4;
  param_1[0x43] = param_2[0x43];
  uVar4 = param_2[0x45];
  param_1[0x44] = param_2[0x44];
  param_1[0x45] = uVar4;
  uVar4 = param_2[0x47];
  param_1[0x46] = param_2[0x46];
  param_1[0x47] = uVar4;
  uVar4 = param_2[0x49];
  param_1[0x48] = param_2[0x48];
  param_1[0x49] = uVar4;
  uVar4 = param_2[0x4b];
  param_1[0x4a] = param_2[0x4a];
  param_1[0x4b] = uVar4;
  uVar4 = param_2[0x4d];
  param_1[0x4c] = param_2[0x4c];
  param_1[0x4d] = uVar4;
  uVar1 = *(undefined4 *)((longlong)param_2 + 0x274);
  uVar2 = *(undefined4 *)(param_2 + 0x4f);
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x27c);
  *(undefined4 *)(param_1 + 0x4e) = *(undefined4 *)(param_2 + 0x4e);
  *(undefined4 *)((longlong)param_1 + 0x274) = uVar1;
  *(undefined4 *)(param_1 + 0x4f) = uVar2;
  *(undefined4 *)((longlong)param_1 + 0x27c) = uVar3;
  uVar1 = *(undefined4 *)((longlong)param_2 + 0x284);
  uVar2 = *(undefined4 *)(param_2 + 0x51);
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x28c);
  *(undefined4 *)(param_1 + 0x50) = *(undefined4 *)(param_2 + 0x50);
  *(undefined4 *)((longlong)param_1 + 0x284) = uVar1;
  *(undefined4 *)(param_1 + 0x51) = uVar2;
  *(undefined4 *)((longlong)param_1 + 0x28c) = uVar3;
  *(undefined1 *)(param_1 + 0x96) = 0;
  if (*(char *)(param_2 + 0x96) != '\0') {
    FUN_140463030(param_1 + 0x52);
    *(undefined1 *)(param_1 + 0x96) = 1;
  }
  uVar4 = param_2[0x98];
  param_1[0x97] = param_2[0x97];
  param_1[0x98] = uVar4;
  uVar4 = param_2[0x9a];
  param_1[0x99] = param_2[0x99];
  param_1[0x9a] = uVar4;
  uVar4 = param_2[0x9c];
  param_1[0x9b] = param_2[0x9b];
  param_1[0x9c] = uVar4;
  uVar4 = param_2[0x9e];
  param_1[0x9d] = param_2[0x9d];
  param_1[0x9e] = uVar4;
  uVar4 = param_2[0xa0];
  param_1[0x9f] = param_2[0x9f];
  param_1[0xa0] = uVar4;
  uVar4 = param_2[0xa2];
  param_1[0xa1] = param_2[0xa1];
  param_1[0xa2] = uVar4;
  uVar4 = param_2[0xa4];
  param_1[0xa3] = param_2[0xa3];
  param_1[0xa4] = uVar4;
  uVar4 = param_2[0xa6];
  param_1[0xa5] = param_2[0xa5];
  param_1[0xa6] = uVar4;
  uVar4 = param_2[0xa8];
  param_1[0xa7] = param_2[0xa7];
  param_1[0xa8] = uVar4;
  uVar1 = *(undefined4 *)((longlong)param_2 + 0x54c);
  uVar2 = *(undefined4 *)(param_2 + 0xaa);
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x554);
  *(undefined4 *)(param_1 + 0xa9) = *(undefined4 *)(param_2 + 0xa9);
  *(undefined4 *)((longlong)param_1 + 0x54c) = uVar1;
  *(undefined4 *)(param_1 + 0xaa) = uVar2;
  *(undefined4 *)((longlong)param_1 + 0x554) = uVar3;
  param_1[0xab] = param_2[0xab];
  *(undefined1 *)(param_1 + 0xb3) = 0;
  if (*(char *)(param_2 + 0xb3) != '\0') {
    FUN_140463190(param_1 + 0xac);
    *(undefined1 *)(param_1 + 0xb3) = 1;
  }
  uVar4 = param_2[0xb5];
  param_1[0xb4] = param_2[0xb4];
  param_1[0xb5] = uVar4;
  uVar4 = param_2[0xb7];
  param_1[0xb6] = param_2[0xb6];
  param_1[0xb7] = uVar4;
  uVar4 = param_2[0xb9];
  param_1[0xb8] = param_2[0xb8];
  param_1[0xb9] = uVar4;
  param_1[0xba] = param_2[0xba];
  uVar4 = param_2[0xbc];
  param_1[0xbb] = param_2[0xbb];
  param_1[0xbc] = uVar4;
  uVar4 = param_2[0xbe];
  param_1[0xbd] = param_2[0xbd];
  param_1[0xbe] = uVar4;
  uVar1 = *(undefined4 *)((longlong)param_2 + 0x5fc);
  uVar2 = *(undefined4 *)(param_2 + 0xc0);
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x604);
  *(undefined4 *)(param_1 + 0xbf) = *(undefined4 *)(param_2 + 0xbf);
  *(undefined4 *)((longlong)param_1 + 0x5fc) = uVar1;
  *(undefined4 *)(param_1 + 0xc0) = uVar2;
  *(undefined4 *)((longlong)param_1 + 0x604) = uVar3;
  param_1[0xc1] = param_2[0xc1];
  *(undefined2 *)(param_1 + 0xc2) = *(undefined2 *)(param_2 + 0xc2);
  lVar7 = (longlong)(param_2[0xc4] - param_2[0xc3]) >> 5;
  if (lVar7 != 0) {
    puVar6 = (undefined4 *)thunk_FUN_140983da8(lVar7 << 5);
  }
  param_1[0xc3] = puVar6;
  param_1[0xc4] = puVar6;
  param_1[0xc5] = puVar6 + lVar7 * 8;
  puVar5 = (undefined4 *)param_2[0xc4];
  for (puVar8 = (undefined4 *)param_2[0xc3]; puVar8 != puVar5; puVar8 = puVar8 + 8) {
    *puVar6 = *puVar8;
    FUN_14031f8f0(puVar6 + 2,puVar8 + 2);
    puVar6 = puVar6 + 8;
  }
  param_1[0xc4] = puVar6;
  param_1[0xc6] = param_2[0xc6];
  return param_1;
}
