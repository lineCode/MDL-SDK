//===- MCJITTestBase.h - Common base class for MCJIT Unit tests  ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements functionality shared by both MCJIT C API tests, and
// the C++ API tests.
//
//===----------------------------------------------------------------------===//

#ifndef MCJIT_TEST_API_COMMON_H
#define MCJIT_TEST_API_COMMON_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

// Used to skip tests on unsupported architectures and operating systems.
// To skip a test, add this macro at the top of a test-case in a suite that
// inherits from MCJITTestBase. See MCJITTest.cpp for examples.
#define SKIP_UNSUPPORTED_PLATFORM \
  do \
    if (!ArchSupportsMCJIT() || !OSSupportsMCJIT()) \
      return; \
  while(0)

namespace llvm {

class MCJITTestAPICommon {
protected:
  MCJITTestAPICommon()
    : HostTriple(sys::getProcessTriple())
  {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

#ifdef LLVM_ON_WIN32
    // On Windows, generate ELF objects by specifying "-elf" in triple
    HostTriple += "-elf";
#endif // LLVM_ON_WIN32
    HostTriple = Triple::normalize(HostTriple);
  }

  /// Returns true if the host architecture is known to support MCJIT
  bool ArchSupportsMCJIT() {
    Triple Host(HostTriple);
    // If ARCH is not supported, bail
    if (MISTD::find(SupportedArchs.begin(), SupportedArchs.end(), Host.getArch())
        == SupportedArchs.end())
      return false;

    // If ARCH is supported and has no specific sub-arch support
    if (MISTD::find(HasSubArchs.begin(), HasSubArchs.end(), Host.getArch())
        == HasSubArchs.end())
      return true;

    // If ARCH has sub-arch support, find it
    SmallVectorImpl<MISTD::string>::const_iterator I = SupportedSubArchs.begin();
    for(; I != SupportedSubArchs.end(); ++I)
      if (Host.getArchName().startswith(I->c_str()))
        return true;

    return false;
  }

  /// Returns true if the host OS is known to support MCJIT
  bool OSSupportsMCJIT() {
    Triple Host(HostTriple);
    if (MISTD::find(UnsupportedOSs.begin(), UnsupportedOSs.end(), Host.getOS())
        == UnsupportedOSs.end()) {
      return true;
    }
    return false;
  }

  MISTD::string HostTriple;
  SmallVector<Triple::ArchType, 4> SupportedArchs;
  SmallVector<Triple::ArchType, 1> HasSubArchs;
  SmallVector<MISTD::string, 2> SupportedSubArchs; // We need to own the memory
  SmallVector<Triple::OSType, 4> UnsupportedOSs;
};

} // namespace llvm

#endif // MCJIT_TEST_API_COMMON_H

