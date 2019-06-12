TITLE Combinations Calculator     (program5b.asm)

; -------------------------------------------------------------
; Author: Jordan
; Last Modified: 08-05-18
; OSU email address: sturtzj@oregonstate.edu
; Course number/section: CS271_400
; Assignment Number: 5b                 Due Date: 08-5-18
;
; Description:  This program quizzes the user on combinations questions. It
;               presents users with a number, n, from which to choose another
;               number, r, elements. The program will then check to see
;               if the user entered the right answer. It will then
;               prompt the user whether to continue, and when the user
;               quits, it displays the final result. 
; -------------------------------------------------------------

INCLUDE Irvine32.inc

; MACROS
; -------------------------------------------------------------
; returns in eax a random value from low to high inclusively
randRange MACRO low, high
    mov     eax, high
    sub     eax, low
    inc     eax
    call    RandomRange
    add     eax, low
ENDM

; outputs integer to console
displayDec MACRO num
    push    eax
    mov     eax, num
    call    WriteDec
    pop     eax
ENDM

; outputs string to console
displayStr MACRO string
    push    edx
    mov     edx, string
    call    WriteString
    pop     edx
ENDM
; -------------------------------------------------------------

.data
n               DWORD ?                     
r               DWORD ?
result          DWORD ?
wins            DWORD 0
losses          DWORD 0
inputString     BYTE 3 DUP (?)
inputInt        DWORD ?
titleProgram    BYTE "Calculating Combinations by Jordan Sturtz", 0
EC1             BYTE "     *EC: This program numbers each problem, and keeps score", 0
greet1          BYTE "Welcome to the combinations challenge. I'll give you a combinations problem " , 0
greet2          BYTE "to solve, and then I'll tell you if you are right. " , 0
problem         BYTE "     Problem ", 0
numN            BYTE "     Number of elements in this set: ", 0
numR            BYTE "     Number of elements to choose: ", 0
question        BYTE "How many combinations? ", 0
incorrect       BYTE "Incorrect! ", 0
correct         BYTE "Correct! ", 0
result1         BYTE "There are ", 0
result2         BYTE " ways to choose ", 0
result3         BYTE " from ", 0
winsStr         BYTE "     Wins: ", 0
lossesStr       BYTE "     Losses: ", 0
continuePrompt  BYTE "Continue? (y/n)", 0
continueError   BYTE 'Please enter either "y" or "n".', 0
numericError    BYTE "The value must be numeric. Try again.", 0
farewellPrompt  BYTE "Thanks for playing!", 0

.code
main PROC
    ; re-seed generator
    call    Randomize

    ; Introduction
    push    OFFSET titleProgram
    push    OFFSET EC1
    push    OFFSET greet1
    push    OFFSET greet2
    call    Introduction

    ; set problem counter
    mov     ebx, 1

repeatLoop:
    ; make new problem
    push    OFFSET n
    push    OFFSET r
    call    getProblem

    ; display new problem
    push    n
    push    r
    push    ebx
    push    OFFSET problem
    push    OFFSET numN
    push    OFFSET numR
    call    displayProblem

    validateLoop:

        ; get input
        push    OFFSET inputString
        push    OFFSET question
        call    getUserData

        ; validate
        push    OFFSET inputString
        push    OFFSET 3
        call    validateIntTheHardWay
        cmp     eax, 0
        jne     inputOK
        displayStr OFFSET numericError
        call    CrLf
        jmp     validateLoop
   
    inputOK:
    mov     inputInt, eax

    ; calculate combination C(n, r), store in result
    push    n
    push    r
    push    OFFSET result
    call    combination
    mov     result, eax

    ; compare answer to userInt
    mov     eax, inputInt
    cmp     eax, result
    je      correctAnswer

    ; incorrect
    inc     losses
    displayStr OFFSET incorrect    
    jmp     showAnswer
    
    correctAnswer:
    inc     wins
    displayStr OFFSET correct

    showAnswer:
    push    result
    push    n
    push    r
    push    OFFSET result1
    push    OFFSET result2
    push    OFFSET result3
    call    displayResults

    ; prompt to continue
    push    OFFSET continuePrompt
    push    OFFSET continueError
    call    askTocontinue
    cmp     eax, 0
    je      endLoop

    ; continue
    inc     ebx
    call    CrLf
    call    CrLf
    jmp     repeatLoop

    endLoop:
    ; display final score
    push    wins
    push    losses
    push    OFFSET winsStr
    push    OFFSET lossesStr
    call    displayScore

    ; display farewell
    call    CrLf
    displayStr OFFSET farewellPrompt
    call CrLf
    exit

