#ifndef OPENXR_EXTENSION_HELPERS_H_
#define OPENXR_EXTENSION_HELPERS_H_ 1

#include "openxr.h"

#define XR_ENUM(type, enm, constant) static const type enm = (type)constant
#define XR_STRUCT_ENUM(enm, constant) XR_ENUM(XrStructureType, enm, constant)
#define XR_REFSPACE_ENUM(enm, constant) XR_ENUM(XrReferenceSpaceType, enm, constant)
#define XR_RESULT_ENUM(enm, constant) XR_ENUM(XrResult, enm, constant)
#define XR_COMPONENT_ENUM(enm, constant) XR_ENUM(XrComponentTypeFB, enm, constant)

#endif