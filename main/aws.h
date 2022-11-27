#ifndef _AWS_
#define _AWS_

#include "string.h"

extern const char root_pem_start[]        asm("_binary_root_pem_start");
extern const char root_pem_end[]          asm("_binary_root_pem_end");
    
extern const char client_crt_start[]           asm("_binary_client_crt_start");
extern const char client_crt_end[]             asm("_binary_client_crt_end");
    
extern const char client_key_start[]            asm("_binary_client_key_start");
extern const char client_key_end[]              asm("_binary_client_key_end");

#ifdef __cplusplus
extern "C" {
#endif

void AWSTask(void *arg);
void AWSRun(void);
void AWSStop(void);

#ifdef __cplusplus
}
#endif

#endif //__AWS__