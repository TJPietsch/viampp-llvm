//===-- VIAMPPISelLowering.cpp - VIAMPP DAG Lowering Implementation ----------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file implements the VIAMPPTargetLowering class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPPISelLowering.h"
#include "VIAMPP.h"
#include "VIAMPPMachineFunctionInfo.h"
#include "VIAMPPSubtarget.h"
#include "VIAMPPTargetMachine.h"
#include "MCTargetDesc/VIAMPPBaseInfo.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "VIAMPP-lower"

VIAMPPTargetLowering::VIAMPPTargetLowering(const VIAMPPTargetMachine &TM, const VIAMPPSubtarget &STI) : TargetLowering(TM), Subtarget(STI) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::VIAMPPTargetLowering\n";
//	outs().changeColor(raw_ostream::WHITE,0);


	// Set up the register classes.
	addRegisterClass(MVT::i64, &VIAMPP::VRegsRegClass);
	addRegisterClass(MVT::f64, &VIAMPP::VRegsRegClass);
//	addRegisterClass(MVT::i64, &VIAMPP::SRegsRegClass);


	// Compute derived properties from the register classes
	computeRegisterProperties(STI.getRegisterInfo());

	// Provide all sorts of necessary set operation action calls
	setStackPointerRegisterToSaveRestore(VIAMPP::StackPtr);	// StackPtr
  
	// TODO: add other  setOperationAction calls if necessary

	// Only bit 0 counts, the rest can hold garbage.
	setBooleanContents(UndefinedBooleanContent);

	// Function alignments (log2)
	setMinFunctionAlignment(Align(8));
	setPrefFunctionAlignment(Align(8));


	// Set Operations
	// setOperationAction  Indicate that the specified operation does not work with the specified type and indicate what to do about
	// it. This enum indicates whether operations are valid for a target, and if not, what action should be used to make them valid.
	// The possible values are:
	//     Legal,       The target natively supports this operation.
	//     Promote,     This operation should be executed in a larger type.
	//     Expand,      Try to expand this to other ops, otherwise use a libcall.
	//     LibCall,     Don't try to expand this to other ops, always use a libcall.
	//     Custom       Use the LowerOperation hook to implement custom lowering.

	// Nodes that require custom lowering
	setOperationAction(ISD::BR_JT,              MVT::Other, Expand);
	setOperationAction(ISD::BR_CC,              MVT::i64,   Expand);
//  setOperationAction(ISD::SELECT,             MVT::i64,   Expand);
	setOperationAction(ISD::SELECT_CC,          MVT::i64,   Custom);


	setOperationAction(ISD::GlobalAddress,      MVT::i64,   Custom);
	setOperationAction(ISD::BlockAddress,       MVT::i64,   Custom);
	setOperationAction(ISD::JumpTable,          MVT::i64,   Custom);
	setOperationAction(ISD::ConstantPool,       MVT::i64,   Custom);
	setOperationAction(ISD::ExternalSymbol,     MVT::i64,   Custom);
  
	// Varargs support
	setOperationAction(ISD::VASTART,            MVT::Other, Custom);
	setOperationAction(ISD::VAARG,              MVT::Other, Expand);
	setOperationAction(ISD::VAEND,              MVT::Other, Expand);
	setOperationAction(ISD::VACOPY,             MVT::Other, Expand);

	// Dynamic stack allocation
	setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i64,   Custom);
	setOperationAction(ISD::STACKSAVE,          MVT::Other, Expand);
	setOperationAction(ISD::STACKRESTORE,       MVT::Other, Expand);

	for (MVT VT : MVT::integer_valuetypes()) {
//	    if (VT == MVT::i64)
//	    	continue;

	    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1, Promote);
	    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1, Promote);
	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::i1, Promote);

//	    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i8, Promote);	//expand
//	    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i8, Promote);	//expand
//	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::i8, Promote);	//expand

//	    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i16, Promote);	//expand
//	    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i16, Promote);	//expand
//	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::i16, Promote);	//expand

//	    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i32, Promote);	//expand
//	    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i32, Promote);	//expand
//	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::i32, Promote);	//expand
	  }

//	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
//	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
//	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);


//	AddPromotedToType(ISD::STORE, MVT::i1, MVT::i64);
//	AddPromotedToType(ISD::STORE, MVT::i8, MVT::i64);
//	AddPromotedToType(ISD::STORE, MVT::i16, MVT::i64);
//	AddPromotedToType(ISD::STORE, MVT::i32, MVT::i64);
//	AddPromotedToType(ISD::STORE, MVT::f32, MVT::f64);

//	AddPromotedToType(ISD::LOAD, MVT::i1, MVT::i64);
//	AddPromotedToType(ISD::LOAD, MVT::i8, MVT::i64);
//	AddPromotedToType(ISD::LOAD, MVT::i16, MVT::i64);
//	AddPromotedToType(ISD::LOAD, MVT::i32, MVT::i64);
//	AddPromotedToType(ISD::LOAD, MVT::f32, MVT::f64);

