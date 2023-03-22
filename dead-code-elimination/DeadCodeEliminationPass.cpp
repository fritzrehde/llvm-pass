#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Transforms/Utils/Local.h"

using namespace llvm;

namespace {

//
// Implementation
//
static bool eliminateDeadCode(Function &F, TargetLibraryInfo *TLI, TargetTransformInfo *TTI) {
    bool MadeChange = false;
    bool AllUnreachableBBsEliminated = false;

    while (!AllUnreachableBBsEliminated) {
        std::vector<Instruction*> Dead;

        // Find all instructions to be removed
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
            if (isInstructionTriviallyDead(&*I, TLI)) {
                Dead.push_back(&*I);
                MadeChange = true;
            }
        }

        // Remove all dead instructions
        for (Instruction *&I : Dead) {
            I->eraseFromParent();
        }

        // eliminate unreachable basic blocks
        std::vector<BasicBlock *> BlocksToSimplify;
        for (BasicBlock &BB : F) {
            BlocksToSimplify.push_back(&BB);
        }

        for (BasicBlock *BB : BlocksToSimplify) {
            if (simplifyCFG(BB, *TTI)) {
                MadeChange = true;
            } else {
                AllUnreachableBBsEliminated = true;
            }
        }
    }

    return MadeChange;
}

struct DeadCodeEliminationPass : PassInfoMixin<DeadCodeEliminationPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        if (!eliminateDeadCode(F, &AM.getResult<TargetLibraryAnalysis>(F), &AM.getResult<TargetIRAnalysis>(F)))
            // All analyses are still valid after this pass (as nothing was modified)
            return PreservedAnalyses::all();

        // TODO: why?
        PreservedAnalyses PA;
        PA.preserveSet<CFGAnalyses>();
        return PA;
    }

    // Declare as a required pass, which may not be skipped (e.g. due to
    // functions being decorated with the optnone LLVM attribute)
    static bool isRequired() { return true; }
};
} // namespace


//
// Registration
//
PassPluginLibraryInfo getPassPluginInfo() {
    const auto callback = [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef name, FunctionPassManager &FPM, auto) {
                if (name == "dead-code-elimination") {
                    FPM.addPass(DeadCodeEliminationPass());
                    return true;
                }
                return false;
        });
    };
    return {LLVM_PLUGIN_API_VERSION, "DeadCodeEliminationPass", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getPassPluginInfo();
}
