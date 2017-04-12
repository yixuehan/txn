#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H
#include <iostream>
//using namespace std ;

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
using namespace boost::asio::ip ;

#include "public/conf/conf.h"

typedef boost::function<void( boost::shared_array<char> , const boost::system::error_code&, size_t )> FnReceive ;
typedef boost::function<void(  const boost::system::error_code&, size_t )> FnSend ;

void async_receiveMsg( boost::shared_ptr<tcp::socket> pSocket, FnReceive func ) ;

void async_sendMsg( boost::shared_ptr<tcp::socket> pSocket, 
      const boost::shared_array<char> pArray, 
      int length, FnSend func ) ;

void async_sendMsg( boost::shared_ptr<tcp::socket> pSocket, 
      const boost::shared_array<char> pArray, 
      FnSend func ) ;

template <typename T>
size_t receiveMsg( boost::shared_ptr<tcp::socket> pSocket, T &dataBuffer )
{
   int lengthByte = getSetting()->get<int>("network.lengthByte") ;
   std::string strLength ;
   boost::shared_array<char> pArray( new char[lengthByte+1] ) ; 
   bzero( pArray.get(), lengthByte+1 ) ; 
   auto rByte = pSocket->receive( boost::asio::buffer(pArray.get(), lengthByte )) ;
   for ( int i = 0; i < lengthByte; ++i ) { 
      strLength.push_back( pArray[i] ) ; 
   }   
   //std::cout << "将要收到的数据长度[" << strLength << "]" << std::endl ;
   //std::cout << "本次收到的数据长度[" << rByte << "]" << std::endl ;
   rByte = stoi( strLength ) ; 
   if ( 0 >= rByte ) { 
      return -1  ;
   } 
   pArray.reset( new char[rByte+1] ) ;
   bzero( pArray.get(), rByte+1 ) ; 
   // 接收数据
   //sync_read( 
   rByte = boost::asio::read( *pSocket, boost::asio::buffer( pArray.get(), rByte ) ) ;
   for ( size_t i = 0; i < rByte; ++i ) {
      dataBuffer.push_back( pArray[i] ) ;
	}
   return rByte ;
}

template <typename T>
size_t sendMsg( boost::shared_ptr<tcp::socket> pSocket,
               const T &dataBuffer, int length )
{
   int lengthByte = getSetting()->get<int>("network.lengthByte") ;
   int len_t = length + lengthByte  ;
   boost::shared_array<char> pBuf( new char[len_t+1] );
   memset( pBuf.get(), 0, len_t + 1 ) ;
   snprintf( pBuf.get(), lengthByte + 1, "%0*d", lengthByte, length ) ;
   for ( int i = 0; i < length; ++i ) {
      pBuf[i + lengthByte] = dataBuffer[i] ;
   }
   //std::cout << "发送的原数据[" << pBuf.get() << std::endl ;
   return boost::asio::write( *pSocket, boost::asio::buffer(pBuf.get(), len_t) ) ;
}

#endif
