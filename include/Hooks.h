#pragma once

namespace Hooks {
    void Install();

    struct SampleVirtualFunctionHook {
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    struct SampleCallHook {
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    class SampleEventSink : public RE::BSTEventSink<RE::TESActivateEvent> {
        RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event,
                                              RE::BSTEventSource<RE::TESActivateEvent>*);
    public:
        static void Install();
    };

}