//	setTruncStoreAction(MVT::i64, MVT::i1, Expand);
//	setTruncStoreAction(MVT::i64, MVT::i8, Expand);
//	setTruncStoreAction(MVT::i64, MVT::i16, Expand);
//	setTruncStoreAction(MVT::i64, MVT::i8, Promote);
//	setTruncStoreAction(MVT::i64, MVT::i16, Promote);

//	setTruncStoreAction(MVT::f64, MVT::f16, Expand);
//	setTruncStoreAction(MVT::f64, MVT::f32, Expand);

	for (MVT VT : MVT::fp_valuetypes()) {
	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::f16, Expand);
	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::f32, Promote);	//expand
	    setLoadExtAction(ISD::EXTLOAD, VT, MVT::f64, Expand);
	}

	setOperationAction(ISD::ConstantFP,			MVT::f64,	Legal);

	// VIAMPP has i64 MUL
//	setOperationAction(ISD::MUL,				MVT::i64,	Legal);

	// VIAMPPdoesn't have i64 MULH{S|U}
//	setOperationAction(ISD::MULHU,				MVT::i64,	Expand);
//	setOperationAction(ISD::MULHS,				MVT::i64,	Expand);

	// VIAMPPdoesn't have {U|S}MUL_LOHI
//	setOperationAction(ISD::UMUL_LOHI,			MVT::i64, 	Expand);
//	setOperationAction(ISD::SMUL_LOHI,			MVT::i64, 	Expand);

}

const char *VIAMPPTargetLowering::getTargetNodeName(unsigned Opcode) const {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPTargetLowering::getTargetNodeName: Opcode: " << Opcode << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	switch ((VIAMPPISD::NodeType)Opcode) {
	case VIAMPPISD::FIRST_NUMBER:  break;
	case VIAMPPISD::RET_FLAG:      return "VIAMPPISD::RetFlag";
	case VIAMPPISD::CALL:          return "VIAMPPISD::CALL";
	case VIAMPPISD::SELECT_CC:     return "VIAMPPISD::SELECT_CC";
	case VIAMPPISD::Wrapper:       return "VIAMPPISD::Wrapper";
	case VIAMPPISD::ADJDYNALLOC:   return "VIAMPPISD::ADJDYNALLOC";
	case VIAMPPISD::HI:			   return "VIAMPPISD::HI";
	case VIAMPPISD::LO:			   return "VIAMPPISD::LO";
	}
	return nullptr;
}

SDValue VIAMPPTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "--VIAMPPTargetLowering::LowerOperation: Opcode: " << Op.getOpcode() << "\n"; Op.dump(); outs() << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	switch (Op.getOpcode()) {
	case ISD::SELECT_CC:          return LowerSELECT_CC(Op, DAG);
	case ISD::GlobalAddress:      return LowerGlobalAddress(Op, DAG);
	case ISD::BlockAddress:       return LowerBlockAddress(Op, DAG);
	case ISD::JumpTable:          return LowerJumpTable(Op, DAG);
	case ISD::ConstantPool:       return LowerConstantPool(Op, DAG);
	case ISD::ExternalSymbol:     return LowerExternalSymbol(Op, DAG);
	case ISD::VASTART:            return LowerVASTART(Op, DAG);
	case ISD::DYNAMIC_STACKALLOC: return LowerDYNAMIC_STACKALLOC(Op, DAG);
	case ISD::RETURNADDR:         return LowerRETURNADDR(Op, DAG);
	case ISD::FRAMEADDR:          return LowerFRAMEADDR(Op, DAG);
	default:
		llvm_unreachable("unimplemented operand");
	}
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                       VIAMPP Inline Assembly Support
//===-------------------------------------------------------------------------------------------------------------------------===//
// getConstraintType - Given a constraint letter, return the type of constraint it is for this target.
VIAMPPTargetLowering::ConstraintType VIAMPPTargetLowering::getConstraintType(StringRef Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:
      break;
    case 'r':
      return C_RegisterClass;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass *> VIAMPPTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint, MVT VT) const {
	if (Constraint.size() == 1) {
	// GCC Constraint Letters
		switch (Constraint[0]) {
		default: break;
		case 'r':   // GENERAL_VREGS
			return std::make_pair(0U, &VIAMPP::VRegsRegClass);
		}
	}

	// Use the default implementation in TargetLowering to convert the register constraint into a member of a register class.
	return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===-------------------------------------------------------------------------------------------------------------------------===//
#include "VIAMPPGenCallingConv.inc"

static unsigned NumFixedArgs;

// Helper function for varargs
static bool CC_VIAMPP_VarArg(unsigned ValNo, MVT ValVT, MVT LocVT, CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
		                  CCState &State) {
	outs().changeColor(raw_ostream::GREEN,0);
	outs() << "VIAMPPTargetLowering::CC_VIAMPP_VarArg\n";
	outs().changeColor(raw_ostream::WHITE,0);

	// Handle fixed arguments with default CC
	if (ValNo < NumFixedArgs)
		return CC_VIAMPP(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State);

	// Promote i8/i16/i32 args to i32 (don't assume the all are already)
	if (LocVT == MVT::i8 || LocVT == MVT::i16 || LocVT == MVT::i32) {
		LocVT = MVT::i64;
		if (ArgFlags.isSExt())
			LocInfo = CCValAssign::SExt;
		else
			if (ArgFlags.isZExt())
				LocInfo = CCValAssign::ZExt;
			else
				LocInfo = CCValAssign::AExt;
	}


	// VarArgs get passed on stack
	unsigned Offset = State.AllocateStack(8, Align(8));
	State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));
	return false;
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Formal Arguments
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
		                                        const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG,
												SmallVectorImpl<SDValue> &InVals) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerFormalArguments\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	switch (CallConv) {
	default:
		llvm_unreachable("Unsupported calling convention");
	case CallingConv::C:
	case CallingConv::Fast:
		return LowerCCCArguments(Chain, CallConv, isVarArg, Ins, DL, DAG, InVals);
	}
}

