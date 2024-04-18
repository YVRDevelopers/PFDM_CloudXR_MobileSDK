# PFDM_CloudXR_MobileSDK
# OpenXR CloudXR Client Demo
- **This software contains source code provided by NVIDIA Corporation**.
- Here's an example of programming for the YVR D1 / D2 / D3 headsets based on the NVIDIA CloudXR SDK, which receives data from the CloudXR server and decodes it for display.

## Requirements
- YVR D1 or D2 or D3
- [CloudXR SDK 4.0](https://developer.nvidia.com/cloudxr-sdk)
- [Google OBOE SDK 1.6.0](https://github.com/google/oboe/releases/tag/1.6.0)
- [OpenXR Mobile SDK](https://developer.yvr.cn/#/)
- For developers who want to modify, build, and debug the client application, a complete Android development environment is required:
  - Android Studio
  - Android SDK 10 (API level 29) or higher.
  - Android build tools 29.0.3
  - Android NDK 21.4.7075529
  - OpenJDK 1.8

## Build
1. Open Android Studio.
2. Complete one of the following tasks:
     - Select **Open existing Android Studio project** on the Welcome screen.
     - Click **File > Open**.
3. Navigate to `PFDM_CloudXR_MobileSDK` folder and open the `PFDM_CloudXR_MobileSDK` folder.
4. Select **Build > Rebuild Project**.

This process should generate an `.apk` file in the `PFDM_CloudXR_MobileSDK\app\build\outputs\apk\debug` directory that can be used to debug or be installed manually. You can also automatically generate an `.apk` file by running Android Studio.

> 💡 To build from the command line, run `gradlew build` from the `PFDM_CloudXR_MobileSDK` folder.

## Install
> 💡 You do not need these steps if you are running directly from Android Studio, it will install the `.apk` for you.
1. Use a USB cable to Connect the VR device to the PC, and a new drive will appear in "My Computer", such as "YVR 2" after a successful connection.
2. Method One
    1. Copy the .apk file to the base device folder as shown in Windows Explorer.
    2. Open the file manager on the VR device, select "Installations" on the right side, and click on the .apk file to install.
3. Method Two
    1. Use **ADB** to install the application from the release `.apk` file.
```
    adb.exe install -r <APK name>.apk
```
> 💡 By default, the ADB.exe program is installed by Android Studio in `C:\Users\{username}\AppData\Local\Android\Sdk\platform-tools`

## Use
1. (**Optional**) Pre-specify the command-line per device:
   1. Create a plain-text file named `CloudXRLaunchOptions.txt` that contains `-s <IP address NVIDIA CloudXR server>`.
  For example, for a server with `IP = 172.16.50.136`, the file should contain `-s 172.16.50.136`.
   2. Copy the `CloudXRLaunchOptions.txt` file to the base device folder as shown in Windows Explorer, or if using ADB, to the `/sdcard/` directory of the device using the following command:
      ```
      adb.exe push CloudXRLaunchOptions.txt /sdcard/CloudXRLaunchOptions.txt
      ```
      See [Command-Line Options](https://docs.nvidia.com/cloudxr-sdk/usr_guide/cmd_line_options.html#command-line-options) for more information about using launch options and a full list of all available options.

2. Start **SteamVR** on the server system.
3. Start the **PFDM_CloudXR_MobileSDK** app on YVR device.
  This process can be completed in one of the following ways:
  - If launch from VR on the device:
     - Open the main menu.
     - Select **Application Library**.
     - Select the **PFDM_CloudXR_MobileSDK** app.
  - If building in Android Studio, deploy/run directly.
> 💡 Must be in the same network segment
> 💡 PC port 48010 cannot be occupied