main ENDP

introduction PROC
; -------------------------------------------------------------
; Description:  Introduces the program
; Receives:     ebp + 20 = offset to title string
;               ebp + 16 = offset to extra credit string
;               ebp + 12 = offset to greet1
;               ebp + 8  = offset to greet2
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp

    ; print strings
    displayStr DWORD PTR [ebp + 20]
    call    CrLf
    displayStr DWORD PTR [ebp + 16]
    call    CrLf
    call    CrLf
    displayStr DWORD PTR [ebp + 12]
    call    CrLf
    displayStr DWORD PTR [ebp + 8]
    call    CrLf
    call    CrLf

    ; clean up stack
    pop     ebp
    ret     8

introduction ENDP

getProblem PROC
; -------------------------------------------------------------
; Description:  Returns in the memory locations passed on the the stack
;               the number of elements from which to choose (n) and
;               the number of ways to choose those elements (r). These 
;               two variables are used in the combination formula:
;               C(num, choose) = n! / (r! * (n! - r!))
; Receives:     ebp + 12 =  offset to DWORD variable representing 
;                           number of set from which to choose
;               ebp + 8  =  offset to DWORD variable representing 
;                           the number of ways to choose from set
; Changes:      eax
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame, save regs    
    push    ebp
    mov     ebp, esp
    push    eax
    push    edx
    push    esi

    ; get value for n in eax, mov to [ebp + 8]
    mov     esi, [ebp + 12]
    randRange 3, 12
    mov     [esi], eax

    ; get value for r (1 - n) in eax, mov to [ebp + 8]
    mov     esi, [ebp + 8]
    mov     edx, eax
    randRange DWORD PTR 1, edx
    mov     [esi], eax

    ; restore stack
    pop     esi
    pop     edx
    pop     eax
    pop     ebp
    ret     8

getProblem ENDP

displayProblem PROC
; -------------------------------------------------------------
; Description:  Displays the combination problem to the user
; Receives:     ebp + 28 = n
;               ebp + 24 = r
;               ebp + 20 = problem counter
;               ebp + 16 = offset to problem string
;               ebp + 12 = offset to first string
;               ebp + 8  = offset to second string
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame
    push    ebp 
    mov     ebp, esp

    ; display problem number
    displayStr DWORD PTR [ebp + 16]
    displayDec DWORD PTR [ebp + 20]
    call    CrLf

    ; display n
    displayStr DWORD PTR [ebp + 12]
    displayDec DWORD PTR [ebp + 28] 
    call    CrLf

    ; display r
    displayStr DWORD PTR [ebp + 8]
    displayDec DWORD PTR [ebp + 24] 
    call    CrLf
    call    CrLf

    ; restore stackframe
    pop     ebp
    ret     24
displayProblem ENDP

getUserData PROC
; -------------------------------------------------------------
; Description:  Prints prompt for user and reads an a string of bytes to input buffer   
; Receives:     ebp + 12 = offset to input buffer
;               ebp + 8  = offset to prompt string of words 
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; stack frame, save regs
    push    ebp 
    mov     ebp, esp
    push    ecx
    push    edx
    push    esi

    ; write prompt
    displayStr DWORD PTR [ebp + 8]

    ; erase contents of input buffer
    mov     ecx, 4
    mov     esi, [ebp + 12]
    eraseLoop:
        mov     DWORD PTR [esi + ecx - 1], 0
        loop    eraseLoop

    ; read string of bytes no more than three in length (max value = 924)
    mov     ecx, 4      
    mov     edx, [ebp + 12]
    call    ReadString

    ; restore stack frame, regs
    pop     esi
    pop     edx
    pop     ecx
    pop     ebp
    ret     8

getUserData ENDP

