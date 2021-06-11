/*
  int new_account(int *, account_t **);
  - Provides a pointer to a account_t representing a new account with a unique id.
  
  int retrieve_account(int, account_t **);
  - Provides a pointer to a account_t representing an active account.
  
  int store_account(account_t *);
  - Takes an account_t pointer and writes it out to permanent storage then frees the account pointer.
  
  int return_account(account_t *);
  - Takes an account pointer that the caller does not to use anymore and frees it. (Does not change the account image on permanent store.)
  
  int delete_account(account_t *);
  - Takes a pointer to an account and resets the account image in permanent store, frees the account pointer and reclaims the id for reuse.
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "atm.h"
#include "atm_db.h"

// file that contains database.
#define ATM_DB_FILE   "./.atm_db"
// multiple of where to put each account in the file.
// an artificial way to simulate a slower backing store by causing more page faults on access.
#define PAD           8192

static int increased_io = 0;
static int increased_cpu = 0;

// instance of an aempty account used to initially fill databse file and overwrite freed accounts.
static account_t blank_account = {
  0,
  0,
  0,
  0
};

/*
  Initialize the database.
  If create, create new file and populate with blank accounts.
  Otherwise look for an existing database file.
  If initialization fails return a ERR_NUM_DATA_UNAVAIL.
*/
void init_account_db(int force_create, int io, int cpu)
{
  
}