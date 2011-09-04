.model small
.code
.586p




; 
; (http://www.uruk.org/orig-grub/mem64mb.html)
;
; Input:
; 	EAX	Function Code	E820h
; 	EBX	Continuation	Contains the "continuation value" to get the
; 				next run of physical memory.  This is the
; 				value returned by a previous call to this
; 				routine.  If this is the first call, EBX
; 				must contain zero.
; 	ES:DI	Buffer Pointer	Pointer to an  Address Range Descriptor
; 				structure which the BIOS is to fill in.
; 	ECX	Buffer Size	The length in bytes of the structure passed
; 				to the BIOS.  The BIOS will fill in at most
; 				ECX bytes of the structure or however much
; 				of the structure the BIOS implements.  The
; 				minimum size which must be supported by both
; 				the BIOS and the caller is 20 bytes.  Future
; 				implementations may extend this structure.
; 	EDX	Signature	'SMAP' -  Used by the BIOS to verify the
; 				caller is requesting the system map
; 				information to be returned in ES:DI.
; Output:
; 	CF	Carry Flag	Non-Carry - indicates no error
; 	EAX	Signature	'SMAP' - Signature to verify correct BIOS
; 				revision.
; 	ES:DI	Buffer Pointer	Returned Address Range Descriptor pointer.
; 				Same value as on input.
; 	ECX	Buffer Size	Number of bytes returned by the BIOS in the
; 				address range descriptor.  The minimum size
; 				structure returned by the BIOS is 20 bytes.
; 	EBX	Continuation	Contains the continuation value to get the
; 				next address descriptor.  The actual
; 				significance of the continuation value is up
; 				to the discretion of the BIOS.  The caller
; 				must pass the continuation value unchanged
; 				as input to the next iteration of the E820
; 				call in order to get the next Address Range
; 				Descriptor.  A return value of zero means that
; 				this is the last descriptor.  Note that the
; 				BIOS indicate that the last valid descriptor
; 				has been returned by either returning a zero
; 				as the continuation value, or by returning
; 				carry.



;
; UINT16 GetE820Entry (
;   UINT32      * _EBX,     // [BP+4]
;   E820_ENTRY  * Entry     // [BP+6]
;   );
;
_GetE820Entry PROC
  push  bp
  mov   bp, sp

  push  edx
  push  es
  push  ecx
  push  ebx
  push  si
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov   eax, 0E820h

  xor   ebx, ebx
  mov   si, [bp+4]
  mov   bx, [si]

  push  ds
  pop   es
  mov   di, [bp+6]

  mov   ecx, 20
  mov   edx, 534D4150h  ; "SMAP"

  int   15h
  jc    ERROR1
  
  mov   [si], bx  ; new index
  mov   eax, 1
  jmp   EXIT

ERROR1:
  xor   eax, eax

EXIT:
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pop   si
  pop   ebx
  pop   ecx
  pop   es
  pop   edx
  mov   sp, bp
  pop   bp
  ret

_GetE820Entry ENDP


END

