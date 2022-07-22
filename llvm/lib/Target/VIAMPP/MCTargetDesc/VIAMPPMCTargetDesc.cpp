//===-- VIAMPPMCTargetDesc.cpp - VIAMPP Target Descriptions ------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file provides VIAMPP specific target descriptions.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPPMCTargetDesc.h"
#include "VIAMPPMCAsmInfo.h"
#include "VIAMPPInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "VIAMPPGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "VIAMPPGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "VIAMPPGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createVIAMPPMCInstrInfo() {
	MCInstrInfo *X = new MCInstrInfo();
	InitVIAMPPMCInstrInfo(X);
	return X;
}

static MCRegisterInfo *createVIAMPPMCRegisterInfo(const Triple &TT) {
	MCRegisterInfo *X = new MCRegisterInfo();
	InitVIAMPPMCRegisterInfo(X, VIAMPP::StackPtr);		// StackPtr
	return X;
}

static MCSubtargetInfo *createVIAMPPMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
	return createVIAMPPMCSubtargetInfoImpl(TT, CPU, /* TuneCPU */ CPU, FS);
}

static MCAsmInfo *createVIAMPPMCAsmInfo(const MCRegisterInfo &MRI, const Triple &TT, const MCTargetOptions &Options) {
	MCAsmInfo *MAI = new VIAMPPMCAsmInfo(TT);
	return MAI;
}

static MCInstPrinter *createVIAMPPMCInstPrinter(const Triple &TT, unsigned SyntaxVariant, const MCAsmInfo &MAI,
		                                     const MCInstrInfo &MII, const MCRegisterInfo &MRI) {
	return new VIAMPPInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeVIAMPPTargetMC() {
  // Register the MC asm info
  TargetRegistry::RegisterMCAsmInfo(TheVIAMPPTarget, createVIAMPPMCAsmInfo);
	
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheVIAMPPTarget, createVIAMPPMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheVIAMPPTarget, createVIAMPPMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheVIAMPPTarget, createVIAMPPMCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheVIAMPPTarget, createVIAMPPMCInstPrinter);

  // Register the MC code emitter
  TargetRegistry::RegisterMCCodeEmitter(TheVIAMPPTarget, createVIAMPPMCCodeEmitter);

  // Register the ASM Backend
  TargetRegistry::RegisterMCAsmBackend(TheVIAMPPTarget,  createVIAMPPAsmBackend);

}
