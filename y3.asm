                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    li          $t0         62                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _x                             
                    li          $t0         62                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _y                             
                    li          $t0         120                            
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         61                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    lw          $t0         _x                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         10                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         121                            
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         61                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    lw          $t0         _y                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         10                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
L4:                                                                      # ProcWhile
                    lw          $t0         _x                             
                    lw          $t1         _y                             
                    beq         $t0         $t1         L1                 
                    lw          $t0         _x                             
                    lw          $t1         _y                             
                    bge         $t0         $t1         L2                 
                    lw          $t0         _y                             
                    lw          $t1         _x                             
                    sub         $t2         $t0         $t1                
                    sw          $t2         _y                             
                    b           L3                                       # ProcIf
L2:                                                                      # ProcIf
                    lw          $t0         _x                             
                    lw          $t1         _y                             
                    sub         $t2         $t0         $t1                
                    sw          $t2         _x                             
L3:                                                                      # ProcIf
                    b           L4                                       # ProcWhile
L1:                                                                      # ProcWhile
                    li          $t0         103                            
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         61                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    lw          $t0         _x                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $t0         10                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_x:                 .word       0                                          
_y:                 .word       0                                          
