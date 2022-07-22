//===- VIAMPPAsmBackend.cpp - AsmBackend for VIAMPP ----------------------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file is part of the VIAMPP AsmBackend.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

// see Lanai, BPF, etc +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=+

#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "MCTargetDesc/VIAMPPFixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class VIAMPPAsmBackend : public MCAsmBackend {
  Triple::OSType OSType;

public:
  VIAMPPAsmBackend(const Target &T, Triple::OSType OSType)
    : MCAsmBackend(support::big), OSType(OSType) {}

	void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target, MutableArrayRef<char> Data,
                    uint64_t Value, bool IsResolved, const MCSubtargetInfo *STI) const override;
	
	bool mayNeedRelaxation(const MCInst & /*Inst*/, const MCSubtargetInfo &STI) const override {
//		outs().changeColor(raw_ostream::BLUE,0);
//		outs() << "VIAMPPAsmBackend::mayNeedRelaxation\n";
//		outs().changeColor(raw_ostream::WHITE,0);

	    return false;
	}

	// No instruction requires relaxation
	bool fixupNeedsRelaxation(const MCFixup & /*Fixup*/, uint64_t /*Value*/, const MCRelaxableFragment * /*DF*/,
	                          const MCAsmLayout & /*Layout*/) const override {
//		outs().changeColor(raw_ostream::BLUE,0);
//		outs() << "VIAMPPAsmBackend::fixupNeedsRelaxation\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		return false;
	}
	
	bool writeNopData(raw_ostream &OS, uint64_t Count) const override;

	std::unique_ptr<MCObjectTargetWriter>createObjectTargetWriter() const override;
	
	unsigned getNumFixupKinds() const  override { return VIAMPP::NumTargetFixupKinds; }

    const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
 //   	outs().changeColor(raw_ostream::BLUE,0);
 //  	outs() << "VIAMPPAsmBackend::getFixupKindInfo\n";
 //   	outs().changeColor(raw_ostream::WHITE,0);

    	const static MCFixupKindInfo Infos[VIAMPP::NumTargetFixupKinds] = {
    	    // This table *must* be in the order that the fixup_* kinds are defined in
            // VIAMPPFixupKinds.h.
            //
            // Name			Offset Size Flags
            { "fixup_VIAMPP_NONE",	0,  	64,   0 },
            { "fixup_VIAMPP_16_pcrel",	48, 	16,   MCFixupKindInfo::FKF_IsPCRel },
	    { "fixup_VIAMPP_32_pcrel",	32, 	32,   MCFixupKindInfo::FKF_IsPCRel },
            { "fixup_VIAMPP_64_pcrel",	0,  	64,   MCFixupKindInfo::FKF_IsPCRel },
	    { "fixup_VIAMPP_64_abs",	0,  	64,   0 },
	    { "fixup_VIAMPP_HI",	32,	32,   0 },
	    { "fixup_VIAMPP_LO",	/*32*/0,	32,   0 }
    	};

    	if (Kind < FirstTargetFixupKind) {
    		return MCAsmBackend::getFixupKindInfo(Kind);
    	}

    	assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() && "Invalid kind!");
    	return Infos[Kind - FirstTargetFixupKind];
	}

  
}; 

bool VIAMPPAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPAsmBackend::writeNopData\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	// VIAMPP should never write nop data
	if (Count == 0)
		return true;

	return false;
}

// Prepare value for the target space
static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value, MCContext &Ctx) {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPAsmBackend::adjustFixupValue: Value: " << Value << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	unsigned Kind = Fixup.getKind();
outs() << "-- Fixup.getKind: " << Kind;
	switch (Kind) {
	default:
		llvm_unreachable("Unsupported fixup kind in adjustFixupValue");
	case FK_Data_1:
	case FK_Data_2:
	case FK_Data_4:
	case FK_Data_8:
outs() << "     FK_Data_1/2/4/8\n";
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_16_pcrel:
outs() << "     fixup_VIAMPP_16_pcrel\n";
		if (!isInt<16>(Value))
			Ctx.reportError(Fixup.getLoc(), "fixup value out of range (16-bit)");
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_32_pcrel:
outs() << "     fixup_VIAMPP_32_pcrel\n";
		if (!isInt<32>(Value))
			Ctx.reportError(Fixup.getLoc(), "fixup value out of range (32-bit)");
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_64_pcrel:
outs() << "     fixup_VIAMPP_64_pcrel\n";
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_64_abs:
outs() << "     fixup_VIAMPP_64_abs\n";
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_HI:
outs() << "     fixup_VIAMPP_HI\n";
		return Value;
		break;
	case VIAMPP::fixup_VIAMPP_LO:
outs() << "     fixup_VIAMPP_LO\n";
		return Value;
		break;
	}
	return Value;
}

void VIAMPPAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup, const MCValue &Target, MutableArrayRef<char> Data,
								  uint64_t Value, bool IsResolved, const MCSubtargetInfo *STI) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPAsmBackend::applyFixup\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	
	MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());
	MCContext &Ctx = Asm.getContext();

	// FixupValue in llvm is in bytes, VIAMPP addressing is on 64-bit words, therefor divide the value by 8 to alligne it.
	Value = adjustFixupValue(Fixup, Value, Ctx) / 8;
//outs() << "--adjustFixupValue (what we have to write into the word): " << Value << " Info.TargetOffset (position in the word): " << Info.TargetOffset/8 << "\n";

	if (!Value) {
		return; // Doesn't change encoding.
	}

	// The offset of the instruction that needs the fixup.
	unsigned Offset = Fixup.getOffset() + Info.TargetOffset / 8;
	// How many bytes need to be fixed.
	unsigned NumBytes = Info.TargetSize / 8;
//outs() << "  Fixup.getOffset (which byte in the word do we start to change: " << Offset << " NumBytes ( how many bytes involved): " << NumBytes << "\n";

	assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

	// For each byte of the fragment that the fixup touches, mask the bits from the fixup value.
	for (unsigned i = 0; i != NumBytes; ++i) {
		unsigned Index = (NumBytes - 1) - i;
		Data[Offset + Index] = uint8_t((Value >> (i * 8)) & 0xff);
	}
}


// currently we use ELF format, if we decide to have our own format we need to hokk in our writere here.
std::unique_ptr<MCObjectTargetWriter>VIAMPPAsmBackend::createObjectTargetWriter() const {
	return createVIAMPPELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

} // end anonymous namespace

MCAsmBackend *llvm::createVIAMPPAsmBackend(const Target &T, const MCSubtargetInfo &STI, const MCRegisterInfo &MRI, 
		                                const MCTargetOptions &Options) {
	
	assert(STI.getTargetTriple().isOSBinFormatELF() && "VIAMPP only supports ELF targets!");

	return new VIAMPPAsmBackend(T, STI.getTargetTriple().getOS());
}
