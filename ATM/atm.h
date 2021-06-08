#ifdef DEBUG
#define DPRINTF(x)  printf x
#else
#define DPRINTF(x)
#endif

/**
  Clients and server communicate by passing request
  and response character strings of fixed formats.
  Request strings start with a digit character representing the transaction id
  the format of the remaing string.
  The first character in a response string is a digit representing success or failure for the request.
  If success is true, the remaining response string format is determined by the transaction type that they are in response to.
  
  Request string formats:
  
                    trans_id  account passwd  amount
                    integer   integer integer integer
  OPEN_ACCT_TRANS   X
  DEPOSIT_TRANS     X         X       X       X
  WITHDRAW_TRANS    X         X       X       X
  BALANCE_TRANS     X         X       X
  SHUTDOWN          X                 X
  
  Response string formats:
  
                    success account passwd  amount  err_msg
                    integer integer integer integer char [ERR_MSG_SIZE]
  OPEN_ACCT_TRANS   X       X       X               X
  DEPOSIT_TRANS     X                               X
  WITHDRAW_TRANS    X                               X
  BALANCE_TRANS     X                       X       X
  SHUTDOWN          X

*/

#define OPEN_ACCT_TRANS   1
#define DEPOSIT_TRANS     2
#define WITHDRAW_TRANS    3
#define BALANCE_TRANS     4
#define SHUTDOWN          5

#define TRANS_SUCCESS     1
#define TRANS_FAILURE     2

#define ERR_MSG_SIZE      20
#define ERR_MSG_TBL_SIZE  10 // must be at least largest ERR_NUM_XXX

#define ERR_MSG_NO_ACCOUNT    "No_More_Accounts"
#define ERR_MSG_DATA_UNAVAIL  "Database_Unavail"
#define ERR_MSG_BAD_ACCOUNT   "Bad_Account_ID"
#define ERR_MSG_BAD_PASSWORD  "Bad_Password"
#define ERR_MSG_INS_FUNDS     "Insufficient_Funds"
#define ERR_MSG_BAD_TRANSID   "Bad_Transaction_ID"

#define ERR_NUM_NO_ACCOUNTS   -2
#define ERR_NUM_DATA_UNAVAIL  -3
#define ERR_NUM_BAD_ACCOUNT   -4
#define ERR_NUM_BAD_PASSWORD  -5
#define ERR_NUM_INS_FUNDS     -6
#define ERR_MUM_BAD_TRANSID   -7
