#ifndef _ACCOUNTSERVER_H
#define _ACCOUNTSERVER_H
#include <string>
#include <thread>

#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "public/communication/communication.h"
#include "public/conf/conf.h"
#include "public/service/service.h"
#include "core.h"

template <typename T>
class Account final : public Service
{
public:
   Account( const std::string &strClassName )
      :Service(strClassName)
   {
      addService() ;
   }

   void addService() ;

   void chat( boost::shared_array<char> pArray, size_t rByte )
   {
      //std::cout << "等两秒回复 收到消息:" << pArray.get() << std::endl ;
      //sleep( 2 ) ;
      async_sendMsg( _pMajorSocket, pArray, rByte, 
         [this, pArray]( const boost::system::error_code &ec, size_t )
         {
            if ( !ec ) {
               async_receiveMsg( _pMajorSocket, boost::bind( &Account::doService, this, _1, _2, _3) ) ;
            }
            else {
               std::cout << "主服务断开" << std::endl ;
               return ;
            }
		   } ) ;
   }
} ;

#endif
