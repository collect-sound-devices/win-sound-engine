using System.Runtime.InteropServices;

namespace SoundDefaultUI;

internal static class TaskbarIdentity
{
    private static readonly Guid AppUserModelPropertySet = new("9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3");

    public static void Set(IntPtr window, string title) => Update(window, title);

    public static void Clear(IntPtr window) => Update(window, null);

    private static void Update(IntPtr window, string? title)
    {
        var interfaceId = typeof(IPropertyStore).GUID;
        SHGetPropertyStoreForWindow(window, in interfaceId, out var store);
        try
        {
            var executablePath = Environment.ProcessPath;
            SetString(store, 2, title is null ? null : $"\"{executablePath}\"");
            SetString(store, 3, title is null ? null : $"{executablePath},0");
            SetString(store, 4, title);
            SetString(store, 5, title is null ? null : "EduardDanziger.SystemAudio");
        }
        finally
        {
            Marshal.ReleaseComObject(store);
        }
    }

    private static void SetString(IPropertyStore store, uint propertyId, string? text)
    {
        var key = new PropertyKey { FormatId = AppUserModelPropertySet, PropertyId = propertyId };
        var value = new PropVariant
        {
            ValueType = (ushort)(text is null ? VarEnum.VT_EMPTY : VarEnum.VT_LPWSTR),
            Value = text
        };
        store.SetValue(in key, in value);
    }

#pragma warning disable SYSLIB1054
    [DllImport("shell32.dll", PreserveSig = false)]
    private static extern void SHGetPropertyStoreForWindow(IntPtr window, in Guid interfaceId,
        [MarshalAs(UnmanagedType.Interface)] out IPropertyStore store);
#pragma warning restore SYSLIB1054

    [StructLayout(LayoutKind.Sequential)]
    private struct PropertyKey
    {
        public Guid FormatId;
        public uint PropertyId;
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct PropVariant
    {
        public ushort ValueType;
        public ushort Reserved1, Reserved2, Reserved3;
        [MarshalAs(UnmanagedType.LPWStr)] public string? Value;
        public IntPtr Reserved4;
    }

    [ComImport]
    [Guid("886D8EEB-8CF2-4446-8D02-CDBA1DBDCF99")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    private interface IPropertyStore
    {
        void GetCount(out uint count);
        void GetAt(uint index, out PropertyKey key);
        void GetValue(in PropertyKey key, out PropVariant value);
        void SetValue(in PropertyKey key, in PropVariant value);
        void Commit();
    }
}
