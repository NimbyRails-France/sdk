
undefined8 * FUN_140488f80(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;

  if (param_1 != param_2) {
    FUN_140494e70();
    if (param_2[2] != 0) {
      puVar4 = (undefined8 *)FUN_140494ef0(param_1,param_2[2],param_1);
      param_1[2] = puVar4;
      puVar5 = (undefined8 *)*puVar4;
      puVar1 = puVar4;
      while (puVar3 = puVar5, puVar3 != (undefined8 *)0x0) {
        puVar1 = puVar3;
        puVar5 = (undefined8 *)*puVar3;
      }
      *param_1 = puVar1;
      puVar1 = (undefined8 *)puVar4[1];
      while (puVar5 = puVar1, puVar5 != (undefined8 *)0x0) {
        puVar4 = puVar5;
        puVar1 = (undefined8 *)puVar5[1];
      }
      param_1[1] = puVar4;
      param_1[4] = param_2[4];
    }
  }
  FUN_140493100(param_1 + 6,param_2 + 6);
  if (param_1 + 0xc != param_2 + 0xc) {
    FUN_14032fe20(param_1 + 0xc,param_2[0xc],param_2[0xd]);
  }
  puVar1 = param_1 + 0xf;
  if (puVar1 != param_2 + 0xf) {
    FUN_140494fd0(puVar1);
    if (param_2[0x11] != 0) {
      puVar5 = (undefined8 *)FUN_140495050(puVar1,param_2[0x11],puVar1);
      param_1[0x11] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x10] = puVar5;
      param_1[0x13] = param_2[0x13];
    }
  }
  FUN_140493310(param_1 + 0x15,param_2 + 0x15);
  if (param_1 + 0x1b != param_2 + 0x1b) {
    FUN_14032fe20(param_1 + 0x1b,param_2[0x1b],param_2[0x1c]);
  }
  if (param_1 + 0x1e != param_2 + 0x1e) {
    FUN_14041cc40(param_1 + 0x1e,param_2[0x1e],param_2[0x1f]);
  }
  if (param_1 + 0x21 != param_2 + 0x21) {
    FUN_14041cdc0(param_1 + 0x21,param_2[0x21],param_2[0x22]);
  }
  if (param_1 + 0x24 != param_2 + 0x24) {
    FUN_14041cf40(param_1 + 0x24,param_2[0x24],param_2[0x25]);
  }
  FUN_14041ace0(param_1 + 0x27,param_2 + 0x27);
  puVar1 = param_1 + 0x2d;
  if (puVar1 != param_2 + 0x2d) {
    FUN_140495130(puVar1);
    if (param_2[0x2f] != 0) {
      puVar5 = (undefined8 *)FUN_1404951d0(puVar1,param_2[0x2f],puVar1);
      param_1[0x2f] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x2e] = puVar5;
      param_1[0x31] = param_2[0x31];
    }
  }
  puVar1 = param_1 + 0x33;
  if (puVar1 != param_2 + 0x33) {
    FUN_1404952b0(puVar1);
    if (param_2[0x35] != 0) {
      puVar5 = (undefined8 *)FUN_140495330(puVar1,param_2[0x35],puVar1);
      param_1[0x35] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x34] = puVar5;
      param_1[0x37] = param_2[0x37];
    }
  }
  puVar1 = param_1 + 0x39;
  if (puVar1 != param_2 + 0x39) {
    FUN_140351be0(puVar1,param_1[0x3b]);
    *puVar1 = puVar1;
    param_1[0x3b] = 0;
    *(undefined1 *)(param_1 + 0x3c) = 0;
    param_1[0x3d] = 0;
    param_1[0x3a] = puVar1;
    if (param_2[0x3b] != 0) {
      puVar5 = (undefined8 *)FUN_140495450(puVar1,param_2[0x3b],puVar1);
      param_1[0x3b] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x3a] = puVar5;
      param_1[0x3d] = param_2[0x3d];
    }
  }
  FUN_140492e40(param_1 + 0x3f,param_2 + 0x3f);
  FUN_140492e40(param_1 + 0x45,param_2 + 0x45);
  FUN_140492db0(param_1 + 0x4b,param_2 + 0x4b);
  FUN_140492d00(param_1 + 0x51,param_2 + 0x51);
  FUN_140492e40(param_1 + 0x57,param_2 + 0x57);
  FUN_140492d00(param_1 + 0x5d,param_2 + 0x5d);
  FUN_140492db0(param_1 + 99,param_2 + 99);
  FUN_140492db0(param_1 + 0x69,param_2 + 0x69);
  FUN_140492c70(param_1 + 0x6f,param_2 + 0x6f);
  FUN_140492c70(param_1 + 0x75,param_2 + 0x75);
  FUN_140492c70(param_1 + 0x7b,param_2 + 0x7b);
  puVar1 = param_1 + 0x81;
  if (puVar1 != param_2 + 0x81) {
    FUN_140495b00(puVar1);
    if (param_2[0x83] != 0) {
      puVar5 = (undefined8 *)FUN_140495b80(puVar1,param_2[0x83],puVar1);
      param_1[0x83] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x82] = puVar5;
      param_1[0x85] = param_2[0x85];
    }
  }
  FUN_140418e20(param_1 + 0x87,param_2 + 0x87);
  if (param_1 + 0x8d != param_2 + 0x8d) {
    FUN_140494470(param_1 + 0x8d,param_2[0x8d],param_2[0x8e]);
  }
  if (param_1 + 0x90 != param_2 + 0x90) {
    FUN_140494610(param_1 + 0x90,param_2[0x90],param_2[0x91]);
  }
  puVar1 = param_1 + 0x93;
  if (puVar1 != param_2 + 0x93) {
    FUN_140418b70(puVar1);
    if (param_2[0x95] != 0) {
      puVar5 = (undefined8 *)FUN_140495c60(puVar1,param_2[0x95],puVar1);
      param_1[0x95] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x94] = puVar5;
      param_1[0x97] = param_2[0x97];
    }
  }
  puVar1 = param_1 + 0x99;
  if (puVar1 != param_2 + 0x99) {
    FUN_140418bf0(puVar1);
    if (param_2[0x9b] != 0) {
      puVar5 = (undefined8 *)FUN_140495da0(puVar1,param_2[0x9b],puVar1);
      param_1[0x9b] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0x9a] = puVar5;
      param_1[0x9d] = param_2[0x9d];
    }
  }
  if (param_1 + 0x9f != param_2 + 0x9f) {
    FUN_140494790(param_1 + 0x9f,param_2[0x9f],param_2[0xa0]);
  }
  puVar1 = param_1 + 0xa2;
  if (puVar1 != param_2 + 0xa2) {
    FUN_140418d30(puVar1);
    if (param_2[0xa4] != 0) {
      puVar5 = (undefined8 *)FUN_140495ea0(puVar1,param_2[0xa4],puVar1);
      param_1[0xa4] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0xa3] = puVar5;
      param_1[0xa6] = param_2[0xa6];
    }
  }
  if (param_1 + 0xa8 != param_2 + 0xa8) {
    FUN_14032fe20(param_1 + 0xa8,param_2[0xa8],param_2[0xa9]);
  }
  *(undefined4 *)(param_1 + 0xab) = *(undefined4 *)(param_2 + 0xab);
  *(undefined4 *)((longlong)param_1 + 0x55c) = *(undefined4 *)((longlong)param_2 + 0x55c);
  *(undefined4 *)(param_1 + 0xac) = *(undefined4 *)(param_2 + 0xac);
  FUN_140488ea0(param_1 + 0xad,param_2 + 0xad);
  puVar1 = param_1 + 0xba;
  if (puVar1 != param_2 + 0xba) {
    FUN_140364730(puVar1);
    if (param_2[0xbc] != 0) {
      puVar5 = (undefined8 *)FUN_140495f90(puVar1,param_2[0xbc],puVar1);
      param_1[0xbc] = puVar5;
      puVar3 = (undefined8 *)*puVar5;
      puVar4 = puVar5;
      while (puVar2 = puVar3, puVar2 != (undefined8 *)0x0) {
        puVar4 = puVar2;
        puVar3 = (undefined8 *)*puVar2;
      }
      *puVar1 = puVar4;
      puVar1 = (undefined8 *)puVar5[1];
      while (puVar4 = puVar1, puVar4 != (undefined8 *)0x0) {
        puVar5 = puVar4;
        puVar1 = (undefined8 *)puVar4[1];
      }
      param_1[0xbb] = puVar5;
      param_1[0xbe] = param_2[0xbe];
    }
  }
  param_1[0xc0] = param_2[0xc0];
  param_1[0xc1] = param_2[0xc1];
  param_1[0xc2] = param_2[0xc2];
  param_1[0xc3] = param_2[0xc3];
  param_1[0xc4] = param_2[0xc4];
  param_1[0xc5] = param_2[0xc5];
  param_1[0xc6] = param_2[0xc6];
  param_1[199] = param_2[199];
  param_1[200] = param_2[200];
  return param_1;
}
