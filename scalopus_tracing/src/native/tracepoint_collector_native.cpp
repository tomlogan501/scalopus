/*
  Copyright (c) 2018-2019, Ivor Wanders
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of the author nor the names of contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

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
#include "tracepoint_collector_native.h"

#include <iostream>
namespace scalopus
{
const uint8_t TracePointCollectorNative::ENTRY = 1;
const uint8_t TracePointCollectorNative::EXIT = 2;

TracePointCollectorNative& TracePointCollectorNative::getInstance()
{
  static TracePointCollectorNative instance;
  return instance;
}

tracepoint_collector_types::ScopeBufferPtr TracePointCollectorNative::getBuffer()
{
  auto tid = static_cast<unsigned long>(pthread_self());
  if (exists(tid))
  {
    // Buffer already existed for this thread.
    return getMap()[tid];
  }
  else
  {
    // Buffer did not exist for this thread, make a new one.
    auto buffer = std::make_shared<tracepoint_collector_types::ScopeBuffer>(
        tracepoint_collector_types::EventContainer{ ringbuffer_size_ });
    insert(tid, buffer);
    return buffer;
  }
  return nullptr;
}

void TracePointCollectorNative::setRingbufferSize(std::size_t size)
{
  ringbuffer_size_ = size;
}

}  // namespace scalopus
