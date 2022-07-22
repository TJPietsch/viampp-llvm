//===-- VIAMPPMCAsmInfo.h - VIAMPP asm properties -----------------------------------------------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the declaration of the VIAMPPMCAsmInfo class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPTARGETASMINFO_H
#define VIAMPPTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class VIAMPPMCAsmInfo : public MCAsmInfoELF {
  virtual void anchor() override;

public:
  explicit VIAMPPMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
