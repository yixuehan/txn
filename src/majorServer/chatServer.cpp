#include "majorServer.h"
#include <queue>
#include <condition_variable>
#include <mutex>

struct StructMessage
{
   std::mutex g_m ;
   bool bRun = false ;
   std::queue<boost::shared_array<char>> msgQueue;
} ;
static std::map<boost::shared_ptr<tcp::socket>, StructMessage> g_mapMessage ;

void toClient( boost::shared_ptr<tcp::socket> pSocket, 
               boost::shared_array<char> pArray, 
               const boost::system::error_code &ec, size_t )
{
   if ( !ec ) {
      g_mapMessage[pSocket].msgQueue.pop() ;
      std::cout << "当前剩余条数：" << g_mapMessage[pSocket].msgQueue.size() << std::endl ;
      if ( g_mapMessage[pSocket].bRun && !g_mapMessage[pSocket].msgQueue.empty() ) {
         g_mapMessage[pSocket].bRun = true ;
         std::cout << "注册新的异步发送" << std::endl ;
         async_sendMsg( pSocket, g_mapMessage[pSocket].msgQueue.front(), boost::bind( toClient, pSocket, pArray,  _1, _2 )  ) ;
      }
      else if ( g_mapMessage[pSocket].bRun ) {
         std::cout  << "队列为空" << std::endl ;
         g_mapMessage[pSocket].bRun = false ;
      }
   }
   else {
      std::cout << "客户端断开" << std::endl ;
   }
}

template<>
void MajorServer<chat>::doAsyncResponse2Client( boost::shared_ptr<tcp::socket> pSocketClient, boost::shared_array<char> pArray)
{ 
   std::cout << "发给聊天室" << std::endl ;
   async_sendMsg( pSocketClient, pArray, 
      [this, pSocketClient, pArray](
         const boost::system::error_code &ec, size_t ) 
         { 
            if ( !ec ) { 
               std::cout << "继续等待请求" << std::endl ; 
               async_receiveMsg( pSocketClient,
                  boost::bind( &MajorServer::doClientRequestServer, this, pSocketClient, _1, _2, _3) ) ; 
            } 
            else { 
               std::cout << "客户请求连接断开" << std::endl ; 
            } 
        }) ; 
   // 将此消息发送给聊天室的其它人
   for ( auto &pSocket : _setClientConnected ) {
      if ( pSocket != pSocketClient ) {
         // 给每个socket发50次消息
         //std::cout << "增加50条消息" << std::endl ;
         //for ( int i = 0; i < 50; ++i ) 
         {
            std::lock_guard<std::mutex> lkg( g_mapMessage[pSocket].g_m ) ;
            g_mapMessage[pSocket].msgQueue.push( pArray ) ;
         }
         std::lock_guard<std::mutex> lkg( g_mapMessage[pSocket].g_m ) ;
         if ( !g_mapMessage[pSocket].bRun && !g_mapMessage[pSocket].msgQueue.empty() ) {
            g_mapMessage[pSocket].bRun = true ;
            std::cout << "注册异步发送" << std::endl ;
            async_sendMsg( pSocket, g_mapMessage[pSocket].msgQueue.front(), 
                  boost::bind( toClient, pSocket, pArray,  _1, _2 )  ) ;
         }
      }
   }
} 

