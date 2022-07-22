//===-- VIAMPPELFObjectWriter.cpp - VIAMPP ELF Writer ------------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "MCTargetDesc/VIAMPPFixupKinds.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

//using namespace llvm;

namespace llvm {

class VIAMPPELFObjectWriter : public MCELFObjectTargetWriter {
public:
  VIAMPPELFObjectWriter(uint8_t OSABI);
  ~VIAMPPELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target, const MCFixup &Fixup, bool IsPCRel) const override;
};

VIAMPPELFObjectWriter::VIAMPPELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ true, OSABI, ELF::EM_VIAMPP, /*HasRelocationAddend*/ true) {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPELFObjectWritter::VIAMPPELFObjectWritter\n";
//	outs().changeColor(raw_ostream::WHITE,0);

}

unsigned VIAMPPELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target, const MCFixup &Fixup, bool IsPCRel) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPELFObjectWritter::getRelocType\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	// determine the type of the relocation
	switch (Fixup.getTargetKind()) {
	default:
return ELF::R_VIAMPP_NONE;
		llvm_unreachable("invalid fixup kind!");	//-=-=-=-=-=-=-=----------------------------------------------------------------
	case VIAMPP::fixup_VIAMPP_16_pcrel:
	case FK_Data_2:
		return ELF::R_VIAMPP_16_pcrel;

	case VIAMPP::fixup_VIAMPP_32_pcrel:
	case FK_Data_4:
		return ELF::R_VIAMPP_32_pcrel;

	case VIAMPP::fixup_VIAMPP_64_pcrel:
	case FK_Data_8:
		return ELF::R_VIAMPP_64_pcrel;

	case VIAMPP::fixup_VIAMPP_HI:
		return ELF::R_VIAMPP_HI;

	case VIAMPP::fixup_VIAMPP_LO:
		return ELF::R_VIAMPP_LO;

	case VIAMPP::fixup_VIAMPP_NONE:
		return ELF::R_VIAMPP_NONE;
	}
}

std::unique_ptr<MCObjectTargetWriter> createVIAMPPELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<VIAMPPELFObjectWriter>(OSABI);
}

} // end namespace llvm
