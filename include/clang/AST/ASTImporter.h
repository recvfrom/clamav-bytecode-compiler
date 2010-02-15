//===--- ASTImporter.h - Importing ASTs from other Contexts -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ASTImporter class which imports AST nodes from one
//  context into another context.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_AST_ASTIMPORTER_H
#define LLVM_CLANG_AST_ASTIMPORTER_H

#include "clang/AST/Type.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"

namespace clang {
  class ASTContext;
  class Decl;
  class DeclContext;
  class Diagnostic;
  class Expr;
  class FileManager;
  class IdentifierInfo;
  class NestedNameSpecifier;
  class Stmt;
  class TypeSourceInfo;
  
  /// \brief Imports selected nodes from one AST context into another context,
  /// merging AST nodes where appropriate.
  class ASTImporter {
    /// \brief The contexts we're importing to and from.
    ASTContext &ToContext, &FromContext;
    
    /// \brief The file managers we're importing to and from.
    FileManager &ToFileManager, &FromFileManager;
    
    /// \brief The diagnostics object that we should use to emit diagnostics.
    Diagnostic &Diags;
    
    /// \brief Mapping from the already-imported types in the "from" context
    /// to the corresponding types in the "to" context.
    llvm::DenseMap<Type *, Type *> ImportedTypes;
    
    /// \brief Mapping from the already-imported declarations in the "from"
    /// context to the corresponding declarations in the "to" context.
    llvm::DenseMap<Decl *, Decl *> ImportedDecls;

    /// \brief Mapping from the already-imported statements in the "from"
    /// context to the corresponding statements in the "to" context.
    llvm::DenseMap<Stmt *, Stmt *> ImportedStmts;

    /// \brief Mapping from the already-imported FileIDs in the "from" source
    /// manager to the corresponding FileIDs in the "to" source manager.
    llvm::DenseMap<unsigned, FileID> ImportedFileIDs;
    
  public:
    ASTImporter(Diagnostic &Diags,
                ASTContext &ToContext, FileManager &ToFileManager,
                ASTContext &FromContext, FileManager &FromFileManager);
    
    virtual ~ASTImporter();
    
    /// \brief Import the given type from the "from" context into the "to"
    /// context.
    ///
    /// \returns the equivalent type in the "to" context, or a NULL type if
    /// an error occurred.
    QualType Import(QualType FromT);

    /// \brief Import the given type source information from the
    /// "from" context into the "to" context.
    ///
    /// \returns the equivalent type source information in the "to"
    /// context, or NULL if an error occurred.
    TypeSourceInfo *Import(TypeSourceInfo *FromTSI);

    /// \brief Import the given declaration from the "from" context into the 
    /// "to" context.
    ///
    /// \returns the equivalent declaration in the "to" context, or a NULL type 
    /// if an error occurred.
    Decl *Import(Decl *FromD);

    /// \brief Import the given declaration context from the "from"
    /// AST context into the "to" AST context.
    ///
    /// \returns the equivalent declaration context in the "to"
    /// context, or a NULL type if an error occurred.
    DeclContext *ImportContext(DeclContext *FromDC);
    
    /// \brief Import the given expression from the "from" context into the
    /// "to" context.
    ///
    /// \returns the equivalent expression in the "to" context, or NULL if
    /// an error occurred.
    Expr *Import(Expr *FromE);

    /// \brief Import the given statement from the "from" context into the
    /// "to" context.
    ///
    /// \returns the equivalent statement in the "to" context, or NULL if
    /// an error occurred.
    Stmt *Import(Stmt *FromS);

    /// \brief Import the given nested-name-specifier from the "from"
    /// context into the "to" context.
    ///
    /// \returns the equivalent nested-name-specifier in the "to"
    /// context, or NULL if an error occurred.
    NestedNameSpecifier *Import(NestedNameSpecifier *FromNNS);
    
    /// \brief Import the given source location from the "from" context into
    /// the "to" context.
    ///
    /// \returns the equivalent source location in the "to" context, or an
    /// invalid source location if an error occurred.
    SourceLocation Import(SourceLocation FromLoc);

    /// \brief Import the given source range from the "from" context into
    /// the "to" context.
    ///
    /// \returns the equivalent source range in the "to" context, or an
    /// invalid source location if an error occurred.
    SourceRange Import(SourceRange FromRange);

    /// \brief Import the given declaration name from the "from"
    /// context into the "to" context.
    ///
    /// \returns the equivalent declaration name in the "to" context,
    /// or an empty declaration name if an error occurred.
    DeclarationName Import(DeclarationName FromName);

    /// \brief Import the given identifier from the "from" context
    /// into the "to" context.
    ///
    /// \returns the equivalent identifier in the "to" context.
    IdentifierInfo *Import(IdentifierInfo *FromId);

    /// \brief Import the given file ID from the "from" context into the 
    /// "to" context.
    ///
    /// \returns the equivalent file ID in the source manager of the "to"
    /// context.
    FileID Import(FileID);
    
    /// \brief Cope with a name conflict when importing a declaration into the
    /// given context.
    ///
    /// This routine is invoked whenever there is a name conflict while 
    /// importing a declaration. The returned name will become the name of the
    /// imported declaration. By default, the returned name is the same as the
    /// original name, leaving the conflict unresolve such that name lookup
    /// for this name is likely to find an ambiguity later.
    ///
    /// Subclasses may override this routine to resolve the conflict, e.g., by
    /// renaming the declaration being imported.
    ///
    /// \param Name the name of the declaration being imported, which conflicts
    /// with other declarations.
    ///
    /// \param DC the declaration context (in the "to" AST context) in which 
    /// the name is being imported.
    ///
    /// \param IDNS the identifier namespace in which the name will be found.
    ///
    /// \param Decls the set of declarations with the same name as the
    /// declaration being imported.
    ///
    /// \param NumDecls the number of conflicting declarations in \p Decls.
    ///
    /// \returns the name that the newly-imported declaration should have.
    virtual DeclarationName HandleNameConflict(DeclarationName Name,
                                               DeclContext *DC,
                                               unsigned IDNS,
                                               NamedDecl **Decls,
                                               unsigned NumDecls);
    
    /// \brief Retrieve the context that AST nodes are being imported into.
    ASTContext &getToContext() const { return ToContext; }
    
    /// \brief Retrieve the context that AST nodes are being imported from.
    ASTContext &getFromContext() const { return FromContext; }
    
    /// \brief Retrieve the file manager that AST nodes are being imported into.
    FileManager &getToFileManager() const { return ToFileManager; }

    /// \brief Retrieve the file manager that AST nodes are being imported from.
    FileManager &getFromFileManager() const { return FromFileManager; }

    /// \brief Retrieve the diagnostic formatter.
    Diagnostic &getDiags() const { return Diags; }
    
    /// \brief Report a diagnostic in the "to" context.
    DiagnosticBuilder ToDiag(SourceLocation Loc, unsigned DiagID);
    
    /// \brief Report a diagnostic in the "from" context.
    DiagnosticBuilder FromDiag(SourceLocation Loc, unsigned DiagID);
    
    /// \brief Note that we have imported the "from" declaration by mapping it
    /// to the (potentially-newly-created) "to" declaration.
    ///
    /// \returns \p To
    Decl *Imported(Decl *From, Decl *To);
  };
}

#endif // LLVM_CLANG_AST_ASTIMPORTER_H
