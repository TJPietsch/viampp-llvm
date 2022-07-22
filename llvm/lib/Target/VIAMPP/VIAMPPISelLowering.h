//===-- VIAMPPISelLowering.h - VIAMPP DAG Lowering Interface -------------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file defines the interfaces that VIAMPP uses to lower LLVM code into a selection DAG.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPISELLOWERING_H
#define VIAMPPISELLOWERING_H

#include "VIAMPP.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class VIAMPPSubtarget;

namespace VIAMPPISD {
enum NodeType {
	FIRST_NUMBER = ISD::BUILTIN_OP_END,

	// Return with a flag operand. Operand 0 is the chain operand.
	RET_FLAG,

	// Branch conditional, condition-code
//	BR_CC,

	// Select the condition code
	SELECT_CC,

	// Wrapper - A wrapper node for TargetConstantPool, TargetExternalSymbol and TargetGlobalAddress.
	Wrapper,

	// CALL - These operations represent an abstract call instruction, which includes a bunch of information.
	CALL,

	// Dynamic Stack allocation for alloca's
	ADJDYNALLOC,

	// Get the Higher/Lower 32 bits of a 64-bit immediate
	HI,
	LO
};
}

//===-------------------------------------------------------------------------------------------------------------------------===//
// TargetLowering Implementation
//===-------------------------------------------------------------------------------------------------------------------------===//

class VIAMPPTargetLowering : public TargetLowering {
public:
	explicit VIAMPPTargetLowering(const VIAMPPTargetMachine &TM, const VIAMPPSubtarget &STI);

	// Provide custom lowering hooks for some operations.
	SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

//	SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerJumpTable(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerConstantPool(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerExternalSymbol(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerFrameIndex(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const;
	SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;

	// This method returns the name of a target specific DAG node.
	const char *getTargetNodeName(unsigned Opcode) const override;

    TargetLowering::ConstraintType getConstraintType(StringRef Constraint) const override;
    std::pair<unsigned, const TargetRegisterClass *> getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
    																			  StringRef Constraint, MVT VT) const override;



private:
	const VIAMPPSubtarget &Subtarget;

	// Lower incoming arguments, copy physregs into vregs
	SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins,
								 const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const override;

	SDValue LowerCCCArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins,
			                  const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const;

	SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const override;

	SDValue LowerCCCCall(SDValue Chain, SDValue Callee, CallingConv::ID CallConv, bool isVarArg, bool isTailCall,
                         const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals,
                         const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL, SelectionDAG &DAG,
					     SmallVectorImpl<SDValue> &InVals) const;

	SDValue LowerCallResult(SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool isVarArg,
		  	  	  	  	    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG,
						    SmallVectorImpl<SDValue> &InVals) const;
  
	bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg, const SmallVectorImpl<ISD::OutputArg> &Outs,
                        LLVMContext &Context) const override;

	SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,  const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL, SelectionDAG &DAG) const override;

	MachineBasicBlock *emitSelect(MachineInstr &MI, MachineBasicBlock *BB) const;
	MachineBasicBlock *emitCallReg(MachineInstr &MI, MachineBasicBlock *BB) const;
	MachineBasicBlock *emitCallIndReg(MachineInstr &MI, MachineBasicBlock *BB) const;
	MachineBasicBlock *emitCallImm(MachineInstr &MI, MachineBasicBlock *BB) const;

	bool shouldConvertConstantLoadToIntImm(const APInt &Imm, Type *Ty) const override {
		return true;
	}
};
}

#endif
