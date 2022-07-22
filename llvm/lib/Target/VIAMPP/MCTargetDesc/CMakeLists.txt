add_llvm_component_library(LLVMVIAMPPDesc
  VIAMPPMCAsmInfo.cpp
  VIAMPPAsmBackend.cpp
  VIAMPPELFObjectWriter.cpp
  VIAMPPInstPrinter.cpp
  VIAMPPMCTargetDesc.cpp
  VIAMPPMCCodeEmitter.cpp

  LINK_COMPONENTS
  MC
  Support
  VIAMPPInfo

  ADD_TO_COMPONENT
  VIAMPP
  )

