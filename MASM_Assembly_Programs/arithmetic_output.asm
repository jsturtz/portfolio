TITLE   Arithmetic Output        (wk1.asm)

; Author: Jordan Sturtz
; Last Modified: 06-06-2018
; OSU email address: sturtzj@oregonstate.edu
; Course number/section: CS271-400
; Assignment Number: 1                Due Date: July 8

; Description:  Gets two integers from user and prints to console the sum, difference, product, 
;               and quotient with remainder. Will also validate that first integer is less than second. 
;               Will also prompt user whether to continue or quit. 

INCLUDE Irvine32.inc
.data

; prompts to use for console output
outPrompt1        BYTE    "Elementary Arithmetic by Jordan Sturtz", 0
outPrompt2        BYTE    "    **EC: This program loops until user quits.", 0 
outPrompt3        BYTE    "    **EC: This program validates that first integer is less than second.", 0 
outPrompt4        BYTE    "Enter 2 numbers, and I'll show you the sum, difference, product, quotient, and remainder.", 0
outPrompt5        BYTE    "Impressed? Bye!", 0
inPrompt1         BYTE    "First number: ", 0
inPrompt2         BYTE    "Second number: ", 0
continuePrompt    BYTE    "Would you like to try again? (y/n): ", 0
badInput          BYTE    "The first integer must be less than the second. Try again. ", 0

; symbols for console output
plusSym           BYTE      " + ", 0
minusSym          BYTE      " - ", 0
productSym        BYTE      " * ", 0
quotientSym       BYTE      " / ", 0
equalSym          BYTE      " = ", 0
remainderSym      BYTE      " remainder ", 0

; variables 
val1              SDWORD     ?    
val2              SDWORD     ?
sum               SDWORD     ?
difference        SDWORD     ?
product           SDWORD     ?
quotient          SDWORD     ?
remainder         SDWORD     ?
no      BYTE  "No", 0
maybe   BYTE    "Maybe", 0
a                 DWORD      5
result  DWORD   ?
.code


main PROC

    ; outputs introduction
    mov     edx, OFFSET outPrompt1
    call    WriteString
    call    CrLf
    mov     edx, OFFSET outPrompt2
    call    WriteString
    call    CrLf
    mov     edx, OFFSET outPrompt3
    call    WriteString
    call    CrLf
    call    CrLf
    mov     edx, OFFSET outPrompt4
    call    WriteString
    call    CrLf

     
topLoop:            ; looping body begins here

    ; get first integer
    call    CrLf
    mov     edx, OFFSET inPrompt1
    call    WriteString
    call    ReadInt
    mov     val1, eax 

    ; get second integer    
    mov     edx, OFFSET inPrompt2
    call    WriteString
    call    ReadInt
    mov     val2, eax 
    call    CrLf

    ; validates input
    cmp     val1, eax
    jge     inputOK
    mov     edx, OFFSET badInput;
    call    WriteString
    jmp     topLoop


inputOK:            ; continues only when first > second

    ; calculate the required values
    mov     eax, val1
    add     eax, val2
    mov     sum, eax
    
    mov     eax, val1
    sub     eax, val2
    mov     difference, eax

    mov     eax, val1
    mul     val2    
    mov     product, eax

    mov     eax, val1
    div     val2    
    mov     quotient, eax
    mov     remainder, edx

    ; display sum
    mov     eax, val1
    call    WriteDec
    mov     edx, OFFSET plusSym
    call    WriteString
    mov     eax, val2
    call    WriteDec
    mov     edx, OFFSET equalSym
    call    WriteString
    mov     eax, sum
    call    WriteDec
    call    CrLf
    
    ; display difference
    mov     eax, val1
    call    WriteDec
    mov     edx, OFFSET minusSym
    call    WriteString
    mov     eax, val2
    call    WriteDec
    mov     edx, OFFSET equalSym
    call    WriteString
    mov     eax, difference
    call    WriteDec
    call    CrLf

    ; display product
    mov     eax, val1
    call    WriteDec
    mov     edx, OFFSET productSym
    call    WriteString
    mov     eax, val2
    call    WriteDec
    mov     edx, OFFSET equalSym
    call    WriteString
    mov     eax, product
    call    WriteDec
   call    CrLf
    
    ; display quotient 
    mov     eax, val1
    call    WriteDec
    mov     edx, OFFSET quotientSym
    call    WriteString
    mov     eax, val2
    call    WriteDec
    mov     edx, OFFSET equalSym
    call    WriteString
    mov     eax, quotient
    call    WriteDec

    ; display remainder
    mov     edx, OFFSET remainderSym
    call    WriteString
    mov     eax, remainder 
    call    WriteDec
    call    CrLf


inputLoop:                  

    ; prompt user whether to continue 
    call    CrLf
    mov     edx, OFFSET continuePrompt
    call    WriteString
    call    ReadChar
    cmp     al, 'y'
    je      topLoop         ; jump back to topLoop if user wants to continue
    cmp     al, 'n'
    jne     inputLoop       ; repeat inputLoop if user fails to enter correct character

    ; say goodbye 
    call    CrLf
    call    CrLf
    mov     edx, OFFSET outPrompt5
    call    WriteString
    call    CrLf
    exit 

main ENDP
END main