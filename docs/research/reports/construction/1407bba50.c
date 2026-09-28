
longlong FUN_1407bba50(longlong param_1)

{
  if ((param_1 != 0) && (*(char *)(param_1 + 0x1f0) == '\x01')) {
    return param_1;
  }
  return 0;
}

