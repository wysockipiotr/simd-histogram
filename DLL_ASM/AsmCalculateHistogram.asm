; ---------------------------------------------------------------------------------------------------------------
;
;	LIBRARY				
;						DLL_ASM
;
;	DESCRIPTION			
;						DLL_ASM provides accelerated SIMD approach to image histogram calculation.
;						Library consists of one exported function (_asm_calculate_histogram).
;						
;	PLATFORM			
;						MASM x86-64
;						SSE
;
; ---------------------------------------------------------------------------------------------------------------


.code

	extern MAX_NUMBER_OF_PIXELS : dword


; ---------------------------------------------------------------------------------------------------------------
;
;	MACRO				
;						_process_16_pixels_using xmm_a xmm_b
;
;	PARAMS				
;						xmm_a, xmm_b -- non-volatile XMM registers 
;
;	REGISTER CONTENT	
;						RSI -- pointer to main histogram
;						RDI -- pointer to auxiliary histogram
;						xmm_a -- contains 16 pixels data
;						xmm_b -- copy of xmm_a
;
;	OPERATION			simultaneously constructs two partial histograms from 16 pixels (bytes)
;
;	PLATFORM			
;						MASM x86-64
;						SSE
;
; ---------------------------------------------------------------------------------------------------------------
_process_16_pixels_using macro xmm_a, xmm_b
		pextrb rax,xmm_a,0
		add dword ptr [rsi+rax*4],1       
		pextrb rbx,xmm_b,1
		add dword ptr [rdi+rbx*4],1       
		pextrb rcx,xmm_a,2
		add dword ptr [rsi+rcx*4],1       
		pextrb rdx,xmm_b,3
		add dword ptr [rdi+rdx*4],1       
		pextrb rax,xmm_a,4
		add dword ptr [rsi+rax*4],1       
		pextrb rbx,xmm_b,5
		add dword ptr [rdi+rbx*4],1       
		pextrb rcx,xmm_a,6
		add dword ptr [rsi+rcx*4],1       
		pextrb rdx,xmm_b,7
		add dword ptr [rdi+rdx*4],1       
		pextrb rax,xmm_a,8
		add dword ptr [rsi+rax*4],1     
		pextrb rbx,xmm_b,9
		add dword ptr [rdi+rbx*4],1     
		pextrb rcx,xmm_a,10
		add dword ptr [rsi+rcx*4],1     
		pextrb rdx,xmm_b,11
		add dword ptr [rdi+rdx*4],1     
		pextrb rax,xmm_a,12
		add dword ptr [rsi+rax*4],1     
		pextrb rbx,xmm_b,13
		add dword ptr [rdi+rbx*4],1     
		pextrb rcx,xmm_a,14
		add dword ptr [rsi+rcx*4],1     
		pextrb rdx,xmm_b,15
		add dword ptr [rdi+rdx*4],1     
; _process_16_pixels_using
endm


; ---------------------------------------------------------------------------------------------------------------
;
;	PROCEDURE			
;						_asm_calculate_histogram
;
;	CPP DECLARATION		
;						extern "C" bool _asm_calculate_histogram(unsigned __int32* histogram, 
;																 unsigned __int8* pixel_buffer,  
;																 unsigned __int32 number_of_pixels);
;
;	PARAMS				
;						histogram -- pointer to 256-byte histogram array, 
;						pixel_buffer -- pointer to buffer of pixel values
;						number_of_pixels -- total number of pixels in pixel_buffer
;
;	OPERATION			
;						creates histogram from 8-bit pixel buffer (one channel of rgb image)  
;
;	MODIFIED REGISTERS	
;						TODO
;
;	PLATFORM			
;						MASM x86-64
;						SSE
;
;	RETURNS				
;						true -- histogram calculation successful 
;						false -- invalid number_of_pixels or alignment 
;
; ---------------------------------------------------------------------------------------------------------------
_asm_calculate_histogram proc frame

		; -----------------------
		push rbp				;
		.pushreg rbp			;
		push rbx				;
		.pushreg rbx			;
		push rsi				;	procedure prolog
		.pushreg rsi			;	prepares suitable stack frame
		push rdi				;	
		.pushreg rdi			;	- nonvolatile registers are pushed onto stack
								;	- 1024 bytes (256 x 4 bytes) of local memory 
		sub rsp, 1032			;	   are reserved for auxiliary histogram buffer
		.allocstack 1032		;	- RSP has to be 16-bytes aligned: 1032 = 1024 (histogram space) + 8 (padding)
		mov rbp, rsp			;	
		.setframe rbp, 0		;
								;
		.endprolog				;
		; -----------------------

		; store number of pixels (r10) and pixel buffer pointer (r11) 
		; for further processing (of remaining [1, 31] pixels)
		mov r10,r8
		mov r11,rdx

		; validate alignment of histogram and pixel buffer

		; rsi points to main histogram
		mov rsi,rcx                    
		
		; histogram is properly aligned if its address MOD 16 = 0
		test rsi,15
		jnz Error                          

		; r9 points to pixel buffer
		mov r9,rdx

		; pixel buffer is properly aligned if its address MOD 16 = 0
		test r9,15
		jnz Error

