//===-- VIAMPPAsmParser.cpp - Parse VIAMPP assembly to MCInst instructions ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/STLExtras.h"
#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

namespace {
struct VIAMPPOperand;

class VIAMPPAsmParser : public MCTargetAsmParser {
	const MCSubtargetInfo &STI;
	MCAsmParser &Parser;
	MCAsmParser &getParser() const { return Parser; }
	MCAsmLexer &getLexer() const { return Parser.getLexer(); }

	// MatchAndEmitInstruction - Recognize a series of operands of a parsed instruction as an actual MCInst and emit it to the
	// specified MCStreamer. This returns false on success and returns true on failure to match.
	//
	// On failure, the target parser is responsible for emitting a diagnostic explaining the match failure.
	bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out, uint64_t &ErrorInfo,
                                 bool MatchingInlineAsm) override;

	bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;
	OperandMatchResultTy tryParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

	// ParseInstruction - Parse one assembly instruction.
    //
    // The parser is positioned following the instruction name. The target specific instruction parser should parse the entire
	// instruction and construct the appropriate MCInst, or emit an error. On success, the entire line should be parsed up to and
	// including the end-of-statement token. On failure, the parser is not required to read to the end of the line.
    //
    // Name - The instruction name.
    // NameLoc - The source location of the name.
    // Operands [out] - The list of parsed operands, this returns ownership of them to the caller.
    // return True on failure.
	bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc, OperandVector &Operands) override;

	// ParseDirective - Parse a target specific assembler directive
	//
	// The parser is positioned following the direc//tive name.  The target specific directive parser should parse the entire
	// directive doing or recording any target specific work, or return true and do nothing if the directive is not target
	// specific. If the directive is specific for the target, the entire line is parsed up to and including the end-of-statement
	// token and false is returned.
	//
	// DirectiveID - the identifier token of the directive.
	bool ParseDirective(AsmToken DirectiveID) override;

	bool ParseRegister(unsigned &RegNo, OperandVector &Operands);

	bool ParseImmediate(OperandVector &Operands);

	const MCExpr *evaluateRelocExpr(const MCExpr *Expr, MCSymbolRefExpr::VariantKind VK);

	MCSymbolRefExpr::VariantKind getVariantKind(StringRef Symbol);
	bool ParseSymbolReference(OperandVector &Operands);

	bool ParseOperand(OperandVector &Operands);

// include auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "VIAMPPGenAsmMatcher.inc"

public:
  VIAMPPAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser, const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), STI(STI), Parser(Parser) {
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
    MCAsmParserExtension::Initialize(Parser);
  }

};

// VIAMPPOperand - Instances of this class represented a parsed machine instruction
// MCParsedAsmOperand - This abstract class represents a source-level assembly instruction operand.  It should be subclassed by
// target-specific code.  This base class is used by target-independent clients and is the interface between parsing an asm
// instruction and recognizing it.
struct VIAMPPOperand : public MCParsedAsmOperand {

	enum KindTy {
		k_Token,
		k_Register,			// A-Type (absolute)
		k_Relative,			// R-Type (reference)
		k_Immediate,		// I-Type (immediate)
		k_Memory,			// O-Type (memory offset) Register+offset
		k_MemoryRelative	// O-Type (memory offset) Register+offset
	} Kind;

	SMLoc StartLoc, EndLoc;

	struct Token {
		const char *Data;
		unsigned Length;
	};

	struct RegOp {
		unsigned Num;
	};

	struct ImmOp {
		const MCExpr *Val;
	};

	struct MemOp {
		const MCExpr *Off;
		unsigned BaseNum;
	};

	union {
		struct Token Tok;
		struct RegOp Reg;
		struct ImmOp Imm;
		struct MemOp Mem;
	};

	VIAMPPOperand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

public:
	VIAMPPOperand(const VIAMPPOperand &o) : MCParsedAsmOperand() {
		Kind = o.Kind;
		StartLoc = o.StartLoc;
		EndLoc = o.EndLoc;
		switch (Kind) {
		case k_Register:
			Reg = o.Reg;
			break;
		case k_Immediate:
			Imm = o.Imm;
			break;
		case k_Token:
			Tok = o.Tok;
			break;
		case k_Memory:
			Mem = o.Mem;
			break;
		case k_MemoryRelative:	// kann man vielleicht mit k_Memory zusammenfassen
			Mem = o.Mem;
			break;
		case k_Relative:
			Reg = o.Reg;
			break;
		}
	}

