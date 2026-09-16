
void FUN_140461330(longlong *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  size_t _Size;
  undefined8 *puVar2;
  undefined8 uVar3;
  void *_Src;
  ulonglong uVar4;
  longlong lVar5;
  undefined8 uVar6;
  ulonglong uVar7;
  longlong lVar8;
  longlong *_Dst;
  longlong lVar9;

  puVar2 = (undefined8 *)param_1[6];
  puVar1 = puVar2 + 1;
  if (puVar1 == (undefined8 *)param_1[8]) {
    uVar3 = *param_2;
    *param_2 = 0;
    lVar9 = *param_1;
    lVar5 = param_1[1];
    if (lVar5 <= (param_1[9] - lVar9 >> 3) + 1) {
      _Src = (void *)param_1[5];
      uVar7 = (longlong)_Src - lVar9 >> 3;
      lVar8 = param_1[9] - (longlong)_Src >> 3;
      _Size = lVar8 * 8 + 8;
      if (uVar7 == 0) {
        lVar9 = lVar5;
        if (lVar5 == 0) {
          lVar9 = 1;
        }
        lVar9 = lVar9 + lVar5 + 2;
        lVar5 = thunk_FUN_140983da8(lVar9 * 8);
        _Dst = (longlong *)(lVar5 + (param_1[5] - *param_1 >> 3) * 8);
        if (*param_1 != 0) {
          memcpy(_Dst,(void *)param_1[5],_Size);
        }
        if ((void *)*param_1 != (void *)0x0) {
          free((void *)*param_1);
        }
        *param_1 = lVar5;
        param_1[1] = lVar9;
      }
      else {
        uVar4 = uVar7 >> 1;
        if (uVar4 < 2) {
          uVar4 = 1;
        }
        _Dst = (longlong *)(lVar9 + (uVar7 - uVar4) * 8);
        memmove(_Dst,_Src,_Size);
      }
      param_1[5] = (longlong)_Dst;
      lVar9 = *_Dst;
      param_1[3] = lVar9;
      param_1[4] = lVar9 + 0x100;
      param_1[9] = (longlong)(_Dst + lVar8);
      lVar9 = _Dst[lVar8];
      param_1[7] = lVar9;
      param_1[8] = lVar9 + 0x100;
    }
    uVar6 = thunk_FUN_140983da8(0x100);
    *(undefined8 *)(param_1[9] + 8) = uVar6;
    *(undefined8 *)param_1[6] = uVar3;
    lVar9 = param_1[9];
    param_1[9] = lVar9 + 8;
    lVar9 = *(longlong *)(lVar9 + 8);
    param_1[7] = lVar9;
    param_1[8] = lVar9 + 0x100;
    param_1[6] = param_1[7];
  }
  else {
    param_1[6] = (longlong)puVar1;
    uVar3 = *param_2;
    *param_2 = 0;
    *puVar2 = uVar3;
  }
  return;
}


// ref 140c2417c caller none
// ref 140441582 caller 1404414a0
// ref 14044164a caller 1404415c0
// ref 140441744 caller 140441680
// ref 140441844 caller 140441780
// ref 14044191f caller 140441880
// ref 1404419fc caller 140441960
// ref 140441ae3 caller 140441a40
// ref 140456a7a caller 140456a10
// ref 140441d2d caller 140441c80
// ref 140441e66 caller 140441d60
// ref 140441f4e caller 140441ea0
// ref 140442130 caller 140442010
