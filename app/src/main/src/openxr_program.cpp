// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "pch.h"
#include "common.h"
#include "options.h"
#include "platformdata.h"
#include "platformplugin.h"
#include "graphicsplugin.h"
#include "openxr_program.h"
#include <common/xr_linear.h>
#include <array>
#include <cmath>
#include <math.h>
#include "CloudXRClientYVR.h"
#include "openxr/openxr_yvr.h"

namespace
{

#if !defined(XR_USE_PLATFORM_WIN32)
#define strcpy_s(dest, source) strncpy((dest), (source), sizeof(dest))
#endif

// [YVR] add to test yvr extensions begin

#define USE_YVR_EXT_INPUT_PROFILE
#define YVR_EXT_TEST_INPUT_PROFILE
#define YVR_EXT_TEST_ACCELERATION
#define YVR_EXT_TEST_CONTROLLER_TRACKING_FUNCTIONS
#define YVR_EXT_TEST_CONFIG_FUNCTIONS
#define YVR_EXT_TEST_FRAME_ENDINFO
#define YVR_EXT_TEST_BOUNDARY_FUNCTIONS
#define YVR_EXT_TEST_DISPLAY_REFRESH_RATE
// #define YVR_EXT_TEST_SHIMS_FUNC_APILAYER
#define YVR_EXT_TEST_SET_CAMERA_FREQUENCY
#define YVR_EXT_TEST_SPACE_DETECT
// #define YVR_EXT_TEST_SET_FORCE_HEAD_MODE
//only open one start
#define YVR_EXT_TEST_COMPOSITION_LAYER_ALPHA_BLEND
//#define FB_EXT_TEST_COMPOSITION_LAYER_ALPHA_BLEND
//only open one end
#define YVR_EXT_TEST_COLOR_SPACE
#define YVR_EXT_TEST_ANTI_GLARE_LEVEL
// #define YVR_EXT_TEST_CAMERA

// #define YVR_EXT_TEST_HAND_IMAGE

#define YVR_EXT_TEST_GESTURE_HAND_TYPE
//#define YVR_EXT_TEST_COLOR_SPACE
// [YVR] add to test yvr extensions end
#define YVR_TEST_QUAD_LAYER //Only support OpenGLES
//#define YVR_TEST_CYLINDER_LAYER //Only support OpenGLES


//add for xr_khr extensions test start
#define XR_KHR_TEST_COLOR_SCALE_BIAS
//add for xr_khr extensions test end

//add for FB extensions test start
//#define YVR_FB_TEST_COMPOSITION_LAYER_IMAGE_LAYOUT
//#define YVR_FB_TEST_COMPOSITION_LAYER_SECURE_CONTENT
//#define YVR_FB_TEST_COMPOSITION_LAYER_SETTINGS
//add for FB extensions test start


inline std::string GetXrVersionString(XrVersion ver) { return Fmt("%d.%d.%d", XR_VERSION_MAJOR(ver), XR_VERSION_MINOR(ver), XR_VERSION_PATCH(ver)); }

namespace Math
{
namespace Pose
{
XrPosef Identity()
{
    XrPosef t{};
    t.orientation.w = 1;
    return t;
}

XrPosef Translation(const XrVector3f& translation)
{
    XrPosef t = Identity();
    t.position = translation;
    return t;
}

XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation)
{
    XrPosef t = Identity();
    t.orientation.x = 0.f;
    t.orientation.y = std::sin(radians * 0.5f);
    t.orientation.z = 0.f;
    t.orientation.w = std::cos(radians * 0.5f);
    t.position = translation;
    return t;
}
}  // namespace Pose
}  // namespace Math

