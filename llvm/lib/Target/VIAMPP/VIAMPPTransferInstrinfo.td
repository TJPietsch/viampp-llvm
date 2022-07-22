//===-------------------------------------------------------------------------------------------------------------------------===//
// Data Transfer Instructions
//===-------------------------------------------------------------------------------------------------------------------------===//

//===-------------------------------------------------------------------------------------------------------------------------===//
// MOV
//===-------------------------------------------------------------------------------------------------------------------------===//
multiclass 2OPMOV<bits<7> opcode, string opstr> {
	//- Reg To Reg -------------------------------------------------------------------------
  	def axa : AxA_Inst<opcode, (outs VRegs:$dest), (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t axa"),
                   []> {}
  
	//- Mem To Reg -------------------------------------------------------------------------
    let mayLoad = 1 in {
  		def axr : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),			// long long, long, int 64/64/32 bit
                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
                   [(set (i64 VRegs:$dest), (load addr:$src2))]> {}
  
		let isCodeGenOnly = 1 in {
//  			def axr_c_s : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// char 8 bit sign-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (sextloadi8 addr:$src2))]> {}
  
//  			def axr_c_z : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// char 8 bit zero-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (zextloadi8 addr:$src2))]> {}
  
//  			def axr_c_a : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// char 8 bit any-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (extloadi8 addr:$src2))]> {}
                   
//  			def axr_s_s : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// short 16 bit sign-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (sextloadi16 addr:$src2))]> {}
  
//  			def axr_s_z : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// short 16 bit zero-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (zextloadi16 addr:$src2))]> {}
  
//  			def axr_s_a : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// short 16 bit any-extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (i64 VRegs:$dest), (extloadi16 addr:$src2))]> {}

//  			def axr_h_a : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// half float 16 bit extend
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
//                   [(set (f64 VRegs:$dest), (extloadf16 addr:$src2))]> {}

  			def axr_f_a : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// float 32 bit extend
                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
                   [(set (f64 VRegs:$dest), (extloadf32 addr:$src2))]> {}
                   
  			def axr_d : AxR_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindirect:$src2),	// double 64 bit
                   !strconcat(opstr, "\t$dest, $src2"),//	\t axr"),
                   [(set (f64 VRegs:$dest), (load addr:$src2))]> {}
    	}
	}  
	
	//- Reg To Mem -------------------------------------------------------------------------
    let mayStore = 1 in {
   		def rxa : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),			// long long, long, int 64/64/32 bit
                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
                   [(store (i64 VRegs:$src2), addr:$dest)]> {}
                   
    	let isCodeGenOnly = 1 in {
//		   	def rxa_c : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),	// char 8 bit trunc
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
//                   [(truncstorei8 (i64 VRegs:$src2), addr:$dest)]> {}
                   
//		   	def rxa_s : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),	// short 16 bit trunc
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
//                   [(truncstorei16 (i64 VRegs:$src2), addr:$dest)]> {}
                   
//		   	def rxa_h : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),	// half float 16 bit trunc
//                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
//                   [(truncstoref16 (f64 VRegs:$src2), addr:$dest)]> {}
                   
		   	def rxa_f : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),	// float 32 bit trunc
                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
                   [(truncstoref32 (f64 VRegs:$src2), addr:$dest)]> {}
                   
		   	def rxa_d : RxA_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VRegs:$src2),	// double 64 bit
                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxa"),
                   [(store (f64 VRegs:$src2), addr:$dest)]> {}
		}
    }
	  
	//- Mem To Mem -------------------------------------------------------------------------
    let mayLoad = 1, mayStore = 1 in {
   		def rxr : RxR_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxr"),                  
                   [(store (load addr:$src2), addr:$dest)]> {}
    }
  
	//- Reg To SysReg ----------------------------------------------------------------------
  	def sxa : SxA_Inst<opcode, (outs SRegs:$dest), (ins VRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t sax"),
                   []> {}

	//- Mem To SysReg ----------------------------------------------------------------------
    let mayLoad = 1 in {
  		def sxr : SxR_Inst<opcode, (outs SRegs:$dest), (ins VIAMPPindirect:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t sxr"),
                   [(store (load addr:$src2), SRegs:$dest)]> {}
  	}

	//- SysReg To Reg ----------------------------------------------------------------------
  	def axs : AxS_Inst<opcode, (outs VRegs:$dest), (ins SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t axs"),
                   []> {}
                  
	//- SysReg To Mem ----------------------------------------------------------------------
    let mayStore = 1 in {
	  	def rxs : RxS_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t rxs"),
                   [(store SRegs:$src2, addr:$dest)]> {}
    }

	//- SysReg To SysReg -------------------------------------------------------------------
  	def sxs : SxS_Inst<opcode, (outs SRegs:$dest),(ins SRegs:$src2),
                   !strconcat(opstr, "\t$dest, $src2"),//	\t sxs"),16
                   []> {}
                  
	//- Imm16 To Reg ------------------------------------------------------------------------
    let mayLoad = 1 in {
	   	def axi : AxI_Inst<opcode, (outs VRegs:$dest), (ins i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),//	\t axi"),
                   [(set VRegs:$dest, VIAMPPimm16:$imm16)]> {}
    }

	//- Imm16 To Mem ------------------------------------------------------------------------
    let mayStore = 1 in {
	   	def rxi : RxI_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),//	\t rxi"),
                   [(store VIAMPPimm16:$imm16, addr:$dest)]> {}
    }
                  
	//- Imm16 To SysReg ---------------------------------------------------------------------
   	def sxi : SxI_Inst<opcode, (outs SRegs:$dest), (ins i64imm:$imm16),
                   !strconcat(opstr, "\t$dest, $imm16"),//	\t sxi"),
                   [(set SRegs:$dest, VIAMPPimm16:$imm16)]> {}

	//- Imm32 To Reg ------------------------------------------------------------------------
    let mayLoad = 1 in {
   		def a2i : A2I_Inst<opcode, (outs VRegs:$dest), (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t a2i"),
                   [(set VRegs:$dest, VIAMPPimm32:$imm32)]> {}
    }

	//- Imm32 To Mem ------------------------------------------------------------------------
    let mayStore = 1 in {
   		def r2i : R2I_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t r2i"),
                   [(store VIAMPPimm32:$imm32, addr:$dest)]> {}
    }
                  
	//- Imm32 To SysReg ---------------------------------------------------------------------
   	def s2i : S2I_Inst<opcode, (outs SRegs:$dest), (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t s2i"),
                   [(set SRegs:$dest, VIAMPPimm32:$imm32)]> {}
} // multiclass 2OPMOV

defm MOV	: 2OPMOV<0b0001110, "mov">;

//===-------------------------------------------------------------------------------------------------------------------------===//
// MOVHI move 32bit immediate to upper part of word
//===-------------------------------------------------------------------------------------------------------------------------===//
multiclass 2OPMOVhi<bits<7> opcode, string opstr> {
  	def a2i : A2I_Inst<opcode, (outs VRegs:$dest), (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t .h a2i"),
                   [(set VRegs:$dest, VIAMPPimm32:$imm32)]> {}

    let mayStore = 1 in {
   		def r2i : R2I_Inst<opcode, (outs), (ins VIAMPPindirect:$dest, i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t .h r2i"),
                   [(store (VIAMPPimm32:$imm32), addr:$dest)]> {}
    }

  	def s2i : S2I_Inst<opcode, (outs SRegs:$dest), (ins i64imm:$imm32),
                   !strconcat(opstr, "\t$dest, $imm32"),//	\t .h s2i"),
                   [(set SRegs:$dest, VIAMPPimm32:$imm32)]> {}
} // multiclass 2OPMOVhi

defm MOVHI  : 2OPMOVhi<0b0001111, "mov.h">;

//===-------------------------------------------------------------------------------------------------------------------------===//
// Data Transfer Instructions with Offset
//===-------------------------------------------------------------------------------------------------------------------------===//
multiclass 2OPMOVoff<bits<7> opcode, string opstr> {	
	//- Reg+offset To Reg -------------------------------------------------------------------
    let mayLoad = 1 in {
  		def aao : AAO_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPregoff:$efa),				// long long, long, int 64/64/32 bit
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off aao"),
                   [(set (i64 VRegs:$dest), addrOff:$efa)]> {}
                   
    	let isCodeGenOnly = 1 in {
  			def aao_d : AAO_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPregoff:$efa),		// double 64 bit
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off aao"),
                   [(set (f64 VRegs:$dest), addrOff:$efa)]> {}
        }
  	}

	//- (Reg)+offset To Reg -----------------------------------------------------------------
    let mayLoad = 1 in {
  		def aro : ARO_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPindoff:$efa),				// long long, lon, int 64/64/32 bit
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off aro"),
                   [(set (i64 VRegs:$dest), (load addrOff:$efa))]> {}
                   
    	let isCodeGenOnly = 1 in {
  			def aro_s_d : AAO_Inst<opcode, (outs VRegs:$dest), (ins VIAMPPregoff:$efa),		// double 64 bit
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off aao"),
                   [(set (f64 VRegs:$dest), (load addrOff:$efa))]> {}
        }
	}


	//- Reg+offset To (Reg) -----------------------------------------------------------------
    let mayLoad = 1, mayStore = 1 in {
		def rao : RAO_Inst<opcode, (outs), (ins  VIAMPPindirect:$dest, VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off rao"),
                   [(store addrOff:$efa, addr:$dest)]> {}
  	}

	//- (Reg)+offset To (Reg) ---------------------------------------------------------------
    let mayLoad = 1, mayStore = 1 in {
  		def rro : RRO_Inst<opcode, (outs), (ins  VIAMPPindirect:$dest, VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off rro"),
                   [(store (load addrOff:$efa), addr:$dest)]> {}
  	}

	//- Reg+offset To SysReg ----------------------------------------------------------------
    let mayLoad = 1 in {
  		def sao : SAO_Inst<opcode, (outs SRegs:$dest), (ins VIAMPPregoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off sao"),
                   [(set SRegs:$dest, addrOff:$efa)]> {}
  	}	
  	
	//- (Reg)+offset To SysReg --------------------------------------------------------------
    let mayLoad = 1 in {
  		def sro : SRO_Inst<opcode, (outs SRegs:$dest), (ins VIAMPPindoff:$efa),
                   !strconcat(opstr, "\t$dest, $efa"),//	\t off sro"),
                   [(set SRegs:$dest, (load addrOff:$efa))]> {}
  	}

	//- Reg To Reg+offset -------------------------------------------------------------------
    let mayStore = 1 in {
	   	def aoa : AOA_Inst<opcode, (outs), (ins VIAMPPregoff:$efa, VRegs:$src),				// long long, long, int 64/64/32 bit
                   !strconcat(opstr, "\t$efa, $src"),//	\t off aoa"),
                   [(store (i64 VRegs:$src), addrOff:$efa)]> {}
		
		let isCodeGenOnly = 1 in {
		   	def aoa_d : AOA_Inst<opcode, (outs), (ins VIAMPPregoff:$efa, VRegs:$src),		// double 64 bit
                   !strconcat(opstr, "\t$efa, $src"),//	\t off aoaf"),
                   [(store (f64 VRegs:$src), addrOff:$efa)]> {}
		}               
	}
	
	//- (Reg) To Reg+offset -----------------------------------------------------------------
    let mayStore = 1, mayLoad = 1 in {
	   	def aor : AOR_Inst<opcode, (outs), (ins VIAMPPregoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),//	\t off aor"),
                   [(store (load addr:$src), addrOff:$efa)]> {}
	}
                  
	//- Reg To (Reg)+offset -----------------------------------------------------------------
    let mayStore = 1 in {
	   	def roa : ROA_Inst<opcode, (outs), (ins VIAMPPindoff:$efa, VRegs:$src),				// long long, long, int 64/64/32 bit
                   !strconcat(opstr, "\t$efa, $src"),//	\t off roa"),
                   [(store (i64 VRegs:$src), (load addrOff:$efa))]> {}

		let isCodeGenOnly = 1 in {
	   		def roa_d : ROA_Inst<opcode, (outs), (ins VIAMPPindoff:$efa, VRegs:$src),		// double 64 bit
                   !strconcat(opstr, "\t$efa, $src"),//	\t off roaf"),
                   [(store (f64 VRegs:$src), (load addrOff:$efa))]> {}
		}
	}

	//- (Reg) To (Reg)+offset ---------------------------------------------------------------
    let mayStore = 1, mayLoad = 1 in {
	   	def ror : ROR_Inst<opcode, (outs),(ins VIAMPPindoff:$efa, VIAMPPindirect:$src),
                   !strconcat(opstr, "\t$efa, $src"),//	\t off ror"),
                   [(store (load addr:$src), (load addrOff:$efa))]> {}
	}	

	//- SysReg To Reg+offset ----------------------------------------------------------------
	let mayStore = 1, isCodeGenOnly = 1 in {
	   	def aos : AOS_Inst<opcode, (outs), (ins VIAMPPregoff:$efa, SRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),//	\t off aoa"),
                   [(store (i64 SRegs:$src), addrOff:$efa)]> {}
	
	}

	//- SysReg To (Reg)+offset --------------------------------------------------------------
	let mayStore = 1, isCodeGenOnly = 1 in {
	   	def ros : ROA_Inst<opcode, (outs), (ins VIAMPPindoff:$efa, SRegs:$src),
                   !strconcat(opstr, "\t$efa, $src"),//	\t off roa"),
                   [(store (i64 SRegs:$src), (load addrOff:$efa))]> {}
	
	}

	//- Imm16 To Reg+offset -----------------------------------------------------------------
    let mayStore = 1, AddedComplexity = 100 in {
	   	def aoi : AOI_Inst<opcode, (outs), (ins VIAMPPregoff:$efa, i64imm:$imm16),
                   !strconcat(opstr, "\t$efa, $imm16"),//	\t off aoi"),
                   [(store (VIAMPPimm16:$imm16), addrOff:$efa)]> {}
    }

	//- Imm16 To (Reg)+offset ---------------------------------------------------------------
    let mayStore = 1, mayLoad = 1, AddedComplexity = 100 in {
	   	def roi : ROI_Inst<opcode, (outs), (ins VIAMPPindoff:$efa, i64imm:$imm16),
                   !strconcat(opstr, "\t$efa, $imm16"),//	\t off roi"),
                   [(store (VIAMPPimm16:$imm16), (load addrOff:$efa))]> {}
    }
} // multiclass 2OPMOVoff

defm MOVoff	: 2OPMOVoff<0b0010000, "mov">;
//defm MOVoff	: 2OPMOVoff<0b0010000, "movoff">;

