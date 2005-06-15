#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <volume/xmlrpc/dispatcher.h>
#include <volume/volume_management.h>

xmlrpc_value *
set_speaker_volume_RP(xmlrpc_env *   const env, 
                      xmlrpc_value * const param_array, 
                      void *         const server_context) {

    printf("entering setSpeakerVolume...\n");
    
    xmlrpc_double level;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(d)", &level);
    if (env->fault_occurred) {
        return NULL;
    }

    result = set_speaker_volume(level);
        
    printf("leaving setSpeakerVolume\n");
    return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *
get_speaker_volume_RP(xmlrpc_env *   const env, 
                      xmlrpc_value * const param_array, 
                      void *         const server_context) {

    printf("entering getSpeakerVolume...\n");
    
    xmlrpc_double result;
    
    result = get_speaker_volume();
    
    printf("leaving getSpeakerVolume\n");
    return xmlrpc_build_value(env, "d", result);
}

xmlrpc_value *
set_micro_volume_RP(xmlrpc_env *   const env, 
                    xmlrpc_value * const param_array, 
                    void *         const server_context) {

    printf("entering setMicroVolume...\n");
    
    xmlrpc_double level;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(d)", &level);
    if (env->fault_occurred) {
        return NULL;
    }

    result = set_micro_volume(level);
        
    printf("leaving setMicroVolume\n");
    return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *
get_micro_volume_RP(xmlrpc_env *   const env, 
                    xmlrpc_value * const param_array, 
                    void *         const server_context) {

    printf("entering getMicroVolume...\n");
    
    xmlrpc_double result;
    
    result = get_micro_volume();
    
    printf("leaving getMicroVolume\n");
    return xmlrpc_build_value(env, "d", result);
}
