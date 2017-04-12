#include <csignal>
#include <cerrno>

#include <iostream>

void serviceRun() ;

int main()
{
   //if ( SIG_ERR == signal( SIGINT, SIG_IGN ) )  {
   //   perror( "signal" ) ;
   //   return -1 ;
   //}
   if ( SIG_ERR == signal( SIGTERM, SIG_IGN ) ) {
      perror( "signal" ) ;
      return -1 ;
   }
   serviceRun() ;
   return 0 ;
}
