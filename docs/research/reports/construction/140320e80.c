
/* Library Function - Single Match
    public: void * __ptr64 __cdecl MRECmpImpl::`scalar deleting destructor'(unsigned int) __ptr64
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void * __thiscall MRECmpImpl::_scalar_deleting_destructor_(MRECmpImpl *this,uint param_1)

{
  FUN_140320ec0(this + 0x20);
  if ((param_1 & 1) != 0) {
    free(this);
  }
  return this;
}

