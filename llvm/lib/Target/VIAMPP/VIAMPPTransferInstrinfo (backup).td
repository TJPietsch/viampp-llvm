//===----------------------------------------------------------------------===//
// Data Transfer Instructions
//===----------------------------------------------------------------------===//

// MOV
multiclass 2OPMOV<bits<7> opcode, string opstr> {

  	def axa : AxA_Inst<opcode, (outs VRegs:$dest),
                   (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   []> {
  	}
  
    let mayLoad = 1 in {
  	def axr : AxR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (load addr:$src2), (i64 VRegs:$dest))]> {
  	}
  	}
  
    let mayLoad = 1 in {
  	def axrf : AxR_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (load addr:$src2), (f64 VRegs:$dest))]> {
  	}
  	}
                  
    let mayStore = 1 in {
   	def rxa : RxA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (i64 VRegs:$src2), addr:$dest)]> {
    }
    }
                  
    let mayStore = 1 in {
   	def rxaf : RxA_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (f64 VRegs:$src2), addr:$dest)]> {
    }
    }
  
    let mayLoad = 1, mayStore = 1 in {
   	def rxr : RxR_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (load addr:$src2), addr:$dest)]> {
    }
    }
  
//    let mayLoad = 1, mayStore = 1 in {
//   	def rxrf : RxR_Inst<opcode, (outs),
//                   (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
//                   !strconcat(opstr, "\t$dest, $src2"),
//                   [(store (load (f64 addr:$src2)), (f64 addr:$dest))]> {
//    }
//    }
  
  	def sxa : SxA_Inst<opcode, (outs SRegs:$dest),
                   (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   []> {
  	}

    let mayLoad = 1 in {
  	def sxr : SxR_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store (load (i64 addr:$src2)), SRegs:$dest)]> {
  	}
  	}

  	def axs : AxS_Inst<opcode, (outs VRegs:$dest),
                   (ins SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   []> {
  	}
                  
    let mayStore = 1 in {
   	def rxs : RxS_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   [(store SRegs:$src2, (i64 addr:$dest))]> {
    }
    }

  	def sxs : SxS_Inst<opcode, (outs SRegs:$dest),
                   (ins SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),
                   []> {
  	}
                  
    let mayLoad = 1 in {
   	def axi : AxI_Inst<opcode, (outs VRegs:$dest),
                   (ins i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),
                   [(set (i64 VRegs:$dest), VIAMPPimm16:$imm16)]> {
    }
    }

    let mayStore = 1 in {
   	def rxi : RxI_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),
                   [(store VIAMPPimm16:$imm16, (i64 addr:$dest))]> {
    }
    }
                  
   	def sxi : SxI_Inst<opcode, (outs SRegs:$dest),
                   (ins i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),
                   [(set SRegs:$dest, VIAMPPimm16:$imm16)]> {
    }

    let mayLoad = 1 in {
   	def a2i : A2I_Inst<opcode, (outs VRegs:$dest),
                   (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(set (i64 VRegs:$dest), VIAMPPimm32:$imm32)]> {
    }
    }

    let mayStore = 1 in {
   	def r2i : R2I_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(store VIAMPPimm32:$imm32, (i64 addr:$dest))]> {
    }
    }
                  
   	def s2i : S2I_Inst<opcode, (outs SRegs:$dest),
                   (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(set SRegs:$dest, VIAMPPimm32:$imm32)]> {
    }
} // multiclass 2OPMOV

defm MOV	: 2OPMOV<0b0001110, "mov">;


// MOVHI move 32bit immediate to upper part of word
multiclass 2OPMOVhi<bits<7> opcode, string opstr> {
  	def a2i : A2I_Inst<opcode, (outs VRegs:$dest),
                   (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(set VRegs:$dest, VIAMPPimm32:$imm32)]> {
    }

    let mayStore = 1 in {
   	def r2i : R2I_Inst<opcode, (outs),
                   (ins VIAMPPindirect:$dest, i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(store (VIAMPPimm32:$imm32), addr:$dest)]> {
    }
    }

  	def s2i : S2I_Inst<opcode, (outs SRegs:$dest),
                   (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),
                   [(set SRegs:$dest, VIAMPPimm32:$imm32)]> {
    }

} // multiclass 2OPMOVhi

defm MOVHI  : 2OPMOVhi<0b0001111, "mov.h">;

//===----------------------------------------------------------------------===//
// Data Transfer Instructions with Offset
//===----------------------------------------------------------------------===//

