#ifndef _AROS_TYPES_SIGINFO_T_H
#define _AROS_TYPES_SIGINFO_T_H

/*
    Copyright © 2010-2012, The AROS Development Team. All rights reserved.
    $Id: /aros/branches/ABI_V1/trunk-aroscsplit/AROS/compiler/arosnixc/include/aros/types/siginfo_t.h 35142 2010-10-23T20:40:12.589298Z verhaegs  $

    Desc: POSIX.1-2008 siginfo_t type definition
*/

#include <aros/types/pid_t.h>
#include <aros/types/uid_t.h>
#include <aros/types/sigevent_s.h>

/* Reasons why a signal was generated */

/* For SIGILL */
#define ILL_ILLOPC	1	    /* illegal opcode */
#define ILL_ILLOPN	2	    /* illegal operand */
#define ILL_ILLADR	3	    /* illegal address mode */
#define ILL_ILLTRP	4	    /* illegal trap */
#define ILL_PRVOPC	5	    /* priviledged opcode */
#define ILL_PRVREG	6	    /* priviledged register */
#define ILL_COPROC	7	    /* coprocessor error */
#define ILL_BADSTK	8	    /* internal stack error */

/* For SIGFPE */
#define FPE_INTDIV	1	    /* integer divide by zero */
#define FPE_INTOVF	2	    /* integer overflow */
#define FPE_FLTDIV	3	    /* floating point divide by zero */
#define FPE_FLTOVF	4	    /* floating point overflow */
#define FPE_FLTUND	5	    /* floating point underflow */
#define FPE_FLTRES	6	    /* floating point inexact result */
#define FPE_FLTINV	7	    /* invalid floating point operation */
#define FPE_FLTSUB	8	    /* subscript out of range */

/* For SIGSEGV */
#define SEGV_MAPERR	1	    /* address not mapped to object */
#define SEGV_ACCERR	2	    /* invalid permissions for object */

/* For SIGBUS */
#define BUS_ADRALN	1	    /* Address alignment */
#define BUS_ADRERR	2	    /* Bus address error */
#define BUS_OBJERR	3	    /* object specific hardware error */

/* For SIGTRAP */
#define TRAP_BRKPT	1	    /* Process breakpoint */
#define TRAP_TRACE	2	    /* Process trace trap */

/* For SIGCHLD */
#define CLD_EXITED	1	    /* Child has exited */
#define CLD_KILLED	2	    /* Child terminated abnormally */
#define CLD_DUMPED	3	    /* Child dumped core */
#define CLD_TRAPPED	4	    /* Traced child has trapped */
#define CLD_STOPPED	5	    /* Traced child has stopped */
#define CLD_CONTINUED	6	    /* Traced child has continued */

/* For SIGPOLL */
#define POLL_IN		1	    /* data input available */
#define POLL_OUT	2	    /* data output available */
#define POLL_MSG	3	    /* input message available */
#define POLL_ERR	4	    /* I/O error */
#define POLL_PRI	5	    /* high priority input available */
#define POLL_HUP	6	    /* device disconnected */

/* Others */
#define SI_USER		0x10001	    /* Signal sent by kill() */
#define SI_QUEUE	0x10002	    /* Signal sent by sigqueue() */
#define SI_TIMER	0x10003	    /* Signal sent by timer */
#define SI_ASYNCIO	0x10004	    /* Signal sent by async I/O */
#define SI_MESGQ	0x10005	    /* Signal generated by message queue */

/* NOTIMPL void *si_addr */
/* NOTIMPL pid_t si_pid */
/* NOTIMPL int si_status */
/* NOTIMPL uid_t si_uid */
/* NOTIMPL long si_band */

/*
    siginfo_t is delivered to sigaction() style signal handlers.
    It's part of the POSIX Realtime Extension
*/
/* ABI_V0 compatibility */
typedef struct
{
    int		    si_signo;	    /* signal number */
    int		    si_errno;	    /* errno value */
    int		    si_code;	    /* signal code */
    pid_t	    si_pid;	    /* sending process ID */
    uid_t           si_uid;	    /* user ID of sending process XXX */
    void *	    si_addr;	    /* address of faulting instruction */
    int		    si_status;	    /* exit value or signal */
    long	    si_band;	    /* band event for SIGPOLL */
    union sigval    si_value;	    /* signal value */
} siginfo_t;

#endif /* _AROS_TYPES_SIGINFO_T_H */
