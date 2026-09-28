
void FUN_1402fec10(longlong param_1,longlong *param_2)

{
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,1);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,1);
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::TNClip");
  FUN_140323440(param_1 + 0x20,param_2);
  FUN_140323520(param_1 + 0x38,param_2);
  FUN_14031ae30(param_1 + 0x50,param_2);
  (**(code **)(*param_2 + 8))(param_2,"linalg::aliases::double2");
  (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x80);
  (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x88);
  (**(code **)(*param_2 + 0x10))(param_2);
  (**(code **)(*param_2 + 0x10))(param_2);
  (**(code **)(*param_2 + 8))(param_2,"linalg::aliases::double2");
  (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x90);
  (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x98);
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING: Could not recover jumptable at 0x0001402fed47. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xa0,8,0);
  return;
}

