
void FUN_140456a10(longlong param_1,undefined8 *param_2)

{
  code *pcVar1;
  int iVar2;
  undefined8 *local_res8;

  iVar2 = _Mtx_lock(param_1 + 0x58);
  if (iVar2 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar1 = (code *)swi(3);
    (*pcVar1)();
    return;
  }
  if (*(int *)(param_1 + 0xa4) == 0x7fffffff) {
    *(undefined4 *)(param_1 + 0xa4) = 0x7ffffffe;
    std::_Throw_Cpp_error(6);
  }
  local_res8 = (undefined8 *)*param_2;
  *param_2 = 0;
  FUN_140461330(param_1,&local_res8);
  if (local_res8 != (undefined8 *)0x0) {
    (**(code **)*local_res8)(local_res8,1);
  }
                    /* WARNING: Could not recover jumptable at 0x000140456aa7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  _Mtx_unlock(param_1 + 0x58);
  return;
}


// ref 140c23a38 caller none
// ref 140441bd0 caller 140441b20
// ref 140441c56 caller 140441c00
