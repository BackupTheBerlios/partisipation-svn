#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <volume_management.h>

xmlrpc_value *
set_speaker_volume(xmlrpc_env *   const env, 
                   xmlrpc_value * const param_array, 
                   void *         const server_context) {

    printf("entering setSpeakerVolume...\n");
    
    xmlrpc_double level;
    
    xmlrpc_decompose_value(env, param_array, "(d)", &level);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("level: %f\n", level);
        
    printf("leaving setSpeakerVolume\n");
    return xmlrpc_build_value(env, "b", 1);
}

xmlrpc_value *
get_speaker_volume(xmlrpc_env *   const env, 
                   xmlrpc_value * const param_array, 
                   void *         const server_context) {

    printf("entering getSpeakerVolume...\n");
    
    // do something
    
    printf("leaving getSpeakerVolume\n");
    return xmlrpc_build_value(env, "d", 0.5);
}

xmlrpc_value *
set_micro_volume(xmlrpc_env *   const env, 
                 xmlrpc_value * const param_array, 
                 void *         const server_context) {

    printf("entering setMicroVolume...\n");
    
    xmlrpc_double level;
    
    xmlrpc_decompose_value(env, param_array, "(d)", &level);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("level: %f\n", level);
        
    printf("leaving setMicroVolume\n");
    return xmlrpc_build_value(env, "b", 1);
}

xmlrpc_value *
get_micro_volume(xmlrpc_env *   const env, 
                 xmlrpc_value * const param_array, 
                 void *         const server_context) {

    printf("entering getMicroVolume...\n");
    
    // do something
    
    printf("leaving getMicroVolume\n");
    return xmlrpc_build_value(env, "d", 0.5);
}