validateIntTheHardWay PROC
; -------------------------------------------------------------
; Description:  Validates that the string of bytes at the input buffer
;               are indeed numeric. If numeric, it will calculate and return 
;               the value of the integer in eax. Otherwise, output error message 
;               and return 0 in eax.
; Receives:     ebp + 12 = offset to input buffer
;               ebp + 8  = size of array
; Changes:      eax
; Returns:      either eax = 0 for invalid, or eax = value of int
; Prereqs:      nothing
; -------------------------------------------------------------
    
    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    sub     esp, 4      ; make room for local variable = ebp - 4
    pushad

    ; set up registers 
    mov     ecx, [ebp + 8]  ; loops same number of times as size of array + 1
    inc     ecx
    mov     esi, [ebp + 12] ; offset to input buffer
    mov     edx, 1          ; holds place-values within loop
    xor     ebx, ebx        ; holds ascii value to be converted
    xor     edi, edi        ; holds final sum

    ; find first ascii char that is neither null nor newline
    findChar:
        mov     bl, [esi + ecx - 1]
        cmp     bl, 0
        je      nextChar    ; make sure ascii =/= null
        cmp     bl, 10
        je      nextChar    ; make sure ascii =/= newline
        jmp     loopArr

        nextChar:
        loop    findChar

    loopArr:
        mov     bl, [esi + ecx - 1]

        ; check whether to throw NotInt error
        cmp     bl, 48
        jl      NotIntError
        cmp     bl, 57
        jg      NotIntError
       
        ; get digit at place value in bl
        sub     bl, 48

        ; multiply by place value
        mov     eax, ebx
        mul     dl

        ; add to sum stored in edi
        add     edi, eax        ; edi adds each successive value

        ; increment edx to next place value
        mov     eax, edx
        mov     edx, 10
        mul     dl          
        mov     edx, eax    ; edx now holds next place value

        skipNull:
        loop    loopArr

        ; restore regs, return value in eax        
        mov     [ebp - 4], edi     ; store value in local variable
        popad                       ; restore all regs
        mov     eax, [ebp - 4]     ; put sum into eax
        add     esp, 4              ; clear local variable space
        pop     ebp                 ; restore base pointer
        ret     8
     
   NotIntError:
        ; restore regs, return eax = 0
        popad
        add     esp, 4
        pop     ebp
        mov     eax, 0
        ret     8

validateIntTheHardWay ENDP

combination PROC
; -------------------------------------------------------------
; Description:      Calculates the combination problem C(n, r) and stores
;                   in memory location stored in ebp + 8
; Receives:         ebp + 16 = n
;                   ebp + 12 = r
;                   ebp + 8  = offset to result
; Changes:          nothing
; Returns:          nothing
; Prerequisites:    nothing
; -------------------------------------------------------------
   
    ; set up stack frame, save regs, make space for local variables
    push    ebp
    mov     ebp, esp
    sub     esp, 4      ; ebp - 4 = local variable for r! 
    sub     esp, 4      ; ebp - 8 = local variable for (n - r)!
    push    edx

    ; calculate r! and store in local variable
    push    DWORD PTR [ebp + 12]
    call    factorial
    mov     [ebp - 4], eax

    ; calculate (n - r)! and store in local variable
    mov     eax, [ebp + 16]
    sub     eax, [ebp + 12]
    push    eax
    call    factorial
    mov     [ebp - 8], eax

    ; calculate n! 
    push    DWORD PTR [ebp + 16]
    call    factorial

    ; divide by r! and (n - r)!
    xor     edx, edx
    div     DWORD PTR [ebp - 4]
    div     DWORD PTR [ebp - 8]

    ; store in result
    mov     [ebp + 8], eax

    ; clean up stack, restore regs
    pop     edx
    add     esp, 8
    pop     ebp
    ret     12

combination ENDP

factorial PROC
; -------------------------------------------------------------
; Description:      Calculates n! and returns in eax
; Receives:         ebp + 8  = n
; Changes:          eax
; Returns:          eax = n!
; Prerequisites:    nothing
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    push    1           ; local variable to use with factorialRec starts at 1
    pushad

    ; skip calling subprocedure if n = 0
    cmp     DWORD PTR [ebp + 8], 0
    je      endProc

    ; call recursive subprocedure with starting value
    mov     esi, ebp
    sub     esi, 4      ; esi now points to memory location of local variable
    push    [ebp + 8]   ; first argument = n
    push    esi         ; second argument starts at 1
    call    factorialRec

