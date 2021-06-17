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
  int db_fd, i, rtn;
  size_t amt;
  off_t pos;
  account_t account = { -1, -1, -1, -1 };
  
  extern int increased_io, increased_cpu;
  
  increased_io = io;
  increased_cpu = cpu;
  
  // force new database if requested
  if (force_create) unlink(ATM_DB_FILE);
  
  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    // force create the file
    if ((db_fd = creat(ATM_DB_FILE, S_IRUSR | S_IWUSR)) < 0) {
      perror("creat of db file");
      exit(1);
    }
    
    // fill in with blank accounts
    for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
      if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < 0) {
        perror("lseeking in db file");
        close(db_fd);
        exit(1);
      }
      if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0) {
        perror("writing to db file");
        close(db_fd);
        exit(1);
      }
    }
   } else {
      // check for consistency
      for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
        if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < 0) {
          perror("lseeking in db file");
          close(db_fd);
          exit(1);
        }
        if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0) {
          perror("writing to db file");
          close(db_fd);
          exit(1);
        }
        
        if (!((account.inuse == 0) || (account.inuse == 1))) {
          fprintf(stderr, "bad data in db file\n");
          close(db_fd);
          exit(1);
        }
      }
    }

    if ((rtn = close(db_fd)) < 0) {
      perror("closing db file");
      exit(1);
    }
}

/**
  return a new account for use.
  id and inuse fields should be set.
  if no more accounts return ERR_NUM_NO_ACCOUNTS,
  if an error accessing the database return ERR_NUM_SRV_DATA_UNAVAIL.
*/
int new_account(int *idp, account_t **raccountpp) 
{
  int db_fd, i, rtn;
  off_t pos;
  size_t amt;
  account_t *accountp;
  
  accountp = (account_t *)malloc(sizeof(account_t));
  
  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    free(accountp);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
    if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < 0) {
      perror("lseeking in db file");
      free(accountp);
      close(db_fd);
      return (ERR_NUM_DATA_UNAVAIL);
    }
    if ((amt = read(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
      perror("reading db file");
      free(accountp);
      close(db_fd);
      return (ERR_NUM_DATA_UNAVAIL);
    }

    if (accountp->inuse == 0)
      break;
  }
  
  if (i == MAX_NUM_ACCOUNTS) {
    free(accountp);
    close(db_fd);
    return (ERR_NUM_BAD_ACCOUNT);
  }
  
  // set values in structure
  accountp->id = i;
  accountp->inuse = 1;
  
  if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0) {
    perror("writing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((rtn = fsync(db_fd)) < 0) {
    perror("syncing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  
  // set return value
  *raccountpp = accountp;
  *idp = i;
  
  if ((rtn = close(db_fd)) < 0) {
    perror("closing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  return(1);
}

/**
  retrieve an account from backing store.
  watch for bad account id or problems accessing db file.
*/
int retrieve_account(int id, account_t **raccountpp) 
{
  int db_fd, rtn;
  off_t pos;
  size_t amt;
  account_t *accountp;
  
  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);
  
  accountp = (account_t *)malloc(sizeof(account_t));
  
  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    free(accountp);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  
  if ((pos = lseek(db_fd, (off_t)PAD*id, SEEK_SET)) < 0) {
    perror("lseeking db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((amt = read(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
    perror("reading db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((accountp->inuse != 1) || (accountp->id != id)) {
    free(accountp);
    close(db_fd);
    return (ERR_NUM_BAD_ACCOUNT);
  }
  
  
}