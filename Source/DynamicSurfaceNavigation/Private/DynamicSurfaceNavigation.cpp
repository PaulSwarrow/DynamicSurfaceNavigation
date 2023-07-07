// Copyright Pagan Games. All rights reserved.

#include "DynamicSurfaceNavigation.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "DNavSurfaceComponentVisualizer.h"
#include "DynamicNavSurfaceComponent.h"
#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "FDynamicSurfaceNavigationModule"

void FDynamicSurfaceNavigationModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    // Check if editor is valid
    if (GUnrealEd)
    {
        // Registerin the move visualizer
        TSharedPtr<DNavSurfaceComponentVisualizer> DSurfaceVisualizer = MakeShareable(new DNavSurfaceComponentVisualizer);
        if (DSurfaceVisualizer.IsValid())
        {
            GUnrealEd->RegisterComponentVisualizer(UDynamicNavSurfaceComponent::StaticClass()->GetFName(), DSurfaceVisualizer);
            DSurfaceVisualizer->OnRegister();
        }
    }
}

void FDynamicSurfaceNavigationModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    // Check if editor is valid
    if (GUnrealEd)
    {
        GUnrealEd->UnregisterComponentVisualizer(UDynamicNavSurfaceComponent::StaticClass()->GetFName());
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicSurfaceNavigationModule, DynamicSurfaceNavigation)