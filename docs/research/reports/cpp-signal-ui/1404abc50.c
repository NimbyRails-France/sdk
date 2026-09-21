// Candidate VA 1404abc50; RVA 0x4abc50
// Ghidra inferred prototype: undefined FUN_1404abc50()

undefined8
FUN_1404abc50(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,
             undefined4 *param_5)

{
  longlong *plVar1;
  undefined1 local_e90 [40];
  undefined1 local_e68 [848];
  undefined4 local_b18;
  undefined4 uStack_b14;
  undefined4 uStack_b10;
  undefined4 uStack_b0c;
  undefined4 local_b08;
  undefined4 uStack_b04;
  undefined4 uStack_b00;
  undefined4 uStack_afc;
  undefined1 local_af8 [2800];

  FUN_1404aafd0(local_e68);
  local_b18 = *param_5;
  uStack_b14 = param_5[1];
  uStack_b10 = param_5[2];
  uStack_b0c = param_5[3];
  local_b08 = param_5[4];
  uStack_b04 = param_5[5];
  uStack_b00 = param_5[6];
  uStack_afc = param_5[7];
  plVar1 = (longlong *)(param_5 + 8);
  if ((longlong *)local_af8 != plVar1) {
    if (0xf < *(ulonglong *)(param_5 + 0xe)) {
      plVar1 = (longlong *)*plVar1;
    }
    FUN_140030630(local_af8,plVar1,*(undefined8 *)(param_5 + 0xc));
  }
  FUN_140247b90(local_e90,param_4);
  FUN_14049fbd0(param_1,local_e90,local_e68);
  FUN_1404aa510(local_e68);
  FUN_140025470(param_4);
  return param_1;
}


// Incoming references
// 0xc26990 DATA caller none
// 0x74e1e9 UNCONDITIONAL_CALL caller 14074e180
