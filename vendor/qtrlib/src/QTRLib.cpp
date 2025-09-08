#include "QTRLib.h"
#include "DrawDebug.h"

namespace QTRLib {
    void OnMessage(SKSE::MessagingInterface::Message* message) {
        QTRLib::DrawDebug::OnMessage(message);
    }
}

void QTRLib::Install() {
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
}