endProc:
    ; clean up stack, return local variable in eax
    popad
    pop     eax         ; result
    pop     ebp
    ret     4
factorial ENDP

factorialRec PROC
; -------------------------------------------------------------
; Description:      Subprocedure of factorial. Performs recursion.
; Receives:         ebp + 12 = n
;                   ebp + 8  = memory location holding current product (starts at 1)
; Changes:          nothing
; Returns:          result in memory pointed to by ebp + 8
; Prerequisites:    nothing
; -------------------------------------------------------------

    ; stack frame, save regs
    push    ebp
    mov     ebp, esp
    pushad

base:
    cmp     DWORD PTR [ebp + 12], 1
    jne     recurse
    
    ; clean up stack
    popad
    pop     ebp
    ret     8

recurse:
    ; multiply current product in memory by n
    mov     esi, [ebp + 8]      ; gets address for result
    mov     eax, [esi]          ; get current product
    mov     ebx, [ebp + 12]     ; current n
    mul     ebx                 ; multiply previous product by current n
    mov     [esi], eax          ; store new product

    ; decrement n, call itself
    dec     ebx
    push    ebx         ; ebx = n - 1
    push    esi         ; esi holds address of current product
    call    factorialRec

    ; clean up stack
    popad
    pop     ebp
    ret     8

factorialRec ENDP

displayResults PROC
; -------------------------------------------------------------
; Description:      Displays result of combination computation
; Receives:         ebp + 28 = result
;                   ebp + 24 = n
;                   ebp + 20 = r
;                   ebp + 16 = offset of result1 string
;                   ebp + 12 = offset of result2 string
;                   ebp + 8  = offset of result3 string
; Changes:          nothing
; Returns:          nothing
; Prerequisites:    nothing
; -------------------------------------------------------------

    ; stack frame, save regs
    push    ebp
    mov     ebp, esp

    ; display result
    displayStr DWORD PTR [ebp + 16]   ; string  = "There are "
    displayDec DWORD PTR [ebp + 28]   ; integer = <result>
    displayStr DWORD PTR [ebp + 12]   ; string  = " ways to choose "
    displayDec DWORD PTR [ebp + 20]   ; integer = <r>
    displayStr DWORD PTR [ebp + 8]    ; string  = " from "
    displayDec DWORD PTR [ebp + 24]   ; integer = <n>
    call    CrLf
    call    CrLf

    ; restore stack, regs
    pop     ebp
    ret     24

displayResults ENDP

displayScore PROC
; -------------------------------------------------------------
; Description:  Displays score to user. Displays both "wins" and "losses"
; Receives:     ebp + 20 = number of wins
;               ebp + 16 = number of losses
;               ebp + 12 = offset to "win" string
;               ebp + 8  = offset to "loss" string 
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame
    push    ebp 
    mov     ebp, esp

    ; display win number
    call    CrLf
    displayStr DWORD PTR [ebp + 12]     ; string  = "Wins: "
    displayDec [ebp + 20]               ; integer = <number of wins>

    ; display loss number
    call    CrLf
    displayStr DWORD PTR [ebp + 8]      ; string = "Losses: "
    displayDec [ebp + 16]               ; integer = <number of losses>
    call    CrLf

    ; restore stack
    pop     ebp
    ret     16

displayScore ENDP

askToContinue PROC
; -------------------------------------------------------------
; Description:      Prompts user whether to continue. Returns answer as bool in eax. 
; Receives:         ebp + 12 = offset to continue prompt string
;                   ebp + 8  = offset to continue error string 
; Changes:          eax
; Returns:          eax = 0 for no continue, 1 for continue
; Prerequisites:    nothing
; -------------------------------------------------------------

    ; set up stack frame
    push    ebp 
    mov     ebp, esp

    topLoop:
        displayStr DWORD PTR [ebp + 12]
        call    ReadChar
        cmp     al, 121    ; ascii 121 = 'y'
        je      continue
        cmp     al, 110    ; ascii 110 = 'n'
        je      noContinue

        ; display error, continue loop
        call    CrLf
        displayStr DWORD PTR [ebp + 8]
        jmp     topLoop

    continue:
        mov     eax, 1
        pop     ebp
        ret     8

    noContinue:
        mov     eax, 0
        pop     ebp
        ret     8

askToContinue ENDP

END main