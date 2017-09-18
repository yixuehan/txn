#include <stdexcept>
#include <thread>
// 主服务的构造
template <typename T>
MajorServer<T>::MajorServer()
{
   // 对外服务管理
   tcp::endpoint epIOServer( tcp::v4(), getSetting()->get<int>("network.ioServerPort") ) ;
   _pIOAccept = new tcp::acceptor( _ioServer, epIOServer ) ;

   // 子服务管理
   tcp::endpoint epChildServer( tcp::v4(), getSetting()->get<int>("network.childServerPort") ) ;
   _pChildAccept = new tcp::acceptor( _ioServer, epChildServer ) ;
}

template <typename T>
void MajorServer<T>::run()
{
   boost::shared_ptr<tcp::socket> pSocket2( new tcp::socket(_ioServer) ) ;
   _pChildAccept->async_accept( *pSocket2, boost::bind(&MajorServer::doChildServiceAccept, this, pSocket2, _1) ) ;
   boost::shared_ptr<tcp::socket> pSocket1( new tcp::socket(_ioServer) ) ;
   _pIOAccept->async_accept( *pSocket1, boost::bind(&MajorServer::doClientAccept, this, pSocket1, _1) ) ;
   for ( int i = 0; i < getSetting()->get<int>( "thread.num" ) ; ++i ) {
       std::thread([this]{_ioServer.run();}).detach() ;
   }
   _ioServer.run() ;
}

// 接收到客户请求连接
template <typename T>
void MajorServer<T>::doClientAccept( boost::shared_ptr<tcp::socket> pSocket, const boost::system::error_code &ec )
{
   if ( !ec ) {
      // 接收
      std::cout << "注册接收函数" << std::endl ;
      // 记录建立的连接
      _setClientConnected.insert( pSocket ) ;
      async_receiveMsg( pSocket, boost::bind( &MajorServer::doClientRequestServer, this, pSocket, _1, _2, _3) ) ;
   }
   else {
      std::cout << "建立客户连接出错" << std::endl ;
   }
   boost::shared_ptr<tcp::socket> pSocket_t( new tcp::socket(_ioServer) ) ;
   std::cout << "等待新的连接" << std::endl ;
   _pIOAccept->async_accept( *pSocket_t, boost::bind(&MajorServer::doClientAccept, this, pSocket_t, _1) ) ;
}

// 接收到子服务接入
template <typename T>
void MajorServer<T>::doChildServiceAccept( boost::shared_ptr<tcp::socket> pSocket, const boost::system::error_code &ec )
{
   if ( !ec ) {
      // 接收
      std::cout << "注册子服务接收函数" << std::endl ;
      // 记录建立的连接
      _setServiceConnected.insert( pSocket ) ;
      async_receiveMsg( pSocket, boost::bind( &MajorServer::doChildConnected, this, pSocket, _1, _2, _3) ) ;
   }
   else {
      std::cout << "建立子服务请求连接出错" << std::endl ;
   }
   boost::shared_ptr<tcp::socket> pSocket_t( new tcp::socket(_ioServer) ) ;
   std::cout << "等待新的子服务连接" << std::endl ;
   _pChildAccept->async_accept( *pSocket_t, boost::bind(&MajorServer::doChildServiceAccept, this, pSocket_t, _1) ) ;

}

// 子服务接入处理
template <typename T>
void MajorServer<T>::doChildConnected( boost::shared_ptr<tcp::socket> pSocket,
                     boost::shared_array<char> pArray,
                     const boost::system::error_code &ec,
                     size_t rbyte )
{
   try {
      if ( !ec ) {
         std::cout << "读到数据[" << pArray.get() << "]" << std::endl ;
         std::cout << "本次收到的数据长度[" << rbyte << "]" << std::endl ;
         static std::stringstream sstr ;
         sstr.str( pArray.get() ) ;
         static boost::property_tree::ptree pt ;
         boost::property_tree::read_json( sstr, pt ) ;
         // 获取请求服务，发给服务提供者
         std::string strServiceName = pt.get<std::string>("serviceName") ;
         std::string strType = pt.get<std::string>("requestType") ;
         std::string strReqService = pt.get<std::string>("interfaceID") ;
         auto itFind = _mapRoute.find( strReqService ) ;
         if ( strReqService == "registerService" ) {
            doRegisterService( pSocket, pt ) ;
         }
         else if ( _mapRoute.end() == itFind ) {
            std::cerr << "无此服务" << std::endl ;
         }
      }
      else {
         std::cerr << "子服务请求断开" << std::endl ;
         doDisconnect( pSocket ) ;
      }
   }
   catch ( boost::property_tree::json_parser_error &ec ) {
      std::cerr << ec.message() << std::endl ;
      //doDisconnect( pSocket ) ;
   }
}

