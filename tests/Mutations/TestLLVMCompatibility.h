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

#include <llvm/IR/Function.h>

namespace test_llvm_compat {

static llvm::Function *internalFunction(llvm::FunctionType *functionType, const llvm::Twine &name,
                                        llvm::Module &module) {
#if LLVM_VERSION_MAJOR < 8
  return llvm::Function::Create(functionType, llvm::Function::InternalLinkage, name, &module);
#else
  return llvm::Function::Create(functionType, llvm::Function::InternalLinkage, name, module);
#endif
}

} // namespace test_llvm_compat
