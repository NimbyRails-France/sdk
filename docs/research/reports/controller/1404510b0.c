
void FUN_1404510b0(longlong param_1)

{
  void *_Memory;
  code *pcVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;

  lVar5 = param_1 + 0x880;
  lVar7 = 0;
  for (lVar3 = param_1; lVar3 != lVar5; lVar3 = lVar3 + 0x88) {
    iVar2 = _Mtx_lock(lVar3);
    if (iVar2 != 0) goto LAB_1404512c0;
    if (*(int *)(lVar3 + 0x4c) == 0x7fffffff) {
      *(undefined4 *)(lVar3 + 0x4c) = 0x7ffffffe;
      goto LAB_1404512b4;
    }
    if (*(longlong *)(lVar3 + 0x60) != 0) {
      if (*(ulonglong *)(lVar3 + 0x68) < 0x80) {
        lVar4 = lVar7;
        lVar6 = lVar7;
        if (*(ulonglong *)(lVar3 + 0x68) != 0) {
          do {
            if (-1 < *(char *)(lVar6 + *(longlong *)(lVar3 + 0x50))) {
              FUN_1402ec870(*(longlong *)(lVar3 + 0x58) + 8 + lVar4);
            }
            lVar6 = lVar6 + 1;
            lVar4 = lVar4 + 0x60;
          } while (lVar6 != *(longlong *)(lVar3 + 0x68));
          lVar4 = *(longlong *)(lVar3 + 0x68);
          *(undefined8 *)(lVar3 + 0x60) = 0;
          memset(*(void **)(lVar3 + 0x50),-0x80,lVar4 + 0x10);
          *(undefined1 *)(lVar4 + *(longlong *)(lVar3 + 0x50)) = 0xff;
          *(ulonglong *)(lVar3 + 0x80) =
               (*(ulonglong *)(lVar3 + 0x68) - (*(ulonglong *)(lVar3 + 0x68) >> 3)) -
               *(longlong *)(lVar3 + 0x60);
        }
      }
      else {
        FUN_140353990(lVar3 + 0x50);
      }
    }
    _Mtx_unlock(lVar3);
  }
  do {
    if (lVar5 == param_1 + 0x1100) {
      return;
    }
    iVar2 = _Mtx_lock(lVar5);
    if (iVar2 != 0) {
LAB_1404512c0:
      std::_Throw_Cpp_error(5);
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
    if (*(int *)(lVar5 + 0x4c) == 0x7fffffff) {
      *(undefined4 *)(lVar5 + 0x4c) = 0x7ffffffe;
LAB_1404512b4:
      std::_Throw_Cpp_error(6);
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
    if (*(longlong *)(lVar5 + 0x60) != 0) {
      if (*(ulonglong *)(lVar5 + 0x68) < 0x80) {
        lVar3 = lVar7;
        lVar4 = lVar7;
        if (*(ulonglong *)(lVar5 + 0x68) != 0) {
          do {
            if ((-1 < *(char *)(lVar3 + *(longlong *)(lVar5 + 0x50))) &&
               (_Memory = *(void **)(*(longlong *)(lVar5 + 0x58) + 8 + lVar4),
               _Memory != (void *)0x0)) {
              free(_Memory);
            }
            lVar3 = lVar3 + 1;
            lVar4 = lVar4 + 0x20;
          } while (lVar3 != *(longlong *)(lVar5 + 0x68));
          lVar3 = *(longlong *)(lVar5 + 0x68);
          *(undefined8 *)(lVar5 + 0x60) = 0;
          memset(*(void **)(lVar5 + 0x50),-0x80,lVar3 + 0x10);
          *(undefined1 *)(lVar3 + *(longlong *)(lVar5 + 0x50)) = 0xff;
          *(ulonglong *)(lVar5 + 0x80) =
               (*(ulonglong *)(lVar5 + 0x68) - (*(ulonglong *)(lVar5 + 0x68) >> 3)) -
               *(longlong *)(lVar5 + 0x60);
        }
      }
      else {
        FUN_1403538b0(lVar5 + 0x50);
      }
    }
    _Mtx_unlock(lVar5);
    lVar5 = lVar5 + 0x88;
  } while( true );
}
