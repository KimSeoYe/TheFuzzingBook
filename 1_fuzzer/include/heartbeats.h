#define BUF_MAX 2048

#define uninitialized_memory_marker "deadbeef" 

char * heartbeats (char * reply, int length, char * memory, char * s) ;

void make_secrets_sentence (char * secrets) ;

void execute_heartbeats (char * reply, int length, char * result) ;
