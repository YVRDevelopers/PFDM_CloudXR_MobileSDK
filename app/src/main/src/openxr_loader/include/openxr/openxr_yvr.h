#ifndef OPENXR_YVR_H_
#define OPENXR_YVR_H_ 1

#include "openxr.h"
#include "openxr_extension_helpers.h"

// clang-format off
#if defined(__cplusplus)
extern "C" {
#endif

typedef XrStructureType XrStructureTypeExtYVR;

// Extension number: 499
#define XR_YVR_android_session_state_enable 1
#define XR_YVR_android_session_state_enable_SPEC_VERSION 1
#define XR_YVR_ANDROID_SESSION_STATE_ENABLE_EXTENSION_NAME "XR_YVR_android_session_state_enable"

// Extension number: 500
#define XR_YVR_space_acceleration_ext 1
#define XR_YVR_space_acceleration_ext_SPEC_VERSION 1
#define XR_YVR_SPACE_ACCELERATION_EXT_EXTENSION_NAME "XR_YVR_space_acceleration_ext"

// Flag bits for XrSpaceAccelerationFlagsExtYVR
typedef XrFlags64 XrSpaceAccelerationFlagsExtYVR;
static const XrSpaceAccelerationFlagsExtYVR XR_SPACE_ACCELERATION_LINEAR_VALID_BIT = 0x00000001;
static const XrSpaceAccelerationFlagsExtYVR XR_SPACE_ACCELERATION_ANGULAR_VALID_BIT = 0x00000002;

XR_STRUCT_ENUM(XR_TYPE_SPACE_ACCELERATION_EXT_YVR, 1000050000);
// XrSpaceAccelerationExtYVR extends XrSpaceVelocity
typedef struct XrSpaceAccelerationExtYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
    XrSpaceAccelerationFlagsExtYVR accelerationFlags;
    XrVector3f linearAcceleration;
    XrVector3f angularAcceleration;
} XrSpaceAccelerationExtYVR;

// Extension number: 501
#define XR_YVR_controller_tracking_functions 1
#define XR_YVR_controller_tracking_functions_SPEC_VERSION 1
#define XR_YVR_CONTROLLER_TRACKING_FUNCTIONS_EXTENSION_NAME "XR_YVR_controller_tracking_functions"

typedef enum XrControllerYVR {
    XR_CONTROLLER_LEFT_YVR = 0,
    XR_CONTROLLER_RIGHT_YVR = 1,
    XR_HAND_MAX_ENUM_YVR = 0x7FFFFFFF
} XrControllerYVR;

typedef struct XrControllerChargeStatusYVR {
    uint8_t isCharging;
    uint8_t batteryPercentRemaining;
} XrControllerChargeStatusYVR;

typedef XrResult(XRAPI_PTR *PFN_xrSetPrimaryControllerHandleYVR)(XrInstance instance, XrControllerYVR device);
typedef XrResult(XRAPI_PTR *PFN_xrGetPrimaryControllerHandleYVR)(XrInstance instance, XrControllerYVR *device);
typedef XrResult(XRAPI_PTR *PFN_xrGetControllerChargeStatusYVR)(XrInstance instance, XrControllerYVR device, XrControllerChargeStatusYVR *status);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrSetPrimaryControllerHandleYVR(XrInstance instance, XrControllerYVR device);
XRAPI_ATTR XrResult XRAPI_CALL xrGetPrimaryControllerHandleYVR(XrInstance instance, XrControllerYVR *device);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerChargeStatusYVR(XrInstance instance, XrControllerYVR device, XrControllerChargeStatusYVR *status);
#endif
#endif

// Extension number: 502
#define XR_YVR_boundary_functions 1
#define XR_YVR_boundary_functions_SPEC_VERSION 1
#define XR_YVR_BOUNDARY_FUNCTIONS_EXTENSION_NAME "XR_YVR_boundary_functions"

typedef enum XrBoundaryTestNodeYVR {
    TEST_NODE_CONTROLLER_LEFT_YVR = 0,
    TEST_NODE_CONTROLLER_RIGHT_YVR,
    TEST_NODE_HMD,
} XrBoundaryTestNodeYVR;