SDValue VIAMPPTargetLowering::LowerCCCArguments(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
		                                     const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG,
		                                     SmallVectorImpl<SDValue> &InVals) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerCCCArguments\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo &MFI = MF.getFrameInfo();
	VIAMPPMachineFunctionInfo *VIAMPPMFI = MF.getInfo<VIAMPPMachineFunctionInfo>();

	// Analyze operands of the call, assigning locations to each operand. VIAMPPCallingConv.td will auto-generate CC_VIAMPP, which knows
	// how to handle operands (what go in registers vs. stack, etc).
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
	CCInfo.AnalyzeFormalArguments(Ins, CC_VIAMPP);

	// Walk through each parameter and push it into InVals. All arguments get passed via the stack (see Frame Layout and ABI).
	for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
//		outs() << "VIAMPPTargetLowering::LowerCCCArguments: i " << i << "\n";
		CCValAssign &VA = ArgLocs[i];

		MachineRegisterInfo &RegInfo = MF.getRegInfo();
		if (VA.isRegLoc()) {
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCArguments: isRegLoc\n";
			outs().changeColor(raw_ostream::WHITE,1);
			// Arguments are passed in registers
			EVT RegVT = VA.getLocVT();
			unsigned VReg = RegInfo.createVirtualRegister(&VIAMPP::VRegsRegClass);
			RegInfo.addLiveIn(VA.getLocReg(), VReg);
			SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);

	        // If this is an 8/16/32-bit value, it is really passed promoted to 64 bits. Insert an assert[sz]ext to capture this,
			// then truncate to the right size.
	        if (VA.getLocInfo() == CCValAssign::SExt)
	          ArgValue = DAG.getNode(ISD::AssertSext, DL, RegVT, ArgValue, DAG.getValueType(VA.getValVT()));
	        else
	        	if (VA.getLocInfo() == CCValAssign::ZExt)
	        		ArgValue = DAG.getNode(ISD::AssertZext, DL, RegVT, ArgValue, DAG.getValueType(VA.getValVT()));

	        if (VA.getLocInfo() != CCValAssign::Full)
	          ArgValue = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), ArgValue);

			InVals.push_back(ArgValue);
		} else {
			assert(VA.isMemLoc());

			SDValue InVal;
			ISD::ArgFlagsTy Flags = Ins[i].Flags;

			if (Flags.isByVal()) {
//				outs().changeColor(raw_ostream::GREEN,1);
//				outs() << "--VIAMPPTargetLowering::LowerCCCArguments: isMemLoc: ByValue\n";
//				outs().changeColor(raw_ostream::WHITE,1);
				/* LLVM counts stack offsets in bytes. VIAMPP is only 64 bit oriented. Therefore we need to divide the offset by 8. */
				/* We can either do this either here, or in RegisterInfo::eliminateFrameIndex. We do it here. */
//				int FI = MFI.CreateFixedObject(Flags.getByValSize(), VA.getLocMemOffset()/8, true);
				int FI = MFI.CreateFixedObject(Flags.getByValSize(), VA.getLocMemOffset(), true);
				InVal = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
			} else {
//				outs().changeColor(raw_ostream::GREEN,1);
//				outs() << "--VIAMPPTargetLowering::LowerCCCArguments: isMemLoc: Load arguments to virtual register\n";
//				outs().changeColor(raw_ostream::WHITE,1);
				// Load the argument to a virtual register
				unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
				if (ObjSize > 8) {
					errs() << "LowerFormalArguments Unhandled argument type: " << EVT(VA.getLocVT()).getEVTString() << "\n";
				}
				// Create the frame index object for this incoming parameter...
				/* LLVM counts stack offsets in bytes. VIAMPP is only 64 bit oriented. Therefore we need to divide the offset by 8. */
				/* For arguments lowering this is the only place we have access to the offset, because instruction selection is */
				/* automatically by the LLVM instruction selector. */
//				int FI = MFI.CreateFixedObject(ObjSize, VA.getLocMemOffset()/8, true);
				int FI = MFI.CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);

				// Create the SelectionDAG nodes corresponding to a load from this parameter
				SDValue FIN = DAG.getFrameIndex(FI, MVT::i64);
				InVals.push_back (DAG.getLoad(VA.getLocVT(), DL, Chain, FIN, MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
			}
		} // end if (VA.isRegLo())
	} // end for


	// Structure Return
	// When a function returns a structure, the address of the return value is placed in the first physical register
	if (MF.getFunction().hasStructRetAttr()) {
		unsigned Reg = VIAMPPMFI->getSRetReturnReg();
		if (!Reg) {
			Reg = MF.getRegInfo().createVirtualRegister(&VIAMPP::VRegsRegClass);
			VIAMPPMFI->setSRetReturnReg(Reg);
		}
		SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[0]);
		Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
	}

	// Variable Arguments
	// Record the frame index of the first variable argument which is a value necessary to VASTART
	if (isVarArg) {
		int FirstVarArg = MFI.CreateFixedObject(8, CCInfo.getNextStackOffset(),  true);
		VIAMPPMFI->setVarArgsFrameIndex(FirstVarArg);
	}

	return Chain;
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Call
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerCall\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	SelectionDAG &DAG                     = CLI.DAG;
	SDLoc &DL                             = CLI.DL;
	SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
	SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
	SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
	SDValue Chain                         = CLI.Chain;
	SDValue Callee                        = CLI.Callee;
	bool &isTailCall                      = CLI.IsTailCall;
	CallingConv::ID CallConv              = CLI.CallConv;
	bool isVarArg                         = CLI.IsVarArg;

	// VIAMPP target does not yet support tail call optimization.
	isTailCall = false;

	switch (CallConv) {
	default:
		report_fatal_error("Unsupported calling convention");
	case CallingConv::Fast:
	case CallingConv::C:
		return LowerCCCCall(Chain, Callee, CallConv, isVarArg, isTailCall, Outs, OutVals, Ins, DL, DAG, InVals);
	}
}

