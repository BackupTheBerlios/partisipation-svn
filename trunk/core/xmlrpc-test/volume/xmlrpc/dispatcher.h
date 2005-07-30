#include <xmlrpc.h>

xmlrpc_value *set_speaker_volume_RP(xmlrpc_env * const env,
                                    xmlrpc_value * const param_array,
                                    void *const server_context);

xmlrpc_value *get_speaker_volume_RP(xmlrpc_env * const env,
                                    xmlrpc_value * const param_array,
                                    void *const server_context);

xmlrpc_value *set_micro_volume_RP(xmlrpc_env * const env,
                                  xmlrpc_value * const param_array,
                                  void *const server_context);

xmlrpc_value *get_micro_volume_RP(xmlrpc_env * const env,
                                  xmlrpc_value * const param_array,
                                  void *const server_context);
