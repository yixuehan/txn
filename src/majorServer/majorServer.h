// 系统主服务
// 管理所有的子进程
// 负责请求路由

#include <cstdlib>

#include <string>
#include <sstream>
#include <set>
#include <mutex>
#include <atomic>
#include <map>

#include <boost/asio.hpp>

#include <boost/thread.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
using namespace boost::asio::ip ;

#include "core.h"
#include "public/conf/conf.h"
#include "public/communication/communication.h"

struct ServiceState
{
   ServiceState( std::string serviceName_, size_t serviceID_, boost::shared_ptr<tcp::socket> socket_ptr_ )
      :serviceName(serviceName_),
       serviceID(serviceID_),
       socket_ptr(socket_ptr_)
   {
      _mapLoad.emplace( socket_ptr, 0 ) ;
   }
   ~ServiceState()
   {
      _mapLoad.erase( socket_ptr ) ;
   }
   std::string serviceName ; // 服务名
   size_t serviceID ; // 同一个服务的第一个编号(相同服务多开的情况)
   boost::shared_ptr<tcp::socket> socket_ptr ;
   bool operator < ( const ServiceState &th ) const
   {
      return ServiceState::_mapLoad[socket_ptr] < _mapLoad[th.socket_ptr] ;
   }
   bool operator==( const boost::shared_ptr<tcp::socket> socket_ptr_ ) const
   {
      return socket_ptr == socket_ptr_ ;
   }
   static std::map<boost::shared_ptr<tcp::socket>, size_t> _mapLoad ;
} ;

// T表示请求数据格式为json, xml, info
template <class T>
class MajorServer final
{
   typedef std::unique_lock<std::mutex> lock_t ;
public:
   // 主服务的构造
   MajorServer() ;

   ~MajorServer()
   {
      delete _pIOAccept ;
      delete _pChildAccept ;
   }

   void run() ;

private:
   MajorServer( const MajorServer & ) = delete ;
   MajorServer &operator=( const MajorServer & ) = delete ;
   // 接收到客户请求连接
   void doClientAccept( boost::shared_ptr<tcp::socket> pSocket, const boost::system::error_code &ec ) ;

   // 接收到子服务接入
   void doChildServiceAccept( boost::shared_ptr<tcp::socket> pSocket, const boost::system::error_code &ec ) ;

   // 子服务接入处理
   void doChildConnected( boost::shared_ptr<tcp::socket> pSocket,
                        boost::shared_array<char> pArray,
                        const boost::system::error_code &ec,
                        size_t rbyte ) ;

   // 客户端请求服务
   void doClientRequestServer( boost::shared_ptr<tcp::socket> pSocket,
                        boost::shared_array<char> pArray,
                        const boost::system::error_code &ec,
                        size_t rbyte) ;
private:
   boost::asio::io_service _ioServer ; // 对外服务管理
   tcp::acceptor *_pIOAccept ; // 对外服务的接收管理

   tcp::acceptor *_pChildAccept ; // 子服务的接收管理

   boost::property_tree::ptree _ptInfo ; // 服务配置
   // 记录服务需要调用的子服务名 : 子服务名根据服务和服务编号唯一标识
   // 服务名对应的socket集合
   std::map<std::string, std::vector<ServiceState>> _mapRoute ;
   // socket对应的服务名
   std::map<boost::shared_ptr<tcp::socket>, std::vector<std::string>> _mapService ;
   // 客户端连接集合
   std::set<boost::shared_ptr<tcp::socket>> _setClientConnected;
   // 子服务连接集合
   std::set<boost::shared_ptr<tcp::socket>> _setServiceConnected;
   std::mutex _mtxRoute ;
   // 记录是第几个相同子服务连入
   std::map<std::string, std::size_t> _mapServiceOrder ;
private:
   // 注册子服务
   void doRegisterService( boost::shared_ptr<tcp::socket> pSocket, const boost::property_tree::ptree &pt ) ;

   // 反注册子服务
   void doUnregisterService( boost::shared_ptr<tcp::socket> pSocket ) ;

   // 断开连接
   void doDisconnect( boost::shared_ptr<tcp::socket> pSocket ) ;

   // 收到请求转发给子服务
   void doAsyncTransRequest( boost::shared_ptr<tcp::socket> pSocketService, 
                             boost::shared_ptr<tcp::socket> pSocketClient,
                             std::string strInterfaceID,
                             boost::shared_array<char> pArray
                           ) ;

   // 子服务应答
   // 直接将应答返回给客户请求：strInterfaceID为原请求功能号，oriData_ptr为源请求信息，用来失败时重新发送
   void doAsyncServiceResponse( boost::shared_ptr<tcp::socket> pSocketService,
                                boost::shared_ptr<tcp::socket> pSocketClient,
                                std::string strInterfaceID,
                                boost::shared_array<char> oriData_ptr) ;

   // 收到应答返回给请求的客户
   void doAsyncResponse2Client( boost::shared_ptr<tcp::socket> pSocketClient, boost::shared_array<char> pArray) ;
} ;

#include "majorServer.ipp"
