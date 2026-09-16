
undefined8 * FUN_1402532c0(undefined8 *param_1,undefined8 *param_2)

{
  int *piVar1;

  *param_1 = 0;
  param_1[1] = 0;
  if (param_2[1] != 0) {
    LOCK();
    piVar1 = (int *)(param_2[1] + 8);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  return param_1;
}


// ref 1404f3a3f caller 1404f3520
// ref 1404f3baa caller 1404f3520
// ref 1404f3e96 caller 1404f3520
// ref 14062ff78 caller 14062fa20
// ref 14062ff7f caller 14062fa20
// ref 1406467e8 caller 1406467c0
// ref 1406467ef caller 1406467c0
