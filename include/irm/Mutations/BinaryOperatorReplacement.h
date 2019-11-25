//
//  Copyright 2019 Mull Project
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

#pragma once

#include "irm/IRMutation.h"
#include <llvm/IR/Instructions.h>

namespace irm {

class BinaryOperatorReplacement : public IRMutation {
public:
  BinaryOperatorReplacement(llvm::Instruction::BinaryOps from, llvm::Instruction::BinaryOps to);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  llvm::Instruction::BinaryOps from;
  llvm::Instruction::BinaryOps to;
};

#define BINARY_REPLACEMENT(FROM, TO)                                                               \
  class FROM##To##TO : public BinaryOperatorReplacement {                                          \
  public:                                                                                          \
    FROM##To##TO() : BinaryOperatorReplacement(llvm::Instruction::FROM, llvm::Instruction::TO) {}  \
  };

BINARY_REPLACEMENT(Add, Sub)
BINARY_REPLACEMENT(FAdd, FSub)
BINARY_REPLACEMENT(Sub, Add)
BINARY_REPLACEMENT(FSub, FAdd)

BINARY_REPLACEMENT(Mul, SDiv)
BINARY_REPLACEMENT(Mul, UDiv)
BINARY_REPLACEMENT(FMul, FDiv)

BINARY_REPLACEMENT(UDiv, URem)
BINARY_REPLACEMENT(SDiv, SRem)
BINARY_REPLACEMENT(FDiv, FRem)

BINARY_REPLACEMENT(UDiv, Mul)
BINARY_REPLACEMENT(SDiv, Mul)
BINARY_REPLACEMENT(FDiv, FMul)

BINARY_REPLACEMENT(URem, UDiv)
BINARY_REPLACEMENT(SRem, SDiv)
BINARY_REPLACEMENT(FRem, FDiv)

BINARY_REPLACEMENT(Shl, LShr)
BINARY_REPLACEMENT(LShr, Shl)
BINARY_REPLACEMENT(AShr, Shl)
BINARY_REPLACEMENT(And, Or)
BINARY_REPLACEMENT(Or, And)
BINARY_REPLACEMENT(Xor, Or)
BINARY_REPLACEMENT(Xor, And)

} // namespace irm
