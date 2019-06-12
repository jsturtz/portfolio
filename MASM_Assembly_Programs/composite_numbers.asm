TITLE Composite Numbers     (wk4.asm)

; -------------------------------------------------------------
; Author: Jordan
; Last Modified: 08-05-18
; OSU email address: sturtzj@oregonstate.edu
; Course number/section: CS271_400
; Assignment Number: 4                 Due Date: 08-5-18
;
; Description:      This program gets a value from the user from standard output and validates that 
;                   the value is between [10 .. 200]. Once the user's request is validated, this 
;                   program generates *request* number of random numbers between [100 .. 999]. It 
;                   displays these numbers in no more than 10 columns with three spaces between them. 
;                   Then, the program sorts these values in descending order, displays the median, 
;                   and then displays the sorted list in descending columns. The list is sorted
;                   by an implementation of quickSort.
; 
; CITATION:         I drew from Miguel Casillas' example of a quicksort algorithm written in MASM. 
;                   I was struggling to figure out how to implement a quicksort without utilizing
;                   two separate arrays (source and destination). I learned from his procedure
;                   how to implement the quicksort algorithm through a value-swapping algorithm.
;                   I modified his value-swapping algorithm to create a quicksort that sorts in
;                   descending order.
;                   LINK: http://www.miguelcasillas.com/?p=354#Results-3
; -------------------------------------------------------------

INCLUDE Irvine32.inc

; global constants
MIN = 10
MAX = 200
LO = 100
HI = 999

.data
request         DWORD 0     
array           DWORD MAX DUP(?)   
titleProgram    BYTE "Sorting Array by Jordan Sturtz", 0
EC1             BYTE "     *EC: This program uses a recursive quicksort algorithm", 0
greetP1         BYTE "This program generates random numbers in the range [100 .. 999]," , 0
greetP2         BYTE "displays the original list, sorts the list, and calculates the" , 0
greetP3         BYTE "median value. Finally, it displays the list sorted in descending order." , 0
prompt          BYTE "How many numbers? [10 .. 200]: ", 0
titleUnsorted   BYTE "The unsorted random numbers: ", 0
titleSorted     BYTE "The sorted list: ", 0
titleMedian     BYTE "The median is ", 0
badInput        BYTE "The value must be between 10 and 200 inclusive. Try again.", 0
farewellPrompt  BYTE "Thanks for playing!", 0

.code
main PROC
    ; re-seed generator
    call    Randomize

    ; Introduction
    push    OFFSET titleProgram
    push    OFFSET EC1
    push    OFFSET greetP1
    call    Introduction
    call    CrLf

    validateLoop:

        ; getUserData
        push    OFFSET prompt
        call    getUserData
        mov     request, eax

        ; validate in range [10 .. 200]
        push    request
        push    OFFSET  badInput
        push    MAX
        push    MIN
        call    validate
        cmp     eax, 0
        je      validateLoop

    ; fill unsorted array with random values [100 .. 999] 
    push    OFFSET array
    push    request
    push    HI
    push    LO
    call    fillArray

    ; display unsorted
    call    CrLf
    push    OFFSET titleUnsorted
    push    OFFSET array
    push    request
    call    displayList
    call    CrLf
    call    CrLf

    ; sort array from largest to smallest with quicksort
    push    OFFSET array
    push    request
    call    QuickSort            

    ; display median
    push    OFFSET titleMedian
    push    OFFSET array
    push    request
    call    displayMedian
    call    CrLf
    call    CrLf

    ; display sorted array
    push    OFFSET titleSorted
    push    OFFSET array
    push    request
    call    displayList
    call    CrLf
    call    CrLf

    ; display farewell
    push    OFFSET farewellPrompt
    call    farewell
    exit	

main ENDP

