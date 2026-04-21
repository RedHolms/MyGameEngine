; It's easier to alias memmove as memcpy because performance not really differs

section .text

global memcpy, memmove
; void* memcpy(void* dst, void const* src, size_t count)
memcpy:
memmove:
; rcx = dst
; rdx = src
; r8 = count

  mov   r9, rdi
  mov   r10, rsi

  mov   rax, rcx
  cmp   rcx, rdx
  je    .return
  ja    .reverse

  ; copy in normal order
  cld
  mov   rsi, rdx
  mov   rdi, rcx
  mov   rcx, r8
  rep movsb
  jmp   .return

.reverse:
  ; copy in reverse order
  std
  lea   rsi, [rdx + r8 - 1]
  lea   rdi, [rcx + r8 - 1]
  mov   rcx, r8
  rep movsb
  cld

.return:
  mov rsi, r10
  mov rdi, r9
  ret
