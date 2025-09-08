#include "Hooks.h"

void Hooks::SampleVirtualFunctionHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::VTABLE_PlayerCharacter[0]).write_vfunc(0x0, thunk);
}

void Hooks::SampleCallHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(0, 0).address() + REL::Relocate(0x0, 0x0), thunk);
}

void Hooks::SampleEventSink::Install() {
    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(new SampleEventSink());
}

void Hooks::SampleVirtualFunctionHook::thunk() {
    originalFunction();
}

void Hooks::SampleCallHook::thunk() {
    originalFunction();
}

RE::BSEventNotifyControl Hooks::SampleEventSink::ProcessEvent(const RE::TESActivateEvent* event,
                                                              RE::BSTEventSource<RE::TESActivateEvent>*) {
    if (!event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    return RE::BSEventNotifyControl::kContinue;
}

void Hooks::Install() {
    // SampleVirtualFunctionHook::Install();
    // SampleCallHook::Install();
    // SampleEventSink::Install();
}