typedef struct XrBoundaryTestResultYVR {
    XrBool32 isTriggering;
    float closestDistance;
    XrVector3f closestPoint;
    XrVector3f closestPointNormal;
} XrBoundaryTestResultYVR;

typedef XrResult(XRAPI_PTR *PFN_xrTestBoundaryNodeYVR)(XrSession session, const XrBoundaryTestNodeYVR deviceNode, XrBoundaryTestResultYVR *result);
typedef XrResult(XRAPI_PTR *PFN_xrTestBoundaryPointYVR)(XrSession session, const XrVector3f point, XrBoundaryTestResultYVR *result);
typedef XrResult(XRAPI_PTR *PFN_xrGetBoundaryDimensionsYVR)(XrSession session, XrVector3f *outDimensions);
typedef XrResult(XRAPI_PTR *PFN_xrGetBoundaryVisibleYVR)(XrSession session, XrBool32 *outVisiable);
typedef XrResult(XRAPI_PTR *PFN_xrGetBoundaryGeometryPointsCountYVR)(XrSession session, uint32_t *outCount);
typedef XrResult(XRAPI_PTR *PFN_xrGetBoundaryGeometryYVR)(XrSession session, uint32_t inputPointCOunt, XrVector3f *outGeometry);
typedef XrResult(XRAPI_PTR *PFN_xrGetGroundDistanceYVR)(XrSession session, float *outDistance);
typedef XrResult(XRAPI_PTR *PFN_xrGetBoundaryConfiguredYVR)(XrSession session, XrBool32 *outConfigured);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrTestBoundaryNodeYVR(XrSession session, const XrBoundaryTestNodeYVR deviceNode, XrBoundaryTestResultYVR *result);
XRAPI_ATTR XrResult XRAPI_CALL xrTestBoundaryPointYVR(XrSession session, const XrVector3f point, XrBoundaryTestResultYVR *result);
XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryDimensionsYVR(XrSession session, XrVector3f *outDimensions);
XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryVisibleYVR(XrSession session, XrBool32 *outVisiable);
XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryGeometryPointsCountYVR(XrSession session, uint32_t *outCount);
XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryGeometryYVR(XrSession session, uint32_t inputPointCOunt, XrVector3f *outGeometry);
XRAPI_ATTR XrResult XRAPI_CALL xrGetGroundDistanceYVR(XrSession session, float *outDistance);
XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryConfiguredYVR(XrSession session, XrBool32 *outConfigured);
#endif
#endif

// Extension number: 503
#define XR_YVR_config_functions 1
#define XR_YVR_config_functions_SPEC_VERSION 1
#define XR_YVR_CONFIG_FUNCTIONS_EXTENSION_NAME "XR_YVR_config_functions"

typedef enum xrHMDBatteryChargeStatusYVR {
    HMD_BATTERY_STATUS_UNKNOWN = 1,
    HMD_BATTERY_STATUS_CHARGING = 2,
    HMD_BATTERY_STATUS_DISCHARGING = 3,
    HMD_BATTERY_STATUS_NOT_CHARGING = 4,
    HMD_BATTERY_STATUS_FULL = 5,
} xrHMDBatteryChargeStatusYVR;

