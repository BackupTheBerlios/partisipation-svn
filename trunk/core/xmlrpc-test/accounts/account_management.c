#include <stdlib.h>
#include <stdio.h>

#include <accounts/account_management.h>
#include <constants.h>

void 
account_get_all(int * accountIds, 
                int * length) {
    
    // do something
    *length = 7;
    if (*length < MAX_ACCOUNTID_AMOUNT) {
        accountIds[0] = 1;
        accountIds[1] = 2;
        accountIds[2] = 3;
        accountIds[3] = 4334;
        accountIds[4] = 4567;
        accountIds[5] = 235;
        accountIds[6] = 2525;
    } else {
        // ERROR
    }
}

int
account_set(int    const accountId,
            char * const attribute,
            char * const value) {
          
    // do something
    
    printf("accountId: %d\n", accountId);
    printf("attribute: %s\n", attribute);
    printf("value: %s\n", value);
        
    return 1;
}
            
char*
account_get(int    const accountId,
            char * const attribute) {
          
    // do something
    printf("accountId: %d\n", accountId);
    printf("attribute: %s\n", attribute);
        
    return "value";
}
            
int
account_create() {
    
    // do something
    printf("leaving accountCreate\n");
    
    return 7;
}
               
int
account_delete(int const accountId) {
    
    // do something
    printf("accountId: %d\n", accountId);
        
    return 1;
}

int
account_register(int const accountId) {
    
    // do something
    printf("accountId: %d\n", accountId);
    
    return 1;
}

int
account_unregister(int const accountId) {
    
    // do something
    printf("accountId: %d\n", accountId);
    
    return 1;
}
