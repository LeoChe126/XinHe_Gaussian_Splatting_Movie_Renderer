#include "CSGaussianRendererModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FCSGaussianRendererModule"

void FCSGaussianRendererModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(
        IPluginManager::Get().FindPlugin(TEXT("CSGaussianRenderer"))->GetBaseDir(),
        TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/CSGaussianRenderer"), PluginShaderDir);
}

void FCSGaussianRendererModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCSGaussianRendererModule, CSGaussianRenderer)
