#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <functional>
#include <mutex>
using namespace std ;

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/socket_base.hpp>
using namespace boost::asio::ip ;
#include "public/communication/communication.h"

#include "public/conf/conf.h"

template <typename T>
void async_receiveData( boost::shared_ptr<tcp::socket> pSocket,
                        T dataBuffer,//boost::shared_array<char> pArray,
                        FnReceive func,
                        const boost::system::error_code &ec,
                        size_t rByte ) 
{
   if ( !ec ) {
      std::string strLength ;
      std::cout << "源长度[" << dataBuffer.get() << "]" << std::endl ;
      for ( size_t i = 0; i < rByte; ++i ) {
         strLength.push_back( dataBuffer[i] ) ;
      }
      //cout << "将要收到的数据长度[" << strLength << "]" << endl ;
      int rByte = 0 ;
      try {
         rByte = stoi( strLength ) ; 
      }
      catch ( std::invalid_argument &e ) {
         std::cerr << e.what() << std::endl ; 
      }
      catch ( std::out_of_range &e ) {
         std::cerr << e.what() << std::endl ; 
      }
      cout << "rbyte:" << rByte << std::endl ;
      if ( 0 >= rByte ) {
         async_receiveMsg( pSocket, func ) ;
         return ;
      }
      // 接收数据
      boost::shared_array<char> pArray( new char[rByte+1] ) ; 
      bzero( pArray.get(), rByte+1 ) ; 
      boost::asio::async_read( *pSocket, boost::asio::buffer( pArray.get(), rByte ), 
         [func, pSocket, pArray]( const boost::system::error_code &ec, size_t sByte)
         {
            std::cout << "接收数据完成，执行回调之前" << std::endl ;
            func( pArray, ec, sByte ) ;
            std::cout << "接收数据完成，执行回调之后" << std::endl ;
         }) ;
      //pSocket->async_receive( boost::asio::buffer( pArray.get(), rByte ), 
      //      boost::bind(func, pArray, _1, _2) ) ;
   }
   else {
      //cout << "客户端 退出" << endl ;
      //pSocket->close() ;
      func( dataBuffer, ec, rByte ) ;
   }
}

//template <typename Fn>
void async_receiveMsg( boost::shared_ptr<tcp::socket> pSocket, FnReceive func )
{
   using namespace boost::asio ;
   // 读取长度，再读取数据
   size_t rByte = getSetting()->get<int>("network.lengthByte") ;
   boost::shared_array<char> pArray( new char[rByte+1] ) ; 
   bzero( pArray.get(), rByte+1 ) ; 

   async_read( *pSocket, buffer( pArray.get(), rByte ), 
         boost::bind(async_receiveData<boost::shared_array<char>>, pSocket, pArray, func, _1, _2) ) ;
}

void async_sendMsg( boost::shared_ptr<tcp::socket> pSocket, 
      const boost::shared_array<char> pArray, 
      FnSend func )
{
   async_sendMsg( pSocket, pArray, strlen(pArray.get()), func ) ; 
}

void async_sendMsg( boost::shared_ptr<tcp::socket> pSocket,
      const boost::shared_array<char> pArray,
      int length, FnSend func )
{  
   // 获取长度
   int lengthByte = getSetting()->get<int>("network.lengthByte") ;
   int len_t = length + lengthByte  ;
   //std::cout << "需要分配的发送数据的缓冲区长度[" << len_t +1 << "]" << std::endl ;
   boost::shared_array<char> pBuf( new char[len_t+1] );
   memset( pBuf.get(), 0, len_t + 1 ) ;
   snprintf( pBuf.get(), lengthByte + 1, "%0*d", lengthByte, length ) ;
   for ( int i = 0; i < length; ++i ) { 
      pBuf[i + lengthByte] = pArray[i] ;
   }
   // memcpy( pBuf.get() + lengthByte, pArray.get(), length ) ;
   // std::cout << "发送的原数据[" << pBuf.get() << "]" << std::endl ;
   
   async_write( *pSocket, boost::asio::buffer(pBuf.get(), len_t), 
         [func, pBuf]( const boost::system::error_code &ec, size_t sByte) 
         {
            //std::cout << "执行发送完成数据[" << sByte << ":" << pBuf.get()  << "]" << std::endl ;
            func( ec, sByte ) ;
            //std::cout << "完成发送完成的回调" << std::endl ;
         }) ;
}

