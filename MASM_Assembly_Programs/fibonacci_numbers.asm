TITLE   Fibonacci Numbers        (wk2.asm)

; Author: Jordan Sturtz
; Last Modified: 07-12-2018
; OSU email address: sturtzj@oregonstate.edu
; Course number/section: CS271-400
; Assignment Number: 2                Due Date: July 15

; Description: This program calculates and displays fibonacci numbers to the nth fibonacci number. Will also validate input to check whether  
;              inputted integer is between 1 and 46 inclusive. Displays fib numbers in aligned columns and also displays an animation of the first 12 fibonacci numbers

INCLUDE Irvine32.inc
.data
; ----------------------------------------------------
; prompts to use for console output
    titlePrompt       BYTE    "Fibonacci Numbers by Jordan Sturtz", 0
    extracredit1      BYTE    "    **EC: This program displays the numbers in aligned columns.", 0 
    extracredit2      BYTE    "    **EC: This program also displays an animation representing the first twelve fibonacci numbers ", 0
    namePrompt        BYTE    "What's your name? ", 0
    greetPrompt       BYTE    "Hello, ", 0
    outPrompt         BYTE    "Enter a number between 1 and 46 inclusive and I will display all the fibonacci numbers up to that number.", 0 
    inPrompt          BYTE    "--> ", 0
    continuePrompt    BYTE    "Would you like to enter another number? (y/n): ", 0
    badInput          BYTE    "The number must be between 1 and 46 inclusive. Try again.", 0
    byePrompt         BYTE    "See ya later, ", 0
    animPrompt        BYTE    "Enjoy this animation that visually represents the first 12 fibonacci terms:", 0

; ----------------------------------------------------
; constants
    UPPER_LIMIT = 46                        ; holds upper limit to how high user can enter int
    CHAR_LIMIT = 80                         ; limits user name

; ----------------------------------------------------
; variables
    nthFib      DWORD   ?                   ; holds num of fibonacci terms requested by user   
    fib1        DWORD   0                   ; holds second to last fib number 
    fib2        DWORD   1                   ; holds last fib number
    fibArray    DWORD   46 dup (?)          ; holds fibonnaci sequence
    count       DWORD   ?                   ; used for nested loop counters
    currentVal  DWORD   ?                   ; holds current value during display phase
    lengthVal   DWORD   ?                   ; holds length of current value
    user        BYTE    CHAR_LIMIT+1 dup (?); holds user-name
    msec        DWORD   ?                   ; holds milliseconds since midnight

    ; remove this shit
    word1 SWORD 4
    dword1 SDWORD 4

.code
main PROC

; -------------------------------------------------------------------------
; introduction
    ; remove this shit
    
    yesOF:
    mov eax,12345h
    mov ebx,1000h
    mul ebx
    jc      yesOF



    mov     edx, OFFSET titlePrompt
    call    WriteString
    call    CrLf
    mov     edx, OFFSET extracredit1
    call    WriteString
    call    CrLf
    mov     edx, OFFSET extracredit2
    call    WriteString
    call    CrLf
    call    CrLf

; -------------------------------------------------------------------------
; get user name
    mov     edx, OFFSET namePrompt
    call    WriteString
    call    CrLf
    mov     edx, OFFSET inPrompt
    call    WriteString
    mov     edx, OFFSET user
    mov     ecx, CHAR_LIMIT
    call    ReadString
    call    CrLf
    mov     edx, OFFSET greetPrompt
    call    WriteString
    mov     edx, OFFSET user
    call    WriteString
    call    CrLf
    call    CrLf
    mov     edx, OFFSET outPrompt
    call    WriteString
    call    CrLf

; -------------------------------------------------------------------------
; get integer from user

validateLoop:

    call    CrLf
    mov     edx, OFFSET inPrompt
    call    WriteString
    call    ReadInt
    mov     nthFib, eax 

; -------------------------------------------------------------------------
; validate integer is between 1 - 46 inclusive

    cmp     nthFib, 1    
    jl      inputBad
    cmp     nthFib, UPPER_LIMIT    
    jg      inputBad
    jmp     inputOK

inputBad:
    mov     edx, OFFSET badInput
    call    WriteString
    call    CrLf
    jmp     validateLoop

