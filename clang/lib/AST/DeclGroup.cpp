//===--- DeclGroup.cpp - Classes for representing groups of Decls -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the DeclGroup and DeclGroupRef classes.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/DeclGroup.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ASTContext.h"
#include "llvm/Support/Allocator.h"
using namespace clang;

DeclGroup* DeclGroup::Create(ASTContext &C, Decl **Decls, unsigned NumDecls) {
  assert(NumDecls > 1 && "Invalid DeclGroup");
  unsigned Size = sizeof(DeclGroup) + sizeof(Decl*) * NumDecls;
  void* Mem = C.Allocate(Size, llvm::AlignOf<DeclGroup>::Alignment);
  new (Mem) DeclGroup(NumDecls, Decls);
  return static_cast<DeclGroup*>(Mem);
}

DeclGroup::DeclGroup(unsigned numdecls, Decl** decls) : NumDecls(numdecls) {
  assert(numdecls > 0);
  assert(decls);
  memcpy(this+1, decls, numdecls * sizeof(*decls));
}

void DeclGroup::Destroy(ASTContext& C) {
  // Decls are destroyed by the DeclContext.
  this->~DeclGroup();
  C.Deallocate((void*) this);
}
