// Candidate VA 140347e20; RVA 0x347e20
// Ghidra inferred prototype: undefined FUN_140347e20()

void FUN_140347e20(longlong param_1,undefined8 param_2,undefined8 param_3,longlong *param_4)

{
  longlong *plVar1;
  longlong lVar2;
  undefined8 uVar3;
  ulonglong *puVar4;
  ulonglong *puVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  code *pcVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  bool bVar12;
  int iVar13;
  longlong lVar14;
  
  bVar12 = false;
  if ((*(longlong *)(param_1 + 0x80) != 0) && (*(char *)(param_1 + 0x180) != '\0')) {
    iVar13 = _Mtx_lock(param_1 + 0x580);
    if (iVar13 != 0) {
      std::_Throw_Cpp_error(5);
      pcVar8 = (code *)swi(3);
      (*pcVar8)();
      return;
    }
    if (*(int *)(param_1 + 0x5cc) == 0x7fffffff) {
      *(undefined4 *)(param_1 + 0x5cc) = 0x7ffffffe;
      std::_Throw_Cpp_error(6);
      pcVar8 = (code *)swi(3);
      (*pcVar8)();
      return;
    }
    lVar2 = *(longlong *)(param_1 + 0x80);
    uVar3 = *(undefined8 *)(lVar2 + 0x2200);
    *(undefined8 *)(lVar2 + 0x2200) = *(undefined8 *)(param_1 + 0xc0);
    *(undefined8 *)(param_1 + 0xc0) = uVar3;
    puVar4 = *(ulonglong **)(lVar2 + 0x2208);
    puVar5 = *(ulonglong **)(param_1 + 0x100);
    if (((puVar4[1] < puVar5[1]) || (*puVar4 < *puVar5)) || ((char)puVar4[2] == '\0')) {
      *(ulonglong **)(lVar2 + 0x2208) = puVar5;
      *(ulonglong **)(param_1 + 0x100) = puVar4;
    }
    else {
      FUN_1403506e0(*(longlong *)(lVar2 + 0x2200) + 0x458);
      FUN_1403506e0(*(longlong *)(param_1 + 0xc0) + 0x458);
    }
    plVar1 = (longlong *)(param_1 + 0x208);
    if (param_4 != plVar1) {
      FUN_14034bdc0(param_4);
      lVar2 = *param_4;
      *param_4 = *plVar1;
      *plVar1 = lVar2;
      lVar2 = param_4[1];
      param_4[1] = *(longlong *)(param_1 + 0x210);
      *(longlong *)(param_1 + 0x210) = lVar2;
      lVar2 = param_4[2];
      param_4[2] = *(longlong *)(param_1 + 0x218);
      *(longlong *)(param_1 + 0x218) = lVar2;
    }
    lVar2 = *(longlong *)(param_1 + 0x210);
    for (lVar14 = *plVar1; lVar14 != lVar2; lVar14 = lVar14 + 0x48) {
      FUN_140015c60(lVar14 + 0x30);
      FUN_140015e70(lVar14 + 0x18);
    }
    *(longlong *)(param_1 + 0x210) = *plVar1;
    bVar12 = true;
    LOCK();
    *(undefined1 *)(param_1 + 0x180) = 0;
    UNLOCK();
    _Mtx_unlock(param_1 + 0x580);
  }
  puVar6 = *(undefined8 **)(param_1 + 0x80);
  if ((puVar6 != (undefined8 *)0x0) && (bVar12)) {
    puVar7 = (undefined8 *)puVar6[0x440];
    *puVar6 = *puVar7;
    if (puVar6 + 1 != puVar7 + 1) {
      FUN_140364060(puVar6 + 1,puVar7[1],puVar7[2]);
    }
    lVar2 = puVar6[0x440];
    uVar9 = *(undefined4 *)(lVar2 + 0x24);
    uVar10 = *(undefined4 *)(lVar2 + 0x28);
    uVar11 = *(undefined4 *)(lVar2 + 0x2c);
    *(undefined4 *)(puVar6 + 4) = *(undefined4 *)(lVar2 + 0x20);
    *(undefined4 *)((longlong)puVar6 + 0x24) = uVar9;
    *(undefined4 *)(puVar6 + 5) = uVar10;
    *(undefined4 *)((longlong)puVar6 + 0x2c) = uVar11;
    iVar13 = FUN_1404775e0(puVar6 + 4);
    *(int *)(puVar6 + 0x41f) = iVar13;
    lVar2 = puVar6[0x440];
    lVar14 = (puVar6[5] * 10000) / 1000000 + puVar6[4];
    puVar6[0x421] = lVar14;
    puVar6[0x420] = lVar14 - iVar13;
    puVar6[0x422] = *(undefined8 *)(lVar2 + 0x30);
    puVar6[0x424] = *(undefined8 *)(lVar2 + 0x38);
    *(undefined4 *)(puVar6 + 0x423) = *(undefined4 *)(lVar2 + 0x40);
    puVar6[0x425] = *(undefined8 *)(lVar2 + 0x48);
    puVar6[0x42b] = *(undefined8 *)(lVar2 + 0x50);
    puVar6[0x42c] = *(undefined8 *)(lVar2 + 0x58);
    *(undefined4 *)(puVar6 + 0x42d) = *(undefined4 *)(lVar2 + 0x60);
    uVar3 = *(undefined8 *)(lVar2 + 0xc0);
    puVar6[0x42e] = *(undefined8 *)(lVar2 + 0xb8);
    puVar6[0x42f] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0xd0);
    puVar6[0x430] = *(undefined8 *)(lVar2 + 200);
    puVar6[0x431] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0xe0);
    puVar6[0x432] = *(undefined8 *)(lVar2 + 0xd8);
    puVar6[0x433] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0xf0);
    puVar6[0x434] = *(undefined8 *)(lVar2 + 0xe8);
    puVar6[0x435] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0x100);
    puVar6[0x436] = *(undefined8 *)(lVar2 + 0xf8);
    puVar6[0x437] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0x110);
    puVar6[0x438] = *(undefined8 *)(lVar2 + 0x108);
    puVar6[0x439] = uVar3;
    uVar3 = *(undefined8 *)(lVar2 + 0x120);
    puVar6[0x43a] = *(undefined8 *)(lVar2 + 0x118);
    puVar6[0x43b] = uVar3;
    uVar9 = *(undefined4 *)(lVar2 + 300);
    uVar10 = *(undefined4 *)(lVar2 + 0x130);
    uVar11 = *(undefined4 *)(lVar2 + 0x134);
    *(undefined4 *)(puVar6 + 0x43c) = *(undefined4 *)(lVar2 + 0x128);
    *(undefined4 *)((longlong)puVar6 + 0x21e4) = uVar9;
    *(undefined4 *)(puVar6 + 0x43d) = uVar10;
    *(undefined4 *)((longlong)puVar6 + 0x21ec) = uVar11;
    uVar9 = *(undefined4 *)(lVar2 + 0x13c);
    uVar10 = *(undefined4 *)(lVar2 + 0x140);
    uVar11 = *(undefined4 *)(lVar2 + 0x144);
    *(undefined4 *)(puVar6 + 0x43e) = *(undefined4 *)(lVar2 + 0x138);
    *(undefined4 *)((longlong)puVar6 + 0x21f4) = uVar9;
    *(undefined4 *)(puVar6 + 0x43f) = uVar10;
    *(undefined4 *)((longlong)puVar6 + 0x21fc) = uVar11;
    LOCK();
    *(undefined4 *)(puVar6 + 0x443) = *(undefined4 *)(lVar2 + 0x178);
    UNLOCK();
    LOCK();
    *(undefined4 *)((longlong)puVar6 + 0x221c) = *(undefined4 *)(puVar6[0x440] + 0x17c);
    UNLOCK();
    lVar2 = puVar6[0x440];
    *(undefined4 *)(puVar6 + 0x470) = *(undefined4 *)(lVar2 + 0x148);
    *(undefined4 *)((longlong)puVar6 + 0x2384) = *(undefined4 *)(lVar2 + 0x14c);
    *(undefined4 *)(puVar6 + 0x471) = *(undefined4 *)(lVar2 + 0x150);
    *(undefined4 *)((longlong)puVar6 + 0x238c) = *(undefined4 *)(lVar2 + 0x154);
    *(undefined4 *)(puVar6 + 0x472) = *(undefined4 *)(lVar2 + 0x158);
    *(undefined4 *)((longlong)puVar6 + 0x2394) = *(undefined4 *)(lVar2 + 0x15c);
    *(undefined4 *)(puVar6 + 0x473) = *(undefined4 *)(lVar2 + 0x160);
    *(undefined4 *)((longlong)puVar6 + 0x239c) = *(undefined4 *)(lVar2 + 0x164);
    *(undefined4 *)(puVar6 + 0x474) = *(undefined4 *)(lVar2 + 0x168);
    *(undefined4 *)((longlong)puVar6 + 0x23a4) = *(undefined4 *)(lVar2 + 0x16c);
    *(undefined4 *)(puVar6 + 0x475) = *(undefined4 *)(lVar2 + 0x170);
    *(undefined4 *)((longlong)puVar6 + 0x23ac) = *(undefined4 *)(lVar2 + 0x174);
    FUN_14044f7e0(puVar6 + 0x14,puVar6,param_2,param_3);
    FUN_1402322b0(puVar6 + 0x2cb,puVar6[0x440] + 0x68);
    FUN_1402322b0(puVar6 + 0x2cd,puVar6[0x440] + 0x78);
    FUN_1402322b0(puVar6 + 0x2cf,puVar6[0x440] + 0x90);
    FUN_1402322b0(puVar6 + 0x2d1,puVar6[0x440] + 0xa0);
  }
  return;
}


// Incoming references
// 0xc1ae04 DATA caller none
// 0x740321 UNCONDITIONAL_CALL caller 1407401f0