// 客户端请求服务
template <typename T>
void MajorServer<T>::doClientRequestServer( boost::shared_ptr<tcp::socket> pSocket,
                     boost::shared_array<char> pArray,
                     const boost::system::error_code &ec,
                     size_t rbyte)
{
   try {
      if ( !ec ) {
         std::cout << "读到数据[" << pArray.get() << "]" << std::endl ;
         std::cout << "本次收到的数据长度[" << rbyte << "]" << __LINE__ << std::endl ;
         static std::stringstream sstr ;
         sstr.str( pArray.get() ) ;
         static boost::property_tree::ptree pt ;
         boost::property_tree::read_json( sstr, pt ) ;
         // 获取请求服务，发给服务提供者
         //boost::system::error_code ec ;
         std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
         std::string strID = pt.get<std::string>("interfaceID") ;
         std::string strType = pt.get<std::string>("requestType") ;
         std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
         auto itFind = _mapRoute.find( strID ) ;
         std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
         std::cout << "strID:" << strID << std::endl ;
         for ( auto &pair : _mapRoute ) {
            std::cout << pair.first << std::endl ;
         }
         if ( _mapRoute.end() == itFind ) {
            std::cerr << "无此服务" << std::endl ;
         }
         else {
            {
         //std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
               lock_t lck( _mtxRoute ) ;
         //std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
               std::sort( itFind->second.begin(), itFind->second.end() ) ;
         //std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
               ++(ServiceState::_mapLoad[itFind->second[0].socket_ptr]) ;
          //     std::cout << "负载:" ;
               for ( auto &service : itFind->second ) {
                  std::cout << ServiceState::_mapLoad[service.socket_ptr] << ' ' ;
               }
               std::cout << std::endl ;
            }
            auto pos = strID.find_last_of( '-' ) ;
            if ( std::string::npos == pos ) {
               throw std::string( "接口名称有误" ) ;
            }
            pt.put( "interfaceID", strID.substr( pos+1 ) ) ;
            sstr.str("") ;
            boost::property_tree::write_json( sstr, pt ) ;
            auto str = sstr.str() ;
            std::cout << "sstr[" << str << "]" << std::endl ;
            pArray.reset( new char[str.length() + 1] )  ;
            strcpy( pArray.get(), str.c_str() ) ;
            std::cout << "向子服务端发送请求[" << pArray.get() << "]" << std::endl ;
            doAsyncTransRequest( itFind->second[0].socket_ptr, pSocket, strID, pArray ) ;
         }
      }
      else {
         std::cerr << "客户请求断开" << std::endl ;
         doDisconnect( pSocket ) ;
      }
   }
   catch ( boost::property_tree::json_parser_error &ec ) {
      std::cerr << ec.message() << std::endl ;
      //doDisconnect( pSocket ) ;
   }
   catch ( std::string &e ) {
      std::cerr << e << std::endl ;
   }
}

   // 注册子服务
template <typename T>
void MajorServer<T>::doRegisterService( boost::shared_ptr<tcp::socket> pSocket, const boost::property_tree::ptree &pt )
{
   // 注册时以服务名和接口名为唯一标识
   try {
      auto &childs = pt.get_child( "interfaceList" ) ;
      std::string strID ;
      std::string strFullID ;
      std::string strServiceName = pt.get<std::string>("serviceName");
      {
         std::unique_lock<std::mutex> lck(_mtxRoute) ;
         // 增加对应的路由列表
         for ( auto iterID = childs.begin();
               iterID != childs.end() ;
               ++iterID ) {
            strID = iterID->second.get_value<std::string>() ;
            strFullID = strServiceName + "-" + strID ;
            //std::cout << "fullName:" << strFullID << std::endl;
            _mapRoute[strFullID].emplace_back( strFullID, _mapRoute[strFullID].size()+1, pSocket) ;
            _mapService[pSocket].push_back( strFullID ) ;
         }

         // 记录当前服务对应的数量
         if ( _mapServiceOrder.find(strServiceName) == _mapServiceOrder.end() ) {
            _mapServiceOrder.emplace( strServiceName, 1 ) ;
         }
         else {
            ++(_mapServiceOrder[strServiceName]) ;
         }
         std::cout << "当前服务对应的ID" << std::endl ;
         for ( auto &s : _mapRoute ) {
            // std::cout << s.first << ' ' ;
            for ( auto &id : s.second ) {
               std::cout << id.serviceName << ' ' ;
            }
            std::cout << std::endl ;
         }
      }
      // 下发服务编号，支持子服务多开的情况
      boost::property_tree::ptree ptRep ;
      ptRep.put( "retMeg", "ok" ) ;
      ptRep.put<int>( "serviceID", _mapServiceOrder[strServiceName]) ;
      std::stringstream ioStr ;
      boost::property_tree::write_json( ioStr, ptRep ) ;
      std::string strRep = ioStr.str() ;
      // 注册成功回复
      sendMsg( pSocket, strRep, strRep.length() ) ;
   }
   catch ( boost::property_tree::json_parser_error &ec ) {
      std::cerr << ec.message() << std::endl ;
      //doDisconnect( pSocket ) ;
   }
}




