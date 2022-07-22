//===----------------------------------------------------------------------===//
// Control Flow Instructions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Branch Instructions
//===----------------------------------------------------------------------===//
// Branch target for short (immediate) jump targets (printed as pcrel imm values)
def branchtarget : Operand<OtherVT> {
  let EncoderMethod = "getImm16Value";
}

multiclass B<bits<7> opcode, string opstr, CondCode Cond> {

  let isBranch = 1, isTerminator = 1 in {
  def aai : AAI_Inst<opcode , (outs),
  					(ins VRegs:$dest, VRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 VRegs:$dest), (i64 VRegs:$src1)), bb:$imm16)]> {
  }
  }
  
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def ari : ARI_Inst<opcode , (outs),
  					(ins VRegs:$dest, VIAMPPindirect:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 VRegs:$dest), (load (i64 addr:$src1))), bb:$imm16)]> {
  }
  }
  					
   let isBranch = 1, isTerminator = 1 in {
   def asi : ASI_Inst<opcode , (outs),
  					(ins VRegs:$dest, SRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 VRegs:$dest), (i64 SRegs:$src1)), bb:$imm16)]> {
  }
  }
    					
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def rai : RAI_Inst<opcode , (outs),
  					(ins VIAMPPindirect:$dest, VRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (load (i64 addr:$dest)), (i64 VRegs:$src1)), bb:$imm16)]> {
  }
  }
  					
  let isBranch = 1, isTerminator = 1 in {					
  def sai : SAI_Inst<opcode , (outs),
  					(ins SRegs:$dest, VRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 SRegs:$dest), (i64 VRegs:$src1)), bb:$imm16)]> {
  }
  }
    					
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def rsi : RSI_Inst<opcode , (outs),
  					(ins VIAMPPindirect:$dest, SRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (load (i64 addr:$dest)), (i64 SRegs:$src1)), bb:$imm16)]> {
  }
  }

  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def rri : RRI_Inst<opcode , (outs),
  			        (ins VIAMPPindirect:$dest, VIAMPPindirect:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (load (i64 addr:$dest)), (load (i64 addr:$src1))), bb:$imm16)]> {
  }
  } 					  					
  
  let mayLoad = 1, isBranch = 1, isTerminator = 1 in {
  def sri : SRI_Inst<opcode , (outs),
  					(ins SRegs:$dest, VIAMPPindirect:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 SRegs:$dest), (load (i64 addr:$src1))), bb:$imm16)]> {
  }
  }
  					
  let isBranch = 1, isTerminator = 1 in {					
  def ssi : SSI_Inst<opcode , (outs),
  					(ins SRegs:$dest, SRegs:$src1, branchtarget:$imm16),
  					!strconcat(opstr, "\t$dest, $src1, $imm16"),
  					[(brcond (Cond (i64 SRegs:$dest), (i64 SRegs:$src1)), bb:$imm16)]> {
  }
  }
 					
  let isBranch = 1, isTerminator = 1 in {					
  def aii : AII_Inst<opcode , (outs),
  					(ins VRegs:$dest, i64imm:$imm16, branchtarget:$imm16br),
  					!strconcat(opstr, "\t$dest, $imm16, $imm16br"),
  					[(brcond (Cond (i64 VRegs:$dest), VIAMPPimm16:$imm16), bb:$imm16br)]> {
  }
  }
  
  let isBranch = 1, isTerminator = 1 in {					
  def rii : RII_Inst<opcode , (outs),
  					(ins VIAMPPindirect:$dest, i64imm:$imm16, branchtarget:$imm16br),
  					!strconcat(opstr, "\t$dest, $imm16, $imm16br"),
  					[(brcond (Cond (load (i64 addr:$dest)), VIAMPPimm16:$imm16), bb:$imm16br)]> {
  }
  }
  					
  def sii : SII_Inst<opcode , (outs),
  					(ins SRegs:$dest, i64imm:$imm16, branchtarget:$imm16br),
  					!strconcat(opstr, "\t$dest, $imm16, $imm16br"),
                    [(brcond (Cond (i64 SRegs:$dest), VIAMPPimm16:$imm16), bb:$imm16br)]> {
  }

} // multiclass Branch

