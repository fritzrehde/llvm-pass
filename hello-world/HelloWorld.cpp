#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

// Implementation
struct HelloWorldPass : PassInfoMixin<HelloWorldPass> {
  // Runs the pass; takes IR unit (to run the pass on) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    // TODO: is it a convention to put this implementation into a visitor(Function &F) method?
    errs() << "(llvm-pass: hello-world) function            : " << F.getName() << "\n";
    errs() << "(llvm-pass: hello-world) number of arguments : " << F.arg_size() << "\n";

    // All analyses are still valid after this pass (as nothing was modified)
    return PreservedAnalyses::all();
  }

  // Declare as a required pass, which may not be skipped (e.g. due to
  // functions being decorated with the optnone LLVM attribute)
  static bool isRequired() { return true; }
};
} // namespace

// Registration
PassPluginLibraryInfo getHelloWorldPluginInfo() {
  const auto callback = [](PassBuilder &PB) {
    PB.registerPipelineParsingCallback(
        [](StringRef name, FunctionPassManager &FPM, auto) {
          if (name == "hello-world") {
            FPM.addPass(HelloWorldPass());
            return true;
          }
          return false;
        });
  };
  return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getHelloWorldPluginInfo();
}
