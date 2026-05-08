#pragma once

namespace mlibc::svr4 {

namespace access {
constexpr int effective_ids = 010;
} // namespace access

namespace clocal {
constexpr int debugcon_write = 1;
} // namespace clocal

namespace hrtsys {

namespace opcode {
constexpr int cntl = 0;
constexpr int alarm = 1;
constexpr int sleep = 2;
} // namespace opcode

namespace command {
constexpr int get_res = 0;
constexpr int tofd = 1;
constexpr int start_it = 2;
constexpr int get_it = 3;
constexpr int int_sleep = 10;
constexpr int bsd = 12;
constexpr int bsd_pend = 13;
constexpr int rbsd = 14;
constexpr int bsd_rep = 15;
constexpr int bsd_cancel = 16;
} // namespace command

namespace flag {
constexpr int done = 0x0001;
constexpr int error = 0x0002;
} // namespace flag

namespace clock {
constexpr int standard = 0x0001;
constexpr int user_virtual = 0x0002;
constexpr int process_virtual = 0x0004;
} // namespace clock

} // namespace hrtsys

namespace msgsys {
constexpr int msgget = 0;
constexpr int msgctl = 1;
constexpr int msgrcv = 2;
constexpr int msgsnd = 3;
} // namespace msgsys

namespace pgrpsys {
constexpr int get_sid = 2;
constexpr int set_sid = 3;
constexpr int get_pgid = 4;
constexpr int set_pgid = 5;
} // namespace pgrpsys

namespace rlimit {
constexpr unsigned long infinity = 0x7fffffffUL;
} // namespace rlimit

namespace semsys {
constexpr int semctl = 0;
constexpr int semget = 1;
constexpr int semop = 2;
} // namespace semsys

namespace shmsys {
constexpr int shmat = 0;
constexpr int shmctl = 1;
constexpr int shmdt = 2;
constexpr int shmget = 3;
} // namespace shmsys

namespace sigpending {
constexpr int query = 1;
} // namespace sigpending

namespace sysi86 {
constexpr int dscr = 75;
} // namespace sysi86

namespace systeminfo {
constexpr int hostname = 2;
} // namespace systeminfo

namespace time {
constexpr long microseconds_per_second = 1000000L;
constexpr unsigned long nanoseconds_per_second = 1000000000UL;
constexpr long clock_ticks_per_second = 100;
} // namespace time

namespace tls {
constexpr unsigned int user_data_acc1 = 0xF2;
constexpr unsigned int data_acc2 = 0xC;
constexpr unsigned int first_selector_index = 7;
constexpr unsigned int max_ldt_index = 8192;
} // namespace tls

} // namespace mlibc::svr4
