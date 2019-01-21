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
#ifndef SCALOPUS_SCOPE_INTERNAL_THREAD_NAMING_H
#define SCALOPUS_SCOPE_INTERNAL_THREAD_NAMING_H

#include "scalopus_general/internal/helper_macros.h"
#include "scalopus_general/internal/thread_name_tracker.h"

// Macro to register the thread name once.
#define TRACE_THREAD_NAME_ONCE(name, have_done_once_varname)                                                           \
  static thread_local bool have_done_once_varname = false;                                                             \
  if (!have_done_once_varname)                                                                                         \
  {                                                                                                                    \
    have_done_once_varname = true;                                                                                     \
    scalopus::ThreadNameTracker::getInstance().setCurrentName(name);                                                   \
  }                                                                                                                    \
  do                                                                                                                   \
  {                                                                                                                    \
  } while (0)

#endif  // SCALOPUS_SCOPE_INTERNAL_THREAD_NAMING_H