introduction PROC
; -------------------------------------------------------------
; Description:  Introduces the program
; Receives:     ebp + 16 = offset to title string
;               ebp + 12 = offset to extra credit string
;               ebp + 8  = offset to rest of string segment
;               
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    pushad

    ; print title 
    mov     edx, [ebp + 16]
    call    WriteString
    call    CrLf

    ; print extra credit string 
    mov     edx, [ebp + 12]
    call    WriteString
    call    CrLf
    call    CrLf

    ; print explanation strings
    push    [ebp + 8]
    push    3
    call    printStrings

    ; clean up stack
    popad
    pop     ebp
    ret     8

introduction ENDP

printStrings PROC
; -------------------------------------------------------------
; Description:  This function prints byte strings from contiguous memory. It will also
;               print newlines whenever a null terminator is found. 
; Receives:     ebp + 12 = offset to starting address for strings
;               ebp + 8  = number of strings to print
; Changes:      nothing
; Returns:      nothing
; Prereqs:      Strings must be bytes that are stored in contiguous memory.
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    pushad

    ; get variables from stack
    mov     ecx, [ebp + 8]
    mov     esi, [ebp + 12]
    
    topLoop:

        stringLoop:
            ; if char = null, jump to nextString
            cmp     BYTE PTR [esi], 0    
            je      nextString

            ; else, write char and increment esi
            mov     eax, [esi]
            call    WriteChar
            inc     esi
            jmp     stringLoop

        ; write newline, increment esi, and loop if there exists another string to print
        nextString:
        call    CrLf
        inc     esi
        loop    topLoop

    ; clean up stack
    popad
    pop     ebp
    ret     8

printStrings ENDP

getUserData PROC
; -------------------------------------------------------------
; Description:  Prints prompt for user and reads an int    
; Receives:     ebp + 8 = offset to prompt string of words
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------
    
    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    push    edx

    ; write string, call readInt
    mov     edx, [ebp + 8] 
    call    WriteString
    call    ReadInt

    ; clean up stack
    pop     edx
    pop     ebp
    ret     4
getUserData ENDP

validate PROC
; -------------------------------------------------------------
; Description: 
; Receives:         ebp + 20 = value to validate
;                   ebp + 16 = offset to array of BYTES error message
;                   ebp + 12 = maximum acceptable value
;                   ebp + 8 = minimum acceptable value
; Changes:          eax 
; Returns:          eax: 1 = valid data, 0 = invalid
; Prerequisites:    nothing
; -------------------------------------------------------------
    
    ; set up base pointer and save regs
    push    ebp
    mov     ebp, esp
    pushad

    ; compare value to max
    mov     eax, [ebp + 20]      ; value to check 
    cmp     eax, [ebp + 12]      ; max value
    jg      invalid    

    ; compare value to min
    cmp     eax, [ebp + 8]
    jl      invalid

    ; restore values from stack
    popad
    pop     ebp

    ; return eax = 1 for valid data
    mov     eax, 1
    ret     16

    
invalid: 
    ; write error message
    mov     edx, [ebp + 16]     
    call    WriteString
    call    CrLf

    ; restore values from stack
    popad
    pop     ebp

    ; return 0 = invalid result
    mov     eax, 0
    ret     16

validate ENDP

fillArray PROC
; -------------------------------------------------------------
; Description: 
; Receives:     ebp + 20 = OFFSET to unsorted array
;               ebp + 16 = size of array
;               ebp + 12 = max value for calling RandomRange
;               ebp + 8  = min value for calling RandomRange
; Changes:      fills unsorted array with random values
; Returns:      nothing
; Prereqs:      nothing 
; -------------------------------------------------------------

    ; set up stack frame and save regs
    push    ebp
    mov     ebp, esp
    pushad

    ; set up registers
    mov     edi, [ebp + 20]     ; offset to array
    mov     ecx, [ebp + 16]     ; loop counter = size of array
    mov     edx, 0              ; used for base-offset addressing below

