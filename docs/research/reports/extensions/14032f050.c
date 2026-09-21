// Candidate VA 14032f050; RVA 0x32f050
// Ghidra inferred prototype: undefined FUN_14032f050()

void FUN_14032f050(undefined8 *param_1,longlong *param_2)

{
  bool bVar1;
  void *_Memory;
  undefined8 *puVar2;
  undefined8 *puVar3;
  ulonglong uVar4;
  ulonglong local_res8;
  undefined8 local_178;
  undefined1 local_170 [8];
  undefined1 local_168 [32];
  undefined1 local_148 [24];
  undefined1 local_130 [32];
  undefined1 local_110 [24];
  undefined1 local_f8 [40];
  undefined1 local_d0 [184];

  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_JUScript@model@nimby@@@serde@@YAXPEAV?$map@_JUScript@model@nimby@@U?$less@_J@eastl@@Vallocator@5@@eastl@@PEAUDeserializer@0@@Z"
            );
  local_res8 = param_1[4];
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
  uVar4 = 0;
  if (local_res8 != 0) {
    do {
      local_178 = 0;
      memset(local_170,0,0x150);
      FUN_140336970(local_170);
      (**(code **)(*param_2 + 0x18))(param_2,&local_178,8,0);
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Script");
      if (((0xbe < (int)param_2[1]) &&
          ((**(code **)(*param_2 + 0x18))(param_2,local_170,8,0), 0xbe < (int)param_2[1])) &&
         (FUN_1402f2290(local_168,param_2), 0xbe < (int)param_2[1])) {
        (**(code **)(*param_2 + 8))(param_2,"nimby::model::Tags");
        if (0x31 < (int)param_2[1]) {
          FUN_1403274d0(local_148,param_2);
        }
        (**(code **)(*param_2 + 0x10))(param_2);
        if (0xbe < (int)param_2[1]) {
          FUN_1402f2290(local_130,param_2);
        }
      }
      if ((0xc3 < (int)param_2[1]) &&
         ((**(code **)(*param_2 + 0x18))(param_2,local_110,8,1), 0xc3 < (int)param_2[1])) {
        FUN_140329aa0(local_f8,param_2);
      }
      if (0xd3 < (int)param_2[1]) {
        FUN_1402f2a90(local_d0,param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
      _Memory = (void *)thunk_FUN_140983da8(0x178);
      *(undefined8 *)((longlong)_Memory + 0x20) = local_178;
      FUN_140341140((longlong)_Memory + 0x28,local_170);
      bVar1 = true;
      puVar2 = param_1;
      if ((undefined8 *)param_1[2] != (undefined8 *)0x0) {
        puVar3 = (undefined8 *)param_1[2];
        do {
          puVar2 = puVar3;
          bVar1 = *(longlong *)((longlong)_Memory + 0x20) < (longlong)puVar2[4];
          if (*(longlong *)((longlong)_Memory + 0x20) < (longlong)puVar2[4]) {
            puVar3 = (undefined8 *)puVar2[1];
          }
          else {
            puVar3 = (undefined8 *)*puVar2;
          }
        } while (puVar3 != (undefined8 *)0x0);
      }
      puVar3 = puVar2;
      if (bVar1) {
        if (puVar2 != (undefined8 *)param_1[1]) {
          puVar2 = (undefined8 *)FUN_14001dcf0(puVar2);
          goto LAB_14032f254;
        }
LAB_14032f25e:
        FUN_14001de10(_Memory,puVar3,param_1);
        param_1[4] = param_1[4] + 1;
      }
      else {
LAB_14032f254:
        if ((longlong)puVar2[4] < *(longlong *)((longlong)_Memory + 0x20)) goto LAB_14032f25e;
        FUN_140334af0((longlong)_Memory + 0x28);
        free(_Memory);
      }
      FUN_140334af0(local_170);
      uVar4 = uVar4 + 1;
    } while (uVar4 < local_res8);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc199ac DATA caller none
// 0x324231 UNCONDITIONAL_CALL caller 140323f50
// 0x32423d UNCONDITIONAL_CALL caller 140323f50
