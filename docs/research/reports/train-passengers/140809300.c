// Candidate VA 140809300; RVA 0x809300
// Ghidra inferred prototype: undefined FUN_140809300()

void FUN_140809300(undefined8 *param_1,longlong *param_2)

{
  undefined8 *puVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  char *pcVar5;
  char *pcVar6;
  longlong lVar7;
  bool bVar8;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 local_80;
  undefined1 local_78 [8];
  undefined1 local_70 [32];
  undefined1 local_50 [72];

  if (*(float *)*param_1 != 0.0) {
    *(float *)((longlong)param_2 + 0x2c) = *(float *)*param_1;
    *(undefined1 *)(param_2 + 5) = 1;
  }
  if (*(float *)param_1[1] != 0.0) {
    *(float *)((longlong)param_2 + 0x34) = *(float *)param_1[1];
    *(undefined1 *)(param_2 + 6) = 1;
  }
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 3;
  FUN_14055b8e0(param_2,param_1[2]);
  (**(code **)(*param_2 + 8))(param_2);
  puVar1 = (undefined8 *)param_1[3];
  pcVar6 = "Purchase preconfigured train";
  lVar2 = puVar1[1];
  bVar8 = *(longlong *)(lVar2 + 0x5f8) != 0;
  if (bVar8) {
    pcVar6 = "Replacing with preconfigured train";
  }
  pcVar5 = "traineditor_buy_tmu_mode";
  if (bVar8) {
    pcVar5 = "traineditor_replace_tmu_mode";
  }
  lVar3 = FUN_1402d82e0(pcVar5,pcVar6);
  local_98 = 0;
  uStack_90 = 0;
  local_88 = 0;
  lVar7 = -1;
  local_80 = 0;
  do {
    lVar7 = lVar7 + 1;
  } while (*(char *)(lVar3 + lVar7) != '\0');
  FUN_140002c00(&local_98,lVar3);
  uVar4 = FUN_1407f3600(lVar2,local_78,puVar1[2],puVar1[3],puVar1[4],param_2,puVar1[5],puVar1[6],
                        puVar1[7],puVar1[8],puVar1[9],puVar1[10],puVar1[1] + 0x608,puVar1[0xb],
                        puVar1[0xc],1,&local_98);
  FUN_1407fab20(*puVar1,uVar4);
  FUN_140002d30(local_50);
  FUN_140002d30(local_70);
                    /* WARNING: Could not recover jumptable at 0x0001408094b0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}


// Incoming references
// 0xc3afd0 DATA caller none
// 0x811302 UNCONDITIONAL_CALL caller 140810fd0
// 0x8118e7 UNCONDITIONAL_CALL caller 140810fd0
