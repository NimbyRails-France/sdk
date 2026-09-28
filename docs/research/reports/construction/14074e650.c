
undefined8 * FUN_14074e650(longlong param_1,longlong param_2,undefined8 *param_3)

{
  undefined8 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  longlong *plVar6;
  undefined8 *puVar7;
  longlong lVar8;
  undefined8 *puVar9;
  
  lVar8 = (param_2 - param_1 >> 3) * 0x1cac083126e978d5;
  if (0 < lVar8) {
    puVar7 = (undefined8 *)(param_1 + 0x50);
    puVar9 = param_3 + 0x52;
    do {
      plVar6 = puVar7 + -6;
      *param_3 = puVar7[-10];
      *(undefined4 *)(puVar9 + -0x51) = *(undefined4 *)(puVar7 + -9);
      *(undefined1 *)((longlong)puVar9 + -0x284) = *(undefined1 *)((longlong)puVar7 + -0x44);
      uVar2 = *(undefined4 *)((longlong)puVar7 + -0x3c);
      uVar3 = *(undefined4 *)(puVar7 + -7);
      uVar4 = *(undefined4 *)((longlong)puVar7 + -0x34);
      *(undefined4 *)(puVar9 + -0x50) = *(undefined4 *)(puVar7 + -8);
      *(undefined4 *)((longlong)puVar9 + -0x27c) = uVar2;
      *(undefined4 *)(puVar9 + -0x4f) = uVar3;
      *(undefined4 *)((longlong)puVar9 + -0x274) = uVar4;
      if (puVar9 + -0x4e != plVar6) {
        if (0xf < (ulonglong)puVar7[-3]) {
          plVar6 = (longlong *)*plVar6;
        }
        FUN_140030630(puVar9 + -0x4e,plVar6,puVar7[-4]);
      }
      *(undefined1 *)(puVar9 + -0x4a) = *(undefined1 *)(puVar7 + -2);
      *(undefined4 *)((longlong)puVar9 + -0x24c) = *(undefined4 *)((longlong)puVar7 + -0xc);
      if (puVar9 + -0x49 != puVar7 + -1) {
        FUN_14032fe20(puVar9 + -0x49,puVar7[-1],*puVar7);
      }
      puVar1 = puVar9 + -0x46;
      if (puVar1 != puVar7 + 2) {
        puVar9[-0x45] = *puVar1;
        FUN_14032ff40(puVar1,puVar7[2],puVar7[3]);
      }
      puVar1 = puVar9 + -1;
      if (puVar1 != puVar7 + 0x47) {
        *puVar9 = *puVar1;
        FUN_14032ff40(puVar1,puVar7[0x47],puVar7[0x48]);
      }
      puVar1 = puVar9 + 0x14;
      if (puVar1 != puVar7 + 0x5c) {
        puVar9[0x15] = *puVar1;
        FUN_14032ff40(puVar1,puVar7[0x5c],puVar7[0x5d]);
      }
      uVar2 = *(undefined4 *)((longlong)puVar7 + 0x32c);
      uVar3 = *(undefined4 *)(puVar7 + 0x66);
      uVar4 = *(undefined4 *)((longlong)puVar7 + 0x334);
      *(undefined4 *)(puVar9 + 0x1d) = *(undefined4 *)(puVar7 + 0x65);
      *(undefined4 *)((longlong)puVar9 + 0xec) = uVar2;
      *(undefined4 *)(puVar9 + 0x1e) = uVar3;
      *(undefined4 *)((longlong)puVar9 + 0xf4) = uVar4;
      uVar2 = *(undefined4 *)((longlong)puVar7 + 0x33c);
      uVar3 = *(undefined4 *)(puVar7 + 0x68);
      uVar4 = *(undefined4 *)((longlong)puVar7 + 0x344);
      *(undefined4 *)(puVar9 + 0x1f) = *(undefined4 *)(puVar7 + 0x67);
      *(undefined4 *)((longlong)puVar9 + 0xfc) = uVar2;
      *(undefined4 *)(puVar9 + 0x20) = uVar3;
      *(undefined4 *)((longlong)puVar9 + 0x104) = uVar4;
      *(undefined4 *)(puVar9 + 0x21) = *(undefined4 *)(puVar7 + 0x69);
      *(undefined4 *)((longlong)puVar9 + 0x10c) = *(undefined4 *)((longlong)puVar7 + 0x34c);
      *(undefined4 *)(puVar9 + 0x22) = *(undefined4 *)(puVar7 + 0x6a);
      *(undefined1 *)((longlong)puVar9 + 0x114) = *(undefined1 *)((longlong)puVar7 + 0x354);
      *(undefined4 *)(puVar9 + 0x23) = *(undefined4 *)(puVar7 + 0x6b);
      if (puVar9 + 0x24 != puVar7 + 0x6c) {
        FUN_1403416a0(puVar9 + 0x24,puVar7[0x6c],puVar7[0x6d]);
      }
      uVar5 = puVar7[0x70];
      lVar8 = lVar8 + -1;
      param_3 = param_3 + 0x7d;
      puVar9[0x27] = puVar7[0x6f];
      puVar9[0x28] = uVar5;
      puVar1 = puVar7 + 0x71;
      uVar2 = *(undefined4 *)((longlong)puVar7 + 0x38c);
      uVar3 = *(undefined4 *)(puVar7 + 0x72);
      uVar4 = *(undefined4 *)((longlong)puVar7 + 0x394);
      puVar7 = puVar7 + 0x7d;
      *(undefined4 *)(puVar9 + 0x29) = *(undefined4 *)puVar1;
      *(undefined4 *)((longlong)puVar9 + 0x14c) = uVar2;
      *(undefined4 *)(puVar9 + 0x2a) = uVar3;
      *(undefined4 *)((longlong)puVar9 + 0x154) = uVar4;
      puVar9 = puVar9 + 0x7d;
    } while (0 < lVar8);
  }
  return param_3;
}

