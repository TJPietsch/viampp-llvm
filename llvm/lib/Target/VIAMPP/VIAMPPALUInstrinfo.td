//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instructions that have a dag pattern (ADD, SUB, MUL, DIV, REM, AND, OR, XOR
//							LSL, LSR, ASR, RL, RR)
// and instructions with dedicated pattern (NAND, NOR, NXOR, SB, CB)
//===----------------------------------------------------------------------===//

// Pattern Fragments
def cb : PatFrag<(ops node:$lhs, node:$rhs), (and node:$lhs, (not node:$rhs))>;
def nand : PatFrag<(ops node:$lhs, node:$rhs), (not (and node:$lhs, node:$rhs))>;
def nor : PatFrag<(ops node:$lhs, node:$rhs), (not (or node:$lhs, node:$rhs))>;
def nxor : PatFrag<(ops node:$lhs, node:$rhs), (not (xor node:$lhs, node:$rhs))>;

multiclass IntBinOp<bits<7> opcode, string opstr, SDNode opnode> {

  	def aaa : AAA_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode VRegs:$src1, VRegs:$src2))]> {
  	}
  
    let mayLoad = 1 in {
  	def aar : AAR_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode VRegs:$src1, (load addr:$src2)))]> {
  	}
  	}

    let mayLoad = 1 in {
  	def ara : ARA_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode (load addr:$src1), VRegs:$src2))]> {
  	}
  	}
	                   
    let mayLoad = 1 in {
   	def arr : ARR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode (load addr:$src1), (load addr:$src2)))]> {
    }
    }
                   
    let mayStore = 1 in {
   	def raa : RAA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode VRegs:$src1, VRegs:$src2), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rar : RAR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode VRegs:$src1, (load addr:$src2)), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rra : RRA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (load addr:$src1), VRegs:$src2), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rrr : RRR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (load addr:$src1), (load addr:$src2)), addr:$dest)]> {
    }
    }

  	def saa : SAA_Inst<opcode, (outs SRegs:$dest),
                   (ins VRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode VRegs:$src1, VRegs:$src2))]> {
  	}
  
    let mayLoad = 1 in {
  	def sar : SAR_Inst<opcode, (outs SRegs:$dest),
                   (ins VRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode VRegs:$src1, (load addr:$src2)))]> {
  	}
  	}
 
    let mayLoad = 1 in {
  	def sra : SRA_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindirect:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode (load addr:$src1), VRegs:$src2))]> {
  	}
  	}
	                   
    let mayLoad = 1 in {
   	def srr : SRR_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindirect:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode (load addr:$src1), (load addr:$src2)))]> {
    }
    }

   	def aai : AAI_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set VRegs:$dest, (opnode VRegs:$src1, VIAMPPimm16:$imm16))]> {
    }

//   	def aia : AIA_Inst<opcode, (outs VRegs:$dest),
//                   (ins i64imm:$imm16, VRegs:$src2),
//                   !strconcat(opstr, "\t$dest, $src1, $src2"),
//                   [(set VRegs:$dest, (opnode VIAMPPimm16:$imm16, VRegs:$src2))]> {
//    }
                   
    let mayLoad = 1 in {
   	def ari : ARI_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set VRegs:$dest, (opnode (load addr:$src1), VIAMPPimm16:$imm16))]> {
    }
    }
                   
//    let mayLoad = 1 in {
//   	def air : AIR_Inst<opcode, (outs VRegs:$dest),
//                   (ins i64imm:$imm16, VIAMPPindirect:$src2),
//                   !strconcat(opstr, "\t$dest, $imm16, $src2"),
//                   [(set VRegs:$dest, (opnode VIAMPPimm16:$imm16, (load addr:$src2)))]> {
//    }
//    }
                   
    let mayStore = 1 in {
   	def rai : RAI_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(store (opnode VRegs:$src1, VIAMPPimm16:$imm16), addr:$dest)]> {
    }
    }
                   
//    let mayStore = 1 in {
//   	def ria : RIA_Inst<opcode, (outs),
//                   (ins VIAMPPindirect:$dest, i64imm:$imm16, VRegs:$src2),
//                   !strconcat(opstr, "\t$dest, $imm16, $src2"),
//                   [(store (opnode VIAMPPimm16:$imm16, VRegs:$src2), addr:$dest)]> {
//    }
//    }
                   
    let mayLoad = 1, mayStore = 1 in {
   	def rri : RRI_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(store  (opnode (load addr:$src1), VIAMPPimm16:$imm16), addr:$dest)]> {
    }
    }
                   
//    let mayLoad = 1, mayStore = 1 in {
//   	def rir : RIR_Inst<opcode, (outs),
//                   (ins VIAMPPindirect:$dest, i64imm:$imm16, VIAMPPindirect:$src2),
//                   !strconcat(opstr, "\t$dest, $imm16, $src2"),
//                   [(store  (opnode VIAMPPimm16:$imm16, (load addr:$src2)), addr:$dest)]> {
//    }
//    }
                   
