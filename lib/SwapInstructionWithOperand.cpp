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

#include "irm/Mutations/SwapInstructionWithOperand.h"

using namespace irm;

SwapInstructionWithOperand::SwapInstructionWithOperand(unsigned int opcode, unsigned int index)
    : opcode(opcode), index(index) {}

bool SwapInstructionWithOperand::canMutate(llvm::Instruction *instruction) {
  return instruction->getOpcode() == opcode && index < instruction->getNumOperands();
}

void SwapInstructionWithOperand::mutate(llvm::Instruction *instruction) {
  assert(canMutate(instruction));
  assert(instruction->getParent());
  instruction->replaceAllUsesWith(instruction->getOperand(index));
}
