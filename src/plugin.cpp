#include "Logger.h"
#include "Hooks.h"
#include "ResurrectionAPI.h"
#include <keyhandler.h>
#include "PrismaUI_API.h"
#include <nlohmann/json.hpp>


using JSON = nlohmann::json;
using namespace RE;
using namespace SKSE;
using namespace logger;

const char* plugin_name = "LastriumPerks.esp";
auto menuName1 = "StatsMenu";

// Declare the global PrismaUI API variable
PRISMA_UI_API::IVPrismaUI1* PrismaUI = nullptr;
static PrismaView view;


// Variables of the "Escape from Death" perk
const int EscapeFromDeath_Mgef = 0x811;
const int EscapeFromDeath_Perk = 0x80F;
const int EscapeFromDeath_Spel = 0x810;
const int EscapeFromDeath_Mesg = 0x80C;


// Variables of the "System critical hits"
const int TEST_Bladesman30 = 0x818; 
const int LAST_TestCrit_Mesg = 0x819;

// Registration of functions
void cast_spell(RE::Actor* victim, RE::Actor* attacker, RE::SpellItem* spell);
void debug_notification(RE::BGSMessage* msg);
void addSubscriber();
void InitializeUI();
void SendComplexData();
static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message);

/* class OurEventSink : public RE::BSTEventSink<RE::TESHitEvent>,
                     public RE::BSTEventSink<RE::TESActivateEvent>,
                     public RE::BSTEventSink<SKSE::CrosshairRefEvent>,
                     public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
                     public RE::BSTEventSink<RE::InputEvent*> {
    OurEventSink() = default;
    OurEventSink(const OurEventSink&) = delete;
    OurEventSink(OurEventSink&&) = delete;
    OurEventSink& operator=(const OurEventSink&) = delete;
    OurEventSink& operator=(OurEventSink&&) = delete;

public:
    static OurEventSink* GetSingleton() {
        static OurEventSink singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event,
                                          RE::BSTEventSource<RE::TESHitEvent>*) {
        auto targetName = event->target->GetBaseObject()->GetName();
        auto sourceName = event->cause->GetBaseObject()->GetName();
        logger::info("{} hit {}", sourceName, targetName);
        if (event->flags.any(RE::TESHitEvent::Flag::kPowerAttack))
            logger::info("Ooooo power attack!");
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event,
                                          RE::BSTEventSource<RE::TESActivateEvent>*) {
        auto activatedName = event->objectActivated->GetBaseObject()->GetName();
        auto activatorName = event->actionRef->GetBaseObject()->GetName();
        logger::info("{} activated {}", activatorName, activatedName);
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(const SKSE::CrosshairRefEvent* event,
                                          RE::BSTEventSource<SKSE::CrosshairRefEvent>*) {
        if (event->crosshairRef) {
            logger::info("Crosshair is over {}", event->crosshairRef->GetBaseObject()->GetName());
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
                                          RE::BSTEventSource<RE::MenuOpenCloseEvent>*) {
                     
        if (event->opening)
            logger::info("OPEN MENU {}", event->menuName);

        else
            logger::info("CLOSE MENU {}", event->menuName);
        return RE::BSEventNotifyControl::kContinue;
        
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr,
                                          RE::BSTEventSource<RE::InputEvent*>*) {
        if (!eventPtr) return RE::BSEventNotifyControl::kContinue;

        auto* event = *eventPtr;
        if (!event) return RE::BSEventNotifyControl::kContinue;

        if (event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton) {
            auto* buttonEvent = event->AsButtonEvent();
            auto dxScanCode = buttonEvent->GetIDCode();
//            logger::info("Pressed key {}", dxScanCode);
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}; */



// The work of the "Escape from Death" perk
class PerkResurrection : public ResurrectionAPI {
    
    bool should_resurrect(RE::Actor* a) const override {
        static auto perk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(EscapeFromDeath_Perk, plugin_name);
        static auto mgef = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(EscapeFromDeath_Mgef, plugin_name);
        return a->HasPerk(perk) && !a->AsMagicTarget()->HasMagicEffect(mgef);
    }

    void resurrect(RE::Actor* a) override {
        a->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, 10000);
        static auto spel = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(EscapeFromDeath_Spel, plugin_name);
        cast_spell(a, a, spel);
        static auto mesg = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSMessage>(EscapeFromDeath_Mesg, plugin_name);
        debug_notification(mesg);
    }
};

