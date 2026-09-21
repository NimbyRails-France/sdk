// Candidate VA 14055ba40; RVA 0x55ba40
// Ghidra inferred prototype: undefined FUN_14055ba40()

void FUN_14055ba40(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  void *_Memory;
  void *pvVar4;
  void *pvVar5;
  longlong *plVar6;
  void *pvVar7;
  undefined8 auStack_40 [3];
  longlong local_28 [4];

  puVar1 = (undefined8 *)(param_1 + 0x60);
  pvVar4 = (void *)0x0;
  *(undefined1 *)(param_1 + 0x18) = 0;
  *(undefined1 *)(param_1 + 0x20) = 0;
  *(undefined1 *)(param_1 + 0x28) = 0;
  *(undefined1 *)(param_1 + 0x30) = 0;
  *(undefined1 *)(param_1 + 0x38) = 0;
  *(undefined1 *)(param_1 + 0x4c) = 0;
  *(undefined1 *)(param_1 + 0x54) = 0;
  auStack_40[0] = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x55) = 0;
  *(undefined4 *)(param_1 + 0x5a) = 0;
  *(undefined4 *)(param_1 + 0x24) = 0;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x34) = 0;
  *(undefined8 *)(param_1 + 0x3c) = 0;
  *(undefined8 *)(param_1 + 0x44) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  *(undefined1 *)(param_1 + 0x59) = 0;
  pvVar5 = pvVar4;
  if (puVar1 != auStack_40) {
    uVar2 = *puVar1;
    *puVar1 = 0;
    uVar3 = *(undefined8 *)(param_1 + 0x70);
    *(undefined8 *)(param_1 + 0x70) = 0;
    pvVar5 = (void *)*puVar1;
    *puVar1 = uVar2;
    *(undefined8 *)(param_1 + 0x68) = uVar2;
    *(undefined8 *)(param_1 + 0x70) = uVar3;
    if (pvVar5 != (void *)0x0) {
      free(pvVar5);
    }
    pvVar5 = (void *)*puVar1;
    *puVar1 = 0;
    *(undefined8 *)(param_1 + 0x68) = 0;
    *(undefined8 *)(param_1 + 0x70) = 0;
  }
  plVar6 = (longlong *)(param_1 + 0x78);
  pvVar7 = pvVar4;
  _Memory = pvVar4;
  if (plVar6 != local_28) {
    FUN_140567e70(plVar6);
    pvVar4 = (void *)*plVar6;
    *plVar6 = 0;
    pvVar7 = *(void **)(param_1 + 0x80);
    *(undefined8 *)(param_1 + 0x80) = 0;
    *(undefined8 *)(param_1 + 0x88) = 0;
    _Memory = pvVar4;
  }
  for (; pvVar4 != pvVar7; pvVar4 = (void *)((longlong)pvVar4 + 0x78)) {
    FUN_14055b8a0(pvVar4);
  }
  if (_Memory != (void *)0x0) {
    free(_Memory);
  }
  if (pvVar5 != (void *)0x0) {
    free(pvVar5);
  }
  return;
}


// Incoming references
// 0xc2d4e8 DATA caller none
// 0x55c654 UNCONDITIONAL_CALL caller 14055c4b0
// 0x55d3dc UNCONDITIONAL_CALL caller 14055d320
// 0x55e116 UNCONDITIONAL_CALL caller 14055df00
// 0x55e4f6 UNCONDITIONAL_CALL caller 14055e1b0
// 0x55efd8 UNCONDITIONAL_CALL caller 14055ef00
// 0x55f2fb UNCONDITIONAL_CALL caller 14055f0b0
// 0x55f53a UNCONDITIONAL_CALL caller 14055f340
// 0x56045d UNCONDITIONAL_CALL caller 14055fe90
// 0x560a4a UNCONDITIONAL_CALL caller 140560940
// 0x560cbe UNCONDITIONAL_CALL caller 140560af0
// 0x5614d5 UNCONDITIONAL_CALL caller 140561190
// 0x561ce5 UNCONDITIONAL_CALL caller 1405618e0
// 0x562413 UNCONDITIONAL_CALL caller 140562290
// 0x562b09 UNCONDITIONAL_CALL caller 140562a10
// 0x55f098 UNCONDITIONAL_CALL caller 14055f010
// 0x55f639 UNCONDITIONAL_CALL caller 14055f570
// 0x55f6f8 UNCONDITIONAL_CALL caller 14055f640
// 0x55f9b2 UNCONDITIONAL_CALL caller 14055f930
// 0x55fa96 UNCONDITIONAL_CALL caller 14055f9c0
// 0x560869 UNCONDITIONAL_CALL caller 1405607c0
// 0x56092e UNCONDITIONAL_CALL caller 140560870
// 0x562d2d UNCONDITIONAL_CALL caller 140562b30
