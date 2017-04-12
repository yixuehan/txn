#include <cstdlib>

#include <iostream>
#include <string>
#include <sstream>
//using namespace std ;
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


#define CLASSNAME Service
#define NOTEMPLATE
#include "public/service/service.h"
#include "public/communication/communication.h"

static Service *g_app_ptr ;

Service::Service( const std::string &strClassName )
{
   setSettingFile( boost::filesystem::path("service") / strClassName / (strClassName + ".info") ) ;
   g_app_ptr = this ;
   using namespace boost::asio::ip ;
   _pMajorSocket.reset( new tcp::socket(_ioService) ) ;
   tcp::endpoint ep( address::from_string(getSetting()->get<std::string>("network.serverAddress")), 
                     getSetting()->get<int>("network.serverPort")) ;
   _pMajorSocket->connect( ep ) ;
   async_receiveMsg( _pMajorSocket, boost::bind( &Service::doService, this, _1, _2, _3) ) ;
}

void Service::registerService( const std::string &strServiceName)
{
   using namespace boost::property_tree ;
   // using path = namespace boost::filesystem::path ;
   //在当前服务名对应的配置文件目录，pubFields 存放公共的请求及应答包头
   //其它以service结尾的文件存放服务的请求和应答字段
   auto p  = boost::filesystem::path(getenv("PROROOT")) / "etc" / "service" / strServiceName / "interface";
   auto iter = boost::filesystem::directory_iterator(p) ;
   decltype(iter) iterEnd ;
   std::string strFileName ;
   for ( ; iter != iterEnd; ++iter ) {
      if ( boost::filesystem::is_regular_file( iter->status() )) {
         strFileName = iter->path().string() ;
         std::cout << strFileName << std::endl ;
         if ( std::string::npos != strFileName.find( ".service" ) ) {
            _services.emplace_back( std::move(strFileName) ) ;
         }
      }
   }
   // 向服务端发送注册请求
   
   ptree pt ;
   pt.put( "interfaceID", "registerService" ) ;
   pt.put( "serviceName", strServiceName );
   pt.put( "requestType", "req" ) ;
   ptree ptID ;
   
   for ( auto &service : _services ) {
      ptID.add( "ID", boost::filesystem::basename(service) ) ;
   }
   pt.add_child( "interfaceList", ptID ) ;
   std::ostringstream os ;
   write_json( os, pt ) ;
   auto strRegister = os.str() ;
   sendMsg( _pMajorSocket, strRegister, strRegister.length() ) ;
   // 接收应答
   std::string strReceive ;
   receiveMsg( _pMajorSocket, strReceive ) ;
   std::cout << strReceive << std::endl ;
}

void Service::doService( boost::shared_array<char> pArray, 
                const boost::system::error_code &ec, size_t rByte)
{   
   if ( !ec ) { 
      boost::property_tree::ptree pt ; 
      std::stringstream ioStr( pArray.get() ) ;
      boost::property_tree::read_json( ioStr, pt ) ;
      auto strID = pt.get<std::string>( "interfaceID" ) ;
      auto iterFind = _mapService.find( strID ) ;
      if ( _mapService.end() == iterFind ) {
         std::cout << "无此服务" << std::endl ;
         return ;
      }
      else {
         iterFind->second( pArray, rByte ) ;
      }
   }
}

void Service::run()
{
   _ioService.run() ;
}

Service *getApp()
{
   return g_app_ptr ;
}

