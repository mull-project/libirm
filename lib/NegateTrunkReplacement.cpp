//
//  Copyright 2020 Mull Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "irm/Mutations/NegateTruncReplacement.h"

#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

using namespace irm;

NegateTruncReplacement::NegateTruncReplacement() {}

bool NegateTruncReplacement::canMutate(llvm::Instruction *instruction) {
  assert(instruction);

  if (llvm::TruncInst *truncInst = llvm::dyn_cast<llvm::TruncInst>(instruction)) {
    if (truncInst->getDestTy()->getIntegerBitWidth() == 1) {
      return true;
    }
  }

  return false;
}

void NegateTruncReplacement::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  assert(canMutate(instruction));

  /// This mutation adds a xor instruction after the given trunc instruction.
  /// The added xor instruction negates the value of the trunc instruction.
  /// On the AST level the example of this transformation can look like this:
  /// bool isTrue(bool a) {
  ///   return a; -> return !a;
  /// }
  ///
  /// Before mutation (reduced version):
  /// %0 = alloca i32
  /// store i32 1, i32* %0
  /// %1 = load i32, i32* %0
  /// %2 = trunc i32 %1 to i1
  /// ret i1 %2
  ///
  /// After mutation (reduced version):
  /// %0 = alloca i32
  /// store i32 1, i32* %0
  /// %1 = load i32, i32* %0
  /// %2 = trunc i32 %1 to i1
  /// %not = xor i1 %2, true
  /// ret i1 %not

  llvm::TruncInst *truncInst = llvm::cast<llvm::TruncInst>(instruction);

  auto notInstruction = llvm::BinaryOperator::CreateNot(truncInst, "not");
  notInstruction->insertAfter(truncInst);

  truncInst->replaceAllUsesWith(notInstruction);
  notInstruction->replaceUsesOfWith(notInstruction, truncInst);
}
