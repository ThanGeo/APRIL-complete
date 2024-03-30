#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include "def.h"
#include "utils.h"

#include <deque>

namespace rasterizerlib
{
    uint32_t** calculatePartialAndUncertain(polygon2d &polygon, uint32_t &orderN);

    void rasterizationBegin(polygon2d &polygon);
    void rasterizationPartialOnlyBegin(polygon2d &polygon);
}


#endif