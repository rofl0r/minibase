#ifndef __BITS_SYSCALL_H__
#define __BITS_SYSCALL_H__

#define NR_restart_syscall          0
#define NR_exit                     1
#define NR_fork                     2
#define NR_read                     3
#define NR_write                    4
#define NR_open                     5
#define NR_close                    6
#define NR_waitpid                  7
#define NR_creat                    8
#define NR_link                     9
#define NR_unlink                  10
#define NR_execve                  11
#define NR_chdir                   12
#define NR_time                    13
#define NR_mknod                   14
#define NR_chmod                   15
#define NR_lchown                  16
#define NR_break                   17
#define NR_oldstat                 18
#define NR_lseek                   19
#define NR_getpid                  20
#define NR_mount                   21
#define NR_umount                  22
#define NR_setuid                  23
#define NR_getuid                  24
#define NR_stime                   25
#define NR_ptrace                  26
#define NR_alarm                   27
#define NR_oldfstat                28
#define NR_pause                   29
#define NR_utime                   30
#define NR_stty                    31
#define NR_gtty                    32
#define NR_access                  33
#define NR_nice                    34
#define NR_ftime                   35
#define NR_sync                    36
#define NR_kill                    37
#define NR_rename                  38
#define NR_mkdir                   39
#define NR_rmdir                   40
#define NR_dup                     41
#define NR_pipe                    42
#define NR_times                   43
#define NR_prof                    44
#define NR_brk                     45
#define NR_setgid                  46
#define NR_getgid                  47
#define NR_signal                  48
#define NR_geteuid                 49
#define NR_getegid                 50
#define NR_acct                    51
#define NR_umount2                 52
#define NR_lock                    53
#define NR_ioctl                   54
#define NR_fcntl                   55
#define NR_mpx                     56
#define NR_setpgid                 57
#define NR_ulimit                  58
#define NR_oldolduname             59
#define NR_umask                   60
#define NR_chroot                  61
#define NR_ustat                   62
#define NR_dup2                    63
#define NR_getppid                 64
#define NR_getpgrp                 65
#define NR_setsid                  66
#define NR_sigaction               67
#define NR_sgetmask                68
#define NR_ssetmask                69
#define NR_setreuid                70
#define NR_setregid                71
#define NR_sigsuspend              72
#define NR_sigpending              73
#define NR_sethostname             74
#define NR_setrlimit               75
#define NR_getrlimit               76
#define NR_getrusage               77
#define NR_gettimeofday            78
#define NR_settimeofday            79
#define NR_getgroups               80
#define NR_setgroups               81
#define NR_select                  82
#define NR_symlink                 83
#define NR_oldlstat                84
#define NR_readlink                85
#define NR_uselib                  86
#define NR_swapon                  87
#define NR_reboot                  88
#define NR_readdir                 89
#define NR_mmap                    90
#define NR_munmap                  91
#define NR_truncate                92
#define NR_ftruncate               93
#define NR_fchmod                  94
#define NR_fchown                  95
#define NR_getpriority             96
#define NR_setpriority             97
#define NR_profil                  98
#define NR_statfs                  99
#define NR_fstatfs                100
#define NR_ioperm                 101
#define NR_socketcall             102
#define NR_syslog                 103
#define NR_setitimer              104
#define NR_getitimer              105
#define NR_stat                   106
#define NR_lstat                  107
#define NR_fstat                  108
#define NR_olduname               109
#define NR_iopl                   110
#define NR_vhangup                111
#define NR_idle                   112
#define NR_vm86                   113
#define NR_wait4                  114
#define NR_swapoff                115
#define NR_sysinfo                116
#define NR_ipc                    117
#define NR_fsync                  118
#define NR_sigreturn              119
#define NR_clone                  120
#define NR_setdomainname          121
#define NR_uname                  122
#define NR_modify_ldt             123
#define NR_adjtimex               124
#define NR_mprotect               125
#define NR_sigprocmask            126
#define NR_create_module          127
#define NR_init_module            128
#define NR_delete_module          129
#define NR_get_kernel_syms        130
#define NR_quotactl               131
#define NR_getpgid                132
#define NR_fchdir                 133
#define NR_bdflush                134
#define NR_sysfs                  135
#define NR_personality            136
#define NR_afs_syscall            137
#define NR_setfsuid               138
#define NR_setfsgid               139
#define NR__llseek                140
#define NR_getdents               141
#define NR__newselect             142
#define NR_flock                  143
#define NR_msync                  144
#define NR_readv                  145
#define NR_writev                 146
#define NR_getsid                 147
#define NR_fdatasync              148
#define NR__sysctl                149
#define NR_mlock                  150
#define NR_munlock                151
#define NR_mlockall               152
#define NR_munlockall             153
#define NR_sched_setparam         154
#define NR_sched_getparam         155
#define NR_sched_setscheduler     156
#define NR_sched_getscheduler     157
#define NR_sched_yield            158
#define NR_sched_get_priority_max 159
#define NR_sched_get_priority_min 160
#define NR_sched_rr_get_interval  161
#define NR_nanosleep              162
#define NR_mremap                 163
#define NR_setresuid              164
#define NR_getresuid              165
#define NR_query_module           166
#define NR_poll                   167
#define NR_nfsservctl             168
#define NR_setresgid              169
#define NR_getresgid              170
#define NR_prctl                  171
#define NR_rt_sigreturn           172
#define NR_rt_sigaction           173
#define NR_rt_sigprocmask         174
#define NR_rt_sigpending          175
#define NR_rt_sigtimedwait        176
#define NR_rt_sigqueueinfo        177
#define NR_rt_sigsuspend          178
#define NR_pread64                179
#define NR_pwrite64               180
#define NR_chown                  181
#define NR_getcwd                 182
#define NR_capget                 183
#define NR_capset                 184
#define NR_sigaltstack            185
#define NR_sendfile               186
#define NR_getpmsg                187
#define NR_putpmsg                188
#define NR_vfork                  189
#define NR_ugetrlimit             190
#define NR_readahead              191
#define NR_mmap2                  192
#define NR_truncate64             193
#define NR_ftruncate64            194
#define NR_stat64                 195
#define NR_lstat64                196
#define NR_fstat64                197
#define NR_pciconfig_read         198
#define NR_pciconfig_write        199
#define NR_pciconfig_iobase       200
#define NR_multiplexer            201
#define NR_getdents64             202
#define NR_pivot_root             203
#define NR_fcntl64                204
#define NR_madvise                205
#define NR_mincore                206
#define NR_gettid                 207
#define NR_tkill                  208
#define NR_setxattr               209
#define NR_lsetxattr              210
#define NR_fsetxattr              211
#define NR_getxattr               212
#define NR_lgetxattr              213
#define NR_fgetxattr              214
#define NR_listxattr              215
#define NR_llistxattr             216
#define NR_flistxattr             217
#define NR_removexattr            218
#define NR_lremovexattr           219
#define NR_fremovexattr           220
#define NR_futex                  221
#define NR_sched_setaffinity      222
#define NR_sched_getaffinity      223
#define NR_tuxcall                225
#define NR_sendfile64             226
#define NR_io_setup               227
#define NR_io_destroy             228
#define NR_io_getevents           229
#define NR_io_submit              230
#define NR_io_cancel              231
#define NR_set_tid_address        232
#define NR_fadvise64              233
#define NR_exit_group             234
#define NR_lookup_dcookie         235
#define NR_epoll_create           236
#define NR_epoll_ctl              237
#define NR_epoll_wait             238
#define NR_remap_file_pages       239
#define NR_timer_create           240
#define NR_timer_settime          241
#define NR_timer_gettime          242
#define NR_timer_getoverrun       243
#define NR_timer_delete           244
#define NR_clock_settime          245
#define NR_clock_gettime          246
#define NR_clock_getres           247
#define NR_clock_nanosleep        248
#define NR_swapcontext            249
#define NR_tgkill                 250
#define NR_utimes                 251
#define NR_statfs64               252
#define NR_fstatfs64              253
#define NR_fadvise64_64           254
#define NR_rtas                   255
#define NR_sys_debug_setcontext   256
#define NR_migrate_pages          258
#define NR_mbind                  259
#define NR_get_mempolicy          260
#define NR_set_mempolicy          261
#define NR_mq_open                262
#define NR_mq_unlink              263
#define NR_mq_timedsend           264
#define NR_mq_timedreceive        265
#define NR_mq_notify              266
#define NR_mq_getsetattr          267
#define NR_kexec_load             268
#define NR_add_key                269
#define NR_request_key            270
#define NR_keyctl                 271
#define NR_waitid                 272
#define NR_ioprio_set             273
#define NR_ioprio_get             274
#define NR_inotify_init           275
#define NR_inotify_add_watch      276
#define NR_inotify_rm_watch       277
#define NR_spu_run                278
#define NR_spu_create             279
#define NR_pselect6               280
#define NR_ppoll                  281
#define NR_unshare                282
#define NR_splice                 283
#define NR_tee                    284
#define NR_vmsplice               285
#define NR_openat                 286
#define NR_mkdirat                287
#define NR_mknodat                288
#define NR_fchownat               289
#define NR_futimesat              290
#define NR_fstatat64              291
#define NR_unlinkat               292
#define NR_renameat               293
#define NR_linkat                 294
#define NR_symlinkat              295
#define NR_readlinkat             296
#define NR_fchmodat               297
#define NR_faccessat              298
#define NR_get_robust_list        299
#define NR_set_robust_list        300
#define NR_move_pages             301
#define NR_getcpu                 302
#define NR_epoll_pwait            303
#define NR_utimensat              304
#define NR_signalfd               305
#define NR_timerfd_create         306
#define NR_eventfd                307
#define NR_sync_file_range2       308
#define NR_fallocate              309
#define NR_subpage_prot           310
#define NR_timerfd_settime        311
#define NR_timerfd_gettime        312
#define NR_signalfd4              313
#define NR_eventfd2               314
#define NR_epoll_create1          315
#define NR_dup3                   316
#define NR_pipe2                  317
#define NR_inotify_init1          318
#define NR_perf_event_open        319
#define NR_preadv                 320
#define NR_pwritev                321
#define NR_rt_tgsigqueueinfo      322
#define NR_fanotify_init          323
#define NR_fanotify_mark          324
#define NR_prlimit64              325
#define NR_socket                 326
#define NR_bind                   327
#define NR_connect                328
#define NR_listen                 329
#define NR_accept                 330
#define NR_getsockname            331
#define NR_getpeername            332
#define NR_socketpair             333
#define NR_send                   334
#define NR_sendto                 335
#define NR_recv                   336
#define NR_recvfrom               337
#define NR_shutdown               338
#define NR_setsockopt             339
#define NR_getsockopt             340
#define NR_sendmsg                341
#define NR_recvmsg                342
#define NR_recvmmsg               343
#define NR_accept4                344
#define NR_name_to_handle_at      345
#define NR_open_by_handle_at      346
#define NR_clock_adjtime          347
#define NR_syncfs                 348
#define NR_sendmmsg               349
#define NR_setns                  350
#define NR_process_vm_readv       351
#define NR_process_vm_writev      352
#define NR_finit_module           353
#define NR_kcmp                   354
#define NR_sched_setattr          355
#define NR_sched_getattr          356
#define NR_renameat2              357
#define NR_seccomp                358
#define NR_getrandom              359
#define NR_memfd_create           360
#define NR_bpf                    361
#define NR_execveat               362
#define NR_switch_endian          363
#define NR_userfaultfd            364
#define NR_membarrier             365
#define NR_mlock2                 378
#define NR_copy_file_range        379
#define NR_preadv2                380
#define NR_pwritev2               381
#define NR_kexec_file_load        382
#define NR_statx                  383
#define NR_pkey_alloc             384
#define NR_pkey_free              385
#define NR_pkey_mprotect          386
#define NR_rseq                   387
#define NR_io_pgetevents          388
#define NR_semget                 393
#define NR_semctl                 394
#define NR_shmget                 395
#define NR_shmctl                 396
#define NR_shmat                  397
#define NR_shmdt                  398
#define NR_msgget                 399
#define NR_msgsnd                 400
#define NR_msgrcv                 401
#define NR_msgctl                 402
#define NR_clock_gettime64        403
#define NR_clock_settime64        404
#define NR_clock_adjtime64        405
#define NR_clock_getres_time64    406
#define NR_clock_nanosleep_time64 407
#define NR_timer_gettime64        408
#define NR_timer_settime64        409
#define NR_timerfd_gettime64      410
#define NR_timerfd_settime64      411
#define NR_utimensat_time64       412
#define NR_pselect6_time64        413
#define NR_ppoll_time64           414
#define NR_io_pgetevents_time64   416
#define NR_recvmmsg_time64        417
#define NR_mq_timedsend_time64    418
#define NR_mq_timedreceive_time64 419
#define NR_semtimedop_time64      420
#define NR_rt_sigtimedwait_time64 421
#define NR_futex_time64           422
#define NR_sched_rr_get_interval_time64 423
#define NR_pidfd_send_signal     424
#define NR_io_uring_setup        425
#define NR_io_uring_enter        426
#define NR_io_uring_register     427

#endif