	// getStartLoc - Gets location of the first token of this operand
	SMLoc getStartLoc() const override { return StartLoc; }

	// getEndLoc - Gets location of the last token of this operand
	SMLoc getEndLoc() const override { return EndLoc; }

	unsigned getReg() const override {
//outs() << "-=- getReg: "; dump(); outs() << "\n";
        assert (Kind == k_Register || Kind == k_Relative && "Invalid access!");
		return Reg.Num;
	}

	const MCExpr *getImm() const {
outs() << "-+-*getImm: "; dump(); outs() << " -+-\n";
		assert (Kind == k_Immediate && "Invalid access!");
		return Imm.Val;
	}

	StringRef getToken() const {
		assert (Kind == k_Token && "Invalid access!");
		return StringRef(Tok.Data, Tok.Length);
	}

	// Functions for testing operand type
	bool isReg()   const override { /*outs() << "isReg\n";*/ return Kind == k_Register; }
	bool isMem()   const override { /*outs() << "isMem\n";*/ return Kind == k_Memory; }
	bool isMemInd()			const { return Kind == k_MemoryRelative; }
	bool isToken() const override { return Kind == k_Token; }
	bool isImm()   const override { /*outs() << "isImm\n";*/ return Kind == k_Immediate; }
	bool isRelative()		const { return Kind == k_Relative; }

	void addExpr(MCInst &Inst, const MCExpr *Expr) const {
	// Add as immediates where possible. Null MCExpr = 0
		if (Expr == 0) {
//outs() << "<<<< Inst.addOperand(MCOperand::createImm(0))\n";
			Inst.addOperand(MCOperand::createImm(0));
		}
		else
			if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
//CE->dump();
//outs() << "<<<< Inst.addOperand(MCOperand::createImm(CE->getValue()))\n";
				Inst.addOperand(MCOperand::createImm(CE->getValue()));
			}
			else {
//outs() << "<<<< Inst.addOperand(MCOperand::createExpr(Expr))\n";
				Inst.addOperand(MCOperand::createExpr(Expr));
			}
	}

	void addRegOperands(MCInst &Inst, unsigned N) const {
		assert(N == 1 && "Invalid number of operands!");
		Inst.addOperand(MCOperand::createReg(getReg()));
	}

	void addImmOperands(MCInst &Inst, unsigned N) const {
outs() << "-+-addImmOperands\n";
		assert(N == 1 && "Invalid number of operands!");
		addExpr(Inst, getImm());
	}

	void addMemOperands(MCInst &Inst, unsigned N) const {
		assert(N == 2 && "Invalid number of operands!");
		Inst.addOperand(MCOperand::createReg(Mem.BaseNum));
		addExpr(Inst, Mem.Off);
	}

	void addMemIndOperands(MCInst &Inst, unsigned N) const {
		assert(N == 2 && "Invalid number of operands!");
		Inst.addOperand(MCOperand::createReg(Mem.BaseNum));
		addExpr(Inst, Mem.Off);
	}

	void print(raw_ostream &OS) const override {
		switch (Kind) {
		case k_Register:
			OS << "Absolute " << Reg.Num;
			break;
		case k_Immediate:
outs() << "print: k-immediate: " << Imm.Val << "\n";
			OS << "Immediate " << Imm.Val;
			break;
		case k_Token:
			OS << Tok.Data;
			break;
		case k_Relative:
			OS << "Relative " << "(" << Reg.Num << ")";
			break;
		case k_Memory:
			OS << "Offset " << Mem.Off << ", Relative " << Mem.BaseNum;
			break;
		case k_MemoryRelative:
			OS << "Offset " << Mem.Off << ", (Relative) " << Mem.BaseNum;
			break;
		}
	}

	static std::unique_ptr<VIAMPPOperand> CreateToken(StringRef Str, SMLoc S) {
		auto Op = std::make_unique<VIAMPPOperand>(k_Token);
		Op->Tok.Data = Str.data();
		Op->Tok.Length = Str.size();
		Op->StartLoc = S;
		Op->EndLoc = S;
		return Op;
	}

	static std::unique_ptr<VIAMPPOperand> CreateReg(unsigned RegNo, SMLoc S, SMLoc E) {
//outs() << "-- Create Reg ---\n";
		auto Op = std::make_unique<VIAMPPOperand>(k_Register);
		Op->Reg.Num = RegNo;
		Op->StartLoc = S;
		Op->EndLoc = E;
		return Op;
	}

	static std::unique_ptr<VIAMPPOperand> CreateRelative(unsigned BaseNum, SMLoc S, SMLoc E) {
		auto Op = std::make_unique<VIAMPPOperand>(k_Relative);
		Op->Reg.Num = BaseNum;
		Op->StartLoc = S;
		Op->EndLoc = E;
		return Op;
	}

	static std::unique_ptr<VIAMPPOperand> CreateImm(const MCExpr *Val, SMLoc S, SMLoc E) {
//outs() << "-- Create Imm ---\n";
		auto Op = std::make_unique<VIAMPPOperand>(k_Immediate);
		Op->Imm.Val = Val;
		Op->StartLoc = S;
		Op->EndLoc = E;
		return Op;
	}

	static std::unique_ptr<VIAMPPOperand> CreateMem(unsigned BaseNum, const MCExpr *Off,  SMLoc S, SMLoc E) {
		auto Op = std::make_unique<VIAMPPOperand>(k_Memory);
		Op->Mem.Off = Off;
		Op->Mem.BaseNum = BaseNum;
		Op->StartLoc = S;
		Op->EndLoc = E;
		return Op;
	}

	static std::unique_ptr<VIAMPPOperand> CreateMemRelative(unsigned BaseNum, const MCExpr *Off,  SMLoc S, SMLoc E) {
		auto Op = std::make_unique<VIAMPPOperand>(k_MemoryRelative);
		Op->Mem.Off = Off;
		Op->Mem.BaseNum = BaseNum;
		Op->StartLoc = S;
		Op->EndLoc = E;
		return Op;
	}
};
} // end anonymous namespace.

