
undefined8 * FUN_14043e3b0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  void *pvVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined8 uVar6;

  *param_1 = *param_2;
  FUN_140339970(param_1 + 1,param_2 + 1);
  param_1[0xf] = param_2[0xf];
  *(undefined4 *)(param_1 + 0x10) = *(undefined4 *)(param_2 + 0x10);
  if (param_1 + 0x11 != param_2 + 0x11) {
    FUN_140341ea0(param_1 + 0x11,param_2[0x11],param_2[0x12]);
  }
  param_1[0x14] = param_2[0x14];
  param_1[0x15] = param_2[0x15];
  *(undefined4 *)(param_1 + 0x16) = *(undefined4 *)(param_2 + 0x16);
  if (*(char *)(param_2 + 0x3a) == '\0') {
    if (*(char *)(param_1 + 0x3a) != '\0') {
      pvVar2 = (void *)param_1[0x1c];
      if ((pvVar2 != (void *)0x0) && (pvVar2 != (void *)param_1[0x20])) {
        free(pvVar2);
      }
      *(undefined1 *)(param_1 + 0x3a) = 0;
    }
  }
  else if (*(char *)(param_1 + 0x3a) == '\0') {
    FUN_140462f40(param_1 + 0x17,param_2 + 0x17);
    *(undefined1 *)(param_1 + 0x3a) = 1;
  }
  else {
    uVar3 = *(undefined4 *)((longlong)param_2 + 0xbc);
    uVar4 = *(undefined4 *)(param_2 + 0x18);
    uVar5 = *(undefined4 *)((longlong)param_2 + 0xc4);
    puVar1 = param_1 + 0x1c;
    *(undefined4 *)(param_1 + 0x17) = *(undefined4 *)(param_2 + 0x17);
    *(undefined4 *)((longlong)param_1 + 0xbc) = uVar3;
    *(undefined4 *)(param_1 + 0x18) = uVar4;
    *(undefined4 *)((longlong)param_1 + 0xc4) = uVar5;
    param_1[0x19] = param_2[0x19];
    param_1[0x1a] = param_2[0x1a];
    *(undefined1 *)(param_1 + 0x1b) = *(undefined1 *)(param_2 + 0x1b);
    if (puVar1 != param_2 + 0x1c) {
      param_1[0x1d] = *puVar1;
      FUN_14033ac20(puVar1,param_2[0x1c],param_2[0x1d]);
    }
    uVar6 = param_2[0x2a];
    param_1[0x29] = param_2[0x29];
    param_1[0x2a] = uVar6;
    uVar6 = param_2[0x2c];
    param_1[0x2b] = param_2[0x2b];
    param_1[0x2c] = uVar6;
    *(undefined1 *)(param_1 + 0x2d) = *(undefined1 *)(param_2 + 0x2d);
    uVar6 = param_2[0x2f];
    param_1[0x2e] = param_2[0x2e];
    param_1[0x2f] = uVar6;
    uVar6 = param_2[0x31];
    param_1[0x30] = param_2[0x30];
    param_1[0x31] = uVar6;
    uVar6 = param_2[0x33];
    param_1[0x32] = param_2[0x32];
    param_1[0x33] = uVar6;
    uVar6 = param_2[0x35];
    param_1[0x34] = param_2[0x34];
    param_1[0x35] = uVar6;
    uVar6 = param_2[0x37];
    param_1[0x36] = param_2[0x36];
    param_1[0x37] = uVar6;
    uVar6 = param_2[0x39];
    param_1[0x38] = param_2[0x38];
    param_1[0x39] = uVar6;
  }
  uVar6 = param_2[0x3c];
  param_1[0x3b] = param_2[0x3b];
  param_1[0x3c] = uVar6;
  uVar6 = param_2[0x3e];
  param_1[0x3d] = param_2[0x3d];
  param_1[0x3e] = uVar6;
  uVar6 = param_2[0x40];
  param_1[0x3f] = param_2[0x3f];
  param_1[0x40] = uVar6;
  uVar6 = param_2[0x42];
  param_1[0x41] = param_2[0x41];
  param_1[0x42] = uVar6;
  param_1[0x43] = param_2[0x43];
  uVar6 = param_2[0x45];
  param_1[0x44] = param_2[0x44];
  param_1[0x45] = uVar6;
  uVar6 = param_2[0x47];
  param_1[0x46] = param_2[0x46];
  param_1[0x47] = uVar6;
  uVar6 = param_2[0x49];
  param_1[0x48] = param_2[0x48];
  param_1[0x49] = uVar6;
  uVar6 = param_2[0x4b];
  param_1[0x4a] = param_2[0x4a];
  param_1[0x4b] = uVar6;
  uVar6 = param_2[0x4d];
  param_1[0x4c] = param_2[0x4c];
  param_1[0x4d] = uVar6;
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x274);
  uVar4 = *(undefined4 *)(param_2 + 0x4f);
  uVar5 = *(undefined4 *)((longlong)param_2 + 0x27c);
  *(undefined4 *)(param_1 + 0x4e) = *(undefined4 *)(param_2 + 0x4e);
  *(undefined4 *)((longlong)param_1 + 0x274) = uVar3;
  *(undefined4 *)(param_1 + 0x4f) = uVar4;
  *(undefined4 *)((longlong)param_1 + 0x27c) = uVar5;
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x284);
  uVar4 = *(undefined4 *)(param_2 + 0x51);
  uVar5 = *(undefined4 *)((longlong)param_2 + 0x28c);
  *(undefined4 *)(param_1 + 0x50) = *(undefined4 *)(param_2 + 0x50);
  *(undefined4 *)((longlong)param_1 + 0x284) = uVar3;
  *(undefined4 *)(param_1 + 0x51) = uVar4;
  *(undefined4 *)((longlong)param_1 + 0x28c) = uVar5;
  if (*(char *)(param_2 + 0x96) == '\0') {
    if (*(char *)(param_1 + 0x96) != '\0') {
      pvVar2 = (void *)param_1[0x7d];
      if ((pvVar2 != (void *)0x0) && (pvVar2 != (void *)param_1[0x81])) {
        free(pvVar2);
      }
      if ((void *)param_1[0x6a] != (void *)0x0) {
        free((void *)param_1[0x6a]);
      }
      if ((void *)param_1[0x67] != (void *)0x0) {
        free((void *)param_1[0x67]);
      }
      *(undefined1 *)(param_1 + 0x96) = 0;
    }
  }
  else if (*(char *)(param_1 + 0x96) == '\0') {
    FUN_140463030(param_1 + 0x52,param_2 + 0x52);
    *(undefined1 *)(param_1 + 0x96) = 1;
  }
  else {
    FUN_14036cef0();
    uVar6 = param_2[0x75];
    puVar1 = param_1 + 0x7d;
    param_1[0x74] = param_2[0x74];
    param_1[0x75] = uVar6;
    uVar6 = param_2[0x77];
    param_1[0x76] = param_2[0x76];
    param_1[0x77] = uVar6;
    uVar6 = param_2[0x79];
    param_1[0x78] = param_2[0x78];
    param_1[0x79] = uVar6;
    uVar3 = *(undefined4 *)((longlong)param_2 + 0x3d4);
    uVar4 = *(undefined4 *)(param_2 + 0x7b);
    uVar5 = *(undefined4 *)((longlong)param_2 + 0x3dc);
    *(undefined4 *)(param_1 + 0x7a) = *(undefined4 *)(param_2 + 0x7a);
    *(undefined4 *)((longlong)param_1 + 0x3d4) = uVar3;
    *(undefined4 *)(param_1 + 0x7b) = uVar4;
    *(undefined4 *)((longlong)param_1 + 0x3dc) = uVar5;
    param_1[0x7c] = param_2[0x7c];
    if (puVar1 != param_2 + 0x7d) {
      param_1[0x7e] = *puVar1;
      FUN_14032a3f0(puVar1,param_2[0x7d],param_2[0x7e]);
    }
    param_1[0x8a] = param_2[0x8a];
    param_1[0x8b] = param_2[0x8b];
    param_1[0x8c] = param_2[0x8c];
    *(undefined4 *)(param_1 + 0x8d) = *(undefined4 *)(param_2 + 0x8d);
    *(undefined4 *)((longlong)param_1 + 0x46c) = *(undefined4 *)((longlong)param_2 + 0x46c);
    *(undefined4 *)(param_1 + 0x8e) = *(undefined4 *)(param_2 + 0x8e);
    *(undefined4 *)((longlong)param_1 + 0x474) = *(undefined4 *)((longlong)param_2 + 0x474);
    *(undefined4 *)(param_1 + 0x8f) = *(undefined4 *)(param_2 + 0x8f);
    param_1[0x90] = param_2[0x90];
    param_1[0x91] = param_2[0x91];
    *(undefined1 *)(param_1 + 0x92) = *(undefined1 *)(param_2 + 0x92);
    uVar6 = *(undefined8 *)((longlong)param_2 + 0x49c);
    *(undefined8 *)((longlong)param_1 + 0x494) = *(undefined8 *)((longlong)param_2 + 0x494);
    *(undefined8 *)((longlong)param_1 + 0x49c) = uVar6;
    param_1[0x95] = param_2[0x95];
  }
  uVar6 = param_2[0x98];
  param_1[0x97] = param_2[0x97];
  param_1[0x98] = uVar6;
  uVar6 = param_2[0x9a];
  param_1[0x99] = param_2[0x99];
  param_1[0x9a] = uVar6;
  uVar6 = param_2[0x9c];
  param_1[0x9b] = param_2[0x9b];
  param_1[0x9c] = uVar6;
  uVar6 = param_2[0x9e];
  param_1[0x9d] = param_2[0x9d];
  param_1[0x9e] = uVar6;
  uVar6 = param_2[0xa0];
  param_1[0x9f] = param_2[0x9f];
  param_1[0xa0] = uVar6;
  uVar6 = param_2[0xa2];
  param_1[0xa1] = param_2[0xa1];
  param_1[0xa2] = uVar6;
  uVar6 = param_2[0xa4];
  param_1[0xa3] = param_2[0xa3];
  param_1[0xa4] = uVar6;
  uVar6 = param_2[0xa6];
  param_1[0xa5] = param_2[0xa5];
  param_1[0xa6] = uVar6;
  uVar6 = param_2[0xa8];
  param_1[0xa7] = param_2[0xa7];
  param_1[0xa8] = uVar6;
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x54c);
  uVar4 = *(undefined4 *)(param_2 + 0xaa);
  uVar5 = *(undefined4 *)((longlong)param_2 + 0x554);
  *(undefined4 *)(param_1 + 0xa9) = *(undefined4 *)(param_2 + 0xa9);
  *(undefined4 *)((longlong)param_1 + 0x54c) = uVar3;
  *(undefined4 *)(param_1 + 0xaa) = uVar4;
  *(undefined4 *)((longlong)param_1 + 0x554) = uVar5;
  param_1[0xab] = param_2[0xab];
  if (*(char *)(param_2 + 0xb3) == '\0') {
    if (*(char *)(param_1 + 0xb3) != '\0') {
      if ((void *)param_1[0xaf] != (void *)0x0) {
        free((void *)param_1[0xaf]);
      }
      *(undefined1 *)(param_1 + 0xb3) = 0;
    }
  }
  else if (*(char *)(param_1 + 0xb3) == '\0') {
    FUN_140463190(param_1 + 0xac,param_2 + 0xac);
    *(undefined1 *)(param_1 + 0xb3) = 1;
  }
  else {
    param_1[0xac] = param_2[0xac];
    *(undefined4 *)(param_1 + 0xad) = *(undefined4 *)(param_2 + 0xad);
    *(undefined4 *)((longlong)param_1 + 0x56c) = *(undefined4 *)((longlong)param_2 + 0x56c);
    *(undefined4 *)(param_1 + 0xae) = *(undefined4 *)(param_2 + 0xae);
    *(undefined4 *)((longlong)param_1 + 0x574) = *(undefined4 *)((longlong)param_2 + 0x574);
    if (param_1 + 0xaf != param_2 + 0xaf) {
      FUN_140341ea0(param_1 + 0xaf,param_2[0xaf],param_2[0xb0]);
    }
    param_1[0xb2] = param_2[0xb2];
  }
  uVar6 = param_2[0xb5];
  param_1[0xb4] = param_2[0xb4];
  param_1[0xb5] = uVar6;
  uVar6 = param_2[0xb7];
  param_1[0xb6] = param_2[0xb6];
  param_1[0xb7] = uVar6;
  uVar6 = param_2[0xb9];
  param_1[0xb8] = param_2[0xb8];
  param_1[0xb9] = uVar6;
  param_1[0xba] = param_2[0xba];
  uVar6 = param_2[0xbc];
  param_1[0xbb] = param_2[0xbb];
  param_1[0xbc] = uVar6;
  uVar6 = param_2[0xbe];
  param_1[0xbd] = param_2[0xbd];
  param_1[0xbe] = uVar6;
  uVar3 = *(undefined4 *)((longlong)param_2 + 0x5fc);
  uVar4 = *(undefined4 *)(param_2 + 0xc0);
  uVar5 = *(undefined4 *)((longlong)param_2 + 0x604);
  *(undefined4 *)(param_1 + 0xbf) = *(undefined4 *)(param_2 + 0xbf);
  *(undefined4 *)((longlong)param_1 + 0x5fc) = uVar3;
  *(undefined4 *)(param_1 + 0xc0) = uVar4;
  *(undefined4 *)((longlong)param_1 + 0x604) = uVar5;
  param_1[0xc1] = param_2[0xc1];
  *(undefined2 *)(param_1 + 0xc2) = *(undefined2 *)(param_2 + 0xc2);
  if (param_1 + 0xc3 != param_2 + 0xc3) {
    FUN_1404611f0(param_1 + 0xc3,param_2[0xc3],param_2[0xc4]);
  }
  param_1[0xc6] = param_2[0xc6];
  return param_1;
}
