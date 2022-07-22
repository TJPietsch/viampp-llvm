//===-- VIAMPPTargetMachine.h - Define TargetMachine for VIAMPP ---*- C++ -*--------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file declares the VIAMPP specific subclass of TargetMachine.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPTARGETMACHINE_H
#define VIAMPPTARGETMACHINE_H

#include "VIAMPP.h"
#include "VIAMPPSubtarget.h"
#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class VIAMPPTargetMachine : public LLVMTargetMachine {
  VIAMPPSubtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  VIAMPPTargetMachine(const Target &T, const Triple &TT, StringRef CPU, StringRef FS, const TargetOptions &Options,
                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT);

//  const VIAMPPSubtarget * getSubtargetImpl() const {
//    return &Subtarget;
//  }
  
  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  virtual const TargetSubtargetInfo *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
};

} // end namespace llvm

#endif
