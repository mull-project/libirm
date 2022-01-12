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

#pragma once

#include "irm/IRMutation.h"
#include <llvm/IR/Instructions.h>

namespace irm {

class SwapInstructionWithOperand : public IRMutation {
public:
  SwapInstructionWithOperand(unsigned opcode, unsigned index);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  unsigned opcode;
  unsigned index;
};

#define SWAP_BINARY_WITH_OPERAND(OP)                                                               \
  class Swap##OP##WithOperand_0 : public SwapInstructionWithOperand {                              \
  public:                                                                                          \
    Swap##OP##WithOperand_0()                                                                      \
        : SwapInstructionWithOperand(llvm::BinaryOperator::BinaryOps::OP, 0) {}                    \
  };                                                                                               \
  class Swap##OP##WithOperand_1 : public SwapInstructionWithOperand {                              \
  public:                                                                                          \
    Swap##OP##WithOperand_1()                                                                      \
        : SwapInstructionWithOperand(llvm::BinaryOperator::BinaryOps::OP, 1) {}                    \
  };

#define SWAP_UNARY_WITH_OPERAND(OP)                                                                \
  class Swap##OP##WithOperand : public SwapInstructionWithOperand {                                \
  public:                                                                                          \
    Swap##OP##WithOperand() : SwapInstructionWithOperand(llvm::UnaryOperator::UnaryOps::OP, 0) {}  \
  };

SWAP_BINARY_WITH_OPERAND(Add)
SWAP_BINARY_WITH_OPERAND(FAdd)

SWAP_BINARY_WITH_OPERAND(Sub)
SWAP_BINARY_WITH_OPERAND(FSub)

SWAP_BINARY_WITH_OPERAND(Mul)
SWAP_BINARY_WITH_OPERAND(FMul)

SWAP_BINARY_WITH_OPERAND(FDiv)
SWAP_BINARY_WITH_OPERAND(UDiv)
SWAP_BINARY_WITH_OPERAND(SDiv)

SWAP_BINARY_WITH_OPERAND(FRem)
SWAP_BINARY_WITH_OPERAND(URem)
SWAP_BINARY_WITH_OPERAND(SRem)

SWAP_BINARY_WITH_OPERAND(Shl)
SWAP_BINARY_WITH_OPERAND(LShr)
SWAP_BINARY_WITH_OPERAND(AShr)

SWAP_BINARY_WITH_OPERAND(And)
SWAP_BINARY_WITH_OPERAND(Or)
SWAP_BINARY_WITH_OPERAND(Xor)
SWAP_UNARY_WITH_OPERAND(FNeg)

} // namespace irm
