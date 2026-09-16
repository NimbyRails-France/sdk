
void * FUN_1403223c0(void *param_1,uint param_2)

{
  undefined8 *puVar1;
  undefined8 *_Memory;

  _Memory = *(undefined8 **)((longlong)param_1 + 0x38);
  while (_Memory != (undefined8 *)0x0) {
    FUN_1402450d0((longlong)param_1 + 0x28,*_Memory);
    puVar1 = (undefined8 *)_Memory[1];
    free(_Memory);
    _Memory = puVar1;
  }
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


// ref 140c188a8 caller none
// ref 140a6d640 caller none
// ref 140a6cff0 caller none
