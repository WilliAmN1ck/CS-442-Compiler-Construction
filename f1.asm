                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    lw          $t0         _ab                            
                    addi        $t0         $t0         1                  
                    sw          $t0         _ab                            
                    lw          $t0         _cde                           
                    addi        $t0         $t0         1                  
                    sw          $t0         _cde                           
                    lw          $t0         _ab                            
                    addi        $t0         $t0         -1                 
                    sw          $t0         _ab                            
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_cde:               .word       1                                          
_ab:                .word       1                                          
