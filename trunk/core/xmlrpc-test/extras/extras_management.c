#include <stdlib.h>
#include <stdio.h>

#include <extras/extras_management.h>

int
send_dtmf(char * const character, 
          int    const callId) {
          
    // do something
    
    printf("char: %s\n", character);
    printf("callId: %d\n", callId);
        
    return 1;
}
