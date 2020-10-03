# libirm

Low-level IR mutations for [LLVM](https://llvm.org) Bitcode.

## Build/Install

The easiest way (not necessarily the right one):

```bash
git clone https://github.com/mull-project/libirm.git --recursive
mkdir build.dir; cd build.dir
cmake -G Ninja ../libirm
ninja all
sudo ninja install
```

You'll get `libirm.a` and required headers under `/usr/local/`.

## Usage

libirm operates on the level of a single LLVM instruction.

To use it you need only two methods:

```c++
bool canMutate(llvm::Instruction *instruction);
void mutate(llvm::Instruction *instruction);
```

The code below changes every [`add`](https://llvm.org/docs/LangRef.html#add-instruction) instruction into [`sub`](https://llvm.org/docs/LangRef.html#sub-instruction) instruction.

```c++
#include <irm/irm.h>

void doSmth() {
  llvm::Function *function = getTheFunctionSomewhere();
  irm::AddToSub mutation;
  for (auto &instruction : llvm::instructions(function)) {
    if (mutation.canMutate(&instruction)) {
      mutation.mutate(&instruction);
    }
  }
}
```

## Available Mutations

The set of currently available mutation is driven by the needs of the [Mull](https://github.com/mull-project/mull) project,
so it is not exhaustive.

### BinaryOperatorReplacement

```c++
BinaryOperatorReplacement(llvm::Instruction::BinaryOps from,
                          llvm::Instruction::BinaryOps to)
```

Replaces binary operator(`from`) with another one (`to`).

Examples:

 - `irm::AddToSub`: `add i32 %a, i32 %b` -> `sub i32 %a, i32 %b`
 - `irm::OrToAnd`: `or i32 %a, i32 %b` -> `and i32 %a, i32 %b`

You can find the list of predefined mutations at [`BinaryOperatorReplacement.h`](include/irm/Mutations/BinaryOperatorReplacement.h).

You can also create any missing mutation manually:

```c++
irm::BinaryOperatorReplacement addToXor(llvm::Instruction::BinaryOps::Add,
                                        llvm::Instruction::BinaryOps::Xor);
```

This will yield the `add i32 %a, i32 %b` to `xor i32 %a, i32 %b` mutation.

### SwapBinaryOperands

```c++
SwapBinaryOperands(llvm::Instruction::BinaryOps op)
```

Swaps operands of a binary operator(`op`).

Examples:

 - `irm::SwapSubOperands`: `sub i32 %a, i32 %b` -> `sub i32 %b, i32 %a`
 - `irm::SwapSDivOperands`: `sdiv i32 %a, i32 %b` -> `sdiv i32 %b, i32 %a`

You can find the list of predefined mutations at [`SwapBinaryOperands.h`](include/irm/Mutations/SwapBinaryOperands.h).

You can also create any missing mutation manually:

```c++
irm::SwapBinaryOperands swapSub(llvm::Instruction::BinaryOps::Sub);
```

This will yield the `sub i32 %a, i32 %b` to `sub i32 %b, i32 %a` mutation.

### SwapInstructionWithOperand

```c++
SwapInstructionWithOperand(unsigned opcode, unsigned index);
```

Replaces all uses of the instruction of kind `opcode` with its `index` operand.

Examples:

 - `irm::SwapAddWithOperand_0`: `add i32 %a, i32 %b` -> `%a`
 - `irm::SwapFNegWithOperand`: `fneg float %a` -> `%a`

You can find the list of predefined mutations at [`SwapInstructionWithOperand.h`](include/irm/Mutations/SwapInstructionWithOperand.h).

### CallReplacement

```c++
CallReplacement(ConstValueConstructor *constConstructor, llvm::Type::TypeID returnTypeId)
```

Replaces a call with return type `returnTypeId` with a constant provided by `ConstValueConstructor`.

For example, `irm::IntCallReplacement mutation(42)` replaces `call i32 @bar()` with `42`.

You can find the list of predefined mutations at [`CallReplacement.h`](include/irm/Mutations/CallReplacement.h).

You can create any missing mutation manually:

```c++
irm::CallReplacement halfReplacement(new FloatingValueConstructor(18.f), llvm::Type::HalfTyID);
```

This yields `call half @bar()` -> `half 18` replacement.

### CmpInstPredicateReplacement

```c++
CmpInstPredicateReplacement(llvm::Instruction::OtherOps cmpType,
                            llvm::CmpInst::Predicate from,
                            llvm::CmpInst::Predicate to)
```

Replaces comparison predicate.
Examples:

 - `irm::ICMP_EQToICMP_NE`: `icmp eq %a, i32 %b` -> `icmp ne i32 %a, i32 %b`
 - `irm::ICMP_SGTToICMP_SGE`: `icmp sgt i32 %a, i32 %b` -> `icmp sge i32 %a, i32 %b`

You can find the list of predefined mutations at [`CmpInstPredicateReplacement.h`](include/irm/Mutations/CmpInstPredicateReplacement.h).

You can also create any missing mutation manually:

```c++
irm::CmpInstPredicateReplacement lessOrEqualToEqual(llvm::Instruction::FCmp,
                                                    llvm::CmpInst::FCMP_OLE,
                                                    llvm::CmpInst::FCMP_OEQ);
```

This will yield the `fcmp ole i32 %a, i32 %b` to `fcmp oeq i32 %a, i32 %b` mutation.

### ConstantReplacement

```c++
ConstantReplacement(ConstValueConstructor *constConstructor,
                    ConstValueConstructor *zeroConstConstructor,
                    llvm::Value::ValueTy valueType,
                    unsigned operandPosition)
```

Replaces a constant with a value provided by `constConstructor` if the constant is zero. If the constant is not zero, then it is replaces by zero value provided by `zeroConstConstructor`.

Applies only to an `operandPosition` operand.

For example, `irm::ConstIntReplacement mutation(42, 1)` will mutate `add 22, 0` to `add 22, 42`, while
 `add i32 22, i32 15` mutates to `add i32 22, i32 0`.

Please note that only second operand is affected in both cases.

You can find the list of predefined mutations at [`ConstantReplacement.h`](include/irm/Mutations/ConstantReplacement.h).

### IntrinsicReplacement

```c++
IntrinsicReplacement(llvm::Intrinsic::ID from,
                     llvm::Intrinsic::ID to)
```

Replaces one intrinsic(`from`) with another(`to`). It is assumed that both intrinsics have the same signature.

For example, `irm::sadd_with_overflowTossub_with_overflow mutation` will replace any call to `llvm.sadd.with.overflow.*` with `llvm.ssub.with.overflow.*`.

You can find the list of predefined mutations at [`IntrinsicReplacement.h`](include/irm/Mutations/IntrinsicReplacement.h).

You can create custom mutations:

```
irm::IntrinsicReplacement sinToCos(llvm::Intrinsic::int_cos,
                                   llvm::Intrinsic::int_sin);
```

### StoreValueReplacement

```c++
StoreValueReplacement(ConstValueConstructor *constConstructor,
                      llvm::Type::TypeID typeID)
```

Similar to `CallReplacement`, but replaces a `store` value with a constant provided by `constConstructor`.
For example, `irm::StoreIntReplacement mutation(15)` will change `store i32 %x, i32* %addr` to `store i32 15, i32* %addr`.

You can find the list of predefined mutations at [`StoreValueReplacement.h`](include/irm/Mutations/StoreValueReplacement.h).

You can create any missing mutation manually:

```c++
irm::StoreValueReplacement halfReplacement(new FloatingValueConstructor(18.f),
                                           llvm::Type::HalfTyID);
```

### VoidCallRemoval

```c++
VoidCallRemoval(bool onlyIntrinsics)
```

Removes a call to a `void` function or intrinsic. There are two shortcut classes `RemoveVoidFunctionCall` and `RemoveVoidIntrinsicsCall`, that remove only function calls and only intrinsics, respectively.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
Please make sure to update tests as appropriate.

## License

[Apache 2](LICENSE)