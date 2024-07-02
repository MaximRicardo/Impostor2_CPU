            #org 0x0000

;Monitor syntax:
;   ':'     Prints out the address of the last address, example: "000800:" will print out IO[0x000800]. Uses 6 digit hex for the address
;   '.'     Will overwrite the value at the last address, example: "123456 20." will make IO[0x123456] = 0x20. Uses 2 digit hex for the value. "123456: 20." is also valid.
;   ','     Same as '.', however it will increment the last accessed address. Example: "123456 10, 20, 25, 50.", will have this effect: IO[0x123456] = 10, IO[0x123457] = 20, IO[0x123458] = 25, IO[0x123459] = 50.
;   'R'     Executes whatever program is at the last accessed address. Example: "012345R", will make pc = 0x2345 and pb = 0x01. "012345 R" is also valid. 
;   Multiple instructions can be in the same command.
            
            mvi sp 0x0000
            mvi db ramBank

            mvi r0 '\'
            jms PrintChr
            mvi r0 10   ;'\n'
            jms PrintChr
            jms PrintChr

MonitorLoop:
            jms ClearInputBuffer
            jms StoreInput
            jms ExecuteInput

            mvi r0 10   ;'\n'
            jms PrintChr
            jms PrintChr

            jmp MonitorLoop



;void, affects r0, r1, r2, r3
ClearInputBuffer:
            mvi r0 0    ;In order to clear, every byte will be set to 0
            mvi r1 2    ;Increment by 2 bytes at a time, because a word will be written at each iteration
            mvi r3 0    ;Indexing register
            mvi r2 inputBufferSize  ;Size of the buffer

    ClrInBffr_ClrLoop:
                cmp r3 r2   ;Continue the loop until the end of the buffer has been reached
                jic ClrInBffr_EndClrLoop

                stw r0 r3 inputBuffer   ;Clear the current word (2 bytes) of the buffer

                add r3 r1   ;Increment the index by 2

                jmp ClrInBffr_ClrLoop

    ClrInBffr_EndClrLoop:
                ret


;void, affects r0, r1, r2, r3
StoreInput: mvi r3 0    ;Indexing register
            mvi r2 inputBufferSize

    StoreInput_Loop:
                jms GetInput

                cmi r0 0    ;NULL
                jie StoreInput_Loop

                jms PrintChr

                cmi r0 10   ;'\n'
                jie StoreInput_End

                cmi r0 8    ;Backspace
                jie StoreInput_Loop_Backspace

                stw r0 r3 inputBuffer

                adi r3 1
                cmp r3 r2
                jnc StoreInput_Loop

    StoreInput_End:
                ret

        StoreInput_Loop_Backspace:

                    cmi r3 0
                    jie StoreInput_Loop

                    sbi r3 1
                    mvi r0 0
                    stw r0 r3 inputBuffer

                    jmp StoreInput_Loop


