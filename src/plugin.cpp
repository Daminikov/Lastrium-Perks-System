#include "Logger.h"
#include "UI.h"
#include "Hooks.h"
#include "QTRLib.h"
#include "ResurrectionAPI.h"

using namespace RE;
using namespace SKSE;
using namespace logger;

const char* plugin_name = "LastriumPerks.esp";


// Переменные перка "Бегство от смерти"
const int EscapeFromDeath_Mgef = 0x811;
const int EscapeFromDeath_Perk = 0x80F;
const int EscapeFromDeath_Spel = 0x810;
const int EscapeFromDeath_Mesg = 0x80C;

// Регистрация функций
void cast_spell(RE::Actor* victim, RE::Actor* attacker, RE::SpellItem* spell);
void debug_notification(RE::BGSMessage* msg);
void OnMessage(SKSE::MessagingInterface::Message* message);
void addSubscriber();
static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message);

// Работа перка "Бегство от смерти"
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

// Старт SKSE
SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SetupLog();
    UI_::Register();
    Hooks::Install();
//    QTRLib::Install();


    auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(skse->QueryInterface(SKSE::LoadInterface::kMessaging));
    if (!g_messaging) {
        logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");

        return false;
    }
    g_messaging->RegisterListener("SKSE", SKSEMessageHandler);
    info("Loading OK!!!");
    return true;
    
}

// Действия при загрузке игры
static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
 //           info("kDataLoaded OK!!!");
            addSubscriber();
            break;
    }
}

// OnMessage
void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoad) {
    }
}

// Каст спелла
void cast_spell(RE::Actor* victim, RE::Actor* attacker, RE::SpellItem* spell) {
    RE::MagicCaster* caster = attacker->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
    if (caster && spell) {
        caster->CastSpellImmediate(spell, false, victim, 1.0f, false, 0.0f, attacker);
    }
}

// Дебаг
void debug_notification(RE::BGSMessage* msg) {
    RE::BSString a;
    msg->GetDescription(a, msg);
    RE::DebugNotification(a.c_str());
}

// Добавить слушателя
void addSubscriber() {
//    info("addSubscriber!!!");
    if (auto pluginHandle = REX::W32::GetModuleHandleA("ResurrectionAPI.dll")) {
        if (auto AddSubscriber = (AddSubscriber_t)GetProcAddress(pluginHandle, "ResurrectionAPI_AddSubscriber")) {
            AddSubscriber(std::make_unique<PerkResurrection>());
        }
    }
}