; Make sure num_pixels is valid
		test r8d,r8d
		jz Error                           ;jump if num_pixels is zero
		cmp r8d, [MAX_NUMBER_OF_PIXELS]
		ja Error                            ;jump if num_pixels too big


; zero-initialize both main and auxiliary histogram buffers
		xor rax,rax

		; rdi points to main histogram buffer
		mov rdi,rsi                  
		
		; 128 * 8 (bytes in quadword) = 1024 bytes (size of histogram)
		mov rcx,128    
		
		; zeros
		rep stosq                           


		; rdi points to auxiliary histogram buffer
		mov rdi,rbp          
		
		; 128 * 8 (bytes in quadword) = 1024 bytes (size of histogram)
		mov rcx,128  
		
		; zeros
		rep stosq                   
		
		
; if total number of pixels < 32, jump to instructions processing remaining [0-31] pixels
		mov rax,r10
		cmp rax,32
		jb Residue


; process all 32-pixel chunks of pixel buffer

		; total number of pixels / 32 = number of 32-pixel chunks 
		shr r8d,5 

		; rdi points to auxiliary histogram 
		mov rdi,rbp                       

		; jump destination alignment
		align 16                            
		
ProcessNext:	
		; load first 16 of 32 pixels (bytes) 
		movdqa xmm0,[r9]                    
		movdqa xmm1,xmm0

		; load second 16 of 32 pixels (bytes)
		movdqa xmm2,[r9+16]                 
		movdqa xmm3,xmm2


		; process pixels 0..15
		_process_16_pixels_using xmm0, xmm1

		; process pixels 16..32
		_process_16_pixels_using xmm2, xmm3


		; move pixel buffer pointer by 32 (so it points to next 32-pixel chunk)
		add r9,32      
		
		; loop while there are any 32-pixel chunks
		;dec r8
		sub r8,1
		jnz ProcessNext                              

		
; combine main and auxiliary histograms

		; set number of necessary iterations (32 * 8 histogram entries = 256 entries (the whole histogram)) 
		mov ecx,32							

		xor rax,rax

MergeNext:
		; xmm0:xmm1 contains main histogram 8-entries chunk
		movdqa xmm0,xmmword ptr [rsi+rax]
		movdqa xmm1,xmmword ptr [rsi+rax+16]

		; add auxiliary histogram chunk to main histogram chunk
		paddd xmm0,xmmword ptr [rdi+rax]        
		paddd xmm1,xmmword ptr [rdi+rax+16]

		; store result under destination address
		movdqa xmmword ptr [rsi+rax],xmm0 
		movdqa xmmword ptr [rsi+rax+16],xmm1

		; advance xmmword ptr for next chunk processing
		add rax,32

		; decrement iteration counter
		sub ecx,1

		; loop until the whole histogram is merged
		jnz MergeNext

; Process remaining [0-31] pixels, which were skipped by the foregoing SSE instruction block
Residue:
		; rax contains total number of pixels
		mov rax, r10

		; rdx contains pointer to the pixel buffer
		mov rdx, r11

		; rdx points to last element in the pixel buffer
		lea rdx, qword ptr [rdx+rax-1]	

		; rax, rcx contain number of remaining pixels [0-31]
		and rax, 31						
		mov rcx, rax		
		
ProcessNextRemaining:		
		; if there are no more pixels jump to successful end 
		cmp rcx, 0		
		je	Ok		
		
		; eax contains intensity of pointed pixel
		movzx eax, byte ptr [rdx]		

		; increment corresponding histogram entry 
		add dword ptr [rsi+rax*4],1		

		; decrement iteration counter
		sub rcx,1			
		
		; move pixel buffer pointer one pixel backwards
		sub	rdx,1	
		
		jmp ProcessNextRemaining


Ok:		; set success return value (true)
		mov eax,1					

Return: 

		; -------------------------------
		lea rsp, [rbp+1032]				;
		pop rdi							;	procedure epilog
		pop rsi							;	dump stack frame
		pop rbx							;	
		pop rbp							;
		; -------------------------------
		
		ret

Error:  ; set failure return value (false)
		xor rax,rax
		jmp Return

_asm_calculate_histogram endp

end
