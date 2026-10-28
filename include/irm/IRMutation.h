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

#include <string>

namespace llvm {
class Instruction;
}

namespace irm {

class IRMutation {
public:
  virtual bool canMutate(llvm::Instruction *instruction) = 0;
  virtual void mutate(llvm::Instruction *instruction) = 0;
  /// Human-readable value this mutation would put in place of the instruction's
  /// original value, e.g. "43" or "true". Must be deterministic and consistent
  /// with mutate(). Empty string means "no instruction-specific description".
  virtual std::string describeReplacement(llvm::Instruction *instruction) {
    return {};
  }
  virtual ~IRMutation() = default;
};

} // namespace irm
