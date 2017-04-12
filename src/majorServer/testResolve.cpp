#include <stdlib.h>
#include <unistd.h>
#include <csignal>
#include <thread>

#include <boost/asio.hpp>
#include <iostream>
#include <future>
using std::cout ;
using std::endl ;

class A
{
public:
   ~A()
   {
      cout << "析构A" << endl ;
   }
} ;
int testThread()
{
   cout << std::this_thread::get_id() << endl ;
   sleep( 3 ) ;
   return 10 ;
}

int main()
{
   cout << std::this_thread::get_id() << endl ;
   std::future<int> fut = std::async( std::launch::deferred, testThread ) ;
   //std::async( testThread ) ;
   A a ;
   using namespace boost::asio::ip ;
   tcp::endpoint ep( address::from_string( "192.168.191.2" ), 12345 ) ;
   tcp::resolver::query q( "www.baidu.com", "12345" ) ;
   boost::asio::io_service ios ;
   tcp::resolver rlv( ios ) ;
   auto iter = rlv.resolve( q ) ;
   decltype(iter) iterEnd ;
   cout << iter->endpoint().address() << " " << iter->endpoint().port() << endl ;
   fut.get() ;
   // cout << iter->host_name() << iter->service_name() << endl ;
}
