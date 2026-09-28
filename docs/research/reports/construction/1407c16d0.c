
void FUN_1407c16d0(longlong param_1,longlong *param_2)

{
  *(longlong *)(*param_2 + 8) = param_2[1];
  *(longlong *)param_2[1] = *param_2;
  FUN_140320ec0(param_2 + 2);
  free(param_2);
  *(longlong *)(param_1 + 0x10) = *(longlong *)(param_1 + 0x10) + -1;
  return;
}