typedef enum xrGetSystemPropertyYVR {
    YVR_SYS_PROP_DISPLAY_RESOLUTION_WIDTH = 0,      // int
    YVR_SYS_PROP_DISPLAY_RESOLUTION_HEIGHT = 1,     // int
    YVR_SYS_PROP_RENDER_TEXTURE_WIDTH = 2,          // int
    YVR_SYS_PROP_RENDER_TEXTURE_HEIGHT = 3,         // int
    YVR_SYS_PROP_GET_CPU_LEVEL = 4,                 // int Range[0, 4]
    YVR_SYS_PROP_GET_GPU_LEVEL = 5,                 // int Range[0, 5]
    YVR_SYS_PROP_GET_HMD_MOUNT_STATUS = 6,          // int mount:1 unmount:0
    YVR_SYS_PROP_GET_BATTERY_CHARGE_STATUS = 7,     // int, see xrHMDBatteryChargeStatusYVR
    YVR_SYS_PROP_GET_BATTERY_LEVEL = 8,             // float Range[0.0f, 1.0f]
    YVR_SYS_PROP_GET_BATTERY_TEMPERATURE = 9,       // float Range[0.0f, 1.0f]
    YVR_SYS_PROP_GET_VOLUME_LEVEL = 10,             // float Range[0.0f, 1.0f]
    YVR_SYS_PROP_GET_CPU_UTILIZATION = 11,          // float Range[0.0f, 1.0f]
    YVR_SYS_PROP_GET_GPU_UTILIZATION = 12,          // float Range[0.0f, 1.0f]
    YVR_SYS_PROP_GET_IPD = 13,                      // float
    YVR_SYS_PROP_GET_LEFT_FRUSTUM_POSE = 14,        // float Array
    YVR_SYS_PROP_GET_RIGHT_FRUSTUM_POSE = 15,       // float Array
} xrGetSystemPropertyYVR;

typedef enum xrSetSystemPropertyYVR {
    YVR_SYS_PROP_SET_EXTRA_LATENCY_MODE = 0,        // int on: 1, off: 0
    YVR_SYS_PROP_SET_CPU_LEVEL = 1,                 // int Range[0, 4]
    YVR_SYS_PROP_SET_GPU_LEVEL = 2,                 // int Range[0, 5]
    YVR_SYS_PROP_SET_ASW_MODE = 3,                  // int on: 1, off: 0

    YVR_SYS_PROP_SET_MAX_NUMBERS = 0xFF,
} xrSetSystemPropertyYVR;

typedef enum xrStateControlYVR {
    XR_STATE_CONTROL_OFF = 0,
    XR_STATE_CONTROL_ON = 1
} xrStateControlYVR;

typedef XrResult(XRAPI_PTR *PFN_xrGetSystemPropertyIntYVR)(XrSession session, const xrGetSystemPropertyYVR propType, int *propData);
typedef XrResult(XRAPI_PTR *PFN_xrGetSystemPropertyFloatYVR)(XrSession session, const xrGetSystemPropertyYVR propType, float *propData);
typedef XrResult(XRAPI_PTR *PFN_xrGetSystemPropertyFloatArrayYVR)(XrSession session, const xrGetSystemPropertyYVR propType, float *propDatas,
                                                                  int *numArrayValues);
typedef XrResult(XRAPI_PTR *PFN_xrSetSystemPropertyIntYVR)(XrSession session, const xrSetSystemPropertyYVR propType, int propData);
typedef XrResult(XRAPI_PTR *PFN_xrSetSystemPropertyFloatYVR)(XrSession session, const xrSetSystemPropertyYVR propType, float propData);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetSystemPropertyIntYVR(XrSession session, const xrGetSystemPropertyYVR propType, int *propData);
XRAPI_ATTR XrResult XRAPI_CALL xrGetSystemPropertyFloatYVR(XrSession session, const xrGetSystemPropertyYVR propType, float *propData);
XRAPI_ATTR XrResult XRAPI_CALL xrGetSystemPropertyFloatArrayYVR(XrSession session, const xrGetSystemPropertyYVR propType, float *propDatas,
                                                                int *numArrayValues);
XRAPI_ATTR XrResult XRAPI_CALL xrSetSystemPropertyIntYVR(XrSession session, const xrSetSystemPropertyYVR propType, int propData);
XRAPI_ATTR XrResult XRAPI_CALL xrSetSystemPropertyFloatYVR(XrSession session, const xrSetSystemPropertyYVR propType, float propData);
#endif
#endif

// Extension number: 504
#define XR_YVR_passthrough 1
#define XR_YVR_passthrough_SPEC_VERSION 1
#define XR_YVR_PASSTHROUGH_EXTENSION_NAME "XR_YVR_passthrough"