fillLoop:

    ; get random value
    mov     eax, [ebp + 12]
    inc     eax
    sub     eax, [ebp + 8]
    call    RandomRange
    add     eax, [ebp + 8]      ; now holds random value [10 .. 200]

    ; mov random value into next spot in array
    mov     [edi + edx], eax
    add     edx, 4
    loop    fillLoop

    ; restore regs
    popad
    pop ebp
    ret     16

fillArray ENDP

quickSort PROC
; -------------------------------------------------------------
; Description:  Implements the quicksort algorithm recursively to fill
;               the inputted array with sorted array of integers from largest to smallest.
; Receives:     ebp + 12 = offset to array
;               ebp + 8  = size of array end index 
; Changes:      restores all registers
; Returns:      nothing
; Prereqs:      only works with an array of DWORDS
;
; CITATION:     I drew from Miguel Casillas' example of a quicksort algorithm written in MASM. 
;               I was struggling to figure out how to implement a quicksort without utilizing
;               two separate arrays (source and destination). I learned from his procedure
;               how to implement the quicksort algorithm through a value-swapping algorithm.
;               I modified his value-swapping algorithm to create a quicksort that sorts in
;               descending order.
;               LINK: http://www.miguelcasillas.com/?p=354#Results-3
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    pushad             

    ; set up regs to call quickSortRecursive

    mov     eax, [ebp + 8]
    dec     eax
    mov     ebx, 4
    mul     ebx                 
    mov     ebx, eax            ; ebx = high index for array
    xor     eax, eax            ; eax = low index for array
    mov     esi, [ebp + 12]     ; esi = array
    call    quickSortRecursive

    ; restore regs
    mov     esp, ebp        ; cleans stack from quickSortRecursive call
    sub     esp, 32         ; decrement to pop regs off stack
    popad                   ; restore all regs
    pop     ebp             ; restore ebp
    ret     8               ; clean up arguments

QuickSort ENDP

quickSortRecursive PROC
; -------------------------------------------------------------
; Description:  Subprocedure of QuickSort
; Receives:     eax = low index of array
;               ebx = high index of array
;               esi = offset to array
; Changes:      all registers
; Returns:      nothing
; Prereqs:      only works with an array of DWORDS
; -------------------------------------------------------------

    ; BASE CASE = array has only one element
    cmp     ebx, eax
    jle     endRecursion

    ; RECURSIVE CASE = more than one element
    push    ebx                     ; save high index for array
    push    eax                     ; save low index for array
    mov     edi, [esi + eax]        ; edi holds pivot
    add     ebx, 4                  ; so that loop below starts at right index

    mainLoop:

        leftValLoop:
            ; search from left-to-right for val < pivot
            add     eax, 4

            ; if low index >= high index, break
            cmp     eax, ebx
            jge     foundLeftVal

            ; check if next val < pivot
            cmp     [esi + eax], edi
            jl      foundLeftVal

            ; if not, continue loop
            jmp     leftValLoop
        
        foundLeftval:
        
        rightValLoop:
            ; search from right to left for val > pivot
            sub     ebx, 4

            ; if next val >= pivot, break
            cmp     [esi + ebx], edi
            jge     foundRightVal
            jmp     rightValLoop

        foundRightVal:

            ; if low index >= high index, don't swap and end loop
            cmp     eax, ebx
            jge     endMainLoop

            ; else, swap the two values
            push    [esi + eax]
            push    [esi + ebx]
            pop     [esi + eax]
            pop     [esi + ebx]

            ; repeat
            jmp     mainLoop

    endMainLoop:

        ; get original indices
        pop     ecx         ; low index
        pop     edi         ; high index

        ; swap the value of the pivot (esi + ecx) with the value at offset ebx
        push    [esi + ecx]
        push    [esi + ebx]
        pop     [esi + ecx]
        pop     [esi + ebx]

        ; save high index, and save offset to pivot value
        push    edi
        push    ebx

        ; reset low index
        mov     eax, ecx
        sub     ebx, 4

        ; recursively calls on left side of array
        call    QuickSortRecursive

        ; set up registers to call quickSortRecursive on right side of array
        pop     eax             ; offset to pivot value becomes new "low index"
        add     eax, 4          ; right side of array begins at pivot + 1
        pop     ebx             ; new high index equal to old high index
        call    QuickSortRecursive

