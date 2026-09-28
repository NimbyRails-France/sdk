
undefined8 * FUN_1402f3a40(undefined8 *param_1)

{
  undefined8 *_Dst;
  
  _Dst = (undefined8 *)FUN_140983da8(0x620);
  memset(_Dst,0,0x620);
  _Dst[1] = 0;
  *_Dst = nimby::model::cmd::tn::Undo::vftable;
  _Dst[2] = 0;
  _Dst[3] = 0;
  FUN_140319110(_Dst + 4);
  *param_1 = _Dst;
  return param_1;
}

