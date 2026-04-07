using System.Runtime.InteropServices;

namespace SoundDefaultUI;


[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SaaDescription
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
    public string PnpId;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
    public byte[] Name; // UTF-8 encoded string

    [MarshalAs(UnmanagedType.Bool)]
    public bool IsRender;

    [MarshalAs(UnmanagedType.Bool)]
    public bool IsCapture;

    public ushort RenderVolume;
    public ushort CaptureVolume;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SaaLogMessage
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 34)]
    public string Timestamp;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
    public string Level;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
    public byte[] Content; // UTF-8 encoded string
}

// ReSharper disable once EnumUnderlyingTypeIsInt
public enum SaaEventType : int
{
    SaaDefaultRenderAttached = 0,
    SaaDefaultCaptureAttached = 1,
    SaaDefaultRenderDetached = 2,
    SaaDefaultCaptureDetached = 3,
    SaaVolumeRenderChanged = 4,
    SaaVolumeCaptureChanged = 5
}

// ReSharper disable once EnumUnderlyingTypeIsInt
public enum SaaResult : int
{
    SaaResultCodeSuccess = 0,
    SaaResultCodeInvalidArgument = 1,
    SaaResultCodeInvalidHandle = 2,
    SaaResultCodeInternalError = 3
}

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void SaaDefaultChangedDelegate(
    SaaEventType eventType
);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void SaaGotLogMessageDelegate(System.IntPtr message);


internal static class SoundAgentApi
{
#pragma warning disable SYSLIB1054 // Warning for DllImport -> LibraryImport
#pragma warning disable CA2101 // Specify marshaling for P/Invoke string arguments
    [DllImport("SoundAgentApi.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    internal static extern SaaResult SaaInitialize(
        out ulong handle,
        SaaGotLogMessageDelegate? gotLogMessageCallback,
        string? appName,
        string? appVersion
#pragma warning restore CA2101
    );

    [DllImport("SoundAgentApi.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    internal static extern SaaResult SaaRegisterCallbacks(
        ulong handle,
        SaaDefaultChangedDelegate? defaultRenderChangedCallback,
        SaaDefaultChangedDelegate? defaultCaptureChangedCallback
    );

    [DllImport("SoundAgentApi.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    internal static extern SaaResult SaaGetDefaultRender(
        ulong handle,
        out SaaDescription description
    );

    [DllImport("SoundAgentApi.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    internal static extern SaaResult SaaGetDefaultCapture(
        ulong handle,
        out SaaDescription description
    );

    [DllImport("SoundAgentApi.dll", CallingConvention = CallingConvention.StdCall)]
    internal static extern SaaResult SaaUnInitialize(
        ulong handle
    );
}


