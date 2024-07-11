#pragma once

#include <iostream>
#include <string>

static unsigned char ucharIntensity(unsigned char uc, float intensity)
{
    return (unsigned char)std::clamp((int)round(((float)uc) * intensity), 0, 255);
}