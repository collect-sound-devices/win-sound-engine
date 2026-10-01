using System.Runtime.InteropServices;
using Windows.Win32;
using Windows.Win32.Foundation;
using Windows.Win32.System.Com.StructuredStorage;
using Windows.Win32.System.Variant;
using Windows.Win32.UI.Shell.PropertiesSystem;

namespace SoundDefaultUI;

internal static class TaskbarIdentity
{
    private const string AppUserModelId = "EduardDanziger.SystemAudio";

    // Must run before any window is created.
    public static void SetProcessId() =>
        PInvoke.SetCurrentProcessExplicitAppUserModelID(AppUserModelId).ThrowOnFailure();

    public static void Set(IntPtr window, string title) => Update(window, title);

    public static void Clear(IntPtr window) => Update(window, null);

    private static void Update(IntPtr window, string? title)
    {
        PInvoke.SHGetPropertyStoreForWindow(new HWND(window), out IPropertyStore store).ThrowOnFailure();
        try
        {
            var executablePath = Environment.ProcessPath;
            SetString(store, PInvoke.PKEY_AppUserModel_RelaunchCommand, title is null ? null : $"\"{executablePath}\"");
            SetString(store, PInvoke.PKEY_AppUserModel_RelaunchIconResource, title is null ? null : $"{executablePath},0");
            SetString(store, PInvoke.PKEY_AppUserModel_RelaunchDisplayNameResource, title);
        }
        finally
        {
            Marshal.ReleaseComObject(store);
        }
    }

    private static unsafe void SetString(IPropertyStore store, in PROPERTYKEY key, string? text)
    {
        fixed (char* chars = text)
        {
            var value = new PROPVARIANT();
            if (text is not null)
            {
                value.vt = VARENUM.VT_LPWSTR;
                value.pwszVal = new PWSTR(chars);
            }
            store.SetValue(in key, in value);
        }
    }
}
