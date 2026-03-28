#include "CSGaussianRendererModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FGSMovieModule"

void FGSMovieModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(
        IPluginManager::Get().FindPlugin(TEXT("GSMovie"))->GetBaseDir(),
        TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/GSMovie"), PluginShaderDir);
}

void FGSMovieModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGSMovieModule, GSMovie)
