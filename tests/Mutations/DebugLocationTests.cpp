#include <gtest/gtest.h>
#include <irm/irm.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Module.h>

using namespace irm;

class DebugLocation : public ::testing::Test {
protected:
  void SetUp() override {
    auto i8 = llvm::Type::getInt8Ty(context);
    auto i1 = llvm::Type::getInt1Ty(context);
    auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), { i8, i8, i1 }, false);
    function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "test", module);

    llvm::DIBuilder builder(module);
    auto file = builder.createFile("test.c", "/");
#if LLVM_VERSION_MAJOR >= 22
    auto unit = builder.createCompileUnit(
        llvm::DISourceLanguageName(llvm::dwarf::DW_LANG_C), file, "", false, "", 0);
#else
    auto unit = builder.createCompileUnit(llvm::dwarf::DW_LANG_C, file, "", false, "", 0);
#endif
    auto subprogram = builder.createFunction(unit,
                                             "test",
                                             "test",
                                             file,
                                             1,
                                             builder.createSubroutineType({}),
                                             1,
                                             llvm::DINode::FlagZero,
                                             llvm::DISubprogram::SPFlagDefinition);
    function->setSubprogram(subprogram);
    builder.finalize();
    location = llvm::DILocation::get(context, 7, 3, subprogram);

    entry = llvm::BasicBlock::Create(context, "entry", function);
    terminator = llvm::ReturnInst::Create(context, entry);
  }

  void expectAllLocated() {
    for (auto &instruction : *entry) {
      if (&instruction != terminator) {
        EXPECT_EQ(instruction.getDebugLoc().get(), location);
      }
    }
  }

  llvm::Instruction *locate(llvm::Value *value) {
    auto instruction = llvm::cast<llvm::Instruction>(value);
    instruction->setDebugLoc(location);
    return instruction;
  }

  llvm::LLVMContext context;
  llvm::Module module{ "test", context };
  llvm::Function *function = nullptr;
  llvm::BasicBlock *entry = nullptr;
  llvm::Instruction *terminator = nullptr;
  llvm::DILocation *location = nullptr;
};

TEST_F(DebugLocation, binaryOperatorReplacement) {
  llvm::IRBuilder<> builder(terminator);
  auto add = locate(builder.CreateAdd(function->getArg(0), function->getArg(1)));
  AddToSub().mutate(add);
  expectAllLocated();
}

TEST_F(DebugLocation, cmpInstPredicateReplacement) {
  llvm::IRBuilder<> builder(terminator);
  auto cmp = locate(builder.CreateICmpEQ(function->getArg(0), function->getArg(1)));
  ICMP_EQToICMP_NE().mutate(cmp);
  expectAllLocated();
}

TEST_F(DebugLocation, intrinsicReplacement) {
  llvm::IRBuilder<> builder(terminator);
  auto sadd = locate(builder.CreateBinaryIntrinsic(
      llvm::Intrinsic::sadd_with_overflow, function->getArg(0), function->getArg(1)));
  sadd_with_overflowTossub_with_overflow().mutate(sadd);
  expectAllLocated();
}

TEST_F(DebugLocation, negateTruncReplacement) {
  llvm::IRBuilder<> builder(terminator);
  auto trunc = locate(builder.CreateTrunc(function->getArg(0), builder.getInt1Ty()));
  NegateTruncReplacement().mutate(trunc);
  expectAllLocated();
}

TEST_F(DebugLocation, negateXORReplacement) {
  llvm::IRBuilder<> builder(terminator);
  auto xorInstruction = locate(builder.CreateXor(function->getArg(2), function->getArg(2)));
  NegateXORReplacement().mutate(xorInstruction);
  expectAllLocated();
}
