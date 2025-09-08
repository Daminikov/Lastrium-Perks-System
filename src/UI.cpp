#include "UI.h"
#include "SKSEMenuFramework.h"
#include "Configuration.h"

void UI::Register() {

    if (!QTRLib::SKSEMenuFramework::IsInstalled()) {
        return;
    }

    QTRLib::SKSEMenuFramework::SetSection(BEAUTIFUL_NAME);
    QTRLib::SKSEMenuFramework::AddSectionItem("Main", Example1::Render);
}


void __stdcall UI::Example1::Render() {
    if (QTRLib::ImGui::Button("Button")) {
        logger::trace("Button pressed");
    }
}