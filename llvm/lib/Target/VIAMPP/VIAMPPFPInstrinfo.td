//===----------------------------------------------------------------------===//
// Floating Point Instructions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// FPU Binary Instructions
//===----------------------------------------------------------------------===//
multiclass FPBinOp<bits<7> opcode, string opstr, SDNode opnode> {

  	def aaa : AAA_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set (f64 VRegs:$dest), (opnode (f64 VRegs:$src1), (f64 VRegs:$src2)))]> {
  	}
  
    let mayLoad = 1 in {
  	def aar : AAR_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set (f64 VRegs:$dest), (opnode (f64 VRegs:$src1), (load addr:$src2)))]> {
  	}
  	}

    let mayLoad = 1 in {
  	def ara : ARA_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set (f64 VRegs:$dest), (opnode (load addr:$src1), (f64 VRegs:$src2)))]> {
  	}
  	}
	                   
    let mayLoad = 1 in {
   	def arr : ARR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(set (f64 VRegs:$dest), (opnode (load addr:$src1), (load addr:$src2)))]> {
    }
    }
                   
    let mayStore = 1 in {
   	def raa : RAA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (f64 VRegs:$src1), (f64 VRegs:$src2)), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rar : RAR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (f64 VRegs:$src1), (load addr:$src2)), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rra : RRA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (load addr:$src1), (f64 VRegs:$src2)), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rrr : RRR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src1, $src2"),
                   [(store (opnode (load addr:$src1), (load addr:$src2)), addr:$dest)]> {
    }
    }

} // multiclass FPBinOp

// Arithmetic
defm FADD  : FPBinOp<0b0110011, "fadd", fadd>;
defm FSUB  : FPBinOp<0b0110100, "fsub", fsub>;
defm FMUL  : FPBinOp<0b0110101, "fmul", fmul>;
defm FDIV  : FPBinOp<0b0110110, "fdiv", fdiv>;
defm FREM  : FPBinOp<0b0110111, "frem", frem>;
defm FMIN  : FPBinOp<0b0111001, "min", fminnum>;
defm FMAX  : FPBinOp<0b0111010, "max", fmaxnum>;
/*
//===----------------------------------------------------------------------===//
// FPU Unary Instructions
//===----------------------------------------------------------------------===//
multiclass FPUnaryOp<bits<7> opcode, string opstr, string desttype, string srctype, SDNode opnode> {
  	def axa : AxA_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set (desttype VRegs:$dest), (opnode (srctype VRegs:$src2)))]> {
  	}
  
    let mayLoad = 1 in {
  	def axr : AxR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(set (desttype VRegs:$dest), (opnode (load (srctype addr:$src2))))]> {
  	}
  	}
                  
    let mayStore = 1 in {
   	def rxa : RxA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (opnode (srctype VRegs:$src2)), (desttype addr:$dest))]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rxr : RxR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (opnode (load (srctype addr:$src2))), (desttype addr:$dest))]> {
    }
    }
} // multiclass FPUnaryOp

// Floating Point <-> Integer Conversion Instructions
defm SItoF	: FPUnaryOp<0b0111110, "si2f", "f64", "i64", sint_to_fp>;
defm UItoF	: FPUnaryOp<0b0111111, "ui2f", "f64", "i64", uint_to_fp>;
defm FtoSI	: FPUnaryOp<0b0111100, "f2si", "i64", "f64", fp_to_sint>;
defm FtoUI	: FPUnaryOp<0b0111101, "f2ui", "i64", "f64", fp_to_uint>;
*/

//===----------------------------------------------------------------------===//
// Branch Instructions
//===----------------------------------------------------------------------===//
multiclass FPB<bits<7> opcode, string opstr, CondCode Cond> {

  let isBranch = 1, isTerminator = 1 in {
  def aai : AAI_Inst<opcode , (outs),
  					(ins VRegs:$dest, VRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (f64 VRegs:$dest), (f64 VRegs:$src1)), bb:$imm16)]> {
  }
  }
  
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def ari : ARI_Inst<opcode , (outs),
  					(ins VRegs:$dest, VIAMPPindirect:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (f64 VRegs:$dest), (load addr:$src1)), bb:$imm16)]> {
  }
  }
    					
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def rai : RAI_Inst<opcode , (outs),
  					(ins VIAMPPindirect:$dest, VRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (load addr:$dest), (f64 VRegs:$src1)), bb:$imm16)]> {
  }
  }

  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def rri : RRI_Inst<opcode , (outs),
  			        (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (load addr:$dest), (load addr:$src1)), bb:$imm16)]> {
  }
  } 					  					

} // multiclass Branch

let isBranch = 1, isTerminator = 1 in {
  defm FBEQ  : FPB<0b1001100, "fbeq", setoeq>;
  defm FBNE  : FPB<0b1001101, "fbne", setone>;
  defm FBG   : FPB<0b1001110, "fbg",  setogt>;
  defm FBGE  : FPB<0b1001111, "fbge", setoge>;
  defm FBL   : FPB<0b1010000, "fbl",  setolt>;
  defm FBLE  : FPB<0b1010001, "fble", setole>;
}

//===----------------------------------------------------------------------===//
// Helper Instructions for building FP Constants
//===----------------------------------------------------------------------===//
def fimm : Operand<f64>;

//===================================
def HI32f64 : SDNodeXForm<fpimm, [{
  uint64_t Val = *N->getValueAPF().bitcastToAPInt().getRawData();
  return CurDAG->getTargetConstant(Val >> 32, SDLoc(N), MVT::i64);
}]>;

def LO32f64 : SDNodeXForm<fpimm, [{
  uint64_t Val = *N->getValueAPF().bitcastToAPInt().getRawData();
  return CurDAG->getTargetConstant(Val & 0xFFFFFFFFULL, SDLoc(N), MVT::i64);
}]>;

def f64lo32 : PatLeaf<(f64 fpimm), [{
  uint64_t Val = *N->getValueAPF().bitcastToAPInt().getRawData();
  return (Val & 0xFFFFFFFFULL) == Val;
}], LO32f64>;

def f64hi32 : PatLeaf<(f64 fpimm), [{
  uint64_t Val = *N->getValueAPF().bitcastToAPInt().getRawData();
  return (Val & 0xFFFFFFFF00000000ULL) == Val;
}], HI32f64>;

let isAsCheapAsAMove = 1, isCodeGenOnly = 1 in {
  def MOVHIf64 : A2I_Inst<0b0001111, (outs VRegs:$dest), (ins fimm:$imm),
                         "mov.h\t$dest, $imm",
                         [(set (f64 VRegs:$dest), f64hi32:$imm)]>;
                         
  def ORIf64 : Ad2I_Inst<0b0010010, (outs VRegs:$dest), (ins VRegs:$src1, fimm:$imm),
                      "or\t$dest, $imm", []> {
                           let Constraints = "$src1 = $dest";

                      }
 }
                     
 def : Pat<(f64 fpimm:$imm), (ORIf64 (MOVHIf64 (HI32f64 fpimm:$imm)),
                            (LO32f64 fpimm:$imm))>;

//===================================
/*

let isPseudo = 1, isCodeGenOnly = 1 in {
	def MOV64 : Pseudo<(outs VRegs:$dest), (ins fimm:$imm),
	                        "# mov64 PSEUDO $imm",
                         []>;
}

//===----------------------------------------------------------------------===//
// Non Instruction Patterns
//===----------------------------------------------------------------------===//
def : Pat<(f64 fpimm:$imm), (MOV64 fpimm:$imm)>;
*/

