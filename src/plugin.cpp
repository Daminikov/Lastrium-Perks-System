#include "Logger.h"
#include "UI.h"
#include "Hooks.h"
#include "QTRLib.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoad) {
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();
    Hooks::Install();
    QTRLib::Install();
    return true;
}