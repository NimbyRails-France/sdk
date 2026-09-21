// Candidate VA 14030f6a0; RVA 0x30f6a0
// Ghidra inferred prototype: undefined FUN_14030f6a0()

void FUN_14030f6a0(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong lVar4;
  longlong local_res8;
  longlong local_res10;
  longlong *local_res18;

  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1,8,0);
  }
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  }
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@V?$variant@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@std@@@serde@@YAXPEBV?$vector@V?$variant@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@std@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = *(longlong *)(param_1 + 0x18) - *(longlong *)(param_1 + 0x10) >> 4;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = *(longlong *)(param_1 + 0x18);
    plVar3 = param_2;
    plVar2 = local_res18;
    for (lVar4 = *(longlong *)(param_1 + 0x10); local_res18 = plVar3, lVar4 != lVar1;
        lVar4 = lVar4 + 0x10) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@serde@@YAXPEBV?$variant@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@std@@PEAUSerializer@0@@Z"
                );
      local_res10 = (longlong)*(char *)(lVar4 + 8);
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
      FUN_14033caa0((longlong)*(char *)(lVar4 + 8) + 1,&local_res18,lVar4);
      (**(code **)(*param_2 + 0x10))(param_2);
      plVar3 = local_res18;
      plVar2 = local_res18;
    }
    local_res18 = plVar2;
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0xcb < (int)param_2[1]) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@UMeta@ScriptStructInstance@model@nimby@@@serde@@YAXPEBV?$vector@UMeta@ScriptStructInstance@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
                );
      local_res8 = *(longlong *)(param_1 + 0x30) - *(longlong *)(param_1 + 0x28) >> 4;
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
      lVar1 = *(longlong *)(param_1 + 0x30);
      for (lVar4 = *(longlong *)(param_1 + 0x28); lVar4 != lVar1; lVar4 = lVar4 + 0x10) {
        (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptStructInstance::Meta");
        if ((0xcb < (int)param_2[1]) &&
           ((**(code **)(*param_2 + 0x18))(param_2,lVar4,8,1), 0xcb < (int)param_2[1])) {
          (**(code **)(*param_2 + 0x18))(param_2,lVar4 + 8,8,1);
        }
        (**(code **)(*param_2 + 0x10))(param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0xcb < (int)param_2[1]) {
                    /* WARNING: Could not recover jumptable at 0x00014030f8a4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x40,8,1);
    return;
  }
  return;
}


// Incoming references
// 0xc17c54 DATA caller none
// 0xaf3f48 DATA caller none
// 0xaf3f58 DATA caller none
// 0x2f2a56 UNCONDITIONAL_CALL caller 1402f29b0
// 0x30d7b5 UNCONDITIONAL_CALL caller 14030d720