// MOV
multiclass 2OPMOVoff<bits<7> opcode, string opstr> {

// Load like
    let mayLoad = 1 in {
  	def aao : AAO_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set (i64 VRegs:$dest), addrOff:$efa)]> {
  	}
  	}
  	
    let mayLoad = 1 in {
  	def aaof : AAO_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set (f64 VRegs:$dest), addrOff:$efa)]> {
  	}
  	}
  	
    let mayLoad = 1 in {
  	def aro : ARO_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set (i64 VRegs:$dest), (load addrOff:$efa))]> {
  	}
  	}
  	
    let mayLoad = 1 in {
  	def arof : ARO_Inst<opcode, (outs VRegs:$dest),
                   (ins VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set (f64 VRegs:$dest), (load addrOff:$efa))]> {
  	}
  	}

    let mayLoad = 1, mayStore = 1 in {
  	def rao : RAO_Inst<opcode, (outs),
                   (ins  VIAMPPindirect:$dest, VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(store (i64 addrOff:$efa), (i64 addr:$dest))]> {
  	}
  	}

//    let mayLoad = 1, mayStore = 1 in {
//  	def raof : RAO_Inst<opcode, (outs),
//                   (ins  VIAMPPindirect:$dest, VIAMPPregoff:$efa),
//                   !strconcat(opstr, "\t$dest, $efa"),
//                   [(store (f64 addrOff:$efa), (f64 addr:$dest))]> {
//  	}
//  	}
  	
    let mayLoad = 1, mayStore = 1 in {
  	def rro : RRO_Inst<opcode, (outs),
                   (ins  VIAMPPindirect:$dest, VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(store (load addrOff:$efa), addr:$dest)]> {
  	}
  	}
  	
//    let mayLoad = 1, mayStore = 1 in {
//  	def rrof : RRO_Inst<opcode, (outs),
//                   (ins  VIAMPPindirect:$dest, VIAMPPindoff:$efa),
//                   !strconcat(opstr, "\t$dest, $efa"),
//                   [(store (load (f64 addrOff:$efa)), (f64 addr:$dest))]> {
//  	}
//  	}

	// system registers
    let mayLoad = 1 in {
  	def sao : SAO_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set SRegs:$dest, addrOff:$efa)]> {
  	}
  	}
  	
    let mayLoad = 1 in {
  	def sro : SRO_Inst<opcode, (outs SRegs:$dest),
                   (ins VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),
                   [(set SRegs:$dest, (load addrOff:$efa))]> {
  	}
  	}

// Store like                  
    let mayStore = 1 in {
   	def aoa : AOA_Inst<opcode, (outs),
                   (ins VIAMPPregoff:$efa, VRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (i64 VRegs:$src), addrOff:$efa)]> {
    }
	}

    let mayStore = 1 in {
   	def aoaf : AOA_Inst<opcode, (outs),
                   (ins VIAMPPregoff:$efa, VRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (f64 VRegs:$src), addrOff:$efa)]> {
    }
	}
                  
    let mayStore = 1, mayLoad = 1 in {
   	def aor : AOR_Inst<opcode, (outs),
                   (ins VIAMPPregoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (load (i64 addr:$src)), (i64 addrOff:$efa))]> {
    }
	}
                  
    let mayStore = 1, mayLoad = 1 in {
   	def aorf : AOR_Inst<opcode, (outs),
                   (ins VIAMPPregoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (load (f64 addr:$src)), addrOff:$efa)]> {
    }
	}
                  
    let mayStore = 1 in {
   	def roa : ROA_Inst<opcode, (outs),
                   (ins VIAMPPindoff:$efa, VRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (i64 VRegs:$src), (load addrOff:$efa))]> {
    }
	}
                  
    let mayStore = 1 in {
   	def roaf : ROA_Inst<opcode, (outs),
                   (ins VIAMPPindoff:$efa, VRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (f64 VRegs:$src), (load addrOff:$efa))]> {
    }
	}
                  
    let mayStore = 1, mayLoad = 1 in {
   	def ror : ROR_Inst<opcode, (outs),
                   (ins VIAMPPindoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (load (i64 addr:$src)), (load addrOff:$efa))]> {
    }
	}
                  
    let mayStore = 1, mayLoad = 1 in {
   	def rorf : ROR_Inst<opcode, (outs),
                   (ins VIAMPPindoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),
                   [(store (load (f64 addr:$src)), (load addrOff:$efa))]> {
    }
	}

    let mayStore = 1 in {
   	def aoi : AOI_Inst<opcode, (outs),
                   (ins VIAMPPregoff:$efa, i64imm:$imm16),
                   !strconcat(opstr, "\t$efa, $imm16"),
                   [(store (VIAMPPimm16:$imm16), addrOff:$efa)]> {
    }
    }

    let mayStore = 1, mayLoad = 1 in {
   	def roi : ROI_Inst<opcode, (outs),
                   (ins VIAMPPindoff:$efa, i64imm:$imm16),
                   !strconcat(opstr, "\t$efa, $imm16"),
                   [(store (VIAMPPimm16:$imm16), (load addrOff:$efa))]> {
    }
    }
} // multiclass 2OPMOVoff

//defm MOVoff	: 2OPMOVoff<0b0010000, "mov">;
defm MOVoff	: 2OPMOVoff<0b0010000, "movoff">;

