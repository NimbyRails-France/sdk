
void FUN_1402f3890(longlong param_1,longlong *param_2)

{
  longlong *local_res8;
  longlong local_18;
  longlong **local_10;
  
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::ModelChangeset");
  local_18 = param_1 + 0x20;
  local_10 = &local_res8;
  local_res8 = param_2;
  FUN_140323bb0(&local_18,0,0xc0,0x180,0x240,0x300,0x3c0,0x480,0x540);
                    /* WARNING: Could not recover jumptable at 0x0001402f395a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}