// Function arguments are copied from virtual regs to (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted
SDValue VIAMPPTargetLowering::LowerCCCCall(SDValue Chain, SDValue Callee, CallingConv::ID CallConv,  bool isVarArg, bool isTailCall,
							            const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals,
							            const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL, SelectionDAG &DAG,
							            SmallVectorImpl<SDValue> &InVals) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerCCCCall\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	// Analyze operands of the call, assigning locations to each operand.
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs, *DAG.getContext());
	GlobalAddressSDNode *GAN = dyn_cast<GlobalAddressSDNode>(Callee);
	MachineFrameInfo &MFI = DAG.getMachineFunction().getFrameInfo();
	auto PtrVT = getPointerTy(DAG.getDataLayout());

	if (isVarArg){
		outs().changeColor(raw_ostream::GREEN,1);
		outs() << "--VIAMPPTargetLowering::LowerCCCCall: isVarArg\n";
		outs().changeColor(raw_ostream::WHITE,0);
		NumFixedArgs = Outs.size();
		for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
			if (!Outs[i].IsFixed) {
				NumFixedArgs = 1;
				break;
			}
		}
		CCInfo.AnalyzeCallOperands(Outs, CC_VIAMPP_VarArg);
	} else {
		CCInfo.AnalyzeCallOperands(Outs,CC_VIAMPP);
	}

	// Get a count of how many bytes are to be pushed on the stack.
	unsigned NumBytes = CCInfo.getNextStackOffset();
	//Create local copies for byval args
	SmallVector<SDValue, 8>ByValArgs;

	// Create local copies for byval arguments
	for (unsigned i = 0, e = Outs.size(); i != e; ++i) {

		ISD::ArgFlagsTy  Flags = Outs[i].Flags;
		if(!Flags.isByVal()) {
//			outs().changeColor(raw_ostream::GREEN,1);
//			outs() << "--VIAMPPTargetLowering::LowerCCCCall: NOT ByValArgs: i: " << i << "\n";
//			outs().changeColor(raw_ostream::WHITE,0);
			continue;
		}

//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPTargetLowering::LowerCCCCall: ByValArgs: i: " << i << "\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		SDValue Arg = OutVals[i];
		unsigned Size = Flags.getByValSize();
		Align Align = Flags.getNonZeroByValAlign();

		int FI = MFI.CreateStackObject(Size, Align, /*isSS=*/ false);
		SDValue FIPtr = DAG.getFrameIndex(FI, PtrVT);
		SDValue SizeNode = DAG.getConstant(Size, DL, MVT::i64);

		Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Align, /*isVolatile=*/ false, /*AlwaysInline=*/ false,
			                  /*isTailCall=*/false, MachinePointerInfo(), MachinePointerInfo());
		ByValArgs.push_back(FIPtr);
	}

	// Insert call sequence start
	Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, DL);

	SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
	SmallVector<SDValue, 12> MemOpChains;
	SDValue StackPtr;

	// Walk the registers/memloc assignements, inserting copies/loads
	for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPTargetLowering::LowerCCCCall: Register/MemLoc: i: " << i << "\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		CCValAssign &VA = ArgLocs[i];
		SDValue Arg = OutVals[i];
		ISD::ArgFlagsTy Flags = Outs[i].Flags;

		// Promote the value if needed.
		switch (VA.getLocInfo()) {
		default:
			llvm_unreachable("Unknown loc info!");
		case CCValAssign::Full:
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCCall: CCValAssigne::Full\n";
			outs().changeColor(raw_ostream::WHITE,0);
			break;
		case CCValAssign::SExt:
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCCall: CCValAssigne::SExt\n";
			outs().changeColor(raw_ostream::WHITE,0);
			Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, VA.getLocVT(), Arg);
			break;
		case CCValAssign::ZExt:
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCCall: CCValAssigne::ZExt\n";
			outs().changeColor(raw_ostream::WHITE,0);
			Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, VA.getLocVT(), Arg);
			break;
		case CCValAssign::AExt:
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCCall: CCValAssigne::Aext\n";
			outs().changeColor(raw_ostream::WHITE,0);
			Arg = DAG.getNode(ISD::ANY_EXTEND, DL, VA.getLocVT(), Arg);
			break;
		}

		//Use local copy if it is a byval arg
		if (Flags.isByVal()) {
//			outs().changeColor(raw_ostream::GREEN,1);
//			outs() << "----VIAMPPTargetLowering::LowerCCCCall: Register/MemLoc: isByVal\n";
//			outs().changeColor(raw_ostream::WHITE,0);

			Arg = ByValArgs[j++];
		}
		// Arguments that can be passed on register must be kept at RegsToPass vector
		if (VA.isRegLoc()) {
//			outs().changeColor(raw_ostream::GREEN,1);
//			outs() << "----VIAMPPTargetLowering::LowerCCCCall: Register/MemLoc: isRegLoc\n";
//			outs().changeColor(raw_ostream::WHITE,0);

			RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
		} else {
			assert(VA.isMemLoc());
//			outs().changeColor(raw_ostream::GREEN,1);
//			outs() << "----VIAMPPTargetLowering::LowerCCCCall: Register/MemLoc: isMemLoc\n";
//			outs().changeColor(raw_ostream::WHITE,0);

			if (StackPtr.getNode() == 0) {
//				outs().changeColor(raw_ostream::GREEN,1);
//				outs() << "--VIAMPPTargetLowering::LowerCCCCall: StackPtr.getNode() == 0\n";
//				outs().changeColor(raw_ostream::WHITE,0);
				StackPtr = DAG.getCopyFromReg(Chain, DL, VIAMPP::StackPtr, PtrVT);	//StackPtr
			}

			/* LLVM counts stack offsets in bytes. VIAMPP is only 64 bit oriented. Therefore we need to divide the offset by 8. */
			/* For call arguments lowering this is the only place we have access to the offset, because instruction selection */
			/*  automatically by the LLVM instruction selector. */
			SDValue PtrOff = DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr, DAG.getIntPtrConstant(VA.getLocMemOffset()/8, DL));
