// 2020 Sticky Snout Studio (Jonas Molgaard)

#include "UBIKRuntime.h"

DEFINE_LOG_CATEGORY(LogUBIKRuntime);

#define LOCTEXT_NAMESPACE "UBIKRuntime"
DECLARE_STATS_GROUP(TEXT("UBIK"), STATGROUP_UBIK, STATCAT_Advanced);
// DisplayName, GroupName (ends up as: "LODZERO"), Third param is always Advanced.

void FUBIKRuntime::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUBIKRuntime::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUBIKRuntime, UBIKRuntime)
