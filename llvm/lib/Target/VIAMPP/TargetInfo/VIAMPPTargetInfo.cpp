//===-- VIAMPPTargetInfo.cpp - VIAMPP Target Implementation ------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheVIAMPPTarget;

extern "C" void LLVMInitializeVIAMPPTargetInfo() {
  RegisterTarget<Triple::viampp> X(TheVIAMPPTarget, "VIAMPP", "64-bit viampp CPU", "VIAMPP");
}
