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
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Intrinsics.h>

namespace irm {

class IntrinsicReplacement : public IRMutation {
public:
  IntrinsicReplacement(llvm::Intrinsic::ID from, llvm::Intrinsic::ID to);
  bool canMutate(llvm::Instruction *instruction) override;

  void mutate(llvm::Instruction *instruction) override;

private:
  llvm::Intrinsic::ID from;
  llvm::Intrinsic::ID to;
};

#define INTRINSIC_REPLACEMENT(FROM, TO)                                                            \
  class FROM##To##TO : public IntrinsicReplacement {                                               \
  public:                                                                                          \
    FROM##To##TO() : IntrinsicReplacement(llvm::Intrinsic::FROM, llvm::Intrinsic::TO) {}           \
  };

INTRINSIC_REPLACEMENT(sadd_with_overflow, ssub_with_overflow)
INTRINSIC_REPLACEMENT(ssub_with_overflow, sadd_with_overflow)

//INTRINSIC_REPLACEMENT(sadd_sat, ssub_sat)
//INTRINSIC_REPLACEMENT(ssub_sat, sadd_sat)

} // namespace irm
