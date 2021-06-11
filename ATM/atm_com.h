#define COMM_BUF_SIZE				100
#define MAX_NUM_CONNECTIONS 100
#define SOCKET_NAME					"./.atm_conn"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// server
void server_comm_init(void);
void server_comm_get_request(int *, char *);
void server_comm_send_response(int, char *);
void server_comm_close_conn(int);
void server_comm_shutdown(void);

// client
void client_comm_init(void);
void client_comm_send_resquest(char *);
void client_comm_get_response(char *);
void client_comm_shutdown(void);
