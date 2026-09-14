
undefined8 *
FUN_140475650(undefined8 *param_1,undefined8 *param_2,undefined8 *param_3,undefined8 *param_4,
             undefined1 param_5)

{
  undefined8 *puVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  
  *param_1 = param_4;
  if (param_2 != param_3) {
    puVar6 = param_2 + 0x18;
    puVar7 = param_2 + 0xc;
    do {
      *param_4 = puVar7[-0xc];
      param_4[1] = puVar7[-0xb];
      *(undefined4 *)(param_4 + 2) = *(undefined4 *)(puVar7 + -10);
      *(undefined4 *)((longlong)param_4 + 0x14) = *(undefined4 *)((longlong)puVar7 + -0x4c);
      uVar2 = *(undefined2 *)(puVar7 + -9);
      param_4[5] = 0;
      param_4[6] = 0;
      param_4[4] = 0;
      *(undefined2 *)(param_4 + 3) = uVar2;
      param_4[4] = puVar7[-8];
      puVar7[-8] = 0;
      uVar4 = param_4[5];
      param_4[5] = puVar7[-7];
      puVar7[-7] = uVar4;
      uVar4 = param_4[6];
      param_4[6] = puVar7[-6];
      puVar7[-6] = uVar4;
      param_4[8] = 0;
      param_4[9] = 0;
      param_4[7] = 0;
      param_4[7] = puVar7[-5];
      puVar7[-5] = 0;
      uVar4 = param_4[8];
      param_4[8] = puVar7[-4];
      puVar7[-4] = uVar4;
      uVar4 = param_4[9];
      param_4[9] = puVar7[-3];
      puVar7[-3] = uVar4;
      param_4[0xb] = 0;
      param_4[0xc] = 0;
      param_4[10] = 0;
      param_4[10] = puVar7[-2];
      puVar7[-2] = 0;
      uVar4 = param_4[0xb];
      param_4[0xb] = puVar7[-1];
      puVar7[-1] = uVar4;
      uVar4 = param_4[0xc];
      param_4[0xc] = *puVar7;
      uVar5 = puVar7[1];
      puVar7[1] = &DAT_140a6d430;
      *puVar7 = uVar4;
      param_4[0xd] = uVar5;
      uVar4 = puVar7[2];
      puVar7[2] = 0;
      param_4[0xe] = uVar4;
      uVar4 = puVar7[3];
      puVar7[3] = 0;
      param_4[0xf] = uVar4;
      uVar4 = puVar7[4];
      puVar7[4] = 0;
      param_4[0x10] = uVar4;
      *(undefined1 *)(param_4 + 0x11) = param_5;
      *(undefined1 *)(param_4 + 0x12) = *(undefined1 *)(puVar7 + 6);
      *(undefined1 *)((longlong)param_4 + 0x91) = *(undefined1 *)((longlong)puVar7 + 0x31);
      *(undefined1 *)((longlong)param_4 + 0x92) = *(undefined1 *)((longlong)puVar7 + 0x32);
      param_4[0x13] = puVar7[7];
      puVar7[7] = 0;
      uVar3 = *(undefined4 *)(puVar7 + 8);
      param_4[0x16] = 0;
      *(undefined4 *)(param_4 + 0x14) = uVar3;
      param_4[0x17] = 0;
      param_4[0x18] = *puVar6;
      param_4[0x16] = 1;
      *(undefined4 *)(param_4 + 0x19) = 0;
      param_4[0x15] = &DAT_140b5abc0;
      puVar6 = puVar6 + 0x20;
      uVar4 = param_4[0x18];
      param_4[0x18] = puVar7[0xc];
      *(undefined4 *)(param_4 + 0x19) = *(undefined4 *)(puVar7 + 0xd);
      puVar7[0xc] = uVar4;
      *(undefined4 *)(puVar7 + 0xd) = 0;
      uVar4 = param_4[0x15];
      param_4[0x15] = puVar7[9];
      puVar7[9] = uVar4;
      uVar4 = param_4[0x16];
      param_4[0x16] = puVar7[10];
      puVar7[10] = uVar4;
      uVar4 = param_4[0x17];
      param_4[0x17] = puVar7[0xb];
      puVar7[0xb] = uVar4;
      param_4[0x1b] = 0;
      param_4[0x1c] = 0;
      param_4[0x1a] = 0;
      param_4[0x1a] = puVar7[0xe];
      puVar7[0xe] = 0;
      uVar4 = param_4[0x1b];
      param_4[0x1b] = puVar7[0xf];
      puVar7[0xf] = uVar4;
      uVar4 = param_4[0x1c];
      param_4[0x1c] = puVar7[0x10];
      puVar7[0x10] = uVar4;
      *(undefined1 *)(param_4 + 0x1d) = *(undefined1 *)(puVar7 + 0x11);
      param_4[0x1e] = puVar7[0x12];
      param_4[0x1f] = puVar7[0x13];
      param_4 = param_4 + 0x20;
      puVar1 = puVar7 + 0x14;
      puVar7 = puVar7 + 0x20;
    } while (puVar1 != param_3);
    *param_1 = param_4;
  }
  return param_1;
}