// Start SKSE
SKSEPluginLoad(const SKSE::LoadInterface *skse) {


    SKSE::Init(skse);
    SetupLog();
    Hooks::Install();


    auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(skse->QueryInterface(SKSE::LoadInterface::kMessaging));
    if (!g_messaging) {
        logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");

        return false;
    }
    g_messaging->RegisterListener("SKSE", SKSEMessageHandler);
 //   auto* eventSink = OurEventSink::GetSingleton();

    // ScriptSource
    auto* eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
 //   eventSourceHolder->AddEventSink<RE::TESHitEvent>(eventSink);
 //   eventSourceHolder->AddEventSink<RE::TESActivateEvent>(eventSink);

    // SKSE
//    SKSE::GetCrosshairRefEventSource()->AddEventSink(eventSink);

    // UI
 //   RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(eventSink);

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", SKSEMessageHandler)) {
        SKSE::log::error("Failed to register message listener");
        return false;
    }

    info("Loading OK!!!");
    return true;
    
}

// Actions when game Messaging Event
static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
            //        case SKSE::MessagingInterface::kPostLoad:
            //        break;
            //        case SKSE::MessagingInterface::kPostPostLoad:
            //        break;
            //        case SKSE::MessagingInterface::kPreLoadGame:
            //        break;
            //        case SKSE::MessagingInterface::kPostLoadGame:
            //        break;
            //        case SKSE::MessagingInterface::kSaveGame:
            //        break;
            //        case SKSE::MessagingInterface::kDeleteGame:
            //        break;
        case SKSE::MessagingInterface::kInputLoaded:
 //           RE::BSInputDeviceManager::GetSingleton()->AddEventSink(OurEventSink::GetSingleton());
            break;
            //      case SKSE::MessagingInterface::kNewGame:
            //      break;
        case SKSE::MessagingInterface::kDataLoaded:
            PrismaUI = static_cast<PRISMA_UI_API::IVPrismaUI1*>(
                PRISMA_UI_API::RequestPluginAPI(PRISMA_UI_API::InterfaceVersion::V1));

            if (!PrismaUI) {
                SKSE::log::error("Failed to initialize PrismaUI API");
                return;
            }

            SKSE::log::info("PrismaUI API initialized successfully");

            InitializeUI();
            addSubscriber();
            break;
    }
}


// Cast spell
void cast_spell(RE::Actor* victim, RE::Actor* attacker, RE::SpellItem* spell) {
    RE::MagicCaster* caster = attacker->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
    if (caster && spell) {
        caster->CastSpellImmediate(spell, false, victim, 1.0f, false, 0.0f, attacker);
    }
}

// PrismaUI
void InitializeUI() {
    // Create a view with DOM ready callback
    view = PrismaUI->CreateView("Lastrium Perks/index.html", [](PrismaView view) -> void {
        
        SKSE::log::info("View DOM is ready {}", view);
    });

    // Next lines is custom KEY DOWN / KEY UP realisation which bases at "src/keyhandler".
    KeyHandler::RegisterSink();
    KeyHandler* keyHandler = KeyHandler::GetSingleton();
    const uint32_t TOGGLE_FOCUS_KEY = 0x3D;  // F3 key

    // Press F3 to focus/unfocus view in-game.
    KeyHandlerEvent toggleEventHandler = keyHandler->Register(TOGGLE_FOCUS_KEY, KeyEventType::KEY_DOWN, []() {
        auto hasFocus = PrismaUI->HasFocus(view);

        if (!hasFocus) {
            // Focus
            if (PrismaUI->Focus(view)) {
                PrismaUI->Show(view);
            }
        } else {
            // Unfocus
            PrismaUI->Unfocus(view);
            PrismaUI->Hide(view);
    
        }
    });


    // If you want to unregister the key event handlers:
    // keyHandler->Unregister(toggleEventHandler);
}

// PrismaUI Send JSON
void SendComplexData() { 

}

// Debug
void debug_notification(RE::BGSMessage* msg) {
    RE::BSString a;
    msg->GetDescription(a, msg);
    RE::DebugNotification(a.c_str());
}

// Add a listener
void addSubscriber() {
//    info("addSubscriber!!!");
    if (auto pluginHandle = REX::W32::GetModuleHandleA("ResurrectionAPI.dll")) {
        if (auto AddSubscriber = (AddSubscriber_t)GetProcAddress(pluginHandle, "ResurrectionAPI_AddSubscriber")) {
            AddSubscriber(std::make_unique<PerkResurrection>());
        }
    }
}