typedef XrResult(XRAPI_PTR *PFN_xrPassthroughStartYVR)(XrSession session);
typedef XrResult(XRAPI_PTR *PFN_xrPassthroughStopYVR)(XrSession session);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrPassthroughStartYVR(XrSession session);
XRAPI_ATTR XrResult XRAPI_CALL xrPassthroughStopYVR(XrSession session);
#endif
#endif

// Extension number: 505
#define XR_YVR_frame_end_info_ext 1
#define XR_YVR_frame_end_info_ext_SPEC_VERSION 1
#define XR_YVR_FRAME_END_INFO_EXT_EXTENSION_NAME "XR_YVR_frame_end_info_ext"

XR_STRUCT_ENUM(XR_TYPE_FRAME_END_INFO_EXT_YVR, 1000050001);
// XrFrameEndInfoExtYVR extends XrFrameEndInfo
typedef struct XrFrameEndInfoExtYVR {
    XrStructureType type;
    const void *XR_MAY_ALIAS next;
    uint32_t minVsync;
} XrFrameEndInfoExtYVR;

// Extension number: 506
#define XR_YVR_foveation 1
XR_DEFINE_HANDLE(XrFoveationProfileYVR)
#define XR_YVR_foveation_SPEC_VERSION 1
#define XR_YVR_FOVEATION_EXTENSION_NAME "XR_YVR_foveation"
typedef XrFlags64 XrSwapchainCreateFoveationFlagsYVR;

// Flag bits for XrSwapchainCreateFoveationFlagsYVR
static const XrSwapchainCreateFoveationFlagsYVR XR_SWAPCHAIN_CREATE_FOVEATION_SCALED_BIN_BIT_YVR = 0x00000001;
static const XrSwapchainCreateFoveationFlagsYVR XR_SWAPCHAIN_CREATE_FOVEATION_FRAGMENT_DENSITY_MAP_BIT_YVR = 0x00000002;

typedef XrFlags64 XrSwapchainStateFoveationFlagsYVR;
// Flag bits for XrSwapchainStateFoveationFlagsYVR

XR_STRUCT_ENUM(XR_TYPE_FOVEATION_PROFILE_CREATE_INFO_YVR, 1000050002);
typedef struct XrFoveationProfileCreateInfoYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
} XrFoveationProfileCreateInfoYVR;

XR_STRUCT_ENUM(XR_TYPE_SWAPCHAIN_CREATE_INFO_FOVEATION_YVR, 1000050003);
// XrSwapchainCreateInfoFoveationYVR extends XrSwapchainCreateInfo
typedef struct XrSwapchainCreateInfoFoveationYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
    XrSwapchainCreateFoveationFlagsYVR flags;
} XrSwapchainCreateInfoFoveationYVR;

XR_STRUCT_ENUM(XR_TYPE_SWAPCHAIN_STATE_FOVEATION_YVR, 1000050004);
// XrSwapchainStateFoveationYVR extends XrSwapchainStateBaseHeaderYVR
typedef struct XrSwapchainStateFoveationYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
    XrSwapchainStateFoveationFlagsYVR flags;
    XrFoveationProfileYVR profile;
} XrSwapchainStateFoveationYVR;

typedef XrResult(XRAPI_PTR *PFN_xrCreateFoveationProfileYVR)(XrSession session, const XrFoveationProfileCreateInfoYVR *createInfo,
                                                             XrFoveationProfileYVR *profile);
typedef XrResult(XRAPI_PTR *PFN_xrDestroyFoveationProfileYVR)(XrFoveationProfileYVR profile);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateFoveationProfileYVR(XrSession session, const XrFoveationProfileCreateInfoYVR *createInfo,
                                                           XrFoveationProfileYVR *profile);

XRAPI_ATTR XrResult XRAPI_CALL xrDestroyFoveationProfileYVR(XrFoveationProfileYVR profile);
#endif
#endif

// Extension number: 507
#define XR_YVR_foveation_configuration 1
#define XR_YVR_foveation_configuration_SPEC_VERSION 1
#define XR_YVR_FOVEATION_CONFIGURATION_EXTENSION_NAME "XR_YVR_foveation_configuration"

