// Candidate VA 14049b1d0; RVA 0x49b1d0
// Ghidra inferred prototype: undefined FUN_14049b1d0()

void FUN_14049b1d0(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  ulonglong local_res8;

  (**(code **)(*param_2 + 8))(param_2,"nimby::model::Versioning");
  if ((0x6f < (int)param_2[1]) && (FUN_1404bc750(param_1,param_2), 0x6f < (int)param_2[1])) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@V?$array@E$0CA@@std@@@serde@@YAXPEAV?$vector@V?$array@E$0CA@@std@@V?$allocator@V?$array@E$0CA@@std@@@2@@std@@PEAUDeserializer@0@@Z"
              );
    local_res8 = *(longlong *)(param_1 + 0x28) - *(longlong *)(param_1 + 0x20) >> 5;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = *(longlong *)(param_1 + 0x20);
    uVar3 = *(longlong *)(param_1 + 0x28) - lVar1 >> 5;
    if (uVar3 != local_res8) {
      if (local_res8 < uVar3) {
        *(ulonglong *)(param_1 + 0x28) = local_res8 * 0x20 + lVar1;
      }
      else if (uVar3 < local_res8) {
        if ((ulonglong)(*(longlong *)(param_1 + 0x30) - lVar1 >> 5) < local_res8) {
          FUN_1404cc240(param_1 + 0x20);
        }
        else {
          uVar2 = FUN_1404cc390(*(longlong *)(param_1 + 0x28),local_res8 - uVar3);
          *(undefined8 *)(param_1 + 0x28) = uVar2;
        }
      }
    }
    lVar1 = *(longlong *)(param_1 + 0x28);
    lVar5 = *(longlong *)(param_1 + 0x20);
    lVar6 = lVar5;
    for (; lVar5 != lVar1; lVar5 = lVar5 + 0x20) {
      lVar6 = lVar6 + 0x20;
      (**(code **)(*param_2 + 8))
                (param_2,"??$visit@E$0CA@@serde@@YAXPEAV?$array@E$0CA@@std@@PEAUDeserializer@0@@Z");
      for (lVar4 = lVar5; lVar4 != lVar6; lVar4 = lVar4 + 1) {
        (**(code **)(*param_2 + 0x18))(param_2,lVar4,1,1);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
                    /* WARNING: Could not recover jumptable at 0x00014049b337. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc26654 DATA caller none
// 0xb0dddc DATA caller none
// 0xb0de14 DATA caller none
// 0x4b6e34 UNCONDITIONAL_CALL caller 1404b6700
