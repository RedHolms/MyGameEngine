;
; RTTI requires 'const type_info::`vftable`' symbol defined in vcruntime.dll.
; We CAN define it ourself but it requires MSVC (Clang doesn't define it for some reason) and I
; fucking HATE MSVC.
;

extern ??1type_info@@UEAA@XZ ; type_info::~type_info(void)
extern __imp_HeapFree ; BOOL (__stdcall *HeapFree)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)

extern ?gAppHeap@Detail@Memory@@3PEAXEA ; HANDLE Memory::Detail::gAppHeap

section .text

global ??_Gtype_info@@UEAAPEAXI@Z
; void *__fastcall type_info::`scalar deleting destructor'(type_info *this, unsigned int bDelete)
??_Gtype_info@@UEAAPEAXI@Z:
; rcx = this
; rdx = bDelete

  mov   [rsp+0x8], rbx                  ; save rbx
  push  rdi                             ; save rdi
  sub   rsp, 0x20                       ; allocate shadow space

  mov   ebx, edx                        ; save bDelete in ebx
  mov   rdi, rcx                        ; save this in rdi

  call ??1type_info@@UEAA@XZ            ; destruct 'type_info' (this)

  test  bl, 1                           ; check if bDelete is true
  jz    .return                         ; return if no need to delete
  test  rdi, rdi                        ; check if this is not nullptr
  jz    .return                         ; return if this is nullptr

  ; Delete this. Load arguments
  mov   rcx, [rel ?gAppHeap@Detail@Memory@@3PEAXEA] ; heapHandle
  xor   edx, edx                        ; flags
  mov   r8, rdi                         ; block
  call  [rel __imp_HeapFree]            ; call the free function

.return:
  mov   rbx, [rsp+0x28+0x8]             ; restore rbx
  mov   rax, rdi                        ; return this
  add   rsp, 0x20                       ; free shadow space
  pop   rdi                             ; resture rdi
  ret                                   ; return

section .rdata

global ??_7type_info@@6B@
; const type_info::`vftable'
??_7type_info@@6B@:
  dq ??_Gtype_info@@UEAAPEAXI@Z
