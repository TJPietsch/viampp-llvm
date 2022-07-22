//===----------------------------------------------------------------------===//
// Sleep Instruction
//===----------------------------------------------------------------------===//
// SLEEP
  def SLEEP : xxx_Inst<0b0110000, (outs), (ins), "sleep", []>;

//===----------------------------------------------------------------------===//
// Signal Instructions
//===----------------------------------------------------------------------===//
multiclass SIG<bits<7> opcode, string opstr> {
    def axa : AxA_Inst<opcode, (outs),
    			   (ins VRegs:$dest, VRegs:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>; 
                    
    let mayLoad = 1 in {
    def axr : AxR_Inst<opcode, (outs),
    			   (ins VRegs:$dest, VIAMPPindirect:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>;
    }

    let mayLoad = 1 in {
    def rxa : RxA_Inst<opcode, (outs),
    			   (ins VIAMPPindirect:$dest, VRegs:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>;
    }
                    
    let mayLoad = 1 in {
    def rxr : RxR_Inst<opcode, (outs),
    			   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>;
    }

    def axs : AxS_Inst<opcode, (outs),
    			   (ins VRegs:$dest, SRegs:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>; 
 
    let mayLoad = 1 in {
    def rxs : RxS_Inst<opcode, (outs),
    			   (ins VIAMPPindirect:$dest, SRegs:$src2),
    			   !strconcat(opstr, "\t$dest, $src2"),
    			   []>;
    }
 
    def axi : AxI_Inst<opcode, (outs),
    			   (ins VRegs:$dest, i16imm:$imm16),
    			   !strconcat(opstr, "\t$dest, $imm16"),
    			   []>; 

    let mayLoad = 1 in {
    def rxi : RxI_Inst<opcode, (outs),
    			   (ins VIAMPPindirect:$dest, i16imm:$imm16),
    			   !strconcat(opstr, "\t$dest, $imm16"),
    			   []>;
    }
 
    def a2i : A2I_Inst<opcode, (outs),
    			   (ins VRegs:$dest, i32imm:$imm32),
    			   !strconcat(opstr, "\t$dest, $imm32"),
    			   []>; 

    let mayLoad = 1 in {
    def r2i : R2I_Inst<opcode, (outs),
    			   (ins VIAMPPindirect:$dest, i32imm:$imm32),
    			   !strconcat(opstr, "\t$dest, $imm32"),
    			   []>;
    }
    
    let mayLoad = 1 in {
  	def aao : AAO_Inst<opcode, (outs),
                   (ins VRegs:$dest, VIAMPPregoff:$src1),
                   !strconcat(opstr, "\t$dest, $src1"),
                   []>;
  	}
  	
    let mayLoad = 1 in {
  	def aro : ARO_Inst<opcode, (outs),
                   (ins VRegs:$dest, VIAMPPindoff:$src1),
                   !strconcat(opstr, "\t$dest, $src1"),
                   []>;
  	}

    let mayLoad = 1, mayStore = 1 in {
  	def rao : RAO_Inst<opcode, (outs),
                   (ins  VIAMPPindirect:$dest, VIAMPPregoff:$src1),
                   !strconcat(opstr, "\t$dest, $src1"),
                   []>;
  	}
  	
    let mayLoad = 1, mayStore = 1 in {
  	def rro : RRO_Inst<opcode, (outs),
                   (ins  VIAMPPindirect:$dest, VIAMPPindoff:$src1),
                   !strconcat(opstr, "\t$dest, $src1"),
                   []>;
  	}
} 

defm SIGNAL : SIG<0b0110001,"signal">;    