inline XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(const std::string& referenceSpaceTypeStr)
{
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Identity();
    if (EqualsIgnoreCase(referenceSpaceTypeStr, "View"))
    {
    	referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Translation({0.f, 0.f, 0.f}),
        	referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "FLOOR"))
    {
    	referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR_EXT;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "ViewFront"))
    {
        // Render head-locked 2m in front of device.
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Translation({0.f, 0.f, -2.f}),
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Local"))
    {
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Stage"))
    {
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeft"))
    {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {-2.f, 0.f, -2.f});
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRight"))
    {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {2.f, 0.f, -2.f});
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeftRotated"))
    {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(3.14f / 3.f, {-2.f, 0.5f, -2.f});
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRightRotated"))
    {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(-3.14f / 3.f, {2.f, 0.5f, -2.f});
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else
    {
        throw std::invalid_argument(Fmt("Unknown reference space type '%s'", referenceSpaceTypeStr.c_str()));
    }
    return referenceSpaceCreateInfo;
}

struct OpenXrProgram : IOpenXrProgram
{
    OpenXrProgram(const std::shared_ptr<Options>& options, const std::shared_ptr<IPlatformPlugin>& platformPlugin,
                  const std::shared_ptr<IGraphicsPlugin>& graphicsPlugin)
        : m_options(options), m_platformPlugin(platformPlugin), m_graphicsPlugin(graphicsPlugin) {}

    ~OpenXrProgram() override
    {
        if (m_input.actionSet != XR_NULL_HANDLE)
        {
            for (auto hand : {Side::LEFT, Side::RIGHT})
            {
                xrDestroySpace(m_input.handSpace[hand]);
            }

            if (m_input.squeezeClickAction){
                xrDestroyAction(m_input.squeezeClickAction);
                m_input.squeezeClickAction = XR_NULL_HANDLE;
            }
            if (m_input.triggerValueAction){
                xrDestroyAction(m_input.triggerValueAction);
                m_input.triggerValueAction = XR_NULL_HANDLE;
            }
            if (m_input.triggerTouchAction){
                xrDestroyAction(m_input.triggerTouchAction);
                m_input.triggerTouchAction = XR_NULL_HANDLE;
            }
            if (m_input.poseAction){
                xrDestroyAction(m_input.poseAction);
                m_input.poseAction = XR_NULL_HANDLE;
            }
            if (m_input.vibrateAction){
                xrDestroyAction(m_input.vibrateAction);
                m_input.vibrateAction = XR_NULL_HANDLE;
            }
            if (m_input.quitAction){
                xrDestroyAction(m_input.quitAction);
                m_input.quitAction = XR_NULL_HANDLE;
            }
            if (m_input.thumbstickClickAction){
                xrDestroyAction(m_input.thumbstickClickAction);
                m_input.thumbstickClickAction = XR_NULL_HANDLE;
            }
            if (m_input.thumbstickTouchAction){
                xrDestroyAction(m_input.thumbstickTouchAction);
                m_input.thumbstickTouchAction = XR_NULL_HANDLE;
            }
            if (m_input.thumbstickxAction){
                xrDestroyAction(m_input.thumbstickxAction);
                m_input.thumbstickxAction = XR_NULL_HANDLE;
            }
            if (m_input.thumbstickyAction){
                xrDestroyAction(m_input.thumbstickyAction);
                m_input.thumbstickyAction = XR_NULL_HANDLE;
            }
            if (m_input.button1Action){
                xrDestroyAction(m_input.button1Action);
                m_input.button1Action = XR_NULL_HANDLE;
            }
            if (m_input.button2Action){
                xrDestroyAction(m_input.button2Action);
                m_input.button2Action = XR_NULL_HANDLE;
            }
            if (m_input.button1touchAction){
                xrDestroyAction(m_input.button1touchAction);
                m_input.button1touchAction = XR_NULL_HANDLE;
            }
            if (m_input.button2touchAction){
                xrDestroyAction(m_input.button2touchAction);
                m_input.button2touchAction = XR_NULL_HANDLE;
            }

            xrDestroyActionSet(m_input.actionSet);
        }

        for (Swapchain swapchain : m_swapchains)
        {
            xrDestroySwapchain(swapchain.handle);
        }


        if (m_appSpace != XR_NULL_HANDLE)
        {
            xrDestroySpace(m_appSpace);
        }

        if (m_session != XR_NULL_HANDLE)
        {
            xrDestroySession(m_session);
        }

        if (m_instance != XR_NULL_HANDLE)
        {
            xrDestroyInstance(m_instance);
        }
    }

    static void LogLayersAndExtensions()
    {
        // Write out extension properties for a given layer.
        const auto logExtensions = [](const char* layerName, int indent = 0)
        {
            uint32_t instanceExtensionCount;
            CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, 0, &instanceExtensionCount, nullptr));

            std::vector<XrExtensionProperties> extensions(instanceExtensionCount);
            for (XrExtensionProperties& extension : extensions)
            {
                extension.type = XR_TYPE_EXTENSION_PROPERTIES;
            }

            CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, (uint32_t)extensions.size(), &instanceExtensionCount, extensions.data()));

            const std::string indentStr(indent, ' ');
            Log::Write(Log::Level::Info, Fmt("%sAvailable Extensions: (%d)", indentStr.c_str(), instanceExtensionCount));
            for (const XrExtensionProperties& extension : extensions)
            {
                Log::Write(Log::Level::Info,
                           Fmt("%s  Name=%s SpecVersion=%d", indentStr.c_str(), extension.extensionName, extension.extensionVersion));
            }
        };

        // Log non-layer extensions (layerName==nullptr).
        logExtensions(nullptr);

        // Log layers and any of their extensions.
        {
            uint32_t layerCount;
            CHECK_XRCMD(xrEnumerateApiLayerProperties(0, &layerCount, nullptr));

            std::vector<XrApiLayerProperties> layers(layerCount);
            for (XrApiLayerProperties& layer : layers)
            {
                layer.type = XR_TYPE_API_LAYER_PROPERTIES;
            }

            CHECK_XRCMD(xrEnumerateApiLayerProperties((uint32_t)layers.size(), &layerCount, layers.data()));

            Log::Write(Log::Level::Info, Fmt("Available Layers: (%d)", layerCount));
            for (const XrApiLayerProperties& layer : layers)
            {
                Log::Write(Log::Level::Verbose, Fmt("  Name=%s SpecVersion=%s LayerVersion=%d Description=%s", layer.layerName,
                                                    GetXrVersionString(layer.specVersion).c_str(), layer.layerVersion, layer.description));
                logExtensions(layer.layerName, 4);
            }
        }
    }

    void LogInstanceInfo()
    {
        CHECK(m_instance != XR_NULL_HANDLE);

        XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
        CHECK_XRCMD(xrGetInstanceProperties(m_instance, &instanceProperties));

        Log::Write(Log::Level::Info, Fmt("Instance RuntimeName=%s RuntimeVersion=%s", instanceProperties.runtimeName,
                                         GetXrVersionString(instanceProperties.runtimeVersion).c_str()));
    }

    void CreateInstanceInternal()
    {
        CHECK(m_instance == XR_NULL_HANDLE);

        // Create union of extensions required by platform and graphics plugins.
        std::vector<const char*> extensions;

        // Transform platform and graphics extension std::strings to C strings.
        const std::vector<std::string> platformExtensions = m_platformPlugin->GetInstanceExtensions();
        std::transform(platformExtensions.begin(), platformExtensions.end(), std::back_inserter(extensions),
                       [](const std::string& ext) { return ext.c_str(); });
        const std::vector<std::string> graphicsExtensions = m_graphicsPlugin->GetInstanceExtensions();
        std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
                       [](const std::string& ext) { return ext.c_str(); });


        #ifdef YVR_EXT_TEST_CONFIG_FUNCTIONS
        extensions.push_back(XR_YVR_CONFIG_FUNCTIONS_EXTENSION_NAME);
        #endif

        #ifdef YVR_EXT_TEST_ACCELERATION
        extensions.push_back(XR_YVR_SPACE_ACCELERATION_EXT_EXTENSION_NAME);
        #endif

        #ifdef YVR_EXT_TEST_DISPLAY_REFRESH_RATE
        extensions.push_back(XR_YVR_DISPLAY_REFRESH_RATE_EXTENSION_NAME);
        #endif

        XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
        createInfo.next = m_platformPlugin->GetInstanceCreateExtension();
        createInfo.enabledExtensionCount = (uint32_t)extensions.size();
        createInfo.enabledExtensionNames = extensions.data();

        strcpy(createInfo.applicationInfo.applicationName, "CloudXR");
        createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

        CHECK_XRCMD(xrCreateInstance(&createInfo, &m_instance));
    }

    void CreateInstance() override
    {
        LogLayersAndExtensions();

        CreateInstanceInternal();

        LogInstanceInfo();
    }

    void LogViewConfigurations()
    {
        CHECK(m_instance != XR_NULL_HANDLE);
        CHECK(m_systemId != XR_NULL_SYSTEM_ID);

        uint32_t viewConfigTypeCount;
        CHECK_XRCMD(xrEnumerateViewConfigurations(m_instance, m_systemId, 0, &viewConfigTypeCount, nullptr));
        std::vector<XrViewConfigurationType> viewConfigTypes(viewConfigTypeCount);
        CHECK_XRCMD(xrEnumerateViewConfigurations(m_instance, m_systemId, viewConfigTypeCount, &viewConfigTypeCount, viewConfigTypes.data()));
        CHECK((uint32_t)viewConfigTypes.size() == viewConfigTypeCount);

        Log::Write(Log::Level::Info, Fmt("Available View Configuration Types: (%d)", viewConfigTypeCount));
        for (XrViewConfigurationType viewConfigType : viewConfigTypes)
        {
            Log::Write(Log::Level::Verbose, Fmt("  View Configuration Type: %s %s", to_string(viewConfigType),
                                                viewConfigType == m_options->Parsed.ViewConfigType ? "(Selected)" : ""));

            XrViewConfigurationProperties viewConfigProperties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
            CHECK_XRCMD(xrGetViewConfigurationProperties(m_instance, m_systemId, viewConfigType, &viewConfigProperties));

            Log::Write(Log::Level::Verbose, Fmt("  View configuration FovMutable=%s", viewConfigProperties.fovMutable == XR_TRUE ? "True" : "False"));

            uint32_t viewCount;
            CHECK_XRCMD(xrEnumerateViewConfigurationViews(m_instance, m_systemId, viewConfigType, 0, &viewCount, nullptr));
            if (viewCount > 0)
            {
                std::vector<XrViewConfigurationView> views(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
                CHECK_XRCMD(xrEnumerateViewConfigurationViews(m_instance, m_systemId, viewConfigType, viewCount, &viewCount, views.data()));

                for (uint32_t i = 0; i < views.size(); i++)
                {
                    const XrViewConfigurationView& view = views[i];

                    Log::Write(Log::Level::Verbose,
                               Fmt("    View [%d]: Recommended Width=%d Height=%d SampleCount=%d", i, view.recommendedImageRectWidth,
                                   view.recommendedImageRectHeight, view.recommendedSwapchainSampleCount));
                    Log::Write(Log::Level::Verbose, Fmt("    View [%d]:     Maximum Width=%d Height=%d SampleCount=%d", i, view.maxImageRectWidth,
                                                        view.maxImageRectHeight, view.maxSwapchainSampleCount));
                }
            }
            else
            {
                Log::Write(Log::Level::Error, Fmt("Empty view configuration type"));
            }

            LogEnvironmentBlendMode(viewConfigType);
        }
    }

    void LogEnvironmentBlendMode(XrViewConfigurationType type)
    {
        CHECK(m_instance != XR_NULL_HANDLE);
        CHECK(m_systemId != 0);

        uint32_t count;
        CHECK_XRCMD(xrEnumerateEnvironmentBlendModes(m_instance, m_systemId, type, 0, &count, nullptr));
        CHECK(count > 0);

        Log::Write(Log::Level::Info, Fmt("Available Environment Blend Mode count : (%d)", count));

        std::vector<XrEnvironmentBlendMode> blendModes(count);
        CHECK_XRCMD(xrEnumerateEnvironmentBlendModes(m_instance, m_systemId, type, count, &count, blendModes.data()));

        bool blendModeFound = false;
        for (XrEnvironmentBlendMode mode : blendModes)
        {
            const bool blendModeMatch = (mode == m_options->Parsed.EnvironmentBlendMode);
            Log::Write(Log::Level::Info, Fmt("Environment Blend Mode (%s) : %s", to_string(mode), blendModeMatch ? "(Selected)" : ""));
            blendModeFound |= blendModeMatch;
        }
        CHECK(blendModeFound);
    }

    void InitializeSystem() override
    {
        CHECK(m_instance != XR_NULL_HANDLE);
        CHECK(m_systemId == XR_NULL_SYSTEM_ID);

        XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
        systemInfo.formFactor = m_options->Parsed.FormFactor;
        CHECK_XRCMD(xrGetSystem(m_instance, &systemInfo, &m_systemId));

        Log::Write(Log::Level::Verbose, Fmt("Using system %d for form factor %s", m_systemId, to_string(m_options->Parsed.FormFactor)));
        CHECK(m_instance != XR_NULL_HANDLE);
        CHECK(m_systemId != XR_NULL_SYSTEM_ID);
    }

    void InitializeDevice() override
    {
        LogViewConfigurations();

        // The graphics API can initialize the graphics device now that the systemId and instance
        // handle are available.
        m_graphicsPlugin->InitializeDevice(m_instance, m_systemId);
    }

    void LogReferenceSpaces()
    {
        CHECK(m_session != XR_NULL_HANDLE);

        uint32_t spaceCount;
        CHECK_XRCMD(xrEnumerateReferenceSpaces(m_session, 0, &spaceCount, nullptr));
        std::vector<XrReferenceSpaceType> spaces(spaceCount);
        CHECK_XRCMD(xrEnumerateReferenceSpaces(m_session, spaceCount, &spaceCount, spaces.data()));

        Log::Write(Log::Level::Info, Fmt("Available reference spaces: %d", spaceCount));
        for (XrReferenceSpaceType space : spaces)
        {
            Log::Write(Log::Level::Verbose, Fmt("  Name: %s", to_string(space)));
        }
    }

    struct InputState
    {
        XrActionSet actionSet{XR_NULL_HANDLE};
        XrAction squeezeClickAction{XR_NULL_HANDLE};
        XrAction triggerValueAction{XR_NULL_HANDLE};
        XrAction triggerTouchAction{XR_NULL_HANDLE};
        XrAction poseAction{XR_NULL_HANDLE};
        XrAction vibrateAction{XR_NULL_HANDLE};
        XrAction quitAction{XR_NULL_HANDLE};
        XrAction thumbstickClickAction{XR_NULL_HANDLE};
        XrAction thumbstickTouchAction{XR_NULL_HANDLE};
        XrAction thumbstickxAction{XR_NULL_HANDLE};
        XrAction thumbstickyAction{XR_NULL_HANDLE};
        XrAction button1Action{XR_NULL_HANDLE};
        XrAction button2Action{XR_NULL_HANDLE};
        XrAction button1touchAction{XR_NULL_HANDLE};
        XrAction button2touchAction{XR_NULL_HANDLE};

        std::array<XrPath, Side::COUNT> handSubactionPath;
        std::array<XrSpace, Side::COUNT> handSpace;
        std::array<float, Side::COUNT> handScale = {{1.0f, 1.0f}};
        std::array<XrBool32, Side::COUNT> handActive;
    };

    void InitializeActions()
    {
        XrResult result;
        // Create an action set.
        {
            XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
            strcpy_s(actionSetInfo.actionSetName, "gameplay");
            strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
            actionSetInfo.priority = 0;
            CHECK_XRCMD(xrCreateActionSet(m_instance, &actionSetInfo, &m_input.actionSet));
        }

        // Get the XrPath for the left and right hands - we will use them as subaction paths.
        CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left", &m_input.handSubactionPath[Side::LEFT]));
        CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right", &m_input.handSubactionPath[Side::RIGHT]));

        // Create actions.
        {
            // Create an input action for grabbing objects with the left and right hands.
            XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
#ifdef USE_YVR_EXT_INPUT_PROFILE
            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
#else
            actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
#endif
            strcpy(actionInfo.actionName, "squeeze_object");
            strcpy(actionInfo.localizedActionName, "squeeze Object");
            actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
            actionInfo.subactionPaths = m_input.handSubactionPath.data();
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.squeezeClickAction));

            actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
            strcpy(actionInfo.actionName, "trigger_value");
            strcpy(actionInfo.localizedActionName, "trigger Value");
            actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
            actionInfo.subactionPaths = m_input.handSubactionPath.data();
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.triggerValueAction));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "trigger_touch");
            strcpy(actionInfo.localizedActionName, "trigger Touch");
            actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
            actionInfo.subactionPaths = m_input.handSubactionPath.data();
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.triggerTouchAction));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "thumbstick_click");
            strcpy(actionInfo.localizedActionName, "thumbstick Click");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.thumbstickClickAction));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "thumbstick_touch");
            strcpy(actionInfo.localizedActionName, "thumbstick Touch");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.thumbstickTouchAction));

            actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
            strcpy(actionInfo.actionName, "thumbstick_x");
            strcpy(actionInfo.localizedActionName, "thumbstick X");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.thumbstickxAction));

            actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
            strcpy(actionInfo.actionName, "thumbstick_y");
            strcpy(actionInfo.localizedActionName, "thumbstick Y");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.thumbstickyAction));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "button1_click");
            strcpy(actionInfo.localizedActionName, "button1 Click");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.button1Action));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "button2_click");
            strcpy(actionInfo.localizedActionName, "button2 Click");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.button2Action));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "button1_touch");
            strcpy(actionInfo.localizedActionName, "button1 Touch");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.button1touchAction));

            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "button2_touch");
            strcpy(actionInfo.localizedActionName, "button2 Touch");
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.button2touchAction));

            // Create an input action getting the left and right hand poses.
            actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
            strcpy(actionInfo.actionName, "hand_pose");
            strcpy(actionInfo.localizedActionName, "Hand Pose");
            actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
            actionInfo.subactionPaths = m_input.handSubactionPath.data();
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.poseAction));

            // Create output actions for vibrating the left and right controller.
            actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
            strcpy(actionInfo.actionName, "vibrate_hand");
            strcpy(actionInfo.localizedActionName, "Vibrate Hand");
            actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
            actionInfo.subactionPaths = m_input.handSubactionPath.data();
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.vibrateAction));

            // Create input actions for quitting the session using the left and right controller.
            // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
            // We will just suggest bindings for both hands, where possible.
            actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
            strcpy(actionInfo.actionName, "quit_session");
            strcpy(actionInfo.localizedActionName, "Quit Session");
            actionInfo.countSubactionPaths = 0;
            actionInfo.subactionPaths = nullptr;
            CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.quitAction));
        }

        std::array<XrPath, Side::COUNT> triggerValuePath;
        std::array<XrPath, Side::COUNT> triggerTouchPath;
        std::array<XrPath, Side::COUNT> squeezeClickPath;
        std::array<XrPath, Side::COUNT> posePath;
        std::array<XrPath, Side::COUNT> thumbstickClickPath;
        std::array<XrPath, Side::COUNT> thumbstickTouchPath;
        std::array<XrPath, Side::COUNT> thumbstickxPath;
        std::array<XrPath, Side::COUNT> thumbstickyPath;
        std::array<XrPath, Side::COUNT> hapticPath;
        std::array<XrPath, Side::COUNT> menuClickPath;
        std::array<XrPath, Side::COUNT> button1Path;
        std::array<XrPath, Side::COUNT> button2Path;
        std::array<XrPath, Side::COUNT> button1touchPath;
        std::array<XrPath, Side::COUNT> button2touchPath;

        result = xrStringToPath(m_instance, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/trigger/value", &triggerValuePath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/trigger/value", &triggerValuePath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/trigger/touch", &triggerTouchPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/trigger/touch", &triggerTouchPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/thumbstick/click", &thumbstickClickPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/thumbstick/click", &thumbstickClickPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/thumbstick/touch", &thumbstickTouchPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/thumbstick/touch", &thumbstickTouchPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/thumbstick/x", &thumbstickxPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/thumbstick/x", &thumbstickxPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/thumbstick/y", &thumbstickyPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/thumbstick/y", &thumbstickyPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/x/click", &button1Path[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/a/click", &button1Path[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/y/click", &button2Path[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/b/click", &button2Path[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/x/touch", &button1touchPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/a/touch", &button1touchPath[Side::RIGHT]);
        result = xrStringToPath(m_instance, "/user/hand/left/input/y/touch", &button2touchPath[Side::LEFT]);
        result = xrStringToPath(m_instance, "/user/hand/right/input/b/touch", &button2touchPath[Side::RIGHT]);


        // Suggest bindings for the yvr Touch.
        {
            XrPath yvrTouchInteractionProfilePath;
            CHECK_XRCMD(
                    xrStringToPath(m_instance, "/interaction_profiles/yvr/touch_controller", &yvrTouchInteractionProfilePath));
            std::vector<XrActionSuggestedBinding> bindings{{{m_input.squeezeClickAction, squeezeClickPath[Side::LEFT]},
                                                            {m_input.squeezeClickAction, squeezeClickPath[Side::RIGHT]},
                                                            {m_input.poseAction, posePath[Side::LEFT]},
                                                            {m_input.poseAction, posePath[Side::RIGHT]},
                                                            {m_input.triggerValueAction, triggerValuePath[Side::LEFT]},
                                                            {m_input.triggerValueAction, triggerValuePath[Side::RIGHT]},
                                                            {m_input.triggerTouchAction, triggerTouchPath[Side::LEFT]},
                                                            {m_input.triggerTouchAction, triggerTouchPath[Side::RIGHT]},
                                                            {m_input.button1Action, button1Path[Side::LEFT]},
                                                            {m_input.button1Action, button1Path[Side::RIGHT]},
                                                            {m_input.button2Action, button2Path[Side::LEFT]},
                                                            {m_input.button2Action, button2Path[Side::RIGHT]},
                                                            {m_input.button1touchAction, button1touchPath[Side::LEFT]},
                                                            {m_input.button1touchAction, button1touchPath[Side::RIGHT]},
                                                            {m_input.button2touchAction, button2touchPath[Side::LEFT]},
                                                            {m_input.button2touchAction, button2touchPath[Side::RIGHT]},
                                                            {m_input.thumbstickClickAction, thumbstickClickPath[Side::LEFT]},
                                                            {m_input.thumbstickClickAction, thumbstickClickPath[Side::RIGHT]},
                                                            {m_input.thumbstickTouchAction, thumbstickTouchPath[Side::LEFT]},
                                                            {m_input.thumbstickTouchAction, thumbstickTouchPath[Side::RIGHT]},
                                                            {m_input.thumbstickxAction, thumbstickxPath[Side::LEFT]},
                                                            {m_input.thumbstickxAction, thumbstickxPath[Side::RIGHT]},
                                                            {m_input.thumbstickyAction, thumbstickyPath[Side::LEFT]},
                                                            {m_input.thumbstickyAction, thumbstickyPath[Side::RIGHT]},
                                                            {m_input.quitAction, menuClickPath[Side::LEFT]},
                                                            {m_input.vibrateAction, hapticPath[Side::LEFT]},
                                                            {m_input.vibrateAction, hapticPath[Side::RIGHT]}}};
            XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
            suggestedBindings.interactionProfile = yvrTouchInteractionProfilePath;
            suggestedBindings.suggestedBindings = bindings.data();
            suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
            CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
        }

        XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
        actionSpaceInfo.action = m_input.poseAction;
        actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
        actionSpaceInfo.subactionPath = m_input.handSubactionPath[Side::LEFT];
        CHECK_XRCMD(xrCreateActionSpace(m_session, &actionSpaceInfo, &m_input.handSpace[Side::LEFT]));
        actionSpaceInfo.subactionPath = m_input.handSubactionPath[Side::RIGHT];
        CHECK_XRCMD(xrCreateActionSpace(m_session, &actionSpaceInfo, &m_input.handSpace[Side::RIGHT]));

        XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
        attachInfo.countActionSets = 1;
        attachInfo.actionSets = &m_input.actionSet;
        CHECK_XRCMD(xrAttachSessionActionSets(m_session, &attachInfo));
    }

    void CreateVisualizedSpaces()
    {
        CHECK(m_session != XR_NULL_HANDLE);
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo("View");
        CHECK_XRCMD(xrCreateReferenceSpace(m_session, &referenceSpaceCreateInfo, &m_ViewSpace));
    }

    void InitializeSession() override
    {
        CHECK(m_instance != XR_NULL_HANDLE);
        CHECK(m_session == XR_NULL_HANDLE);

        {
            Log::Write(Log::Level::Verbose, Fmt("Creating session..."));

            XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
            createInfo.next = m_graphicsPlugin->GetGraphicsBinding();
            createInfo.systemId = m_systemId;
            CHECK_XRCMD(xrCreateSession(m_instance, &createInfo, &m_session));
        }

        LogReferenceSpaces();
        InitializeActions();
        CreateVisualizedSpaces();

        {
            XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(m_options->AppSpace);
            CHECK_XRCMD(xrCreateReferenceSpace(m_session, &referenceSpaceCreateInfo, &m_appSpace));
        }

#ifdef YVR_EXT_TEST_DISPLAY_REFRESH_RATE
        xrGetInstanceProcAddr(m_instance, "xrRequestDisplayRefreshRateYVR", (PFN_xrVoidFunction*)(&m_pfnXrGetDisplayRefreshRateYvr));
        CHECK_XRCMD(m_pfnXrGetDisplayRefreshRateYvr(m_session, m_displayRefreshRate));
#endif // YVR_EXT_TEST_DISPLAY_REFRESH_RATE
        Log::Write(Log::Level::Info, Fmt("device fps:%0.3f", m_displayRefreshRate));
    }

    void CreateSwapchains() override
    {
        CHECK(m_session != XR_NULL_HANDLE);
        CHECK(m_swapchains.empty());
        CHECK(m_configViews.empty());

        // Read graphics properties for preferred swapchain length and logging.
        XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
        CHECK_XRCMD(xrGetSystemProperties(m_instance, m_systemId, &systemProperties));

        // Log system properties.
        Log::Write(Log::Level::Info, Fmt("System Properties: Name=%s VendorId=%d", systemProperties.systemName, systemProperties.vendorId));
        Log::Write(
            Log::Level::Info,
            Fmt("System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d", systemProperties.graphicsProperties.maxSwapchainImageWidth,
                systemProperties.graphicsProperties.maxSwapchainImageHeight, systemProperties.graphicsProperties.maxLayerCount));
        Log::Write(Log::Level::Info, Fmt("System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
                                         systemProperties.trackingProperties.orientationTracking == XR_TRUE ? "True" : "False",
                                         systemProperties.trackingProperties.positionTracking == XR_TRUE ? "True" : "False"));

        // Note: No other view configurations exist at the time this code was written. If this
        // condition is not met, the project will need to be audited to see how support should be
        // added.
        CHECK_MSG(m_options->Parsed.ViewConfigType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, "Unsupported view configuration type");

        // Query and cache view configuration views.
        uint32_t viewCount;
        CHECK_XRCMD(xrEnumerateViewConfigurationViews(m_instance, m_systemId, m_options->Parsed.ViewConfigType, 0, &viewCount, nullptr));
        m_configViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        CHECK_XRCMD(
            xrEnumerateViewConfigurationViews(m_instance, m_systemId, m_options->Parsed.ViewConfigType, viewCount, &viewCount, m_configViews.data()));

        // Create and cache view buffer for xrLocateViews later.
        m_views.resize(viewCount, {XR_TYPE_VIEW});

        // Create the swapchain and get the images.
        if (viewCount > 0)
        {
            // Select a swapchain format.
            uint32_t swapchainFormatCount;
            CHECK_XRCMD(xrEnumerateSwapchainFormats(m_session, 0, &swapchainFormatCount, nullptr));
            std::vector<int64_t> swapchainFormats(swapchainFormatCount);
            CHECK_XRCMD(xrEnumerateSwapchainFormats(m_session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount, swapchainFormats.data()));
            CHECK(swapchainFormatCount == swapchainFormats.size());
            m_colorSwapchainFormat = m_graphicsPlugin->SelectColorSwapchainFormat(swapchainFormats);

            // Print swapchain formats and the selected one.
            {
                std::string swapchainFormatsString;
                for (int64_t format : swapchainFormats)
                {
                    const bool selected = format == m_colorSwapchainFormat;
                    swapchainFormatsString += " ";
                    if (selected)
                    {
                        swapchainFormatsString += "[";
                    }
                    swapchainFormatsString += std::to_string(format);
                    if (selected)
                    {
                        swapchainFormatsString += "]";
                    }
                }
                Log::Write(Log::Level::Verbose, Fmt("Swapchain Formats: %s", swapchainFormatsString.c_str()));
            }

            // Create a swapchain for each view.
            for (uint32_t i = 0; i < viewCount; i++)
            {
                const XrViewConfigurationView& vp = m_configViews[i];
                Log::Write(Log::Level::Info, Fmt("Creating swapchain for view %d with dimensions Width=%d Height=%d SampleCount=%d", i,
                                                 vp.recommendedImageRectWidth, vp.recommendedImageRectHeight, vp.recommendedSwapchainSampleCount));

                // Create the swapchain.
                XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
                swapchainCreateInfo.arraySize = 1;
                swapchainCreateInfo.format = m_colorSwapchainFormat;
                swapchainCreateInfo.width = vp.recommendedImageRectWidth;
                swapchainCreateInfo.height = vp.recommendedImageRectHeight;
                swapchainCreateInfo.mipCount = 1;
                swapchainCreateInfo.faceCount = 1;
                swapchainCreateInfo.sampleCount = m_graphicsPlugin->GetSupportedSwapchainSampleCount(vp);
                swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
                Swapchain swapchain;
                swapchain.width = swapchainCreateInfo.width;
                swapchain.height = swapchainCreateInfo.height;
                CHECK_XRCMD(xrCreateSwapchain(m_session, &swapchainCreateInfo, &swapchain.handle));

                m_swapchains.push_back(swapchain);

                uint32_t imageCount;
                CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr));
                // XXX This should really just return XrSwapchainImageBaseHeader*
                std::vector<XrSwapchainImageBaseHeader*> swapchainImages =
                    m_graphicsPlugin->AllocateSwapchainImageStructs(imageCount, swapchainCreateInfo);
                CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, swapchainImages[0]));

                m_swapchainImages.insert(std::make_pair(swapchain.handle, std::move(swapchainImages)));
            }
        }
    }

    // Return event if one is available, otherwise return null.
    const XrEventDataBaseHeader* TryReadNextEvent()
    {
        // It is sufficient to clear the just the XrEventDataBuffer header to
        // XR_TYPE_EVENT_DATA_BUFFER
        XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&m_eventDataBuffer);
        *baseHeader = {XR_TYPE_EVENT_DATA_BUFFER};
        const XrResult xr = xrPollEvent(m_instance, &m_eventDataBuffer);
        if (xr == XR_SUCCESS)
        {
            if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST)
            {
                const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
                Log::Write(Log::Level::Warning, Fmt("%d events lost", eventsLost));
            }

            return baseHeader;
        }
        if (xr == XR_EVENT_UNAVAILABLE)
        {
            return nullptr;
        }
        THROW_XR(xr, "xrPollEvent");
    }

    void PollEvents(bool* exitRenderLoop, bool* requestRestart) override
    {
        *exitRenderLoop = *requestRestart = false;

        // Process all pending messages.
        while (const XrEventDataBaseHeader* event = TryReadNextEvent())
        {
            switch (event->type)
            {
                case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
                {
                    const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
                    Log::Write(Log::Level::Warning, Fmt("XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime));
                    *exitRenderLoop = true;
                    *requestRestart = true;
                    return;
                }
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
                {
                    auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
                    HandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
                    break;
                }
                case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
//                    LogActionSourceName(m_input.grabAction, "Grab");
                    LogActionSourceName(m_input.quitAction, "Quit");
                    LogActionSourceName(m_input.poseAction, "Pose");
                    LogActionSourceName(m_input.vibrateAction, "Vibrate");
                    break;
                case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                default:
                {
                    Log::Write(Log::Level::Verbose, Fmt("Ignoring event type %d", event->type));
                    break;
                }
            }
        }
    }

    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop, bool* requestRestart)
    {
        const XrSessionState oldState = m_sessionState;
        m_sessionState = stateChangedEvent.state;

        Log::Write(Log::Level::Info, Fmt("XrEventDataSessionStateChanged: state %s->%s session=%lld time=%lld", to_string(oldState),
                                         to_string(m_sessionState), stateChangedEvent.session, stateChangedEvent.time));

        if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != m_session))
        {
            Log::Write(Log::Level::Error, "XrEventDataSessionStateChanged for unknown session");
            return;
        }

        switch (m_sessionState)
        {
            case XR_SESSION_STATE_READY:
            {
                CHECK(m_session != XR_NULL_HANDLE);
                XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                sessionBeginInfo.primaryViewConfigurationType = m_options->Parsed.ViewConfigType;
                CHECK_XRCMD(xrBeginSession(m_session, &sessionBeginInfo));
                m_sessionRunning = true;
                break;
            }
            case XR_SESSION_STATE_STOPPING:
            {
                CHECK(m_session != XR_NULL_HANDLE);
                m_sessionRunning = false;
                CHECK_XRCMD(xrEndSession(m_session))
                break;
            }
            case XR_SESSION_STATE_EXITING:
            {
                *exitRenderLoop = true;
                // Do not attempt to restart because user closed this session.
                *requestRestart = false;
                break;
            }
            case XR_SESSION_STATE_LOSS_PENDING:
            {
                *exitRenderLoop = true;
                // Poll for a new instance.
                *requestRestart = true;
                break;
            }
            default:
                break;
        }
    }

    void LogActionSourceName(XrAction action, const std::string& actionName) const
    {
        XrBoundSourcesForActionEnumerateInfo getInfo = {XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
        getInfo.action = action;
        uint32_t pathCount = 0;
        CHECK_XRCMD(xrEnumerateBoundSourcesForAction(m_session, &getInfo, 0, &pathCount, nullptr));
        std::vector<XrPath> paths(pathCount);
        CHECK_XRCMD(xrEnumerateBoundSourcesForAction(m_session, &getInfo, uint32_t(paths.size()), &pathCount, paths.data()));

        std::string sourceName;
        for (uint32_t i = 0; i < pathCount; ++i)
        {
            constexpr XrInputSourceLocalizedNameFlags all = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                            XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                            XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

            XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
            nameInfo.sourcePath = paths[i];
            nameInfo.whichComponents = all;

            uint32_t size = 0;
            CHECK_XRCMD(xrGetInputSourceLocalizedName(m_session, &nameInfo, 0, &size, nullptr));
            if (size < 1)
            {
                continue;
            }
            std::vector<char> grabSource(size);
            CHECK_XRCMD(xrGetInputSourceLocalizedName(m_session, &nameInfo, uint32_t(grabSource.size()), &size, grabSource.data()));
            if (!sourceName.empty())
            {
                sourceName += " and ";
            }
            sourceName += "'";
            sourceName += std::string(grabSource.data(), size - 1);
            sourceName += "'";
        }

        Log::Write(Log::Level::Info, Fmt("%s action is bound to %s", actionName.c_str(), ((!sourceName.empty()) ? sourceName.c_str() : "nothing")));
    }

    bool IsSessionRunning() const override { return m_sessionRunning; }

    bool IsSessionFocused() const override { return m_sessionState == XR_SESSION_STATE_FOCUSED; }

    void SetHandleState(cxrControllerEvent* events, int inputindex, float fvalue, uint64_t inputTimeNS){
        events->clientTimeNS = inputTimeNS;
        events->clientInputIndex = inputindex;
        events->inputValue.valueType = cxrInputValueType_float32;
        events->inputValue.vF32 = fvalue;

        return ;
    }

    void SetHandleState(cxrControllerEvent* events, int inputindex, uint32_t bvalue, uint64_t inputTimeNS){
        events->clientTimeNS = inputTimeNS;
        events->clientInputIndex = inputindex;
        events->inputValue.valueType = cxrInputValueType_boolean;
        events->inputValue.vBool = bvalue;

        return ;
    }

    uint64_t mapButtons(int hand, uint64_t inputTimeNS, cxrControllerEvent* events, uint32_t& inputindex) {
        uint64_t buttons = 0;
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        XrResult xr = XR_SUCCESS;
        uint32_t buttonValue = 0;

        // button1 click
        getInfo.action = m_input.button1Action;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        XrActionStateBoolean grabValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mClickValue[hand][buttonValue] ){
            if (hand == Side::LEFT){
                SetHandleState(events+inputindex,14,grabValue.currentState, inputTimeNS);
            } else{
                SetHandleState(events+inputindex,12,grabValue.currentState, inputTimeNS);
            }
            inputindex++;
            mClickValue[hand][buttonValue] = grabValue.currentState;
        }
        buttonValue++;

        // button2 click
        getInfo.action = m_input.button2Action;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mClickValue[hand][buttonValue]){
            if (hand == Side::LEFT){
                SetHandleState(events+inputindex,15,grabValue.currentState, inputTimeNS);
            } else{
                SetHandleState(events+inputindex,13,grabValue.currentState, inputTimeNS);
            }
            mClickValue[hand][buttonValue] = grabValue.currentState;
            inputindex++;
        }
        buttonValue++;

        // thumbstickClickAction
        getInfo.action = m_input.thumbstickClickAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mClickValue[hand][buttonValue]) {
            SetHandleState(events+inputindex,8,grabValue.currentState, inputTimeNS);
            inputindex++;
            mClickValue[hand][buttonValue] = grabValue.currentState;
        }
        buttonValue++;

        // kButtonStart click
        if (hand == Side::LEFT)
        {
            XrActionStateGetInfo quitInfo{XR_TYPE_ACTION_STATE_GET_INFO, nullptr,
                                          m_input.quitAction, XR_NULL_PATH};
            XrActionStateBoolean quitValue{XR_TYPE_ACTION_STATE_BOOLEAN};
            CHECK_XRCMD(xrGetActionStateBoolean(m_session, &quitInfo, &quitValue));
            if (grabValue.currentState != mClickValue[hand][buttonValue]) {
                SetHandleState(events+inputindex,1,grabValue.currentState, inputTimeNS);
                inputindex++;
                mClickValue[hand][buttonValue] = grabValue.currentState;
            }
        }
        buttonValue++;

        // squeezeClickAction
        getInfo.action = m_input.squeezeClickAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        XrActionStateBoolean squeezeClick{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &squeezeClick));
        if (grabValue.currentState != mClickValue[hand][buttonValue]){
            SetHandleState(events+inputindex,2,grabValue.currentState, inputTimeNS);
            inputindex++;
            mClickValue[hand][buttonValue] = grabValue.currentState;
        }

        return buttons;
    }

    uint64_t mapTouch(int hand, uint64_t inputTimeNS, cxrControllerEvent* events, uint32_t& inputindex) {
        uint64_t buttons = 0;
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        XrResult xr = XR_SUCCESS;
        XrActionStateBoolean grabValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        uint32_t touchValue = 0;

        // button1touch
        getInfo.action = m_input.button1touchAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mTouchValue[hand][touchValue]){
            if (hand == Side::LEFT){
                SetHandleState(events+inputindex,18,grabValue.currentState, inputTimeNS);
            } else{
                SetHandleState(events+inputindex,16,grabValue.currentState, inputTimeNS);
            }
            mTouchValue[hand][touchValue] = grabValue.currentState;
            inputindex++;
        }
        touchValue++;

        // button2touch
        getInfo.action = m_input.button2touchAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mTouchValue[hand][touchValue]){
            if (hand == Side::LEFT){
                SetHandleState(events+inputindex,19,grabValue.currentState, inputTimeNS);
            } else{
                SetHandleState(events+inputindex,17,grabValue.currentState, inputTimeNS);
            }
            inputindex++;
            mTouchValue[hand][touchValue] = grabValue.currentState;
        }
        touchValue++;

        // triggerTouchAction
        getInfo.action = m_input.triggerTouchAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mTouchValue[hand][touchValue]){
            SetHandleState(events+inputindex,3,grabValue.currentState, inputTimeNS);
            inputindex++;
            mTouchValue[hand][touchValue] = grabValue.currentState;
        }
        touchValue++;

        // thumbstickTouchAction
        getInfo.action = m_input.thumbstickTouchAction;
        getInfo.subactionPath = m_input.handSubactionPath[hand];
        CHECK_XRCMD(xrGetActionStateBoolean(m_session, &getInfo, &grabValue));
        if (grabValue.currentState != mTouchValue[hand][touchValue]){
            SetHandleState(events+inputindex,9,grabValue.currentState, inputTimeNS);
            inputindex++;
            mTouchValue[hand][touchValue] = grabValue.currentState;
        }
        touchValue++;

        return buttons;
    }

    void PollActions() override
    {
        m_input.handActive = {XR_FALSE, XR_FALSE};

        // Sync actions
        const XrActiveActionSet activeActionSet{m_input.actionSet, XR_NULL_PATH};
        XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
        syncInfo.countActiveActionSets = 1;
        syncInfo.activeActionSets = &activeActionSet;
        CHECK_XRCMD(xrSyncActions(m_session, &syncInfo));

        cxrVRTrackingState trackingState{};

        // Get pose and grab action state and start haptic vibrate when hand is 90% squeezed.
        for (auto hand : {Side::LEFT, Side::RIGHT})
        {
            const uint64_t inputTimeNS = GetTimeInNS();

            cxrControllerEvent events[64] = {};
            uint32_t eventCount = 0;
            uint32_t valueindex = 0;

            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.subactionPath = m_input.handSubactionPath[hand];

            getInfo.action = m_input.poseAction;
            XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
            CHECK_XRCMD(xrGetActionStatePose(m_session, &getInfo, &poseState));
            m_input.handActive[hand] = poseState.isActive;

            if (true == poseState.isActive){

                mapButtons( hand, inputTimeNS, events, eventCount);
                mapTouch( hand, inputTimeNS, events, eventCount);

                getInfo.action = m_input.triggerValueAction;
                getInfo.subactionPath = m_input.handSubactionPath[hand];
                XrActionStateFloat grabFloat{XR_TYPE_ACTION_STATE_FLOAT};
                CHECK_XRCMD(xrGetActionStateFloat(m_session, &getInfo, &grabFloat));
                if ( grabFloat.isActive == XR_TRUE && mfloatValue[hand][valueindex] != grabFloat.currentState){
                    SetHandleState(&events[eventCount],4,grabFloat.currentState, inputTimeNS);
                    eventCount++;
                    mfloatValue[hand][valueindex] = grabFloat.currentState;
                }
                valueindex++;

                getInfo.action = m_input.thumbstickxAction;
                getInfo.subactionPath = m_input.handSubactionPath[hand];
                grabFloat.type = XR_TYPE_ACTION_STATE_FLOAT;
                CHECK_XRCMD(xrGetActionStateFloat(m_session, &getInfo, &grabFloat));
                if ( grabFloat.isActive == XR_TRUE && mfloatValue[hand][valueindex] != grabFloat.currentState){
                    SetHandleState(&events[eventCount],10,grabFloat.currentState, inputTimeNS);
                    eventCount++;
                    mfloatValue[hand][valueindex] = grabFloat.currentState;
                }
                valueindex++;

                getInfo.action = m_input.thumbstickyAction;
                getInfo.subactionPath = m_input.handSubactionPath[hand];
                grabFloat.type = XR_TYPE_ACTION_STATE_FLOAT;
                CHECK_XRCMD(xrGetActionStateFloat(m_session, &getInfo, &grabFloat));
                if ( grabFloat.isActive == XR_TRUE && mfloatValue[hand][valueindex] != grabFloat.currentState){
                    SetHandleState(&events[eventCount],11,grabFloat.currentState, inputTimeNS);
                    eventCount++;
                    mfloatValue[hand][valueindex] = grabFloat.currentState;
                }
                valueindex = 0;

                m_cloudxr->SetEvent(hand, events, eventCount);
            }else{
                m_cloudxr->SetEvent(hand, nullptr, 0);
            }

        }
//        if (m_cloudxr.get()) {
//            m_cloudxr->SetTrackingState(trackingState);
//        }
    }

    void RenderFrame() override
    {
        CHECK(m_session != XR_NULL_HANDLE);

        XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
        XrFrameState frameState{XR_TYPE_FRAME_STATE};
        CHECK_XRCMD(xrWaitFrame(m_session, &frameWaitInfo, &frameState));

        XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
        CHECK_XRCMD(xrBeginFrame(m_session, &frameBeginInfo));

        std::vector<XrCompositionLayerBaseHeader*> layers;
        XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
        if (frameState.shouldRender == XR_TRUE)
        {
            if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer))
            {
                layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
            }
        }

        XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
        frameEndInfo.displayTime = frameState.predictedDisplayTime;
        frameEndInfo.environmentBlendMode = m_options->Parsed.EnvironmentBlendMode;
        frameEndInfo.layerCount = (uint32_t)layers.size();
        frameEndInfo.layers = layers.data();
        CHECK_XRCMD(xrEndFrame(m_session, &frameEndInfo));
    }

    bool RenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer)
    {
        XrResult res;

        XrViewState viewState{XR_TYPE_VIEW_STATE};
        uint32_t viewCapacityInput = (uint32_t)m_views.size();
        uint32_t viewCountOutput;

        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = m_options->Parsed.ViewConfigType;
        viewLocateInfo.displayTime = predictedDisplayTime;
        viewLocateInfo.space = m_appSpace;

        res = xrLocateViews(m_session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, m_views.data());
        CHECK_XRRESULT(res, "xrLocateViews");
        if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
            (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0)
        {
            return false;  // There is no valid tracking poses for the views.
        }

        // get ipd
        float ipd;
        PFN_xrGetSystemPropertyFloatYVR pfnGetSystemPropertyFloatYVR;
        xrGetInstanceProcAddr(m_instance, "xrGetSystemPropertyFloatYVR", (PFN_xrVoidFunction*)(&pfnGetSystemPropertyFloatYVR));
        res = pfnGetSystemPropertyFloatYVR(m_session, YVR_SYS_PROP_GET_IPD, &ipd);
        CHECK_XRRESULT(res, "xrGetSystemPropertyFloatYVR");

//        Log::Write(Log::Level::Info, Fmt("ipd is  %f, view: %f,%f,%f,%f", ipd,m_views[0].fov.angleLeft,
//                                         m_views[0].fov.angleRight,m_views[0].fov.angleUp,m_views[0].fov.angleDown));


        CHECK(viewCountOutput == viewCapacityInput);
        CHECK(viewCountOutput == m_configViews.size());
        CHECK(viewCountOutput == m_swapchains.size());

        projectionLayerViews.resize(viewCountOutput);

#ifdef YVR_EXT_TEST_ACCELERATION
        XrSpaceAccelerationExtYVR acceleration{XR_TYPE_SPACE_ACCELERATION_EXT_YVR};
        XrSpaceVelocity velocity{XR_TYPE_SPACE_VELOCITY, &acceleration};
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION, &velocity};
#else
        XrSpaceVelocity velocity{XR_TYPE_SPACE_VELOCITY};
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION, &velocity};
#endif
        res = xrLocateSpace(m_ViewSpace, m_appSpace, predictedDisplayTime, &spaceLocation);
        CHECK_XRRESULT(res, "xrLocateSpace");

        XrPosef handPose[Side::COUNT];
        for (auto hand : {Side::LEFT, Side::RIGHT}) {
            XrSpaceLocation handLocation{XR_TYPE_SPACE_LOCATION};
            res = xrLocateSpace(m_input.handSpace[hand], m_appSpace, predictedDisplayTime, &handLocation);
            CHECK_XRRESULT(res, "xrLocateSpace");
            if (XR_UNQUALIFIED_SUCCESS(res)) {
                if ((handLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
                    (handLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                    handPose[hand] = handLocation.pose;
                }
            }
        }

        m_cloudxr->SetSenserPoseState(spaceLocation.pose, velocity.linearVelocity, velocity.angularVelocity,
                                      acceleration.angularAcceleration,acceleration.linearAcceleration,
                                      handPose[Side::LEFT],handPose[Side::RIGHT], ipd, std::abs(m_views[0].fov.angleLeft), std::abs(m_views[0].fov.angleUp));

        cxrFramesLatched framesLatched{};
        bool framevaild = m_cloudxr->LatchFrame(&framesLatched);

        // Render view to the appropriate part of the swapchain image.
        for (uint32_t i = 0; i < viewCountOutput; i++)
        {
            // Each view has a separate swapchain which is acquired, rendered to, and released.
            const Swapchain viewSwapchain = m_swapchains[i];

            XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

            uint32_t swapchainImageIndex;
            CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

            XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
            waitInfo.timeout = XR_INFINITE_DURATION;
            CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

            projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
            projectionLayerViews[i].pose = m_views[i].pose;
            projectionLayerViews[i].fov = m_views[i].fov;
            projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
            projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
            projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

            const XrSwapchainImageBaseHeader* const swapchainImage = m_swapchainImages[viewSwapchain.handle][swapchainImageIndex];
            const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLESKHR*>(swapchainImage)->image;
            if (m_cloudxr->SetupFramebuffer(colorTexture, i, projectionLayerViews[i].subImage.imageRect.extent.width, projectionLayerViews[i].subImage.imageRect.extent.height)) {
                cxrVideoFrame &videoFrame = framesLatched.frames[i];
                m_cloudxr->BlitFrame(&framesLatched, framevaild, i);
            }

            XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
        }

        if (framevaild) {
            m_cloudxr->ReleaseFrame(&framesLatched);
        }

        layer.space = m_appSpace;
        layer.layerFlags = m_options->Parsed.EnvironmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND
                         ? XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT
                         : 0;
        layer.viewCount = (uint32_t)projectionLayerViews.size();
        layer.views = projectionLayerViews.data();
        return true;
    }


    bool CreateCloudxrClient() override {
        m_cloudxr = std::make_shared<CloudXRClient>();
        return true;
    }

    void SetCloudxrClientPaused(bool pause) override {
        if (m_cloudxr.get()) {
            m_cloudxr->SetPaused(pause);
        }
    }

    void StartCloudxrClient() override {
        if (m_cloudxr.get()) {
            m_cloudxr->Initialize(m_instance, m_systemId, m_session, m_displayRefreshRate, (void*)this, [](void *arg, int controllerIdx, float amplitude, float seconds, float frequency) {
                Log::Write(Log::Level::Error, Fmt("this:%p, index:%d, amplitude:%f, seconds:%f, frequency:%f", arg, controllerIdx, amplitude, seconds, frequency));
                OpenXrProgram* thiz = (OpenXrProgram*)arg;
                XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
                vibration.amplitude = amplitude;
                vibration.duration = seconds * 1000 * 10000000;  //nanoseconds
                vibration.frequency = frequency;
                XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
                hapticActionInfo.action = thiz->m_input.vibrateAction;
                hapticActionInfo.subactionPath = thiz->m_input.handSubactionPath[controllerIdx];
                CHECK_XRCMD(xrApplyHapticFeedback(thiz->m_session, &hapticActionInfo, (XrHapticBaseHeader*)&vibration));
            });
        }
    }

  private:
    const std::shared_ptr<const Options> m_options;
    std::shared_ptr<IPlatformPlugin> m_platformPlugin;
    std::shared_ptr<IGraphicsPlugin> m_graphicsPlugin;
    XrInstance m_instance{XR_NULL_HANDLE};
    XrSession m_session{XR_NULL_HANDLE};
    XrSpace m_appSpace{XR_NULL_HANDLE};
    XrSystemId m_systemId{XR_NULL_SYSTEM_ID};

    std::vector<XrViewConfigurationView> m_configViews;
    std::vector<Swapchain> m_swapchains;
    
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> m_swapchainImages;
    
    std::vector<XrView> m_views;
    int64_t m_colorSwapchainFormat{-1};

    // Application's current lifecycle state according to the runtime
    XrSessionState m_sessionState{XR_SESSION_STATE_UNKNOWN};
    bool m_sessionRunning{false};

    XrEventDataBuffer m_eventDataBuffer;
    InputState m_input;

    float mfloatValue[Side::COUNT][3] = {0.0f};
    bool mTouchValue[Side::COUNT][10] = {false};
    bool mClickValue[Side::COUNT][10] = {false};

    std::shared_ptr<CloudXRClient> m_cloudxr;
    XrSpace m_ViewSpace{XR_NULL_HANDLE};
    PFN_xrRequestDisplayRefreshRateYVR m_pfnXrGetDisplayRefreshRateYvr;
    float m_displayRefreshRate = 72.0f;
    uint32_t m_deviceROM;
};
}  // namespace

std::shared_ptr<IOpenXrProgram> CreateOpenXrProgram(const std::shared_ptr<Options>& options, const std::shared_ptr<IPlatformPlugin>& platformPlugin,
                                                    const std::shared_ptr<IGraphicsPlugin>& graphicsPlugin)
{
    return std::make_shared<OpenXrProgram>(options, platformPlugin, graphicsPlugin);
}
