section .text

global memset
; void* memset(void* dst, uint8_t value, size_t count)
memset:
; rcx = dst
; dl = value
; r8 = count

  push  rdi                             ; save rdi

  mov   r9, rcx                         ; save dst in r9
  mov   rdi, rcx                        ; store dst in rdi (dest for stosb)
  mov   al, dl                          ; store value in al (value for stosb)
  mov   rcx, r8                         ; store count in rcx (count for stosb)
  rep stosb                             ; fill the memory

  pop   rdi                             ; restore rdi
  mov   rax, r9                         ; store dst as return value
  ret                                   ; return
