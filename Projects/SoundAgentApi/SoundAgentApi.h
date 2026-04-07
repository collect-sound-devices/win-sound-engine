#ifndef SOUND_AGENT_API_H
#define SOUND_AGENT_API_H

/**
 * @file SoundAgentApi.h
 * @brief C API to monitor and query default audio devices.
 * Steps:
 * 1. Call ::SaaInitialize -> get handle.
 * 2. (Optional) ::SaaRegisterCallbacks to receive change events.
 * 3. Call ::SaaGetDefaultRender / ::SaaGetDefaultCapture to query devices.
 * 4. Call ::SaaUnInitialize before exit / unloading.
 * Threading: Serialize initialize/uninitialize. Callbacks may fire on worker thread; keep them fast and thread-safe.
 * Errors: 0 = success. See ::SaaResultCode for named values.
 * Strings: ANSI, truncated with null terminator.
 * Logging: Supply log callback at init to receive async log messages.
 */

#include <Windows.h>

#ifdef ED_EXPORTS
#define SAA_EXPORT_IMPORT_DECL __declspec(dllexport)  /**< Export when building DLL */
#else
#define SAA_EXPORT_IMPORT_DECL __declspec(dllimport)  /**< Import when consuming DLL */
#endif

#ifdef __cplusplus
    extern "C" {
#endif

    /** Opaque instance handle (do not interpret). Obtained via ::SaaInitialize. */
    typedef DWORD64 SaaHandle;

    /** Result code storage type. See ::SaaResultCode for named values. */
    typedef INT32 SaaResult;

    /** Result code values returned in ::SaaResult. */
    typedef enum {
        SaaResultCodeSuccess = 0,
        SaaResultCodeInvalidArgument = 1,
        SaaResultCodeInvalidHandle = 2,
        SaaResultCodeInternalError = 3
    } SaaResultCode;

    /** Device description. Unused fields zeroed. BOOL uses Win32 TRUE/FALSE. */
    typedef struct {
        CHAR   PnpId[80];          /**< Device PnP id. */
        CHAR   Name[128];          /**< Device friendly name. */
        BOOL   IsRender;           /**< Device can render audio. */
        BOOL   IsCapture;          /**< Device can capture audio. */
        UINT16 RenderVolume;       /**< Current render volume (implementation units). */
        UINT16 CaptureVolume;      /**< Current capture volume (implementation units). */
    } SaaDescription;

    /** OS info. */
    typedef struct {
        CHAR   Name[256];          /**< Extended operating system name. */
    } SaaOsInfo;

    /** Log message forwarded from internal logger. */
    typedef struct {
        CHAR Timestamp[34]; /**< Timestamp string. */
        CHAR Level[12];    /**< Severity: trace, debug, info, warn, warning, error, critical. */
        CHAR Content[256]; /**< Message text. */
    } SaaLogMessage;

    /** Default device / volume change notification event type. */
    typedef enum {
        SaaDefaultRenderAttached = 0,
        SaaDefaultCaptureAttached = 1,
        SaaDefaultRenderDetached = 2,
        SaaDefaultCaptureDetached = 3,
        SaaVolumeRenderChanged = 4,
        SaaVolumeCaptureChanged = 5
    } SaaEventType;

    /** Default device / volume change notification. */
    typedef void(__stdcall* TSaaDefaultChangedCallback)(
        _In_ SaaEventType event
        );

    /** Asynchronous log message callback. */
    typedef void(__stdcall* TSaaGotLogMessageCallback)(
        _In_ SaaLogMessage message
        );

    /**
     * Initialize library. Acquire handle. Optionally set log callback + app id info.
     * handle: out handle. gotLogMessageCallback: optional. appName/appVersion: optional identifiers.
     */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaInitialize(
            _Out_ SaaHandle* handle,
            _In_opt_ TSaaGotLogMessageCallback gotLogMessageCallback,
            _In_opt_ const CHAR* appName,
            _In_opt_ const CHAR* appVersion
        );

    /**
     * Register or replace callbacks for default render/capture changes. Pass NULL to disable each.
     * Implicitly refreshes internal device list.
     */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaRegisterCallbacks(
            _In_ SaaHandle handle,
            _In_opt_ TSaaDefaultChangedCallback defaultRenderChangedCallback,
            _In_opt_ TSaaDefaultChangedCallback defaultCaptureChangedCallback
        );

    /** Get current default render device (or zeroed struct if none). description must be non-null. */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaGetDefaultRender(
            _In_ SaaHandle handle,
            _Out_ SaaDescription* description
        );

    /** Get current default capture device (or zeroed struct if none). description must be non-null. */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaGetDefaultCapture(
            _In_ SaaHandle handle,
            _Out_ SaaDescription* description
        );

    /** Get operating system name (or zeroed struct if unavailable). osInfo must be non-null. */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaGetOperationSystemName(
            _In_ SaaHandle handle,
            _Out_ SaaOsInfo* osInfo
        );

    /** Uninitialize library. Invalidate handle. Safe to call multiple times (idempotent). */
    SAA_EXPORT_IMPORT_DECL
        SaaResult __stdcall SaaUnInitialize(
            _In_ SaaHandle handle
        );

#ifdef __cplusplus
}
#endif

#endif // SOUND_AGENT_API_H
