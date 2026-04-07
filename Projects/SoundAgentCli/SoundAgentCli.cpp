#include "stdafx.h"

#include "internal/TimeUtil.h"

#include "OsInfo.h"
#include "public/CoInitRaiiHelper.h"
#include "public/SoundAgentInterface.h"

#include <filesystem>
#include <memory>
#include <tchar.h>
#include <magic_enum/magic_enum.hpp>

#include "internal/SpdLogger.h"


class ServiceObserver final : public SoundDeviceObserverInterface {
public:
    explicit ServiceObserver(SoundDeviceCollectionInterface & collection)
        : collection_(collection)
    {
        SetUpLog();
    }

    static void SetUpLog()
    {
        ed::model::Logger::Inst().SetOutputToConsole(true);
        try
        {
            if (std::filesystem::path logFile;
                ed::utility::AppPath::GetAndValidateLogFilePathName(
                    logFile, RESOURCE_FILENAME_ATTRIBUTE)
                )
            {
                ed::model::Logger::Inst().SetPathName(logFile);
            }
            else
            {
                spdlog::warn("Log file can not be written.");
            }
        }
        catch (const std::exception& ex)
        {
            spdlog::warn("Logging set-up partially done; Log file can not be used: {}.", ex.what());
        }

        spdlog::info("Operation system: {}.", ed::audio::GetOperationSystemName());

    }


    DISALLOW_COPY_MOVE(ServiceObserver);
    ~ServiceObserver() override = default;

public:
    static void PrintDeviceInfo(const SoundDeviceInterface* device, size_t i)
    {
        const auto idString = device->GetPnpId();
        spdlog::info("[{}]: {}, \"{}\", {}, Volume {} / {}",
            i,
            idString,
            device->GetName(),
            magic_enum::enum_name(device->GetFlow()),
            device->GetCurrentRenderVolume(),
            device->GetCurrentCaptureVolume());
    }

    void PrintCollection() const
    {
        for (size_t i = 0; i < collection_.GetSize(); ++i)
        {
            const std::unique_ptr<SoundDeviceInterface> deviceSmartPtr(collection_.CreateItem(i));
            PrintDeviceInfo(deviceSmartPtr.get(), i);
        }
        spdlog::info("");
    }

    void ResetCollectionContentAndPrintIt() const
    {
        spdlog::info("Regenerating device list.");
        collection_.ResetContent();
        PrintCollection();
        spdlog::info("Press Enter to regenerate device list; To stop, type S or Q and press Enter");
    }

    void OnCollectionChanged(SoundDeviceEventType event, const std::string& devicePnpId) override
    {
        spdlog::info("Event caught: {}. Device PnP id: {}",
            magic_enum::enum_name(event),
            devicePnpId);

        spdlog::info("Print collection...");
        PrintCollection();
        spdlog::info("Press Enter to regenerate device list; To stop, type S or Q and press Enter");
    }

private:
    SoundDeviceCollectionInterface & collection_;
};

namespace
{
    bool StopAndWaitForInput()
    {
        for (;;)
        {
            std::string line;
            std::getline(std::cin, line);
            if (line == "S" || line == "s" || line == "Q" || line == "q")
            {
                return false;
            }
            if (line.empty())
            {
                return true;
            }

            spdlog::info("Input {} not recognized.", line);
        }
    }
}

int _tmain(int argc, _TCHAR * argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

    ed::CoInitRaiiHelper coInitHelper;

    const auto coll(SoundAgent::CreateDeviceCollection());

    ServiceObserver o(*coll);
    coll->Subscribe(o);

    bool continueLoop = true;

    while (continueLoop)
    {
        o.ResetCollectionContentAndPrintIt();

        continueLoop = StopAndWaitForInput();
    }

    spdlog::info("Print collection final state...");
    o.PrintCollection();

    coll->Unsubscribe(o);

    return 0;
}
