// Candidate VA 1404415c0; RVA 0x4415c0
// Ghidra inferred prototype: undefined FUN_1404415c0()

void FUN_1404415c0(undefined8 param_1,longlong param_2,undefined8 *param_3)

{
  undefined8 uVar1;
  code *pcVar2;
  int iVar3;
  undefined8 *puVar4;
  undefined8 *local_res10;

  if ((param_2 != 0) && (param_3 != (undefined8 *)0x0)) {
    uVar1 = *param_3;
    puVar4 = (undefined8 *)FUN_140983da8(0x10);
    *puVar4 = nimby::model::SimCmd::TrainIntervention::vftable;
    puVar4[1] = uVar1;
    iVar3 = _Mtx_lock(param_2 + 0x58);
    if (iVar3 != 0) {
      std::_Throw_Cpp_error(5);
      pcVar2 = (code *)swi(3);
      (*pcVar2)();
      return;
    }
    if (*(int *)(param_2 + 0xa4) == 0x7fffffff) {
      *(undefined4 *)(param_2 + 0xa4) = 0x7ffffffe;
      std::_Throw_Cpp_error(6);
      pcVar2 = (code *)swi(3);
      (*pcVar2)();
      return;
    }
    local_res10 = puVar4;
    FUN_140461330(param_2,&local_res10);
    if (local_res10 != (undefined8 *)0x0) {
      (**(code **)*local_res10)(local_res10,1);
    }
    _Mtx_unlock(param_2 + 0x58);
  }
  return;
}


// Incoming references
// 0xc230f0 DATA caller none
// 0xb0754c DATA caller none
// 0xb0755c DATA caller none
// 0x4534da DATA caller 140452f40
