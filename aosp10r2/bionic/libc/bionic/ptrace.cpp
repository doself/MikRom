/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdarg.h>
#include <unistd.h>
#include <sys/ptrace.h>


extern "C" long __ptrace(int req, pid_t pid, void* addr, void* data);

long ptrace(int req, ...) {
  bool is_peek = (req == PTRACE_PEEKUSR || req == PTRACE_PEEKTEXT || req == PTRACE_PEEKDATA);
  long peek_result;

  va_list args;
  va_start(args, req);
  pid_t pid = va_arg(args, pid_t);
  void* addr = va_arg(args, void*);
  void* data;
  if (is_peek) {
    data = &peek_result;
  } else {
    data = va_arg(args, void*);
  }
  va_end(args);

   ///ADD START
     int caller_uid=getuid();

     //int caller_pid=getpid();
     //caller_uid>10000说明是普通App调用
     if(caller_uid>10000)
     {

        if(req ==PTRACE_TRACEME)
        {
           //自己ptrace自己,直接返回成功
           return 0;
        }
      if(req==PTRACE_ATTACH)
      {
          int caller_ppid=getppid();
        if(caller_ppid==pid)
        {
           //如果是子进程ptrace父进程，直接返回成功
           return 0;
        }
            //TODO 也可以通过读取/proc/$pid/status获取进程的uid，如果uid和当前调用ptrace的uid一致，也直接返回成功

      }
     }
     ///ADD END


  long result = __ptrace(req, pid, addr, data);
  if (is_peek && result == 0) {
    return peek_result;
  }
  return result;
}
