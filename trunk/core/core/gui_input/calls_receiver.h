#ifndef HCALLS_RECEIVER_USED
#define HCALLS_RECEIVER_USED

int gi_make_call(int const accountId, char *const callee);

int gi_end_call(int const callId);

int gi_accept_call(int const callId);

#endif
