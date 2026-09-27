# Windows Sound Engine (WinSoundEngine)

WinSoundEngine is a sound device scanner engine that contains tools
to detects and outputs plug-and-play audio endpoint devices under Windows.
It handles audio notifications and device changes.

It contains:
- C/C++ sound agent core library SoundAgentLib
- SoundAgentApi DLL, that provides a simple ANSI C API to the library for external clients
- SoundDefaultUI, a lightweight C# UI monitor with SoundAgentApi as backend
- **win-sound-engine**, a Go-module that leverages SoundAgentApi for Go-clients
- win-sound-logger, a simple Go CLI tool to log audio device information to the console, using SoundAgentApi as backend

## Architecture

```mermaid
flowchart BT

classDef dottedBox fill:transparent, fill-opacity:0.55, stroke-dasharray:10 8, stroke-width:2px;
classDef stressedBox fill:#f0f0f0, fill-opacity:0.2, stroke-dasharray:10 8, stroke-width:4px;
classDef invisibleNode fill:transparent, stroke:transparent;

coreAudioApi["Core Audio
(Windows API)"]

subgraph scannerBackend["<b>win-sound-engine (C++ / Go module)</b>"]
    invisible3["
    
    
    
    
    "]
    class invisible3 invisibleNode
    goCgoWrapper["SoundLibWrap
    (Go package)"]
    soundAgentApiDll["ANSI C SoundAgentApi.dll,
    SoundDeviceCollection
    (C++ class)"]
    invisible4["
    
    
    
    
    "]
    class invisible4 invisibleNode
end
class scannerBackend stressedBox

coreAudioApi --->|Device and volume change
notifications| soundAgentApiDll
soundAgentApiDll ---> |Read device characteristics| coreAudioApi

winSoundScannerService["<b>WinSoundScanner</b>
(Go Windows Service)"]

subgraph eventTopicKafkaMicroservice["
"]
    eventTopic[("Event Topic
    (Kafka topic)")]
    class eventTopic dottedBox
    kafkaRestForwarder["KafkaToRestApiForwarder
    (.NET microservice)"]
    class kafkaRestForwarder dottedBox
end
class eventTopicKafkaMicroservice dottedBox
    
subgraph requestQueueRabbitMqMicroservice["
"]
    requestQueue[("Request Queue
    (RabbitMQ channel)")]
    rabbitMqRestForwarder["RmqToRestApiForwarder
    (.NET microservice)"]
end
class requestQueueRabbitMqMicroservice dottedBox

deviceRepositoryApi["Device Repository Server
(REST API)"]

winSoundScannerService ---> |Access device| goCgoWrapper
goCgoWrapper --->|Device events| winSoundScannerService

goCgoWrapper --> |C API calls| soundAgentApiDll
soundAgentApiDll -->|C / C++ callbacks| goCgoWrapper

winSoundScannerService -.-> |Publish device events| eventTopic
winSoundScannerService -->|Publish request messages| requestQueue

eventTopic -->|Fetch events| kafkaRestForwarder
kafkaRestForwarder --> |Detect events| eventTopic
kafkaRestForwarder -.->|POST/PUT requests| deviceRepositoryApi
requestQueue -->|Fetch messages| rabbitMqRestForwarder
rabbitMqRestForwarder --> |Detect messages| requestQueue
rabbitMqRestForwarder -->|POST/PUT requests| deviceRepositoryApi
```

## Go Modules Generated

- **win-sound-engine**: Go module that wraps the C++ core logic and provides a Go API for [WinSoundScanner](https://github.com/collect-sound-devices/win-sound-scanner-go)

## Executables Generated

- **SoundDefaultUI**: Lightweight WPF UI showing the live volume levels of the default audio devices, output and input device separately.
  ![SoundDefaultUI screenshot](202509011440SoundDefaultUI.jpg)
- **SoundAgentCli**: Command-line test CLI.
- **win-sound-logger.exe**: Simple Go test CLI that logs the current default audio devices and later device/volume change events to the console.

## Install and Run

### `SoundDefaultUI.exe`
- Download and unzip the latest rollout of **SoundDefaultUI-x.x.x**. from the latest repository
release's assets, [Release](https://github.com/collect-sound-devices/win-sound-engine/releases/latest)

- Install certificates and unblock the `SoundDefaultUI.exe` per PowerShell (start as Administrator):
    ```powershell
    Import-Certificate -FilePath .\CodeSign.cer -CertStoreLocation Cert:\LocalMachine\Root
    Unblock-File -Path .\SoundDefaultUI.exe
    ```
### 'SoundAgentCli.exe'

- Download and unzip the latest rollout of **SoundAgentCli-x.x.x**. from the latest repository
  release's assets, [Release](https://github.com/collect-sound-devices/win-sound-engine/releases/latest)

### 'win-sound-logger'

- Download and unzip the latest rollout of **win-sound-logger-x.x.x**. from the latest repository
  release's assets, [Release](https://github.com/collect-sound-devices/win-sound-engine/releases/latest)


## Developer Environment, How to Build

### Prerequisites

- Visual Studio 2026 (or later) with C++ and .NET toolchain installed.
- .NET 10 SDK installed (required to build the WPF `SoundDefaultUI` project).
- Go SDK installed (required to build `win-sound-logger`).
- vcpkg installed and bootstrapped:
  - Clone vcpkg and run the bootstrap script (Windows): `bootstrap-vcpkg.bat`
  - Ensure `vcpkg.exe` is available on PATH or configure your build to point to it.
    
### Building

```powershell
&"c:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe" WinSoundEngine.sln /target:Rebuild -restore /p:Configuration=Release /p:RestorePackagesConfig=true

.\scripts\buildGo.ps1 
```
The resulting binaries:
- `Projects\SoundDefaultUI\bin\Release\net10.0-windows10*\SoundDefaultUI.exe`
- `Projects\SoundAgentCli\bin\x64\Release\SoundAgentCli.exe`
- `x64\Release\win-sound-logger.exe`

## License

This project is licensed under the terms of the [MIT License](LICENSE).

## Contact

Eduard Danziger

Email: [edanziger@gmx.de](mailto:edanziger@gmx.de)
