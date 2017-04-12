#define CLASSNAME Account
#define TYPENAME chat
#include "accountServer.h"

ADDBEGIN
ADD( "chat", chat )
ADDEND

Account<chat> theApp( "account" ) ;

