// Candidate VA 14030d720; RVA 0x30d720
// Ghidra inferred prototype: undefined FUN_14030d720()

void FUN_14030d720(longlong param_1,longlong *param_2)

{
  longlong local_res8;

  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x20,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x28,8,0);
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptStructInstance");
  FUN_14030f6a0(param_1 + 0x30,param_2);
  (**(code **)(*param_2 + 0x10))(param_2);
  local_res8 = (longlong)*(int *)(param_1 + 0x80);
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
  return;
}


// Incoming references
// 0xc17bac DATA caller none
// 0xa6d9d0 DATA caller none
