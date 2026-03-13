/*
 * Copyright The async-profiler authors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __s390x__

#include <errno.h>
#include <string.h>
#include "stackFrame.h"

#define REG(l)  _ucontext->uc_mcontext.gregs[l]

uintptr_t& StackFrame::pc() {
    return (uintptr_t&)_ucontext->uc_mcontext.psw.addr;
}

uintptr_t& StackFrame::sp() {
    return (uintptr_t&)REG(15);
}

uintptr_t& StackFrame::fp() {
    return *((uintptr_t*)REG(15));
}

uintptr_t& StackFrame::retval() {
    return (uintptr_t&)REG(2);
}

uintptr_t StackFrame::link() {
    return (uintptr_t)REG(14);
}

uintptr_t StackFrame::arg0() {
    return (uintptr_t)REG(2);
}

uintptr_t StackFrame::arg1() {
    return (uintptr_t)REG(3);
}

uintptr_t StackFrame::arg2() {
    return (uintptr_t)REG(4);
}

uintptr_t StackFrame::arg3() {
    return (uintptr_t)REG(5);
}

uintptr_t StackFrame::jarg0() {
    return arg0();
}

uintptr_t StackFrame::method() {
    return (uintptr_t)REG(9);
}

uintptr_t StackFrame::senderSP() {
    return (uintptr_t)REG(10);
}

void StackFrame::ret() {
    pc() = link();
}

bool StackFrame::unwindStub(instruction_t* entry, const char* name, uintptr_t& pc, uintptr_t& sp, uintptr_t& fp) {
    instruction_t* ip = (instruction_t*)pc;
    if (ip == entry
        || strncmp(name, "itable", 6) == 0
        || strncmp(name, "vtable", 6) == 0
        || strcmp(name, "InlineCacheBuffer") == 0)
    {
        pc = link();
        return true;
    }
    return false;
}

bool StackFrame::unwindCompiled(NMethod* nm, uintptr_t& pc, uintptr_t& sp, uintptr_t& fp) {
    // Not yet implemented
    return false;
}

bool StackFrame::unwindPrologue(NMethod* nm, uintptr_t& pc, uintptr_t& sp, uintptr_t& fp) {
    // Not yet implemented
    return false;
}

bool StackFrame::unwindEpilogue(NMethod* nm, uintptr_t& pc, uintptr_t& sp, uintptr_t& fp) {
    // Not yet implemented
    return false;
}

bool StackFrame::unwindAtomicStub(const void*& pc) {
    // Not needed
    return false;
}

void StackFrame::adjustSP(const void* entry, const void* pc, uintptr_t& sp) {
    // Not yet implemented
}

bool StackFrame::skipFaultInstruction() {
    unsigned char op = *((unsigned char*)pc());
    int len;
    // s390x instruction length encoding:
    // 00 -> 2 bytes, 01/10 -> 4 bytes, 11 -> 6 bytes
    if ((op & 0xc0) == 0x00) len = 2;
    else if ((op & 0xc0) == 0xc0) len = 6;
    else len = 4;

    pc() += len;
    return true;
}

bool StackFrame::checkInterruptedSyscall() {
    return retval() == (uintptr_t)-EINTR;
}

bool StackFrame::isSyscall(instruction_t* pc) {
    // SVC opcode is 0x0A. Instruction is 2 bytes.
    return (*((unsigned char*)pc) == 0x0a);
}

#endif // __s390x__
