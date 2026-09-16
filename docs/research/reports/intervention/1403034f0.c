
void FUN_1403034f0(longlong param_1,longlong *param_2)

{
  undefined8 local_res8;

  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  local_res8 = 0;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
  *(undefined4 *)(param_1 + 0x20) = (undefined4)local_res8;
  FUN_1403197b0(param_1 + 0x28,param_2);
  return;
}


// ref 140c1727c caller none
// ref 140a6d658 caller none
// ref 140a6d008 caller none
