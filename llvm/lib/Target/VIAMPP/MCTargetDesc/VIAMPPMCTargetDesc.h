//===-- VIAMPPMCTargetDesc.h - VIAMPP Target Descriptions ----------------------------------------------------------*- C++ -*-===//
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

#ifndef VIAMPPMCTARGETDESC_H
#define VIAMPPMCTARGETDESC_H

#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class MCAsmInfo;
class MCCodeGenInfo;
class MCInstPrinter;
class MCAsmBackend;
class Target;
class Triple;
class StringRef;
class raw_pwrite_stream;

extern Target TheVIAMPPTarget;

MCCodeEmitter *createVIAMPPMCCodeEmitter(const MCInstrInfo &MII, const MCRegisterInfo &MRI, MCContext &Ctx);

MCAsmBackend *createVIAMPPAsmBackend(const Target &T, const MCSubtargetInfo &STI, const MCRegisterInfo &MRI,
                                  const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createVIAMPPELFObjectWriter(uint8_t OSABI);

} // End llvm namespace

// Defines symbolic names for VIAMPP registers. This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "VIAMPPGenRegisterInfo.inc"

// Defines symbolic names for the VIAMPP instructions.
#define GET_INSTRINFO_ENUM
#include "VIAMPPGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "VIAMPPGenSubtargetInfo.inc"

#endif