//   	def aii : AII_Inst<opcode, (outs),
//                   (ins VRegs:$dest, i64imm:$imm16, i64imm:$imm16br),
//                   !strconcat(opstr, "\t$dest, $imm16, $imm16br"),
//                   [(set VRegs:$dest, (opnode VIAMPPimm16:$imm16, VIAMPPimm16:$im16br))]> {
//    }

//   	def sii : SII_Inst<opcode, (outs SRegs:$dest),
//                   (ins i64imm:$1imm16, i64imm:$imm16br),
//                   !strconcat(opstr, "\t$dest, $imm16, $imm16br"),
//                   [(set SRegs:$dest, (opnode VIAMPPimm16:$imm16, VIAMPPimm16:$imm16br))]> {
//    }

   	def sai : SAI_Inst<opcode, (outs SRegs:$dest),
                   (ins VRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set SRegs:$dest, (opnode VRegs:$src1, VIAMPPimm16:$imm16))]> {
    }
                   
    let mayLoad = 1 in {
   	def sri : SRI_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindirect:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set SRegs:$dest, (opnode (load addr:$src1), VIAMPPimm16:$imm16))]> {
    }
    }

//   	def sia : SIA_Inst<opcode, (outs SRegs:$dest),
//                   (ins i64imm:$imm16, VRegs:$src2),
//                   !strconcat(opstr, "\t$dest, $imm16, $src2"),
//                   [(set SRegs:$dest, (opnode VIAMPPimm16:$imm16, VRegs:$src2))]> {
//    }
                   
//    let mayLoad = 1 in {
//   	def sir : SIR_Inst<opcode, (outs SRegs:$dest),
//                   (ins i64imm:$imm16, VIAMPPindirect:$src2),
//                   !strconcat(opstr, "\t$dest, $imm16, $src2"),
//                   [(set SRegs:$dest, (opnode VIAMPPimm16:$imm16, (load addr:$src2)))]> {
//    }
//    }
  
  	def asa : ASA_Inst<opcode, (outs VRegs:$dest),
                   (ins SRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode SRegs:$src1, VRegs:$src2))]> {
  	}
 
    let mayLoad = 1 in { 
  	def asr : ASR_Inst<opcode, (outs VRegs:$dest),
                   (ins SRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode SRegs:$src1, (load addr:$src2)))]> {
  	}
	}                   
  
  	def asi : ASI_Inst<opcode, (outs VRegs:$dest),
                   (ins SRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set VRegs:$dest, (opnode SRegs:$src1, VIAMPPimm16:$imm16))]> {
  	}
   
    let mayStore = 1 in {
   	def rsa : RSA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, SRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode SRegs:$src1, VRegs:$src2), addr:$dest)]> {
    }
    }
   
    let mayLoad = 1, mayStore = 1 in {
   	def rsr : RSR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, SRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode SRegs:$src1, (load addr:$src2)), addr:$dest)]> {
    }
    }
   
    let mayStore = 1 in {
   	def rsi : RSI_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, SRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(store (opnode SRegs:$src1, VIAMPPimm16:$imm16), addr:$dest)]> {
    }
    }
   
   	def ssa : SSA_Inst<opcode, (outs SRegs:$dest),
                   (ins SRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode SRegs:$src1, VRegs:$src2))]> {
    }
   
    let mayLoad = 1 in {
   	def ssr : SSR_Inst<opcode, (outs SRegs:$dest),
                   (ins SRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set SRegs:$dest, (opnode SRegs:$src1, (load addr:$src2)))]> {
    }
    }
   
   	def ssi : SSI_Inst<opcode, (outs SRegs:$dest),
                   (ins SRegs:$src1, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $src1, $imm16"),
                   [(set SRegs:$dest, (opnode SRegs:$src1, VIAMPPimm16:$imm16))]> {
    }
  
  	def aas : AAS_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode VRegs:$src1, SRegs:$src2))]> {
  	}
	                   
    let mayLoad = 1 in {
   	def ars : ARS_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set VRegs:$dest, (opnode (load addr:$src1), SRegs:$src2))]> {
    }
    }
   
    let mayStore = 1 in {
   	def ras : RAS_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode VRegs:$src1, SRegs:$src2), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rrs : RRS_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (load addr:$src1), SRegs:$src2), addr:$dest)]> {
    }
    }
      
    let Constraints = "$src1 = $dest" in {
    def axa : AxA_Inst<opcode, (outs VRegs:$dest),
    			   (ins VRegs:$src1, VRegs:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   [(set VRegs:$dest, (opnode VRegs:$src1, VRegs:$src2))]> {
    }
    }

} // multiclass IntBinOp