// Auto-generated by TableGen
static unsigned MatchRegisterName(StringRef Name);
static unsigned MatchRegisterAltName(StringRef Name);

bool VIAMPPAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out,
		                                   uint64_t &ErrorInfo, bool MatchingInlineAsm) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::MatchAndEmitInstruction\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	MCInst Inst;
	SMLoc ErrorLoc;

	switch (MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm)) {
	default: break;
	case Match_Success:
//outs() << "MatchAndEmitInstruction: success\n";
    	Out.emitInstruction(Inst, STI);
    	return false;
	case Match_MissingFeature:
		return Error(IDLoc, "instruction use requires option to be enabled");
	case Match_MnemonicFail:
		return Error(IDLoc, "unrecognized instruction mnemonic");
	case Match_InvalidOperand:
		ErrorLoc = IDLoc;
		if (ErrorInfo != ~0U) {
//outs() << "MatchAndEmitInstruction: error\n";
			if (ErrorInfo >= Operands.size())
				return Error(IDLoc, "too few operands for instruction");

			ErrorLoc = Operands[ErrorInfo]->getStartLoc();
			if (ErrorLoc == SMLoc())
				ErrorLoc = IDLoc;
		}
		return Error(ErrorLoc, "invalid operand for instruction");
	}

	llvm_unreachable("Unknown match type detected!");
}

bool VIAMPPAsmParser::ParseDirective(AsmToken DirectiveID) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::ParseDirective\n";
//	outs().changeColor(raw_ostream::WHITE,0);
  return true;
}

bool VIAMPPAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::ParseRegister\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	switch (tryParseRegister(RegNo, StartLoc, EndLoc)) {
	case MatchOperand_ParseFail:
		return Error(StartLoc, "invalid register name");
	case MatchOperand_Success:
//outs() << "Register success\n";
		return false;
	case MatchOperand_NoMatch:
		return true;
	}

	llvm_unreachable("unknown match result type");
}

OperandMatchResultTy VIAMPPAsmParser::tryParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::tryParseRegister\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	if (getLexer().getKind() == AsmToken::Identifier) {
		auto Name = getLexer().getTok().getIdentifier(); //.lower();
		RegNo = MatchRegisterName(Name);
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPAsmParser::tryParseRegister: RegNo: " << RegNo << "\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		if (RegNo == VIAMPP::NoRegister) {
			RegNo = MatchRegisterAltName(Name);																// Whats wrong here. Where is my alt names ??????
			if (RegNo == VIAMPP::NoRegister) {
//outs().changeColor(raw_ostream::GREEN,1);
//outs() << "--VIAMPPAsmParser::tryParseRegister no match\n";
//outs().changeColor(raw_ostream::WHITE,0);
				return MatchOperand_NoMatch;
			}
		}

		AsmToken const &T = getParser().getTok();
		StartLoc = T.getLoc();
		EndLoc = T.getEndLoc();
		getLexer().Lex(); // eat register token

		return MatchOperand_Success;
	}

	return MatchOperand_ParseFail;
}

