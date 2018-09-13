#include "araytag.h"

#include <string.h>


ArrayTag::ArrayTag()
{
    memset(&tags,0,sizeof (tags));
}

void *ArrayTag::getTagValue(eGCodeTag index)
{
    void *tag = nullptr;
    switch (index) {
    case eG0:
        tag = &tags.G0;
        break;
    case eG1:
        tag = &tags.G1;
        break;
    case eG2:
        tag = &tags.G2;
        break;
    case eG3:
        tag = &tags.G3;
        break;
    case eG4:
        tag = &tags.G4;
        break;
    case eG6:
        tag = &tags.G6;
        break;
    case eG10:
        tag = &tags.G10;
        break;
    case eG20:
        tag = &tags.G20;
        break;
    case eG21:
        tag = &tags.G21;
        break;
    case eG28:
        tag = &tags.G28;
    case eG29_1:
        tag = &tags.G29_1;
        break;
    case eG29_2:
        tag = &tags.G29_2;
        break;
    case eG30:
        tag = &tags.G30;
        break;
    case eG33:
        tag = &tags.G33;
        break;
    case eG90:
        tag = &tags.G90;
        break;
    case eG91:
        tag = &tags.G91;
        break;
    case eG92:
        tag = &tags.G92;
        break;
    case eG92_1:
        tag = &tags.G92_1;
        break;
    case eM104:
        tag = &tags.M104;
        break;
    case eM106:
        tag = &tags.M106;
        break;
    case eM107:
        tag = &tags.M107;
        break;
    case eM109:
        tag = &tags.M109;
    case eM140:
        tag = &tags.M140;
        break;
    case eM190:
        tag = &tags.M190;
        break;
    case eM550:
        tag = &tags.M550;
        break;
    case eM82:
        tag = &tags.M82;
        break;
    case eM84:
        tag = &tags.M84;
        break;
    default:
        tag = nullptr;
    }

    return  tag;
}