typedef enum XrFoveationLevelYVR {
    XR_FOVEATION_LEVEL_NONE_YVR = 0,
    XR_FOVEATION_LEVEL_LOW_YVR = 1,
    XR_FOVEATION_LEVEL_MEDIUM_YVR = 2,
    XR_FOVEATION_LEVEL_HIGH_YVR = 3,
    XR_FOVEATION_LEVEL_MAX_ENUM_YVR = 0x7FFFFFFF
} XrFoveationLevelYVR;

typedef enum XrFoveationDynamicYVR {
    XR_FOVEATION_DYNAMIC_DISABLED_YVR = 0,
    XR_FOVEATION_DYNAMIC_LEVEL_ENABLED_YVR = 1,
    XR_FOVEATION_DYNAMIC_MAX_ENUM_YVR = 0x7FFFFFFF
} XrFoveationDynamicYVR;

XR_STRUCT_ENUM(XR_TYPE_FOVEATION_LEVEL_PROFILE_CREATE_INFO_YVR, 1000050005);
// XrFoveationLevelProfileCreateInfoYVR extends XrFoveationProfileCreateInfoYVR
typedef struct XrFoveationLevelProfileCreateInfoYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
    XrFoveationLevelYVR level;
    float verticalOffset;
    XrFoveationDynamicYVR dynamic;
} XrFoveationLevelProfileCreateInfoYVR;

// Extension number: 508
#define XR_YVR_swapchain_update_state 1
#define XR_YVR_swapchain_update_state_SPEC_VERSION 3
#define XR_YVR_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME "XR_YVR_swapchain_update_state"
typedef struct XR_MAY_ALIAS XrSwapchainStateBaseHeaderYVR {
    XrStructureType type;
    void *XR_MAY_ALIAS next;
} XrSwapchainStateBaseHeaderYVR;

typedef XrResult(XRAPI_PTR *PFN_xrUpdateSwapchainYVR)(XrSwapchain swapchain, const XrSwapchainStateBaseHeaderYVR *state);
typedef XrResult(XRAPI_PTR *PFN_xrGetSwapchainStateYVR)(XrSwapchain swapchain, XrSwapchainStateBaseHeaderYVR *state);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrUpdateSwapchainYVR(XrSwapchain swapchain, const XrSwapchainStateBaseHeaderYVR *state);

XRAPI_ATTR XrResult XRAPI_CALL xrGetSwapchainStateYVR(XrSwapchain swapchain, XrSwapchainStateBaseHeaderYVR *state);
#endif
#endif

// Extension number: 509
#define XR_YVR_display_refresh_rate 1
#define XR_YVR_display_refresh_rate_SPEC_VERSION 1
#define XR_YVR_DISPLAY_REFRESH_RATE_EXTENSION_NAME "XR_YVR_display_refresh_rate"

XR_STRUCT_ENUM(XR_TYPE_EVENT_DATA_DISPLAY_REFRESH_RATE_CHANGED_YVR, 1000050006);
typedef struct XrEventDataDisplayRefreshRateChangedYVR {
    XrStructureType type;
    const void *XR_MAY_ALIAS next;
    float fromDisplayRefreshRate;
    float toDisplayRefreshRate;
} XrEventDataDisplayRefreshRateChangedYVR;

typedef XrResult(XRAPI_PTR *PFN_xrEnumerateDisplayRefreshRatesYVR)(XrSession session, uint32_t displayRefreshRateCapacityInput,
                                                                   uint32_t *displayRefreshRateCountOutput, float *displayRefreshRates);
typedef XrResult(XRAPI_PTR *PFN_xrGetDisplayRefreshRateYVR)(XrSession session, float *displayRefreshRate);
typedef XrResult(XRAPI_PTR *PFN_xrRequestDisplayRefreshRateYVR)(XrSession session, float displayRefreshRate);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrEnumerateDisplayRefreshRatesYVR(XrSession session, uint32_t displayRefreshRateCapacityInput,
                                                                 uint32_t *displayRefreshRateCountOutput, float *displayRefreshRates);
