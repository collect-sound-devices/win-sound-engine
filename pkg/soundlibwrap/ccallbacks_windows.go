//go:build windows && cgo

package soundlibwrap

/*
#include <stdint.h>
#include <stdbool.h>
#include "SoundAgentApi.h"
*/
import "C"

//export cgoSaaDefaultRenderChanged
func cgoSaaDefaultRenderChanged(event C.SaaEventType) {
	switch event {
	case C.SaaDefaultRenderAttached:
		NotifyDefaultRenderChanged(true)
	case C.SaaDefaultRenderDetached:
		NotifyDefaultRenderChanged(false)
	case C.SaaVolumeRenderChanged:
		NotifyRenderVolumeChanged()
	}
}

//export cgoSaaDefaultCaptureChanged
func cgoSaaDefaultCaptureChanged(event C.SaaEventType) {
	switch event {
	case C.SaaDefaultCaptureAttached:
		NotifyDefaultCaptureChanged(true)
	case C.SaaDefaultCaptureDetached:
		NotifyDefaultCaptureChanged(false)
	case C.SaaVolumeCaptureChanged:
		NotifyCaptureVolumeChanged()
	}
}

//export cgoSaaGotLogMessage
func cgoSaaGotLogMessage(msg *C.SaaLogMessage) {
	timestamp := C.GoString(&msg.Timestamp[0])
	level := C.GoString(&msg.Level[0])
	content := C.GoString(&msg.Content[0])
	NotifyGotLogMessage(timestamp, level, content)
}