//			SDValue PtrOff = DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr, DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));
			MemOpChains.push_back(DAG.getStore(Chain, DL, Arg, PtrOff, MachinePointerInfo()));
		}
	}

	// Transform all store nodes into one single node because all store nodes are independent of each other.
	if (!MemOpChains.empty())
		Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);
//Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, ArrayRef<SDValue>(&MemOpChains[0], MemOpChains.size()));

	// Build a sequence of copy-to-reg nodes chained together with token chain and flag operands which copy the outgoing args into
	// registers.  The InFlag is necessary since all emitted instructions must be stuck together.
	SDValue InFlag;
	for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPTargetLowering::LowerCCCCall: getCopyToReg\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		Chain = DAG.getCopyToReg(Chain, DL, RegsToPass[i].first, RegsToPass[i].second, InFlag);
		InFlag = Chain.getValue(1);
	}

	// Get the function address
	// If the callee is a GlobalAddress node (quite common, every direct call is) turn it into a TargetGlobalAddress node so that
	// legalize doesn't hack it. Likewise ExternalSymbol -> TargetExternalSymbol.
	uint8_t OpFlag = VIAMPPII::MO_NO_FLAG;
	if (GAN) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPTargetLowering::LowerCCCCall: Global Address Node (GAN)\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		Callee = DAG.getTargetGlobalAddress(GAN->getGlobal(), DL, PtrVT, 0,OpFlag); // - may change PtrVT to MVT::i64 or so
	} else
		if (ExternalSymbolSDNode *ESN = dyn_cast<ExternalSymbolSDNode>(Callee)) {
			outs().changeColor(raw_ostream::GREEN,1);
			outs() << "--VIAMPPTargetLowering::LowerCCCCall: External Symbol Node\n";
			outs().changeColor(raw_ostream::WHITE,0);
			Callee = DAG.getTargetExternalSymbol(ESN->getSymbol(), PtrVT, OpFlag); // - may change PtrVT to MVT::i64 or so
		}

	// Returns a chain & a flag for retval copy to use.
	SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
	SmallVector<SDValue, 8> Ops;
	Ops.push_back(Chain);
	Ops.push_back(Callee);

	// Add argument registers to the end of the list so that they are known live into the call.
	for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
		Ops.push_back(DAG.getRegister(RegsToPass[i].first, RegsToPass[i].second.getValueType()));

	if (InFlag.getNode())
		Ops.push_back(InFlag);

	Chain = DAG.getNode(VIAMPPISD::CALL, DL, NodeTys, Ops);
	InFlag = Chain.getValue(1);

	// Create the CALLSEQ_END node
	Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, DL, PtrVT, true), DAG.getConstant(0, DL, PtrVT, true), InFlag, DL);
	InFlag = Chain.getValue(1);

	// Handle result values, copying them out of physregs into vregs that we return.
	return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, DL, DAG, InVals);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Call Results