endRecursion:
    ret     

quickSortRecursive ENDP

displayMedian PROC
; -------------------------------------------------------------
; Description:  Displays the median value of an array of DWORDs.
; Receives:     ebp + 16 = offset to title
;               ebp + 12 = offset to array
;               ebp + 8  = size of array
; Changes:      nothing
; Returns:      nothing
; Prereqs:      nothing
; -------------------------------------------------------------

    ; set up stack frame, save regs
    push    ebp
    mov     ebp, esp
    pushad 

    ; display title
    mov     edx, [ebp + 16]
    call    WriteString            

    ; divide size of array / 2. Branch to either odd result or even
    mov     eax, [ebp + 8]
    mov     ebx, 2
    xor     edx, edx
    div     ebx
    cmp     edx, 0
    je      evenResult
    jmp     oddResult

oddResult:
    ; if odd, multiply by DWORD to get index 
    mov     esi, [ebp + 12]     ; offset to array
    mov     ebx, 4
    mul     ebx
    mov     eax, [esi + eax]          ; get value stored at at exact median
    jmp     display

evenResult:
    ; if even, calculate mean of middle two values
    mov     esi, [ebp + 12]
    mov     ebx, 4
    mul     ebx
    mov     ebx, eax            ; ebx holds offset of second value
    mov     eax, [esi + ebx]    ; second of two values
    add     eax, [esi + ebx - 4]      ; add second of two values
    mov     ebx, 2
    xor     edx, edx
    div     ebx                 ; eax = arithmetic mean of middle two (truncated)

display:
    ; display result in eax
    call    WriteDec    
    
    ; clean up stack
    popad
    pop     ebp
    ret     12
displayMedian ENDP

displayList PROC
; -------------------------------------------------------------
; Description:      Prints the values in an array to the console in rows of ten.
; Receives:         ebp + 16 = OFFSET to title of array
;                   ebp + 12 = OFFSET to array
;                   ebp + 8  = size of list
; Changes:          nothing
; Returns:          nothing
; Prerequisites:    nothing
; -------------------------------------------------------------

    ; set up stack frame and regs
    push    ebp
    mov     ebp, esp
    pushad

    ; display title
    mov     edx, [ebp + 16]
    call    WriteString
    call    CrLf

    ; set up values for loop
    xor     ecx, ecx
    xor     eax, eax
    mov     ecx, [ebp + 8]      ; size of list for loop
    mov     esi, [ebp + 12]     ; offset to array
    mov     ebx, 0              ; used to find offset from base array pointer

topLoop:
    
    ; check whether to print newline
    mov     eax, ebx
    push    ebx
    mov     ebx, 10
    xor     edx, edx
    div     ebx
    cmp     edx, 0
    jne     noNewLine
    call    CrLf

noNewLine:

    ; print current value
    pop     ebx
    mov     eax, [esi + ebx * 4]
    call    WriteDec

    ; print three whitespaces
    push    ecx         ; save loop counter to do nested loop
    mov     ecx, 3
    mov     eax, ' '

    whiteSpace:
        call    WriteChar     
        loop    whiteSpace

    ; update counters, continue loop
    pop     ecx         ; loop counter
    inc     ebx         ; index counter 
    loop    topLoop

    ; clean up stack
    popad
    pop     ebp
    ret     12

displayList ENDP

farewell PROC
; -------------------------------------------------------------
; Description:  Outputs global string to say goodbye to user
; Receives:     esp + 4 = offset to farewell string
; Changes:      nothing
; Returns:      nothing
; Prereqs: 
; -------------------------------------------------------------

    mov     edx, [esp + 4]
    call    WriteString
    call    CrLf
    ret     4
farewell ENDP

END main