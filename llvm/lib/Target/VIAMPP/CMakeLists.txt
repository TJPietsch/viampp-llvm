add_llvm_component_group(VIAMPP)

set(LLVM_TARGET_DEFINITIONS VIAMPP.td)

tablegen(LLVM VIAMPPGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM VIAMPPGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM VIAMPPGenCallingConv.inc -gen-callingconv)
tablegen(LLVM VIAMPPGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM VIAMPPGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM VIAMPPGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM VIAMPPGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM VIAMPPGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM VIAMPPGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(VIAMPPCommonTableGen)

add_llvm_target(VIAMPPCodeGen
  VIAMPPAsmPrinter.cpp
  VIAMPPFrameLowering.cpp
  VIAMPPInstrInfo.cpp
  VIAMPPISelDAGToDAG.cpp
  VIAMPPISelLowering.cpp
  VIAMPPMCInstLower.cpp
  VIAMPPRegisterInfo.cpp
  VIAMPPSubtarget.cpp
  VIAMPPTargetMachine.cpp
  VIAMPPMachineFunctionInfo.cpp
  VIAMPPOptMachine.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  VIAMPPDesc
  VIAMPPInfo
  MC
  SelectionDAG
  Support
  Target

  ADD_TO_COMPONENT
  VIAMPP
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