//
// Lower the result values of a call into the appropriate copies out of appropriate physical registers.
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool isVarArg,
                                      const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG,
                                      SmallVectorImpl<SDValue> &InVals) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerCallResult\n";
	outs().changeColor(raw_ostream::WHITE,0);

	// Assign locations to each value returned by this call.
	SmallVector<CCValAssign, 16> RVLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs, *DAG.getContext());

	CCInfo.AnalyzeCallResult(Ins, RetCC_VIAMPP);

	// Copy all of the result registers out of their specified physreg.
	for (unsigned i = 0; i != RVLocs.size(); ++i) {
		Chain = DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(), RVLocs[i].getValVT(), InFlag).getValue(1);
		InFlag = Chain.getValue(2);
		InVals.push_back(Chain.getValue(0));
	}

	return Chain;
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Can Lower Return
//
// CanLowerReturn - true iff the function's return value can be lowered to registers.
//===-------------------------------------------------------------------------------------------------------------------------===//
bool VIAMPPTargetLowering::CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg,
                                   const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::CanLowerReturn\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SmallVector<CCValAssign, 16> RVLocs;
	CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
	return CCInfo.CheckReturn(Outs, RetCC_VIAMPP);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Return
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
		                               const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals,
                                       const SDLoc &DL, SelectionDAG &DAG) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerReturn\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	MachineFunction &MF = DAG.getMachineFunction();
  
	// CCValAssign - represent the assignment of the return value to a location
	SmallVector<CCValAssign, 16> RVLocs;

	// CCState - Info about the registers and stack slot (which registers are already assigned and which stack slots are used).
	CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());

	// Analize return values.
	CCInfo.AnalyzeReturn(Outs, RetCC_VIAMPP);

	SDValue Flag;
	SmallVector<SDValue, 4> RetOps(1, Chain);

	// Copy the result values into the output registers.
    for (unsigned i = 0; i != RVLocs.size(); ++i) {
//    	outs().changeColor(raw_ostream::GREEN,1);
//    	outs() << "--VIAMPPTargetLowering::LowerReturn: in for RVLocs[i]: " << i << "\n";
//    	outs().changeColor(raw_ostream::WHITE,0);

    	CCValAssign &VA = RVLocs[i];
    	assert(VA.isRegLoc() && "Can only return in registers!");

    	Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    	// Guarantee that all emitted copies are stuck together, avoiding something bad.
    	Flag = Chain.getValue(1);
    	RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
	}  // end for

	// Returning structs by value requires that we copy the SRetReturnReg argument into ReturnReg for the return. We saved the
    // argument into into a virtual register in the entry block, so that we can copy the it now out and into ReturnReg.
	if (MF.getFunction().hasStructRetAttr()) {
		VIAMPPMachineFunctionInfo *VIAMPPMFI = MF.getInfo<VIAMPPMachineFunctionInfo>();

		// return the virtual register into which the sreg argument is passed.
		unsigned Reg = VIAMPPMFI->getSRetReturnReg();
		assert(Reg && "SRetReturnReg should have been set in LowerFormalArguments.");

		SDValue Val = DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
		Chain = DAG.getCopyToReg(Chain, DL, VIAMPP::ReturnReg, Val, Flag);
		Flag = Chain.getValue(1);
		RetOps.push_back(DAG.getRegister(VIAMPP::ReturnReg, getPointerTy(DAG.getDataLayout())));
	}

	// Update chain
	RetOps[0] = Chain;

	// Add the flag if we have it.
	if (Flag.getNode())
		RetOps.push_back(Flag);

	return DAG.getNode(VIAMPPISD::RET_FLAG, DL, MVT::Other, RetOps);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower RETURNADDR
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerRETURNADDR\n";
	outs().changeColor(raw_ostream::WHITE,0);

	const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo &MFI = MF.getFrameInfo();

	MFI.setReturnAddressIsTaken(true);

	EVT VT = Op.getValueType();
	SDLoc DL(Op);
	unsigned Depth = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
	if (Depth) {
		SDValue FrameAddr = LowerFRAMEADDR(Op, DAG);
		SDValue Offset = DAG.getConstant(DAG.getDataLayout().getPointerSize(), DL, MVT::i64);
		return DAG.getLoad(VT, DL, DAG.getEntryNode(), DAG.getNode(ISD::ADD, DL, VT, FrameAddr, Offset), MachinePointerInfo());
	}

	// Return the link register, which contains the return address. Mark it an implicit live-in.
	unsigned Reg = MF.addLiveIn(TRI->getRARegister(), getRegClassFor(MVT::i64));
	return DAG.getCopyFromReg(DAG.getEntryNode(), DL, Reg, VT);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower FRAMEADDR
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerFRAMEADDR\n";
	outs().changeColor(raw_ostream::WHITE,0);

	MachineFrameInfo &MFI = DAG.getMachineFunction().getFrameInfo();
	MFI.setFrameAddressIsTaken(true);
	EVT VT = Op.getValueType();
	SDLoc DL(Op);
	unsigned Depth = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
	SDValue FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), DL, VIAMPP::FramePtr, VT);		// FramePtr
	while (Depth--)
		FrameAddr = DAG.getLoad(VT, DL, DAG.getEntryNode(), FrameAddr, MachinePointerInfo());
	return FrameAddr;
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower VASTART
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerVASTART\n";
	outs().changeColor(raw_ostream::WHITE,0);

	MachineFunction &MF = DAG.getMachineFunction();
	VIAMPPMachineFunctionInfo *FuncInfo = MF.getInfo<VIAMPPMachineFunctionInfo>();

	SDLoc DL(Op);
	SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(), getPointerTy(DAG.getDataLayout()));

	// vastart just stores the address of the VarArgsFrameIndex slot into the memory location argument.
	const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
	return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1), MachinePointerInfo(SV));
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower DYNAMIC_STACKALLOC
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerDYNAMIC_STACKALLOC\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SDValue Chain = Op.getOperand(0);
	SDValue Size = Op.getOperand(1);
	SDLoc DL(Op);

	unsigned SPReg = getStackPointerRegisterToSaveRestore();

	// Get a reference to the stack pointer.
	SDValue StackPointer = DAG.getCopyFromReg(Chain, DL, SPReg, MVT::i64);

	// Subtract the dynamic size from the actual stack size to obtain the new stack size.
	SDValue Sub = DAG.getNode(ISD::SUB, DL, MVT::i64, StackPointer, Size);

	// For VIAMPP, the outgoing memory arguments area should be on top of the alloca area on the stack i.e., the outgoing memory
	// arguments should be at a lower address than the alloca area. Move the alloca area down the stack by adding back the space
	// reserved for outgoing arguments to SP here.
	//
	// We do not know what the size of the outgoing args is at this point. So, we add a pseudo instruction ADJDYNALLOC that will
	// adjust the stack pointer. We replace this instruction with on that has the correct, known offset in emitPrologue().
	SDValue ArgAdjust = DAG.getNode(VIAMPPISD::ADJDYNALLOC, DL, MVT::i64, Sub);

	// The Sub result contains the new stack start address, so it must be placed in the stack pointer register.
	SDValue CopyChain = DAG.getCopyToReg(Chain, DL, SPReg, Sub);

	SDValue Ops[2] = { ArgAdjust, CopyChain };
	return DAG.getMergeValues(Ops, DL);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                      Calling Convention Implementation END
