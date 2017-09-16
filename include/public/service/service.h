#ifndef _SERVICE_H
#define _SERVICE_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/algorithm/string.hpp>
//using namespace boost::asio::ip ;

#ifndef CLASSNAME
   #error "请先定义CLASSNAME为当前类名"
#endif
//
#if defined(TYPENAME)
   #define ADDBEGIN template<> \
                    void CLASSTYPENAME::addService() {
   #define CLASSTYPENAME CLASSNAME<TYPENAME>
#elif !defined(TYPENAME) && !defined(NOTEMPLATE)
   #define CLASSTYPENAME CLASSNAME
   #define ADDBEGIN void CLASSTYPENAME::addService() {
   #warn "如果是非模板类请定义宏[NOTEMPLATE]"
#endif


#define ADD( strID, func ) if ( _mapService.end() != _mapService.find( strID ) ) { \
                                  std::cout << "此服务重复注册" << std::endl ; \
                               } else { \
                                  _mapService.emplace( strID, boost::bind( &CLASSNAME::func, this, _1, _2 ) ) ; \
                               }
#define ADDEND }

class Service
{
public:
   Service( const std::string &strClassName ) ;
   // 告诉主服务自己提供哪些服务
   void registerService( const std::string &) ;
   void doService( boost::shared_array<char> pArray,
                            const boost::system::error_code &ec, size_t rByte) ;
   void run() ;
protected:
   boost::asio::io_service _ioService ;
   boost::shared_ptr<boost::asio::ip::tcp::socket> _pMajorSocket ;
   std::vector<std::string> _services  ;
   std::map<std::string, boost::function<void (boost::shared_array<char>, size_t)>> _mapService ;
} ;

Service *getApp() ;

// 定义主函数
#if defined(TYPENAME)
   #define CONCATNAME1(a) #a
   #define CONCATNAME(a) CONCATNAME1(a)
   void serviceRun()
   {
      std::string strClassName = CONCATNAME(CLASSNAME) ;
      boost::algorithm::to_lower( strClassName ) ;
      getApp()->registerService(strClassName) ;
      getApp()->run() ;
   }
#endif

#endif
