// Candidate VA 14043e130; RVA 0x43e130
// Ghidra inferred prototype: undefined FUN_14043e130()

undefined8 FUN_14043e130(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;

  if ((*(char *)(param_1 + 0x4b0) != '\0') && (*(char *)(param_1 + 0x490) != '\0')) {
    return 5;
  }
  if (*(char *)(param_1 + 0xb0) != '\0') {
    iVar1 = *(int *)(param_1 + 0xa8);
    if (iVar1 == 0) {
      return 3;
    }
    if (iVar1 == 1) {
      return 4;
    }
    if (iVar1 == 3) {
      return 1;
    }
    if (iVar1 == 4) {
      return 7;
    }
  }
  if ((*(char *)(param_1 + 0x4b0) != '\0') && (*(longlong *)(param_1 + 0x458) != 0)) {
    return 6;
  }
  if ((*(char *)(param_1 + 0x1d0) == '\0') &&
     ((*(char *)(param_1 + 0x598) != '\0' && (*(int *)(param_1 + 0x568) != 0)))) {
    if (*(int *)(param_1 + 0x574) != 0) {
      return 8;
    }
    if (*(int *)(param_1 + 0x570) != 0) {
      return 10;
    }
    uVar2 = 0;
    if (*(int *)(param_1 + 0x56c) == 0) {
      uVar2 = 9;
    }
    return uVar2;
  }
  return 0;
}


// Incoming references
// 0x44cd6b UNCONDITIONAL_CALL caller 14044cc70
// 0x598daa UNCONDITIONAL_CALL caller 140598a30
// 0x599523 UNCONDITIONAL_CALL caller 140599130
// 0x59f21a UNCONDITIONAL_CALL caller 14059efc0
// 0x636654 UNCONDITIONAL_CALL caller 140633f80
// 0x6810f5 UNCONDITIONAL_CALL caller 140680c70
// 0x8040c7 UNCONDITIONAL_CALL caller 140804000
