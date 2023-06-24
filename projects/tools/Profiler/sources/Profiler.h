// Taken from https://pastebin.com/qw5Neq4U and changed a bit
// Authors of initial code:
//      https://gist.github.com/davechurchill
//      https://gist.github.com/TheCherno

#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

#ifdef PROFILING
#define PROFILE_SCOPE(name) projectSolar::ProfilerTimer timer##__LINE__(name)
#define PROFILE_FUNCTION()  PROFILE_SCOPE(__FUNCTION__)
#define PROFILE_BEGIN(sessionName) projectSolar::Profiler::Instance().beginSession(sessionName)
#define PROFILE_END() projectSolar::Profiler::Instance().endSession()
#define PROFILE_SCOPE_A(name) projectSolar::ProfilerTimer timer_A(name)
#define PROFILE_SCOPE_B(name) projectSolar::ProfilerTimer timer_B(name)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#define PROFILE_BEGIN(sessionName)
#define PROFILE_END()
#define PROFILE_SCOPE_A(name)
#define PROFILE_SCOPE_B(name)
#endif

#ifndef PROFILING_FILE_PATH
    #define PROFILING_FILE_PATH "logs/profiling_results.json"
#endif


namespace projectSolar
{
    struct ProfileResult
    {
        const std::string   name;
        uint64_t            start;
        uint64_t            end;
        std::thread::id     threadID;
    };

    class Profiler
    {
    public:
        static Profiler& Instance();

        ~Profiler();

        void beginSession(std::string_view name, const std::string& filepath = PROFILING_FILE_PATH);
        void endSession() noexcept;
        void writeProfile(const ProfileResult& result);
        void writeHeader();
        void writeFooter();

    private:
        std::string     m_sessionName = "None";
        std::ofstream   m_outputStream;
        uint32_t        m_profileCount = 0;
        std::mutex      m_lock;
        bool            m_activeSession = false;

        Profiler() = default;
    };

    class ProfilerTimer
    {
    public:
        explicit ProfilerTimer(const std::string& name);

        ~ProfilerTimer();

        void stop() noexcept;

    private:
        ProfileResult m_result;

        std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
        bool m_stopped = false;
    };
}