//===-------------------------------------------------------------------------------------------------------------------------===//


//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operations
//===-------------------------------------------------------------------------------------------------------------------------===//

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower SELECT_CC
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPTargetLowering::LowerSELECT_CC\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	SDValue LHS = Op.getOperand(0);
	SDValue RHS = Op.getOperand(1);
	SDValue TrueV = Op.getOperand(2);
	SDValue FalseV = Op.getOperand(3);
	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
	SDLoc DL(Op);

	SDValue TargetCC = DAG.getConstant(CC, DL, LHS.getValueType());
	SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
	SDValue Ops[] = {LHS, RHS, TargetCC, TrueV, FalseV};

	return DAG.getNode(VIAMPPISD::SELECT_CC, DL, VTs, Ops);
}


//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operation GlobalAddress
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerGlobalAddress\n";
	outs().changeColor(raw_ostream::WHITE,0);

	auto N = cast<GlobalAddressSDNode>(Op);
	assert(N->getOffset() == 0 && "Invalid offset for global address");

	SDLoc DL(Op);
	const GlobalValue *GV = N->getGlobal();
	int64_t Offset = N->getOffset();

    uint8_t OpFlagHi = VIAMPPII::MO_ABS_HI;
    uint8_t OpFlagLo = VIAMPPII::MO_ABS_LO;

    // Create the TargetGlobalAddress node, folding in the constant offset.
    SDValue Hi = DAG.getTargetGlobalAddress(GV, DL, getPointerTy(DAG.getDataLayout()), Offset, OpFlagHi);
    SDValue Lo = DAG.getTargetGlobalAddress(GV, DL, getPointerTy(DAG.getDataLayout()), Offset, OpFlagLo);
    Hi = DAG.getNode(VIAMPPISD::HI, DL, MVT::i64, Hi);
    Lo = DAG.getNode(VIAMPPISD::LO, DL, MVT::i64, Lo);

    return DAG.getNode(ISD::OR, DL, MVT::i64, Hi, Lo);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operation BlockAddress
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerBlockAddress\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SDLoc DL(Op);
//	auto PtrVT = getPointerTy(DAG.getDataLayout());
	const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();

	uint8_t OpFlagHi = VIAMPPII::MO_ABS_HI;
	uint8_t OpFlagLo = VIAMPPII::MO_ABS_LO;

	SDValue Hi = DAG.getBlockAddress(BA, MVT::i64, /*isTarget=*/true, OpFlagHi);
	SDValue Lo = DAG.getBlockAddress(BA, MVT::i64, /*isTarget=*/true, OpFlagLo);
	Hi = DAG.getNode(VIAMPPISD::HI, DL, MVT::i64, Hi);
	Lo = DAG.getNode(VIAMPPISD::LO, DL, MVT::i64, Lo);