bool VIAMPPAsmParser::ParseImmediate(OperandVector &Operands) {
	outs().changeColor(raw_ostream::RED,1);
	outs() << "VIAMPPAsmParser::ParseImmediate\n";
	outs().changeColor(raw_ostream::WHITE,0);
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() -1);

  const MCExpr *EVal;
  switch(getLexer().getKind()) {
    default: return true;
    case AsmToken::Plus:
    case AsmToken::Minus:
    case AsmToken::Integer:
      if(getParser().parseExpression(EVal))
        return true;

      Operands.push_back(VIAMPPOperand::CreateImm(EVal, S, E));
      return false;
  }
}

MCSymbolRefExpr::VariantKind VIAMPPAsmParser::getVariantKind(StringRef Symbol) {
	outs().changeColor(raw_ostream::RED,1);
	outs() << "VIAMPPAsmParser::getVariantKind\n";
	outs().changeColor(raw_ostream::WHITE,0);
	MCSymbolRefExpr::VariantKind VK = StringSwitch<MCSymbolRefExpr::VariantKind>(Symbol)
		.Case("hi", MCSymbolRefExpr::VK_VIAMPP_ABS_HI)
		.Case("lo", MCSymbolRefExpr::VK_VIAMPP_ABS_LO)
		.Default(MCSymbolRefExpr::VK_None);

	return VK;
}

const MCExpr *VIAMPPAsmParser::evaluateRelocExpr(const MCExpr *Expr, MCSymbolRefExpr::VariantKind VK) {
	outs().changeColor(raw_ostream::RED,1);
	outs() << "VIAMPPAsmParser::evaluateRelocExpr\n";
	outs().changeColor(raw_ostream::WHITE,0);
  // Check the type of the expression.
  if (const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(Expr)) {
outs() << "    // It's a constant, evaluate lo or hi value.\n";
    if (VK == MCSymbolRefExpr::VK_VIAMPP_ABS_LO) {
      long long Val = MCE->getValue();
      return MCConstantExpr::create(Val, getContext());
    } else if (VK == MCSymbolRefExpr::VK_VIAMPP_ABS_HI) {
      long long Val = MCE->getValue();
      long long LoSign = Val & 0x80000000;
      Val = (Val & 0xffffffff00000000) >> 32;
      // Lower part is treated as a signed int, so if it is negative we must add 1 to the hi part to compensate.
      if (LoSign)
        Val++;
      return MCConstantExpr::create(Val, getContext());
    } else {
      return nullptr;
    }
  }

  if (const MCSymbolRefExpr *MSRE = dyn_cast<MCSymbolRefExpr>(Expr)) {
outs() << "    // It's a symbol, create a symbolic expression from the symbol.\n";
    StringRef Symbol = MSRE->getSymbol().getName();
    return MCSymbolRefExpr::create(Symbol, VK, getContext());
  }

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
outs() << "    // It's a binary expression, map operands.\n";
    const MCExpr *LExp = evaluateRelocExpr(BE->getLHS(), VK);
    const MCExpr *RExp = evaluateRelocExpr(BE->getRHS(), VK);
    return MCBinaryExpr::create(BE->getOpcode(), LExp, RExp, getContext());
  }

  if (const MCUnaryExpr *UN = dyn_cast<MCUnaryExpr>(Expr)) {
outs() << "    // It's an unary expression, map operand.\n";
    const MCExpr *UnExp = evaluateRelocExpr(UN->getSubExpr(), VK);
    return MCUnaryExpr::create(UN->getOpcode(), UnExp, getContext());
  }

  return nullptr;
}

bool VIAMPPAsmParser::ParseSymbolReference(OperandVector &Operands) {
	outs().changeColor(raw_ostream::RED,1);
	outs() << "VIAMPPAsmParser::ParseSymbolReference\n";
	outs().changeColor(raw_ostream::WHITE,0);
  SMLoc S = Parser.getTok().getLoc();
  StringRef Identifier;
  if (Parser.parseIdentifier(Identifier))
    return true;

  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  MCSymbolRefExpr::VariantKind VK = getVariantKind(Identifier);
  if(VK != MCSymbolRefExpr::VK_None) {
outs() << "    // Parse a relocation expression.\n";
    SMLoc ExprS = Parser.getTok().getLoc();

    const MCExpr *EVal;
    if(getParser().parseExpression(EVal))
      return true;

    SMLoc ExprE = Parser.getTok().getLoc();

    const MCExpr *Res = evaluateRelocExpr(EVal, VK);
    if(!Res)
      return Error(ExprS, "unsupported relocation expression");

    Operands.push_back(VIAMPPOperand::CreateImm(Res, S, ExprE));
    return false;
  } else {
outs() << "    // Parse a symbol\n";
    MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
    const MCExpr *Res = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
    Operands.push_back(VIAMPPOperand::CreateImm(Res, S, E));
    return false;
  }
}

