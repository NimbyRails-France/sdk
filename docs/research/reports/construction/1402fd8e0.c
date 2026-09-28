
void FUN_1402fd8e0(longlong param_1,longlong *param_2)

{
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::Signal");
  FUN_14030fde0(param_1 + 0x20,param_2);
                    /* WARNING: Could not recover jumptable at 0x0001402fd95e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}