//	SDValue Result = DAG.getNode(ISD::OR, DL, MVT::i32, Hi, Lo);
//	return Result;
	return DAG.getNode(ISD::OR, DL, MVT::i64, Hi, Lo);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operation JumpTable
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerJumpTable(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerJumpTable\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SDLoc DL(Op);
	JumpTableSDNode *JT = cast<JumpTableSDNode>(Op);
	auto PtrVT = getPointerTy(DAG.getDataLayout());

    uint8_t OpFlagHi = VIAMPPII::MO_ABS_HI;
    uint8_t OpFlagLo = VIAMPPII::MO_ABS_LO;

    SDValue Hi = DAG.getTargetJumpTable(JT->getIndex(), PtrVT, OpFlagHi);
    SDValue Lo = DAG.getTargetJumpTable(JT->getIndex(), PtrVT, OpFlagLo);
    Hi = DAG.getNode(VIAMPPISD::HI, DL, MVT::i64, Hi);
    Lo = DAG.getNode(VIAMPPISD::LO, DL, MVT::i64, Lo);
    return DAG.getNode(ISD::OR, DL, MVT::i64, Hi, Lo);

}


//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operation ConstantPools
//===-------------------------------------------------------------------------------------------------------------------------===//

SDValue VIAMPPTargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerConstantPool\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SDLoc DL(Op);
	ConstantPoolSDNode *N = cast<ConstantPoolSDNode>(Op);
	const Constant *C = N->getConstVal();

    uint8_t OpFlagHi = VIAMPPII::MO_ABS_HI;
    uint8_t OpFlagLo = VIAMPPII::MO_ABS_LO;

    SDValue Hi = DAG.getTargetConstantPool(C, MVT::i64, N->getAlign(), N->getOffset(), OpFlagHi);
    SDValue Lo = DAG.getTargetConstantPool(C, MVT::i64, N->getAlign(), N->getOffset(), OpFlagLo);
    Hi = DAG.getNode(VIAMPPISD::HI, DL, MVT::i64, Hi);
    Lo = DAG.getNode(VIAMPPISD::LO, DL, MVT::i64, Lo);

    return DAG.getNode(ISD::OR, DL, MVT::i64, Hi, Lo);
}

//===-------------------------------------------------------------------------------------------------------------------------===//
//                  Lower Operation External Symbol
//===-------------------------------------------------------------------------------------------------------------------------===//
SDValue VIAMPPTargetLowering::LowerExternalSymbol(SDValue Op, SelectionDAG &DAG) const {
	outs().changeColor(raw_ostream::GREEN,1);
	outs() << "VIAMPPTargetLowering::LowerExternalSymbol\n";
	outs().changeColor(raw_ostream::WHITE,0);

	SDLoc DL(Op);
	const char *Sym = cast<ExternalSymbolSDNode>(Op)->getSymbol();
	auto PtrVT = getPointerTy(DAG.getDataLayout());
	SDValue Result = DAG.getTargetExternalSymbol(Sym, PtrVT);

	return DAG.getNode(VIAMPPISD::Wrapper, DL, PtrVT, Result);
}

