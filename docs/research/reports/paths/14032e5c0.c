// Candidate VA 14032e5c0; RVA 0x32e5c0
// Ghidra inferred prototype: undefined FUN_14032e5c0()

void FUN_14032e5c0(undefined8 *param_1,longlong *param_2)

{
  bool bVar1;
  void *_Memory;
  undefined8 *puVar2;
  undefined8 *puVar3;
  ulonglong uVar4;
  ulonglong local_res8;
  undefined8 local_508;
  undefined1 local_500 [1256];
  
  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_JUTrack@model@nimby@@@serde@@YAXPEAV?$map@_JUTrack@model@nimby@@U?$less@_J@eastl@@Vallocator@5@@eastl@@PEAUDeserializer@0@@Z"
            );
  local_res8 = param_1[4];
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  uVar4 = 0;
  if (local_res8 != 0) {
    do {
      local_508 = 0;
      memset(local_500,0,0x4e8);
      FUN_1403361f0(local_500);
      (**(code **)(*param_2 + 0x18))(param_2,&local_508,8,0);
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Track");
      FUN_140310850(local_500,param_2);
      (**(code **)(*param_2 + 0x10))(param_2);
      _Memory = (void *)thunk_FUN_140983da8(0x510);
      *(undefined8 *)((longlong)_Memory + 0x20) = local_508;
      FUN_14033f9c0((longlong)_Memory + 0x28,local_500);
      bVar1 = true;
      puVar3 = param_1;
      if ((undefined8 *)param_1[2] != (undefined8 *)0x0) {
        puVar2 = (undefined8 *)param_1[2];
        do {
          puVar3 = puVar2;
          bVar1 = *(longlong *)((longlong)_Memory + 0x20) < (longlong)puVar3[4];
          if (*(longlong *)((longlong)_Memory + 0x20) < (longlong)puVar3[4]) {
            puVar2 = (undefined8 *)puVar3[1];
          }
          else {
            puVar2 = (undefined8 *)*puVar3;
          }
        } while (puVar2 != (undefined8 *)0x0);
      }
      puVar2 = puVar3;
      if (bVar1) {
        if (puVar3 != (undefined8 *)param_1[1]) {
          puVar2 = (undefined8 *)FUN_14001dcf0(puVar3);
          goto LAB_14032e6f4;
        }
LAB_14032e701:
        FUN_14001de10(_Memory,puVar3,param_1);
        param_1[4] = param_1[4] + 1;
      }
      else {
LAB_14032e6f4:
        if ((longlong)puVar2[4] < *(longlong *)((longlong)_Memory + 0x20)) goto LAB_14032e701;
        FUN_14032c8a0((longlong)_Memory + 0x28);
        free(_Memory);
      }
      FUN_14032c8a0(local_500);
      uVar4 = uVar4 + 1;
    } while (uVar4 < local_res8);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc19964 DATA caller none
// 0x323fa7 UNCONDITIONAL_CALL caller 140323f50
// 0x323fb3 UNCONDITIONAL_CALL caller 140323f50