// 反注册子服务
template <typename T>
void MajorServer<T>::doUnregisterService( boost::shared_ptr<tcp::socket> pSocket )
{
   for ( auto &ID : _mapService[pSocket] ) {
      auto _mapFind = _mapRoute.find( ID ) ;
      _mapFind->second.erase(std::find(_mapFind->second.begin(), _mapFind->second.end(), pSocket )) ;
      //_mapRoute.erase( ID ) ;
   }
   _mapService.erase( pSocket ) ;
   _setServiceConnected.erase( pSocket ) ;
         std::cout << "当前服务对应的ID" << std::endl ;
   for ( auto &s : _mapRoute ) {
      std::cout << s.first << ' ' ;
      for ( auto &id : s.second ) {
         std::cout << id.serviceID << ' ' ;
      }
   }
   std::cout << std::endl ;
}

// 断开连接
template <typename T>
void MajorServer<T>::doDisconnect( boost::shared_ptr<tcp::socket> pSocket )
{
   pSocket->close() ;
   if ( _setClientConnected.end() != _setClientConnected.find( pSocket) ) {
      std::cerr << "移除客户连接" << std::endl ;
      _setClientConnected.erase( pSocket) ;
   }
   else if ( _setServiceConnected.end() != _setServiceConnected.find( pSocket) ) {
      std::cerr << "移除子服务连接" << std::endl ;
      doUnregisterService( pSocket ) ;
   }
}

// 收到请求转发给子服务
template <typename T>
void MajorServer<T>::doAsyncTransRequest( boost::shared_ptr<tcp::socket> pSocketService,
                          boost::shared_ptr<tcp::socket> pSocketClient,
                          std::string strInterfaceID,
                          boost::shared_array<char> pArray
                        )
{
   async_sendMsg( pSocketService, pArray,
      [this, pSocketService, pSocketClient, strInterfaceID, pArray]( const boost::system::error_code &ec, size_t )
          {
             if ( !ec ) {
                doAsyncServiceResponse( pSocketService, pSocketClient, strInterfaceID, pArray ) ;
             }
             else {
                // 断开当前子服务，尝试其它子服务
                doDisconnect( pSocketService ) ;
                auto iter = _mapRoute.find( strInterfaceID ) ;
                if ( (_mapRoute.end() == iter) || (iter->second.size() == 0) ) {
                   std::cout << "子服务" << __LINE__ << std::endl;
                }
                else {
                   std::cout << "尝试其它子服务" << std::endl ;
                   doAsyncTransRequest( iter->second[0].socket_ptr, pSocketClient, strInterfaceID, pArray ) ;
                }
             }
          }) ;
}

// 收到应答返回给请求的客户
template <typename T>
void MajorServer<T>::doAsyncServiceResponse( boost::shared_ptr<tcp::socket> pSocketService,
                                boost::shared_ptr<tcp::socket> pSocketClient,
                                std::string strInterfaceID,
                                boost::shared_array<char> oriData_ptr)
{
   async_receiveMsg( pSocketService,
         [this, pSocketClient, pSocketService, strInterfaceID, oriData_ptr](boost::shared_array<char> pArray,
            const boost::system::error_code& ec, size_t )
         {
            if ( !ec ) {
                 doAsyncResponse2Client( pSocketClient, pArray ) ;
                 lock_t lck( _mtxRoute ) ;
                 --(ServiceState::_mapLoad[pSocketService]) ;
              }
              else {
                // 断开当前子服务，尝试其它子服务
                doDisconnect( pSocketService ) ;
                auto iter = _mapRoute.find( strInterfaceID ) ;
                if ( (_mapRoute.end() == iter) || (iter->second.size() == 0) ) {
                   std::cout << "子服务断开" << __LINE__ << std::endl;
                }
                else {
                   std::cout << "尝试其它子服务" << std::endl ;
                   doAsyncTransRequest( iter->second[0].socket_ptr, pSocketClient, strInterfaceID, oriData_ptr ) ;
                }
              }
         } ) ;
}