bool VIAMPPAsmParser::ParseOperand(OperandVector &Operands) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::ParseOperand\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	SMLoc StartLoc, EndLoc;
	unsigned RegNum;
	const MCExpr *Imm;


	unsigned Token = getLexer().getKind();
//outs() << "TOKEN TOKEN: " << Token << "\n";
	switch (Token) {
//	switch(getLexer().getKind()) {
	case AsmToken::Identifier: {		// Absolute - look for a VReg
outs() << "Parse Identifier\n";
		if (!ParseRegister(RegNum, StartLoc, EndLoc)) {
outs() << "Parse Register: push back Register\n";
			Operands.push_back(VIAMPPOperand::CreateReg(RegNum, StartLoc, EndLoc));
			return false;
		} else {

/*			if (!ParseImmediate(Operands) || !ParseSymbolReference(Operands)) {
outs().changeColor(raw_ostream::RED,1);
outs() << "parse Immediate or Symbol: return true\n";
outs().changeColor(raw_ostream::WHITE,0);
return false;
			}
/*/			const MCExpr *Val;
			SMLoc ExprLoc = getLexer().getLoc();
			if (getParser().parseExpression(Val))
				return Error(ExprLoc, "expected expression operand");

			int64_t Res;
			if (Val->evaluateAsAbsolute(Res)) {
				outs().changeColor(raw_ostream::RED,1);
				outs() << "Identifier, but not register. Symbol or Relocation: evaluateAsAbsolute\n";
				outs().changeColor(raw_ostream::WHITE,0);
				if (Res < -32768 || Res > 32767)		// change to isInt<16>
					return Error(ExprLoc, "invalid offset");
			}
			Operands.push_back(VIAMPPOperand::CreateImm(Val, ExprLoc, getLexer().getLoc()));

			if (getLexer().isNot(AsmToken::EndOfStatement)) {
				SMLoc Loc = getLexer().getLoc();
				getParser().eatToEndOfStatement();
				return Error(Loc, "unexpected token");
			}
			return false;
//*/
		}
		return true;
	}
	case AsmToken::Hash: {			// Immediate - Look for an Immediate
		StartLoc = getParser().getTok().getLoc();
outs() << "Parse Immediate\n";
		getLexer().Lex();									// eat "#"
		const MCExpr *Imm;
		if (!getParser().parseExpression(Imm)) {
			SMLoc EndLoc = getParser().getTok().getLoc();
//outs() << "--pushback createImm ---\n";
			Operands.push_back(VIAMPPOperand::CreateImm(Imm, StartLoc, EndLoc));
			return false;
		}
		return true;
	}
	case AsmToken::LParen: {								// opening "(" check for Relative, a (VReg)
//outs() << "Parse (Register)\n";
		getLexer().Lex();									// eat "("
		StartLoc = getParser().getTok().getLoc();

		if (ParseRegister(RegNum, StartLoc, EndLoc)) {		// check if valid register name
//outs() << "parse register done";
			return true;
		}
//outs() << "check for closing )\n";
		if (getLexer().getKind() != AsmToken::RParen) {		// check for closing ")"
//outs() << "closing ) missing\n";
			return true;
		}
		EndLoc = getParser().getTok().getEndLoc();
		getLexer().Lex();									// eat ")"
		Operands.push_back(VIAMPPOperand::CreateRelative(RegNum, StartLoc, EndLoc));
		return false;
	}
	case AsmToken::LBrac: {									// opening "[" ckeck for [off]VReg or [off](Vreg)
//outs() << "Parse  [offset]Register or [offset](Register)\n";
		getLexer().Lex();									// eat "["
		StartLoc = getParser().getTok().getLoc();

		if (getParser().parseExpression(Imm)) {				// check immediate
//outs() << "Parse Imm NOT successfull!";
			return true;
		}
//outs() << "check for closing ]\n";
		if (getLexer().getKind() != AsmToken::RBrac) {		// check for closing "]"
//outs() << "Missing ]\n";
			return true;
		}
		getLexer().Lex();									// eat "]"
//outs() << "Parse Register\n";

		EndLoc = getParser().getTok().getLoc();
		SMLoc RegStartLoc;
		if (getLexer().getKind() == AsmToken::LParen) {		// we have a relative register
//outs() << "Look for relative register\n";
			getLexer().Lex();								// eat "("
			RegStartLoc = getParser().getTok().getLoc();
			if (ParseRegister(RegNum, RegStartLoc, EndLoc)) {
//outs() << "Parse Reg: " << RegNum << "\n";
				return true;
			}
//outs() << "check for closing )\n";
			if (getLexer().getKind() != AsmToken::RParen) {	// check for closing ")"
//outs() << "closing ) missing\n";
				return true;
			}
			EndLoc = getParser().getTok().getEndLoc();
			getLexer().Lex();								// eat ")"
//outs() << "nach EndLoc, vor PushBack, indirect register type\n";
			Operands.push_back(VIAMPPOperand::CreateMemRelative(RegNum, Imm, StartLoc, EndLoc));
			return false;
		} else {											// just a register
//outs() << "Look for register\n";
			RegStartLoc = getParser().getTok().getLoc();
			if (!ParseRegister(RegNum, RegStartLoc, EndLoc)) {
//outs() << "Parse Reg: " << RegNum << "\n";
				EndLoc = getParser().getTok().getEndLoc();
//outs() << "nach EndLoc, vor PushBack, register type\n";
				Operands.push_back(VIAMPPOperand::CreateMem(RegNum, Imm, StartLoc, EndLoc));
				return false;
			}
			return true;
		}
	}
	default:
//outs() << "-- Parse Operand -- DEFAULT -- Token Value: " << Token << " Hash is: " << AsmToken::Hash << "\n";
		return true;

	}

	return Error(StartLoc, "unsupported operand");
}

