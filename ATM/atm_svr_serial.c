#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "atm.h"
#include "atm_com.h"
#include "atm_db.h"
#include "atm_svr_password.h"

void atm_server_init(int, char **);
void open_account(char *);
void deposit(char *, char *);
void withdraw(char *, char *);
void balance(char *, char *);
int shutdown_req(char *, char *);
void handle_bad_trans_id(char *, char *);

static char atm_err_tbl[ERR_MSG_TBL_SIZE][ERR_MSG_SIZE];

/**
  Basic processing loop of the bank database server.
  The program initializes the database then loops, accepting transaction requests from clients.
*/
extern int main(int argc, char *argv[]) {
  /**
    The character string buffers passed from a format
    defined in atm.h
    and the parameters are read in and out using sprinting processing calls sprintf() and sscanf()
  */
  char req_bug[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];
  /**
    A handle on which connection (client) the requst came from.
    Not used initially but included so that in the future different requests could be processed concurrently.
  */
  int conn;
  /**
    The transaction type of the current request.
  */
  int trans_id;
  int done = 0;
  
  DPRINTF(("SERVER: main() pid %d, ppid %d, pgrp %d\n", getpid(), getppid(), getpgrp()));
}