#pragma once

// ------------------------------------------------------------------------------
//  Standard library FIRST.
//  CommonLibSSE-NG v8 generates __<Target>Plugin.cpp containing only
//  #include "REL/Relocation.h" and #include "SKSE/SKSE.h", while REL/Version.h
//  uses std::uint16_t / std::array / std::less without including anything itself.
//  This PCH is what feeds them (same fix as in SKSE-Template BEST+).
// ------------------------------------------------------------------------------
#include <algorithm>
#include <array>
#include <chrono>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#pragma warning(push)
#include <spdlog/sinks/basic_file_sink.h>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <REL/Relocation.h>
#include <fstream>

#ifdef NDEBUG
    #include <spdlog/sinks/basic_file_sink.h>
#else
    #include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

namespace logger = SKSE::log;
using namespace std::literals;
using namespace std;

using FormID = RE::FormID;
using RefID = RE::FormID;

const RefID player_refid = 20;



namespace util {
    using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#define RELOCATION_OFFSET(SE, AE) REL::VariantOffset(SE, AE, 0).offset()