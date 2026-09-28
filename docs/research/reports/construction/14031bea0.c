
undefined8 * FUN_14031bea0(undefined8 *param_1)

{
  undefined8 *_Dst;
  
  _Dst = (undefined8 *)FUN_140983da8(0xe8);
  memset(_Dst,0,0xe8);
  _Dst[1] = 0;
  *_Dst = nimby::model::cmd::tn::CreateSignal::vftable;
  _Dst[2] = 0;
  _Dst[3] = 0;
  _Dst[4] = 0;
  *(undefined1 *)(_Dst + 5) = 1;
  _Dst[6] = 0;
  _Dst[7] = 0;
  _Dst[8] = 0;
  _Dst[9] = 0xf;
  *(undefined1 *)(_Dst + 6) = 0;
  *(undefined4 *)(_Dst + 10) = 0;
  _Dst[0xb] = 0;
  _Dst[0xc] = 0;
  _Dst[0xd] = 0;
  _Dst[0xf] = 0;
  *(undefined4 *)(_Dst + 0x10) = 0;
  *(undefined1 *)((longlong)_Dst + 0x84) = 0;
  *(undefined4 *)(_Dst + 0x12) = 0;
  _Dst[0x13] = 0;
  _Dst[0x14] = 0;
  _Dst[0x15] = 0;
  _Dst[0x16] = 0;
  _Dst[0x17] = 0;
  _Dst[0x18] = 0;
  *(undefined2 *)(_Dst + 0xe) = 0x101;
  *(undefined4 *)(_Dst + 0x11) = 0x708;
  *(undefined2 *)((longlong)_Dst + 0x8c) = 0x101;
  *param_1 = _Dst;
  return param_1;
}

