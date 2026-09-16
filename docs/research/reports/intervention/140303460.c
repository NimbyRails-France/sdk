
void FUN_140303460(longlong param_1,longlong *param_2)

{
  longlong local_res8;

  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  local_res8 = (longlong)*(int *)(param_1 + 0x20);
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
  FUN_1403196d0(param_1 + 0x28,param_2);
  return;
}


// ref 140c17270 caller none
// ref 140a6d650 caller none
// ref 140a6d000 caller none
