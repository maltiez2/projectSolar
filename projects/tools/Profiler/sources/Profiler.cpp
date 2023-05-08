#include "Profiler.h"

namespace projectSolar
{
    Profiler& Profiler::Instance()
    {
        static Profiler instance;
        return instance;
    }
    Profiler::~Profiler()
    {
        endSession();
    }

    void Profiler::beginSession(std::string_view name, const std::string& filepath)
    {
        if (m_activeSession)
        {
            endSession();
        }
        m_activeSession = true;
        m_outputStream.open(filepath);
        writeHeader();
        m_sessionName = name;
        m_profileCount = 0;
    }
    void Profiler::endSession() noexcept
    {
        if (!m_activeSession)
        { 
            return;
        }
        m_activeSession = false;
        writeFooter();
        m_outputStream.close();
        m_profileCount = 0;
    }
    void Profiler::writeProfile(const ProfileResult& result)
    {
        std::lock_guard lock(m_lock);

        if (m_profileCount > 0)
        {
            m_outputStream << ",";
        }

        m_profileCount++;

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << "{";
        m_outputStream << R"("cat":"function",)";
        m_outputStream << R"("dur":)" << (result.end - result.start) << ',';
        m_outputStream << R"("name":")" << name << R"(",)";
        m_outputStream << R"("ph":"X",)";
        m_outputStream << R"("pid":0,)";
        m_outputStream << R"("tid":)" << result.threadID << ",";
        m_outputStream << R"("ts":)" << result.start;
        m_outputStream << "}";
    }
    void Profiler::writeHeader()
    {
        m_outputStream << R"({"otherData": {},"traceEvents":[)";
    }
    void Profiler::writeFooter()
    {
        m_outputStream << "]}";
    }



    ProfilerTimer::ProfilerTimer(const std::string& name) :
        m_result({ name, 0, 0, std::thread::id() })
    {
        m_startTimepoint = std::chrono::steady_clock::now();
    }
    ProfilerTimer::~ProfilerTimer()
    {
        if (!m_stopped)
        {
            stop();
        }
    }

    void ProfilerTimer::stop() noexcept
    {
        auto endTimepoint = std::chrono::steady_clock::now();

        m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        m_result.end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        m_result.threadID = std::this_thread::get_id();
        Profiler::Instance().writeProfile(m_result);

        m_stopped = true;
    }
}
