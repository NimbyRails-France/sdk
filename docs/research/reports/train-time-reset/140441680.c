// Candidate VA 140441680; RVA 0x441680
// Ghidra inferred prototype: undefined FUN_140441680()

void FUN_140441680(undefined8 param_1,longlong param_2,undefined8 *param_3,undefined4 *param_4)

{
  undefined8 uVar1;
  code *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined8 *puVar7;
  undefined8 *local_res10;

  if (((param_2 != 0) && (param_3 != (undefined8 *)0x0)) && (param_4 != (undefined4 *)0x0)) {
    uVar1 = *param_3;
    puVar7 = (undefined8 *)FUN_140983da8();
    *(undefined4 *)((longlong)puVar7 + 0x22) = 0;
    *(undefined2 *)((longlong)puVar7 + 0x26) = 0;
    puVar7[2] = 0;
    puVar7[3] = 0;
    *puVar7 = nimby::model::SimCmd::TrainWarp::vftable;
    *(undefined2 *)(puVar7 + 4) = 0x101;
    puVar7[1] = uVar1;
    uVar3 = param_4[1];
    uVar4 = param_4[2];
    uVar5 = param_4[3];
    uVar1 = *(undefined8 *)(param_4 + 4);
    *(undefined4 *)(puVar7 + 2) = *param_4;
    *(undefined4 *)((longlong)puVar7 + 0x14) = uVar3;
    *(undefined4 *)(puVar7 + 3) = uVar4;
    *(undefined4 *)((longlong)puVar7 + 0x1c) = uVar5;
    puVar7[4] = uVar1;
    iVar6 = _Mtx_lock(param_2 + 0x58);
    if (iVar6 != 0) {
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
    local_res10 = puVar7;
    FUN_140461330(param_2,&local_res10);
    if (local_res10 != (undefined8 *)0x0) {
      (**(code **)*local_res10)(local_res10,1);
    }
    _Mtx_unlock(param_2 + 0x58);
  }
  return;
}


// Incoming references
// 0xc23114 DATA caller none
// 0xb07580 DATA caller none
// 0xb07590 DATA caller none
// 0x453602 DATA caller 140452f40