XRAPI_ATTR XrResult XRAPI_CALL xrGetDisplayRefreshRateYVR(XrSession session, float *displayRefreshRate);
XRAPI_ATTR XrResult XRAPI_CALL xrRequestDisplayRefreshRateYVR(XrSession session, float displayRefreshRate);

#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */

// Extension number: 510
#define XR_YVR_space_warp 1
#define XR_YVR_space_warp_SPEC_VERSION     2
#define XR_YVR_SPACE_WARP_EXTENSION_NAME   "XR_YVR_space_warp"
typedef XrFlags64 XrCompositionLayerSpaceWarpInfoFlagsYVR;

// Flag bits for XrCompositionLayerSpaceWarpInfoFlagsYVR
static const XrCompositionLayerSpaceWarpInfoFlagsYVR XR_COMPOSITION_LAYER_SPACE_WARP_INFO_FRAME_SKIP_BIT_YVR = 0x00000001;

XR_STRUCT_ENUM(XR_TYPE_COMPOSITION_LAYER_SPACE_WARP_INFO_YVR, 1000050008);
// XrCompositionLayerSpaceWarpInfoYVR extends XrCompositionLayerProjectionView
typedef struct XrCompositionLayerSpaceWarpInfoYVR {
    XrStructureType                           type;
    const void* XR_MAY_ALIAS                  next;
    XrCompositionLayerSpaceWarpInfoFlagsYVR   layerFlags;
    XrSwapchainSubImage                       motionVectorSubImage;
    XrPosef                                   appSpaceDeltaPose;
    XrSwapchainSubImage                       depthSubImage;
    float                                     minDepth;
    float                                     maxDepth;
    float                                     nearZ;
    float                                     farZ;
} XrCompositionLayerSpaceWarpInfoYVR;

XR_STRUCT_ENUM(XR_TYPE_SYSTEM_SPACE_WARP_PROPERTIES_YVR, 1000050009);
// XrSystemSpaceWarpPropertiesYVR extends XrSystemProperties
typedef struct XrSystemSpaceWarpPropertiesYVR {
    XrStructureType  type;
    void* XR_MAY_ALIAS    next;
    uint32_t              recommendedMotionVectorImageRectWidth;
    uint32_t              recommendedMotionVectorImageRectHeight;
} XrSystemSpaceWarpPropertiesYVR;

// Extension number: 511
#define XR_YVR_color_space 1
#define XR_YVR_color_space_SPEC_VERSION    1
#define XR_YVR_COLOR_SPACE_EXTENSION_NAME  "XR_YVR_color_space"

typedef enum XrColorSpaceYVR {
    XR_COLOR_SPACE_Linear_YVR = 0,
    XR_COLOR_SPACE_SRGB_YVR = 1,
} XrColorSpaceYVR;

typedef XrResult (XRAPI_PTR *PFN_xrSetColorSpaceYVR)(XrSession session, const XrColorSpaceYVR colorspace);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrSetColorSpaceYVR(
    XrSession                                   session,
    const XrColorSpaceYVR                        colorspace);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */

// Extension number: 512
#define XR_YVR_hand_tracking_mesh 1
#define XR_YVR_hand_tracking_mesh_SPEC_VERSION 3
#define XR_YVR_HAND_TRACKING_MESH_EXTENSION_NAME "XR_YVR_hand_tracking_mesh"
typedef struct XrVector4sYVR {
    int16_t    x;
    int16_t    y;
    int16_t    z;
    int16_t    w;
} XrVector4sYVR;

typedef struct XrHandTrackingMeshYVR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    uint32_t              jointCapacityInput;
    uint32_t              jointCountOutput;
    XrPosef*              jointBindPoses;
    float*                jointRadii;
    XrHandJointEXT*       jointParents;
    uint32_t              vertexCapacityInput;
    uint32_t              vertexCountOutput;
    XrVector3f*           vertexPositions;
    XrVector3f*           vertexNormals;
    XrVector2f*           vertexUVs;
    XrVector4sYVR*        vertexBlendIndices;
    XrVector4f*           vertexBlendWeights;
    uint32_t              indexCapacityInput;
    uint32_t              indexCountOutput;
    int16_t*              indices;
} XrHandTrackingMeshYVR;

