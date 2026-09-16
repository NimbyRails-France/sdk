// Candidate VA 14043e9a0; RVA 0x43e9a0
// Ghidra inferred prototype: undefined FUN_14043e9a0()

longlong * FUN_14043e9a0(longlong *param_1,longlong param_2,longlong param_3)

{
  longlong lVar1;
  undefined4 uVar2;
  undefined4 uVar3;

  uVar3 = 0xffffffff;
  *param_1 = 0;
  *(undefined4 *)(param_1 + 1) = 0;
  *(undefined1 *)((longlong)param_1 + 0xc) = 1;
  param_1[2] = 0;
  param_1[3] = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)((longlong)param_1 + 0x24) = 0xffffffff;
  *(undefined1 *)(param_1 + 5) = 0;
  *(undefined8 *)((longlong)param_1 + 0x2c) = 0xffffffffffffffff;
  *(undefined8 *)((longlong)param_1 + 0x34) = 0xffffffffffffffff;
  *(undefined8 *)((longlong)param_1 + 0x3c) = 0;
  *(undefined1 *)((longlong)param_1 + 0x44) = 0;
  if (param_3 != 0) {
    uVar2 = 0xffffffff;
    if (*(char *)(param_3 + 0x5d0) != '\0') {
      lVar1 = *(longlong *)(param_3 + 0x5a8);
      param_1[1] = *(longlong *)(param_3 + 0x5a0);
      param_1[2] = lVar1;
      lVar1 = *(longlong *)(param_3 + 0x5b8);
      param_1[3] = *(longlong *)(param_3 + 0x5b0);
      param_1[4] = lVar1;
      uVar2 = *(undefined4 *)(param_3 + 0x5c8);
      *(undefined4 *)(param_1 + 6) = uVar2;
      if (*(char *)(param_3 + 0x5f0) != '\0') {
        uVar3 = *(undefined4 *)(param_3 + 0x5e8);
        *(undefined4 *)((longlong)param_1 + 0x2c) = uVar3;
      }
    }
    *param_1 = param_2;
    *(undefined4 *)((longlong)param_1 + 0x34) = uVar3;
    *(undefined4 *)(param_1 + 7) = uVar2;
    if (param_2 == 0) {
      *(undefined1 *)(param_1 + 5) = 1;
    }
    else if (*(longlong *)(param_2 + 0x50) == *(longlong *)(param_2 + 0x58)) {
      *(undefined1 *)(param_1 + 5) = 1;
      return param_1;
    }
  }
  return param_1;
}


// Incoming references
// 0x804dad UNCONDITIONAL_CALL caller 140804000