bool VIAMPPAsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc, OperandVector &Operands) {
//	outs().changeColor(raw_ostream::GREEN,1);
//	outs() << "VIAMPPAsmParser::ParseInstruction: " << Name << " ---------------------------------------------\n";
//	outs().changeColor(raw_ostream::WHITE,0);


	// First operand is token for instruction
	Operands.push_back(VIAMPPOperand::CreateToken(Name, NameLoc));

	// If there are no more operands, then finish
	if (getLexer().is(AsmToken::EndOfStatement)) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "----VIAMPPAsmParser::ParseInstruction: EOS\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		return false;
	}
//outs() << "vor pars first operand\n";
	// Parse first operand
bool pop = !ParseOperand(Operands);
//outs() << "Parse first Operand: " << pop << "\n";
//	if (ParseOperand(Operands)) {
	if (!pop) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "----VIAMPPAsmParser::ParseInstruction: Parse First Operand\n";
//		outs().changeColor(raw_ostream::WHITE,0);
//		return true;
	}
//outs() << "nach pars first operand\n";

	// Parse until end of statement, consuming commas between operands
	while (getLexer().isNot(AsmToken::EndOfStatement) && getLexer().is(AsmToken::Comma)) {
//outs().changeColor(raw_ostream::GREEN,1);
//outs() << "----VIAMPPAsmParser::ParseInstruction: WHILE\n";
//outs().changeColor(raw_ostream::WHITE,0);
		// Consume comma token
		getLexer().Lex();

		// Parse next operand
		if(!ParseOperand(Operands)) {
//outs().changeColor(raw_ostream::GREEN,1);
//outs() << "--VIAMPPAsmParser::ParseInstruction: Parse Next Operand\n";
//outs().changeColor(raw_ostream::WHITE,0);
//			return true;
		}
	}

	if (getLexer().isNot(AsmToken::EndOfStatement)) {
//		outs().changeColor(raw_ostream::GREEN,1);
//		outs() << "--VIAMPPAsmParser::ParseInstruction: End of Statement\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		return Error(getLexer().getTok().getLoc(), "unexpected token in operand list");
	} else {

//outs().changeColor(raw_ostream::GREEN,1);
//outs() << "VIAMPPParseInstruction: End Parse Instruction: " << Name << " -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n";
//outs().changeColor(raw_ostream::WHITE,0);
			return false;
	}
//outs() << "End of VIAMPPAsmParser::ParseInstruction vor return false\n";
	return false;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeVIAMPPAsmParser() {
  RegisterMCAsmParser<VIAMPPAsmParser> X(TheVIAMPPTarget);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "VIAMPPGenAsmMatcher.inc"