// Arithmetic
defm ADD  : IntBinOp<0b0000000, "add", add>;
defm ADDU : IntBinOp<0b0000001, "addu", add>;
defm SUB  : IntBinOp<0b0000010, "sub", sub>;
defm SUBU : IntBinOp<0b0000011, "subu", sub>;
defm MUL  : IntBinOp<0b0000100, "mul", mul>;
defm DIV  : IntBinOp<0b0000110, "div", sdiv>;
defm DIVU : IntBinOp<0b0000111, "divu", udiv>;
defm REM  : IntBinOp<0b0001000, "rem", urem>;
defm REMU : IntBinOp<0b0001001, "remu", srem>;
defm MIN  : IntBinOp<0b0001010, "min", smin>;
defm MINU : IntBinOp<0b0001011, "minu", umin>;
defm MAX  : IntBinOp<0b0001100, "max", smax>;
defm MAXU : IntBinOp<0b0001101, "maxu", umax>;

// Logic
defm AND  : IntBinOp<0b0010001, "and", and>;
defm OR   : IntBinOp<0b0010010, "or",  or>;
defm XOR  : IntBinOp<0b0010011, "xor", xor>;
defm NAND : IntBinOp<0b0010101, "nand", nand>;
defm NOR  : IntBinOp<0b0010110, "nor", nor>;
defm NXOR : IntBinOp<0b0010111, "nxor", nxor>;
defm SB   : IntBinOp<0b0011000, "sb", or>;
defm CB   : IntBinOp<0b0011001, "cb", cb>;

// Shift & Rotate
defm LSL  : IntBinOp<0b0011010, "lsl", shl>;
defm LSR  : IntBinOp<0b0011011, "lsr", srl>;
defm ASR  : IntBinOp<0b0011100, "asr", sra>;
defm RL   : IntBinOp<0b0011101, "rl", rotl>;
defm RR   : IntBinOp<0b0011110, "rr", rotr>;

//===----------------------------------------------------------------------===//
// Instructions that need their own dag pattern
//===----------------------------------------------------------------------===//

// NOT
multiclass IntUnaryOp<bits<7> opcode, string opstr> {
  	def axa : AxA_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set VRegs:$dest, (not VRegs:$src2))]> {
  	}
  
    let mayLoad = 1 in {
  	def axr : AxR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set VRegs:$dest, (not (load addr:$src2)))]> {
  	}
  	}
                  
    let mayStore = 1 in {
   	def rxa : RxA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (not VRegs:$src2), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rxr : RxR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (not (load addr:$src2)), addr:$dest)]> {
    }
    }
    
  	def sxa : SxA_Inst<opcode, (outs SRegs:$dest),
                   (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set SRegs:$dest, (not VRegs:$src2))]> {
  	}
  
    let mayLoad = 1 in {
  	def sxr : SxR_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set SRegs:$dest, (not (load addr:$src2)))]> {
  	}
  	}

  	def axs : AxS_Inst<opcode, (outs VRegs:$dest),
                   (ins SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set VRegs:$dest, (not SRegs:$src2))]> {
  	}
                  
    let mayStore = 1 in {
   	def rxs : RxS_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (not SRegs:$src2), addr:$dest)]> {
    }
    }
} // multiclass IntUnaryOp

defm NOT	: IntUnaryOp<0b0010100, "not">;

//===----------------------------------------------------------------------===//
// Instructions with Source equal Destination
//===----------------------------------------------------------------------===//

multiclass DS<bits<7> opcode, string opstr, SDNode opnode> {
    def ad2i : Ad2I_Inst<opcode, (outs VRegs:$dest), (ins VRegs:$src1, i64imm:$imm32),
//    def ad2i : Ad2I_Inst<opcode, (outs VRegs:$dest), (ins VRegs:$src1, i32imm:$imm32),
                     !strconcat(opstr, "\t$dest, $imm32"),
                     [(set VRegs:$dest, (opnode VRegs:$src1, VIAMPPimm32:$imm32))]> {
                     let Constraints = "$src1 = $dest";
                     let AddedComplexity = 100;
    } 
           
    def sd2i : Sd2I_Inst<opcode, (outs SRegs:$dest), (ins SRegs:$src1, i64imm:$imm32),
//    def sd2i : Sd2I_Inst<opcode, (outs SRegs:$dest), (ins SRegs:$src1, VIAMPPimm32:$imm32),
                     !strconcat(opstr, "\t$dest, $imm32"),
                     [(set SRegs:$dest, (opnode SRegs:$src1, VIAMPPimm32:$imm32))]> {
                     let Constraints = "$src1 = $dest";
    }        
} // multiclass DS

defm ADD : DS<0b0000000, "add", add>;
defm SUB : DS<0b0000010, "sub", sub>;
defm OR  : DS<0b0010010, "or",  or>;    
    