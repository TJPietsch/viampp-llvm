//===-- VIAMPPTargetMachine.cpp - Define TargetMachine for VIAMPP ------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPPTargetMachine.h"
#include "VIAMPP.h"
#include "VIAMPPFrameLowering.h"
#include "VIAMPPInstrInfo.h"
#include "VIAMPPISelLowering.h"
#include "VIAMPPSubtarget.h"
#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

static std::string computeDataLayout() {
	// Build the data layout
	// http://llvm.org/docs/LangRef.html#data-layout

	std::string dataLayout = "";

	// for further information see https://llvm.org/doc/LangRef.html section Data Layout
	dataLayout += "E"; // Big-endian
	dataLayout += "-m:e"; // ELF style name mangling
	dataLayout += "-p:64:64"; // Set 64-bit pointer size with 64-bit alignment

//	dataLayout += "-S64";		// natural stack alignment set to 64-bit (8-byte)

	dataLayout += "-i1:64:64"; // Set i1 to 64-bit and align to a 64-bit word
	dataLayout += "-i8:64:64"; // Set i8 to 64-bit and align to a 64-bit word
	dataLayout += "-i16:64:64"; // Set i16 to 64-bit and align to a 64-bit word
	dataLayout += "-i32:64:64"; // Set i32 to 64-bit and align to a 64-bit word
	dataLayout += "-i64:64:64";

	dataLayout += "-f16:64:64";
	dataLayout += "-f32:64:64"; // Set f32 to 64-bit and align to a 64-bit word
	dataLayout += "-f64:64:64";

	dataLayout += "-a:0:64"; // Align aggregates to a 64-bit word
	dataLayout += "-n64"; // Set native integer width to 64-bits

	return dataLayout;
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
	if (!RM.hasValue())
		return Reloc::Static;
	return *RM;
}

VIAMPPTargetMachine::VIAMPPTargetMachine(const Target &T, const Triple &TT, StringRef CPU, StringRef FS, const TargetOptions &Options,
                                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(), TT, CPU, FS, Options, getEffectiveRelocModel(RM),
    		            getEffectiveCodeModel(CM, CodeModel::Small) /*CM*/, OL),
						Subtarget(TT, CPU, FS, *this), TLOF(std::make_unique<TargetLoweringObjectFileELF>()) {
	initAsmInfo();
}

namespace {
// VIAMPP Code Generator Pass Configuration Options.
class VIAMPPPassConfig : public TargetPassConfig {
public:
	VIAMPPPassConfig(VIAMPPTargetMachine &TM, PassManagerBase &PM) : TargetPassConfig(TM, PM) {}

	VIAMPPTargetMachine &getVIAMPPTargetMachine() const {
		return getTM<VIAMPPTargetMachine>();
	}

	//  virtual bool addPreISel() override;		// not needed now, maybe later!
	virtual bool addInstSelector() override;
	virtual void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *VIAMPPTargetMachine::createPassConfig(PassManagerBase &PM) {
	return new VIAMPPPassConfig(*this, PM);
}

//bool VIAMPPPassConfig::addPreISel() { return false; }		// not needed now, maybe later!

bool VIAMPPPassConfig::addInstSelector() {
	addPass(createVIAMPPISelDag(getVIAMPPTargetMachine(), getOptLevel()));

	return false;
}

// Special pass for optimizations before emitting the code
void VIAMPPPassConfig::addPreEmitPass() {
	// Pass to replace MOVoff instructions when imm is zero.
	addPass(createVIAMPPOptMachinePass());
}

// Force static initialization.
extern "C" void LLVMInitializeVIAMPPTarget() {
	RegisterTargetMachine<VIAMPPTargetMachine> X(TheVIAMPPTarget);
}
