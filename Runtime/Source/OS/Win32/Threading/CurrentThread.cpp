#include "Threading/CurrentThread.hpp"

#include "Win32Api/Threads.hpp"

void CurrentThread::Yield() {
  Win32::CurrentThread::Yield();
}

void CurrentThread::StupidSleep(uint milliseconds) {
  Win32::CurrentThread::StupidSleep(milliseconds);
}