let isBranch = 1, isTerminator = 1 in {
  defm BEQ  : B<0b0100000, "beq", seteq>;
  defm BNE  : B<0b0100001, "bne", setne>;
  defm BG   : B<0b0100010, "bg",  setgt>;
  defm BGU  : B<0b0100011, "bgu", setugt>;
  defm BGE  : B<0b0100100, "bge", setge>;
  defm BGEU : B<0b0100101, "bgeu", setuge>;
  defm BL   : B<0b0100110, "bl",  setlt>;
  defm BLU  : B<0b0100111, "blu", setult>;
  defm BLE  : B<0b0101000, "ble", setle>;
  defm BLEU : B<0b0101001, "bleu", setule>;
}

           
//===----------------------------------------------------------------------===//
// Call Instructions (pc relative, save return address)
//===----------------------------------------------------------------------===//
def calltarget : Operand<i64> {
  let EncoderMethod = "getImm32Value";
}

// Call register
//let usesCustomInserter = 1 in {
let isCall = 1 in {
  def CallReg : Pseudo<(outs), (ins VRegs:$dest),
                        "# callreg PSEUDO $dest",
                        [(VIAMPPcall VRegs:$dest)]>;
}
//}

// Call (register)  - indirect register
let isCall = 1, usesCustomInserter = 1, Uses = [PC] in {
  def CallIndReg : Pseudo<(outs), (ins VIAMPPindirect:$dest),
                           "# callreg PSEUDO ($dest)",
                           [(VIAMPPcall (load addr:$dest))]>; 
}

// Call immediate, global address, external symbol, ....
let isCall = 1, isPseudo = 1 in {
  def Call : Pseudo<(outs), (ins calltarget:$dest),
                         "# call PSEUDO $dest",
                         []>;
}

// Patterns for Call Operations
def : Pat<(VIAMPPcall (i64 tglobaladdr:$dest)), (Call tglobaladdr:$dest)>;
def : Pat<(VIAMPPcall (i64 texternalsym:$dest)), (Call texternalsym:$dest)>;

def : Pat<(VIAMPPcall imm:$dest), (Call imm:$dest)>;
def : Pat<(VIAMPPcall VRegs:$dest), (CallReg VRegs:$dest)>;
def : Pat<(VIAMPPcall (load VRegs:$dest)), (CallIndReg VRegs:$dest)>;

//===----------------------------------------------------------------------===//
// Return Instruction (pc relative)
//===----------------------------------------------------------------------===//
let isReturn = 1, isTerminator = 1, isBarrier = 1, /*isCodeGenOnly = 1,*/
    dest = 0b0000000000000000, src2 = 0b0000000000000011 in {
  def RET : SxA_Inst<0b0001110, (outs), (ins),
                    "mov\tPC, LinkReg",
                    [(VIAMPPretFlag)]>;
}

//===----------------------------------------------------------------------===//
// Jump Instructions (pc relative)
//===----------------------------------------------------------------------===//
// jump target
def jumptarget : Operand<OtherVT> {
  let EncoderMethod = "getImm32Value";
}

// Jump direct
let isBranch = 1, isTerminator = 1, isBarrier = 1, /*isCodeGenOnly = 1,*/
    dest = 0b0000000000000000 in {
  def JD : Sd2I_Inst<0b0000000, (outs), (ins jumptarget:$imm32),
                   "add\tPC, $imm32",
                   [(br bb:$imm32)]>;
}

// Jump Indirect immediate
let isBranch = 1, isTerminator = 1, isBarrier = 1, isIndirectBranch = 1, 
    isCodeGenOnly = 1, dest = 0b0000000000000000 in {
  def JI : S2I_Inst<0b0000000, (outs), (ins jumptarget:$imm32),
                   "add\tPC, $imm32",
                   [(brind tblockaddress:$imm32)]>;
}

// Jump Indirect register
let isBranch = 1, isTerminator = 1, isBarrier = 1, isIndirectBranch = 1, 
    isCodeGenOnly = 1, dest = 0b0000000000000000 in {
  def JA : Sd2I_Inst<0b0000000, (outs), (ins VRegs:$src2),
                   "add\tPC, $src2",
                   [(brind VRegs:$src2)]>;
}

// Jump Indirect (register)
let isBranch = 1, isTerminator = 1, isBarrier = 1, isIndirectBranch = 1, 
    isCodeGenOnly = 1, dest = 0b0000000000000000 in {
  def JR : Sd2I_Inst<0b0000000, (outs), (ins jumptarget:$src2),
                   "add\tPC, $src2",
                   [(brind (load addr:$src2))]>;
}