;void, affects r0, r1, r2, r3
ExecuteInput:
            mvi r3 0

    ExecuteInput_Loop:
                ldw r0 r3 inputBuffer
                ani r0 0x00ff   ;Clear the MSB
                
                adi r3 1

                cmi r0 0
                jie ExecuteInput_End

                cmi r3 inputBufferSize
                jic ExecuteInput_End

                cmi r0 ' '
                jie ExecuteInput_Loop
                cmi r0 'R'
                jie ExecuteInput_Branch

                jms IsDigit
                cmi r0 16
                jne ExecuteInput_Digit

                jmp ExecuteInput_Loop

        ExecuteInput_Digit:

                mov r1 r0   ;Keep the current digit

                ;Get the next digit
                ldw r0 r3 inputBuffer
                ani r0 0x00ff   ;Clear the MSB

                adi r3 1

                jms IsDigit
                cmi r0 16
                jie ExecuteInput_Digit_ReadLoop ;Go to the next loop

                sli r1 4    ;Get the full 8 bit value
                add r1 r0

                mov r2 r1   ;Store it in r2

            ExecuteInput_Digit_ReadLoop:
                    ;Get the next digit
                    
                    ldw r0 r3 inputBuffer
                    ani r0 0x00ff   ;Clear the MSB

                    adi r3 1

                    jms IsDigit
                    cmi r0 16
                    jie ExecuteInput_Digit_ReadLoop_End

                    sli r1 4
                    add r1 r0

                    jmp ExecuteInput_Digit_ReadLoop

            ExecuteInput_Digit_ReadLoop_End:

                    sbi r3 1

                    ldw r0 r3 inputBuffer
                    ani r0 0x00ff   ;Clear the MSB

                    adi r3 1

                    cmi r0 '.'  ;If the number ends in '.', then overwrite the last dereferenced number
                    jie ExecuteInput_Digit_WriteDeref
                    cmi r0 ','  ;If the number ends in ',', then overwrite the last dereferenced number, then increment the address
                    jie ExecuteInput_Digit_WriteDeref

                    siw r2 lastAddressBank
                    siw r1 lastAddressLSB

                    cmi r0 ':'  ;If the number ends in ':', then print the dereferenced number
                    jie ExecuteInput_Digit_PrintDeref
                    cmi r0 'R'  ;If the number ends in 'R' then branch
                    jie ExecuteInput_Branch

                    jmp ExecuteInput_Loop

            ExecuteInput_Digit_PrintDeref:
                    ;Print a new line
                    mvi r0 10           ;'\n'
                    jms PrintChr

                    mov db r2           ;Move to the correct bank
                    ldw r0 r1 0x0000    ;Dereference r1
                    mvi db ramBank      ;Move back to the ram bank

                    jms PrintByte       ;Print the byte

                    jmp ExecuteInput_Loop   ;Go back to the start

            ExecuteInput_Digit_WriteDeref:
                    psh r0  ;Keep the character at the end of the number

                    ;Print a new line
                    mvi r0 10           ;'\n'
                    jms PrintChr

                    ;Print the address
                    psh r2

                    liw r2 lastAddressBank
                    ani r2 0xff
                    liw r1 lastAddressLSB

                    mov r0 r2   ;Print bank
                    psh r1
                    jms PrintByte
                    pop r1

                    mov r0 r1   ;Print MSB
                    sri r0 8
                    ani r0 0xff
                    psh r1
                    jms PrintByte
                    pop r1

                    mov r0 r1   ;Print LSB
                    ani r0 0xff
                    psh r1
                    jms PrintByte
                    pop r1

                    mvi r0 ':'
                    jms PrintChr
                    mvi r0 ' '
                    jms PrintChr

                    pop r2

                    mov r0 r2
                    psh r1
                    jms PrintByte   ;Print the byte that will be written with
                    pop r1

                    liw db lastAddressBank  ;Get the correct bank
                    ldw r0 r1 0x0000
                    ani r0 0xff00           ;Logical OR the high byte with the second byte at the address
                    bor r2 r0               ;This makes sure only one byte gets written
                    stw r2 r1 0x0000        ;Write the byte

                    mvi db ramBank          ;Go back to the ram bank

                    pop r0  ;Get the character at the end of the number again
                    cmi r0 ','
                    jne ExecuteInput_Loop

                    adi r1 1    ;If it ended in ',', then increment the address
                    siw r1 lastAddressLSB

                    jmp ExecuteInput_Loop

        ExecuteInput_Branch:
                mvi r0 10   ;'\n'
                jms PrintChr

                mvi r0 'R'
                jms PrintChr
                mvi r0 10   ;'\n'
                jms PrintChr

                ;Branch to the last accessed address

                ;The address to branch to is in r1, and the bank is in r2

                psh db          ;Move the data bank to whatever bank the program is in
                mov db pb

                siw r1 JAB_Address  ;Store the address to jump to as an argument to the jab opcode
                siw r2 JAB_Bank     ;Store the bank to jump to as an argument to the jab opcode

                pop db

                ;This should be illegal, however it is the only way to jump to a dynamic 24 bit address
                ;The last addressed bank and address will be stored here, and then will act as operands to the jab opcode
                ;This means that the program CANNOT be put in ROM, and must be put within read AND write memory. Or atleast these following 6 bytes must be.
                jab
        JAB_Bank:       nop     ;The bank with padded with one byte of zeroes. Looks like this: 00000000xxxxxxxx, where x is the bank to branch to
        JAB_Address:    nop     ;The rest is the lower 2 bytes of the address

                ;ret                     ;Branch to the address on the stack using ret

    ExecuteInput_End:
            ret


;int (int digit r0)
IsDigit:    cmi r0 '0'              ;If it's less than '0' it isn't a number
            jnc IsHexDigit_NotDigit
            cmi r0 ':'  ;'9'+1       If (r0 >= '0' && r0 < ':'), it's a decimal digit
            jnc IsHexDigit_DecDigit

            cmi r0 'a'              ;If (r0 < 'a' && notDecimal) then it's not a number
            jnc IsHexDigit_NotDigit
            cmi r0 'g'  ;'f'+1       If (r0 >= 'a' && < 'g') then it's a hex digit
            jnc IsHexDigit_HexDigit

    IsHexDigit_NotDigit:            ;Else it's not a digit
            mvi r0 16               ;Return 16
            ret

    IsHexDigit_DecDigit:            ;Return the value of the digit
            sbi r0 '0'
            ret

    IsHexDigit_HexDigit:            ;Return the value of the digit
            sbi r0 'W'  ;'a'-10
            ret


;void (int digit r0), affects r0
PrintDigit: cmi r0 10   ;9+1
            jnc PrintDigit_Dec

            ;Convert from int to hex digit >= a
            adi r0 'W'  ;'a'-10
            jms PrintChr
            ret

    PrintDigit_Dec:
            ;Convert from int to hex digit < a
            adi r0 '0'
            jms PrintChr
            ret


;void (int num r0), affects r0, r1
PrintByte:  mov r1 r0

            ;Print the first digit
            sri r0 4
            ani r0 0x000f
            jms PrintDigit
            mov r0 r1

            ;Print the second digit
            ani r0 0x000f
            jms PrintDigit

            ret


;int
GetInput:   mvi db 0x0003    ;Reads one character from the keyboard and returns it in r0
            liw r0 0x0000
            mvi db ramBank
            ret


;void
PrintChr:   mvi db ttyBank  ;Print the character in r0
            siw r0 0x0000
            mvi db ramBank
            ret



inputBank: = 3
ttyBank: = 2
ramBank: = 1
prgBank: = 0

inputBufferSize: = 0xff ;Size of input buffer
inputBuffer: = 0x0000   ;Location of input buffer in RAM

lastAddressBank: = 0x0100   ;Big endian makes it easier to store without overwriting other values
lastAddressLSB: = 0x0101
lastAddressMSB: = 0x0102
