#include "SamplePanels.h"
#include <PanelRegistry.h>

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------
void registerSamplePanels()
{
    // Dummy config panels removed - HW Configuration is now via Tools menu dialog.
    // Add any future non-config sample panels here.
    (void)DockManager::PanelRegistry::instance(); // ensure registry exists
}
