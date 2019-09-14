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

namespace irm {

class VoidCallRemoval : public IRMutation {
public:
  explicit VoidCallRemoval(bool onlyIntrinsics);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  bool onlyIntrinsics;
};

class RemoveVoidFunctionCall : public VoidCallRemoval {
public:
  RemoveVoidFunctionCall() : VoidCallRemoval(false) {}
};

class RemoveVoidIntrinsicsCall : public VoidCallRemoval {
public:
  RemoveVoidIntrinsicsCall() : VoidCallRemoval(true) {}
};

} // namespace irm
