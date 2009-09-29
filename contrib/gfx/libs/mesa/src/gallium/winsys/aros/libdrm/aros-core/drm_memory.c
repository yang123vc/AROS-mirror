#include "drmP.h"

#include <proto/exec.h>

void *drm_calloc(size_t nmemb, size_t size, int area)
{
    return AllocMem(nmemb * size, MEMF_PUBLIC | MEMF_CLEAR);
}

void *drm_alloc(size_t size, int area)
{
    return AllocMem(size, MEMF_PUBLIC);
}

void drm_free(void *pt, size_t size, int area)
{
    FreeMem(pt, size);
}

void drm_core_ioremap(struct drm_map *map, struct drm_device *dev)
{
#if !defined(__AROS__)    
    if (drm_core_has_AGP(dev) &&
        dev->agp && dev->agp->cant_use_aperture && map->type == _DRM_AGP)
        map->handle = agp_remap(map->offset, map->size, dev);
    else
#endif        
        map->handle = drm_pci_ioremap(dev->pcidriver, (APTR)map->offset, map->size);
}
