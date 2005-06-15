#include <stdlib.h>
#include <stdio.h>

#include <accounts/account_management.h>

void 
account_get_all(int * accountIds) {
    
    // do something
    accountIds = (int *) malloc(3 * sizeof(int));
    accountIds[0] = 1;
    accountIds[1] = 2;
    accountIds[2] = 3;
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