// XrHandTrackingScaleYVR extends XrHandJointLocationsEXT
typedef struct XrHandTrackingScaleYVR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    float                 sensorOutput;
    float                 currentOutput;
    XrBool32              overrideHandScale;
    float                 overrideValueInput;
} XrHandTrackingScaleYVR;

typedef XrResult (XRAPI_PTR *PFN_xrGetHandMeshYVR)(XrHandTrackerEXT handTracker, XrHandTrackingMeshYVR* mesh);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetHandMeshYVR(
    XrHandTrackerEXT                            handTracker,
    XrHandTrackingMeshYVR*                       mesh);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */

// Extension number: 513
#define XR_YVR_hand_tracking_aim 1
#define XR_YVR_hand_tracking_aim_SPEC_VERSION 2
#define XR_YVR_HAND_TRACKING_AIM_EXTENSION_NAME "XR_YVR_hand_tracking_aim"
typedef XrFlags64 XrHandTrackingAimFlagsYVR;

// Flag bits for XrHandTrackingAimFlagsYVR
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_COMPUTED_BIT_YVR = 0x00000001;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_VALID_BIT_YVR = 0x00000002;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_INDEX_PINCHING_BIT_YVR = 0x00000004;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_MIDDLE_PINCHING_BIT_YVR = 0x00000008;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_RING_PINCHING_BIT_YVR = 0x00000010;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_LITTLE_PINCHING_BIT_YVR = 0x00000020;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_SYSTEM_GESTURE_BIT_YVR = 0x00000040;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_DOMINANT_HAND_BIT_YVR = 0x00000080;
static const XrHandTrackingAimFlagsYVR XR_HAND_TRACKING_AIM_MENU_PRESSED_BIT_YVR = 0x00000100;

// XrHandTrackingAimStateYVR extends XrHandJointLocationsEXT
typedef struct XrHandTrackingAimStateYVR {
    XrStructureType             type;
    void* XR_MAY_ALIAS          next;
    XrHandTrackingAimFlagsYVR   status;
    XrPosef                     aimPose;
    float                       pinchStrengthIndex;
    float                       pinchStrengthMiddle;
    float                       pinchStrengthRing;
    float                       pinchStrengthLittle;
} XrHandTrackingAimStateYVR;

// Extension number: 514
#define XR_YVR_adaptive_resolution 1
#define XR_YVR_adaptive_resolution_SPEC_VERSION  1
#define XR_YVR_ADAPTIVE_RESOLUTION_EXTENSION_NAME "XR_YVR_adaptive_resolution"

typedef enum XrAdapterResolutionPolicyYVR {
    HIGH_QUALITY,
    BALANCED,
    BATTERY_SAVING
}XrAdapterResolutionPolicyYVR;

XR_STRUCT_ENUM(XR_TYPE_ADAPTER_RESOLUTIONINFO_YVR, 1000513001);
typedef struct XrAdapterResolutionInfoYVR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrAdapterResolutionPolicyYVR resolutionPolicy;
} XrAdapterResolutionInfoYVR;


typedef XrResult (XRAPI_PTR *PFN_xrGetRecommendedResolution)(XrSession session, const XrAdapterResolutionInfoYVR* info,
                    XrExtent2Di* outRecommendedResolution);

#ifndef XR_NO_PROTOTYPES
#ifdef XR_EXTENSION_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetRecommendedResolution(
    XrSession                                   session,
    const XrAdapterResolutionInfoYVR*              info,
    XrExtent2Di*                outRecommendedResolution);
#endif /* XR_EXTENSION_PROTOTYPES */
#endif /* !XR_NO_PROTOTYPES */

// clang-format on

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENXR_YVR_H_
