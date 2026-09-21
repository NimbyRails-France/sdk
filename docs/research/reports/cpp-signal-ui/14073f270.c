// Candidate VA 14073f270; RVA 0x73f270
// Ghidra inferred prototype: undefined FUN_14073f270()

void FUN_14073f270(longlong param_1,undefined8 param_2)

{
  undefined1 local_68 [32];
  undefined **local_48;
  undefined1 local_40 [48];
  undefined ***local_10;

  FUN_140247b90(local_68);
  local_10 = (undefined ***)0x0;
  local_48 = std::
             _Func_impl_no_alloc<`public:_void___cdecl_nimby::shell::UITxn::save_local_game(std::filesystem::path)___ptr64'::`2'::<lambda_1>,void,nimby::shell::UIState&___ptr64>
             ::vftable;
  FUN_140247b90(local_40,local_68);
  local_10 = &local_48;
  if (*(ulonglong *)(param_1 + 0x170) < *(ulonglong *)(param_1 + 0x178)) {
    *(ulonglong *)(param_1 + 0x170) = *(ulonglong *)(param_1 + 0x170) + 0x40;
    FUN_140004b80();
  }
  else {
    FUN_14074c320(param_1 + 0x168,&local_48);
  }
  if (local_10 != (undefined ***)0x0) {
    (*(code *)(*local_10)[4])(local_10,local_10 != &local_48);
  }
  FUN_140025470(local_68);
  *(undefined1 *)(param_1 + 0x198) = 1;
  FUN_140025470(param_2);
  return;
}


// Incoming references
// 0xc3713c DATA caller none
// 0x5e593f UNCONDITIONAL_CALL caller 1405e4f40
// 0x5e5a97 UNCONDITIONAL_CALL caller 1405e4f40
