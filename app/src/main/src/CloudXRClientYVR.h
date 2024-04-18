/* Created by kevin at 2022/11/07
  this file wrapped the cloudxr client
*/

#ifndef CLOUDXR_YVR_CLIENT_H_
#define CLOUDXR_YVR_CLIENT_H_ 1

#pragma once
#include "pch.h"
#include "common.h"
#include <oboe/Oboe.h>
#include <CloudXRClient.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <map>
#include <memory>
#include <mutex>

#define DEFAULT_IPD 0.0640f
namespace Side
{
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
}  // namespace Side

typedef void (*traggerHapticCallback)(void* arg, int controllerIdx, float amplitude, float seconds, float frequency);

static uint64_t GetTimeInNS() {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return ((uint64_t)(now.tv_sec * 1e9) + now.tv_nsec);
}

static constexpr int inputCountQuest = 21;
static const float BUTTON_THRESHHOLD = 0.5f;

static const char* inputPathsQuest[inputCountQuest] =
{
                "/input/system/click",
                "/input/application_menu/click",
                "/input/trigger/click",
                "/input/trigger/touch",
                "/input/trigger/value",         // triggerValueAction
                "/input/grip/click",
                "/input/grip/touch",
                "/input/grip/value",
                "/input/joystick/click",
                "/input/joystick/touch",
                "/input/joystick/x",
                "/input/joystick/y",
                "/input/a/click",
                "/input/b/click",
                "/input/x/click", // Touch has X/Y on L controller, so we'll map the raw strings.
                "/input/y/click",
                "/input/a/touch",
                "/input/b/touch",
                "/input/x/touch",
                "/input/y/touch",
                "/input/thumb_rest/touch",
};

static cxrInputValueType inputValueTypesQuest[inputCountQuest] =
{
                cxrInputValueType_boolean, //input/system/click
                cxrInputValueType_boolean, //input/menu/click
                cxrInputValueType_boolean, //input/squeeze/click
                cxrInputValueType_boolean, //input/trigger/touch
                cxrInputValueType_float32, //input/trigger/value
                cxrInputValueType_boolean, //input/grip/click
                cxrInputValueType_boolean, //input/grip/touch
                cxrInputValueType_float32, //input/grip/value
                cxrInputValueType_boolean, //input/joystick/click
                cxrInputValueType_boolean, //input/joystick/touch
                cxrInputValueType_float32, //input/joystick/x
                cxrInputValueType_float32, //input/joystick/y
                cxrInputValueType_boolean, //input/a/click
                cxrInputValueType_boolean, //input/b/click
                cxrInputValueType_boolean, //input/x/click
                cxrInputValueType_boolean, //input/y/click
                cxrInputValueType_boolean, //input/a/touch
                cxrInputValueType_boolean, //input/b/touch
                cxrInputValueType_boolean, //input/x/touch
                cxrInputValueType_boolean, //input/y/touch
                cxrInputValueType_boolean, //input/thumb_rest/touch
};

class CloudXRClient : public oboe::AudioStreamDataCallback {
public:

    CloudXRClient();

    ~CloudXRClient();

    void Initialize(XrInstance instance, XrSystemId systemId, XrSession session, float fps, void* arg, traggerHapticCallback traggerHaptic);

    void SetPaused(bool pause);

    bool LatchFrame(cxrFramesLatched *framesLatched);

    void BlitFrame(cxrFramesLatched *framesLatched, bool frameValid, uint32_t eye);

    void ReleaseFrame(cxrFramesLatched *framesLatched);

    void SetSenserPoseState(XrPosef& pose, XrVector3f& linearVelocity, XrVector3f& angularVelocity,
                            XrVector3f& angularAcceleration, XrVector3f& linearAcceleration,
                            XrPosef &handPoseleft,XrPosef &handPoseright, float ipd,float procX,float procY);

    XrQuaternionf cxrToQuaternion(const cxrMatrix34 &m);

    XrVector3f cxrGetTranslation(const cxrMatrix34 &m);

    void SetTrackingState(cxrVRTrackingState &trackingState);

    void SetEvent(int handindex, cxrControllerEvent* events, uint32_t eventCount);

    bool SetupFramebuffer(GLuint colorTexture, uint32_t eye, uint32_t width, uint32_t height);

private:

    bool Start();

    void Stop();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    bool CreateReceiver();

    void TeardownReceiver();

    void GetDeviceDesc(cxrDeviceDesc *params) const;

    void GetTrackingState(cxrVRTrackingState *trackingState);

    void ProcessControllers();

    void TriggerHaptic(const cxrHapticFeedback *);

    cxrTrackedDevicePose ConvertPose(const XrPosef& inPose, float rotationX = 0);

    cxrBool RenderAudio(const cxrAudioFrame *audioFrame);

    void FillBackground();

private:
    cxrReceiverHandle mReceiver;
    cxrClientState mClientState;
    cxrDeviceDesc mDeviceDesc;
    cxrConnectionDesc mConnectionDesc;
    cxrGraphicsContext mContext;
    cxrVRTrackingState mTrackingState;
    cxrControllerHandle     mcxrControllers[Side::COUNT] = {0};

    cxrControllerEvent mevents[Side::COUNT][64] = {};
    uint32_t meventCount[Side::COUNT] = {};

    XrInstance mInstance;
    XrSystemId mSystemId;
    XrSession  mSession;

    std::mutex mPoseMutex;
    XrPosef    mHeadPose;
    XrVector3f mLinearVelocity;
    XrVector3f mAngularVelocity;
    XrVector3f mAngularAcceleration;
    XrVector3f mLinearAcceleration;

    std::map<uint64_t, std::vector<XrView>> mPoseViewsMap;
    XrPosef mHandPose[Side::COUNT];
    bool mbHandPose[Side::COUNT];
    std::shared_ptr<oboe::AudioStream> mPlaybackStream;

    float mTargetDisplayRefresh = 72.0f;

    bool mIsPaused = true;
    bool mWasPaused = true;
    float mIPD;
    float mprocX;
    float mprocY;


    GLuint mFramebuffers[2];
    uint32_t mDefaultBGColor = 0xFF000000; // black to start until we set around OnResume.
    uint32_t mBGColor = mDefaultBGColor;

    traggerHapticCallback m_traggerHapticCallback;
    void*                 m_callbackArg;
};

// Row-major 4x4 matrix.
typedef struct pxrMatrix4f_ {
    float M[4][4];
} pxrMatrix4f;

#endif // CLOUDXR_YVR_CLIENT_H_