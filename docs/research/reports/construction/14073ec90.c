
ulonglong FUN_14073ec90(longlong param_1,int param_2)

{
  longlong lVar1;
  int *piVar2;
  
  lVar1 = *(longlong *)(param_1 + 0x250);
  piVar2 = *(int **)(*(longlong *)(lVar1 + 0x4f0) +
                    ((ulonglong)(longlong)param_2 % (ulonglong)*(uint *)(lVar1 + 0x4f8)) * 8);
  do {
    if (piVar2 == (int *)0x0) {
LAB_14073ecce:
      return (ulonglong)piVar2 & 0xffffffffffffff00;
    }
    if (param_2 == *piVar2) {
      if (piVar2 != *(int **)(*(longlong *)(lVar1 + 0x4f0) + *(longlong *)(lVar1 + 0x4f8) * 8)) {
        return CONCAT71((int7)((ulonglong)piVar2 >> 8),
                        *(ulonglong *)(piVar2 + 4) < *(ulonglong *)(piVar2 + 2));
      }
      goto LAB_14073ecce;
    }
    piVar2 = *(int **)(piVar2 + 6);
  } while( true );
}

