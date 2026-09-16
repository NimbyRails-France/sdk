
undefined8 * FUN_140450420(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  void *pvVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;

  *param_1 = *param_2;
  FUN_1404507d0(param_1 + 1,param_2 + 1);
  puVar1 = param_2 + 0x11;
  puVar2 = param_1 + 0x11;
  param_1[0xf] = param_2[0xf];
  *(undefined4 *)(param_1 + 0x10) = *(undefined4 *)(param_2 + 0x10);
  if (puVar2 != puVar1) {
    uVar3 = *puVar2;
    *puVar2 = 0;
    uVar4 = param_1[0x13];
    param_1[0x13] = 0;
    pvVar5 = (void *)*puVar2;
    *puVar2 = uVar3;
    param_1[0x12] = uVar3;
    param_1[0x13] = uVar4;
    if (pvVar5 != (void *)0x0) {
      free(pvVar5);
    }
    uVar3 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar3;
    uVar3 = param_1[0x12];
    param_1[0x12] = param_2[0x12];
    param_2[0x12] = uVar3;
    uVar3 = param_1[0x13];
    param_1[0x13] = param_2[0x13];
    param_2[0x13] = uVar3;
  }
  param_1[0x14] = param_2[0x14];
  param_1[0x15] = param_2[0x15];
  *(undefined4 *)(param_1 + 0x16) = *(undefined4 *)(param_2 + 0x16);
  if (*(char *)(param_2 + 0x3a) == '\0') {
    if (*(char *)(param_1 + 0x3a) != '\0') {
      pvVar5 = (void *)param_1[0x1c];
      if ((pvVar5 != (void *)0x0) && (pvVar5 != (void *)param_1[0x20])) {
        free(pvVar5);
      }
      *(undefined1 *)(param_1 + 0x3a) = 0;
    }
  }
  else {
    FUN_14045b2d0(param_1 + 0x17);
  }
  uVar3 = param_2[0x3c];
  param_1[0x3b] = param_2[0x3b];
  param_1[0x3c] = uVar3;
  uVar3 = param_2[0x3e];
  param_1[0x3d] = param_2[0x3d];
  param_1[0x3e] = uVar3;
  uVar3 = param_2[0x40];
  param_1[0x3f] = param_2[0x3f];
  param_1[0x40] = uVar3;
  uVar3 = param_2[0x42];
  param_1[0x41] = param_2[0x41];
  param_1[0x42] = uVar3;
  param_1[0x43] = param_2[0x43];
  uVar3 = param_2[0x45];
  param_1[0x44] = param_2[0x44];
  param_1[0x45] = uVar3;
  uVar3 = param_2[0x47];
  param_1[0x46] = param_2[0x46];
  param_1[0x47] = uVar3;
  uVar3 = param_2[0x49];
  param_1[0x48] = param_2[0x48];
  param_1[0x49] = uVar3;
  uVar3 = param_2[0x4b];
  param_1[0x4a] = param_2[0x4a];
  param_1[0x4b] = uVar3;
  uVar3 = param_2[0x4d];
  param_1[0x4c] = param_2[0x4c];
  param_1[0x4d] = uVar3;
  uVar6 = *(undefined4 *)((longlong)param_2 + 0x274);
  uVar7 = *(undefined4 *)(param_2 + 0x4f);
  uVar8 = *(undefined4 *)((longlong)param_2 + 0x27c);
  *(undefined4 *)(param_1 + 0x4e) = *(undefined4 *)(param_2 + 0x4e);
  *(undefined4 *)((longlong)param_1 + 0x274) = uVar6;
  *(undefined4 *)(param_1 + 0x4f) = uVar7;
  *(undefined4 *)((longlong)param_1 + 0x27c) = uVar8;
  uVar6 = *(undefined4 *)((longlong)param_2 + 0x284);
  uVar7 = *(undefined4 *)(param_2 + 0x51);
  uVar8 = *(undefined4 *)((longlong)param_2 + 0x28c);
  *(undefined4 *)(param_1 + 0x50) = *(undefined4 *)(param_2 + 0x50);
  *(undefined4 *)((longlong)param_1 + 0x284) = uVar6;
  *(undefined4 *)(param_1 + 0x51) = uVar7;
  *(undefined4 *)((longlong)param_1 + 0x28c) = uVar8;
  if (*(char *)(param_2 + 0x96) == '\0') {
    FUN_14045f560(param_1 + 0x52);
  }
  else {
    FUN_14045b070();
  }
  uVar3 = param_2[0x98];
  param_1[0x97] = param_2[0x97];
  param_1[0x98] = uVar3;
  uVar3 = param_2[0x9a];
  param_1[0x99] = param_2[0x99];
  param_1[0x9a] = uVar3;
  uVar3 = param_2[0x9c];
  param_1[0x9b] = param_2[0x9b];
  param_1[0x9c] = uVar3;
  uVar3 = param_2[0x9e];
  param_1[0x9d] = param_2[0x9d];
  param_1[0x9e] = uVar3;
  uVar3 = param_2[0xa0];
  param_1[0x9f] = param_2[0x9f];
  param_1[0xa0] = uVar3;
  uVar3 = param_2[0xa2];
  param_1[0xa1] = param_2[0xa1];
  param_1[0xa2] = uVar3;
  uVar3 = param_2[0xa4];
  param_1[0xa3] = param_2[0xa3];
  param_1[0xa4] = uVar3;
  uVar3 = param_2[0xa6];
  param_1[0xa5] = param_2[0xa5];
  param_1[0xa6] = uVar3;
  uVar3 = param_2[0xa8];
  param_1[0xa7] = param_2[0xa7];
  param_1[0xa8] = uVar3;
  uVar6 = *(undefined4 *)((longlong)param_2 + 0x54c);
  uVar7 = *(undefined4 *)(param_2 + 0xaa);
  uVar8 = *(undefined4 *)((longlong)param_2 + 0x554);
  *(undefined4 *)(param_1 + 0xa9) = *(undefined4 *)(param_2 + 0xa9);
  *(undefined4 *)((longlong)param_1 + 0x54c) = uVar6;
  *(undefined4 *)(param_1 + 0xaa) = uVar7;
  *(undefined4 *)((longlong)param_1 + 0x554) = uVar8;
  param_1[0xab] = param_2[0xab];
  if (*(char *)(param_2 + 0xb3) == '\0') {
    if (*(char *)(param_1 + 0xb3) != '\0') {
      if ((void *)param_1[0xaf] != (void *)0x0) {
        free((void *)param_1[0xaf]);
      }
      *(undefined1 *)(param_1 + 0xb3) = 0;
    }
  }
  else {
    FUN_14045bfc0(param_1 + 0xac);
  }
  uVar3 = param_2[0xb5];
  puVar1 = param_2 + 0xc3;
  puVar2 = param_1 + 0xc3;
  param_1[0xb4] = param_2[0xb4];
  param_1[0xb5] = uVar3;
  uVar3 = param_2[0xb7];
  param_1[0xb6] = param_2[0xb6];
  param_1[0xb7] = uVar3;
  uVar3 = param_2[0xb9];
  param_1[0xb8] = param_2[0xb8];
  param_1[0xb9] = uVar3;
  param_1[0xba] = param_2[0xba];
  uVar3 = param_2[0xbc];
  param_1[0xbb] = param_2[0xbb];
  param_1[0xbc] = uVar3;
  uVar3 = param_2[0xbe];
  param_1[0xbd] = param_2[0xbd];
  param_1[0xbe] = uVar3;
  uVar6 = *(undefined4 *)((longlong)param_2 + 0x5fc);
  uVar7 = *(undefined4 *)(param_2 + 0xc0);
  uVar8 = *(undefined4 *)((longlong)param_2 + 0x604);
  *(undefined4 *)(param_1 + 0xbf) = *(undefined4 *)(param_2 + 0xbf);
  *(undefined4 *)((longlong)param_1 + 0x5fc) = uVar6;
  *(undefined4 *)(param_1 + 0xc0) = uVar7;
  *(undefined4 *)((longlong)param_1 + 0x604) = uVar8;
  param_1[0xc1] = param_2[0xc1];
  *(undefined2 *)(param_1 + 0xc2) = *(undefined2 *)(param_2 + 0xc2);
  if (puVar2 != puVar1) {
    FUN_140458a00(puVar2);
    uVar3 = *puVar2;
    *puVar2 = *puVar1;
    *puVar1 = uVar3;
    uVar3 = param_1[0xc4];
    param_1[0xc4] = param_2[0xc4];
    param_2[0xc4] = uVar3;
    uVar3 = param_1[0xc5];
    param_1[0xc5] = param_2[0xc5];
    param_2[0xc5] = uVar3;
  }
  param_1[0xc6] = param_2[0xc6];
  return param_1;
}
