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

class SwapBinaryOperands : public IRMutation {
public:
  explicit SwapBinaryOperands(llvm::Instruction::BinaryOps op);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  llvm::Instruction::BinaryOps op;
};

#define SWAP_OPERANDS(OP)                                                                          \
  class Swap##OP##Operands : public SwapBinaryOperands {                                           \
  public:                                                                                          \
    Swap##OP##Operands() : SwapBinaryOperands(llvm::Instruction::OP) {}                            \
  };

SWAP_OPERANDS(Add)
SWAP_OPERANDS(FAdd)

SWAP_OPERANDS(Sub)
SWAP_OPERANDS(FSub)

SWAP_OPERANDS(Mul)
SWAP_OPERANDS(FMul)

SWAP_OPERANDS(FDiv)
SWAP_OPERANDS(UDiv)
SWAP_OPERANDS(SDiv)

SWAP_OPERANDS(FRem)
SWAP_OPERANDS(URem)
SWAP_OPERANDS(SRem)

SWAP_OPERANDS(Shl)
SWAP_OPERANDS(LShr)
SWAP_OPERANDS(AShr)

SWAP_OPERANDS(And)
SWAP_OPERANDS(Or)
SWAP_OPERANDS(Xor)

} // namespace irm
