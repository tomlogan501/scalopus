/*
  Copyright (c) 2019, Ivor Wanders
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "scalopus_lttng/native_trace_source.h"
#include "tracepoint_collector_native.h"
#include <sstream>

namespace scalopus
{
NativeTraceSource::NativeTraceSource(NativeTraceProvider::WeakPtr provider) : provider_(provider)
{
}

NativeTraceSource::~NativeTraceSource()
{
  stopInterval();
}

void NativeTraceSource::startInterval()
{
  stopInterval();
  in_interval_.store(true);
}

void NativeTraceSource::stopInterval()
{
  in_interval_.store(false);
}

void NativeTraceSource::work()
{
}

bool NativeTraceSource::isRecording() const
{
  return in_interval_.load();
}

std::vector<json> NativeTraceSource::finishInterval()
{
  std::vector<json> res;

  stopInterval();

  // Update mappings.
  auto provider = provider_.lock();
  if (provider != nullptr)
  {
    provider->updateMapping();
  }

  // Obtain the data chunks.
  std::vector<DataPtr> data;
  {
    std::lock_guard<decltype(data_mutex_)> lock(data_mutex_);
    recorded_data_.swap(data);
  }

  // Now, we start converting the chunks of data we obtain into trace events.
  for (const auto& dptr : data)
  {
    // First, we parse the bson we got and convert it to events.
    const json parsed = json::from_bson(*dptr);
    unsigned int pid{ 0 };
    parsed.at("pid").get_to(pid);
    tracepoint_collector_types::ThreadedEvents events;
    parsed.at("events").get_to(events);

    for (const auto& thread_events : events)
    {
      // Events are grouped by thread id.
      const auto& tid = thread_events.first;
      for (const auto& event : thread_events.second)
      {
        const auto& timestamp_ns_since_epoch = std::get<0>(event);
        const auto& trace_id = std::get<1>(event);
        const auto& type = std::get<2>(event);

        // Finally, we can create a trace type that can be used by devtools.
        json entry;
        entry["ts"] = static_cast<double>(timestamp_ns_since_epoch) / 1000.0;
        entry["cat"] = "PERF";
        entry["tid"] = tid;
        entry["pid"] = pid;
        entry["name"] = provider->getScopeName(pid, trace_id);
        switch (type)
        {
          case TracePointCollectorNative::ENTRY:
            entry["ph"] = "B";
            break;
          case TracePointCollectorNative::EXIT:
            entry["ph"] = "E";
            break;
          default:
            break;
        }
        res.push_back(entry);
      }
    }
  }

  return res;
}

void NativeTraceSource::addData(const DataPtr& incoming_data)
{
  std::lock_guard<decltype(data_mutex_)> lock(data_mutex_);
  recorded_data_.emplace_back(incoming_data);
}
}  // namespace scalopus
