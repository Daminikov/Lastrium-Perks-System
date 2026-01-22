#pragma once

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