inputOK:

; -------------------------------------------------------------------------
; fill fibArray with fibonacci numbers up to 46th fib number

    mov     ecx, 46             ; counter for loop
    mov     ebx, 0              ; counter for index
    mov     esi, OFFSET fibArray 

makeArray:

    ; add latest fib value to array
    mov     eax, fib2
    mov     [esi + 4 * ebx], eax
    
    ; update next two fib values
    mov     currentVal, eax   
    add     eax, fib1   
    mov     fib2, eax   
    mov     eax, currentVal   
    mov     fib1, eax   
    
    inc     ebx
    loop    makeArray

; -------------------------------------------------------------------------
; display fibonacci numbers to user up to nthFib

    mov     ecx, nthFib         ; counter for loop
    mov     ebx, 0              ; counter for index

displayArray:
    
    ; prints a new line only if five values have already been printed
    xor     edx, edx
    mov     eax, ebx
    mov     esi, 5          
    div     esi 
    cmp     edx, 0          
    je      printNewLine
    jmp     noNewLine

printNewLine:
    call    CrLf

noNewLine:

    ; write decimal to output
    mov     esi, OFFSET fibArray;     
    mov     eax, [esi + 4 * ebx]
    call    WriteDec

    ; output variable amount of whitespace to align columns
    call    lenInt          ; lenInt defined below main procedure
    mov     lengthVal, eax
    mov     count, ecx      ; store ecx for outer loop
    mov     eax, 15  
    sub     eax, lengthVal   
    mov     ecx, eax  
    xor     eax, eax        ; clears to use for WriteChar

printWhiteSpace:            

    ; prints (15 - lenInt) whitespace characters
    mov     al, ' ' 
    call    WriteChar
    loop    printWhiteSpace
    
    mov     ecx, count     
    inc     ebx
    loop    displayArray

; -------------------------------------------------------------------------
; print cool animation (EC)
    call    CrLf
    call    CrLf
    mov     edx, OFFSET animPrompt
    call    WriteString
    call    CrLf

    mov     ecx, 12             ; counter for loop
    mov     ebx, 0              ; counter for index

animationLoop:
    
    mov     eax, [esi + 4 * ebx] 
    mov     currentVal, eax
    mov     count, ecx      
    mov     ecx, currentVal
    
    ; writes dash character equal to currentVal
    printDash:
    xor     eax, eax
    mov     al, '-'
    call    WriteChar
    loop printDash
    
    ; delay output for quarter of a second
    call    GetMSeconds
    mov     msec, eax
    add     msec, 250 

timedDelay:
    call    GetMSeconds
    cmp     msec, eax
    jge     timedDelay 

    ; loops back to animationLoop to write next line
    inc     ebx
    mov     ecx, count
    call    CrLf
    loop    animationLoop

; -------------------------------------------------------------------------
; print final goodbye
    call    CrLf
    mov     edx, OFFSET byePrompt
    call    WriteString
    mov     edx, OFFSET user
    call    WriteString
    mov     al, '!'
    call    WriteChar
    call    CrLf

exit 
main ENDP

;---------------------------------------------------------------------
; lenInt
; 
; Calculates and returns the length of a 32-bit number unsigned int. The 
; length is understood to be the length in decimal. For example, the number
; "1024" would return 4. The max length of a 32 bit unsigned int is 10, 
; so this function will return between 1 - 10. 

; Receives: EAX, which holds the 32-bit unsigned int.
; Returns:  EAX = length of the number
;---------------------------------------------------------------------
lenInt PROC uses ebx ecx esi edx
    mov     ebx, eax    ; holds currentVal temporarily 
    mov     eax, 1      ; used to multiply by 10 to increase decimal base for comparison
    mov     ecx, 10     ; used to hold 10 to use with mul
    mov     esi, 0      ; used to hold the "place value" to be returned

    topLoop:
    inc     esi
    mul     ecx
    
    ; if CF is on, then val in ebx must have ten decimal places
    jc      endLoop    

    ; will loop and multiply eax by 10 until value in ebx is less than eax
    cmp     eax, ebx 
    jl      topLoop     

    ; move the counter back into eax for return

    endLoop:
    mov     eax, esi    
    ret

lenInt ENDP 

END main