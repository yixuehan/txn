#define CLASSNAME Pubfunc

#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "pubfunc.h"

#include <stack>

enum class CalcType
{
   op,
   data
} ;

union Element
{
   char op ;
   double data ;
} ;

struct DataOP
{
   CalcType calctype ;
   Element  element ;
} ;

extern std::map<char, unsigned char> _mapPriority ;

// 定义优先级
struct Priority
{
   Priority( char op )
      :op( op ),
       priority( _mapPriority[op] )
   {
   }
   char op ;
   unsigned char priority ;
   bool operator<( const Priority &th ) const
   {
      return priority < th.priority ;
   } ;
   bool operator>( const Priority &th ) const
   {
      return priority > th.priority ;
   } ;
   bool operator==( const Priority &th ) const
   {
      return priority == th.priority ;
   } ;
   static unsigned char getProperty( char op )
   {
      static bool bInit = false ;
      if ( !bInit ) {
         _mapPriority.emplace( '+', 1 ) ;
         _mapPriority.emplace( '-', 1 ) ;
         _mapPriority.emplace( '*', 3 ) ;
         _mapPriority.emplace( '/', 3 ) ;
         _mapPriority.emplace( '^', 5 ) ;
      }
      return _mapPriority[op] ;
   }
} ;

std::map<char, unsigned char> Priority::_mapPriority ;

template <typename T1, typename T2>
bool operator<( T1 t1, T2 t2 )
{
   return t1 < t2 ;
}


ADDBEGIN
ADD( "calculator", calculator )
ADDEND

template <typename T1, typename T2>
auto calc( T1 t1, T2 t2, char op ) -> decltype(t1+t2)
{
   switch ( op ) {
   case '+' : 
      return t1 + t2 ;
   case '-':
      return t1 - t2 ;
   case '*':
      return t1 * t2 ;
   case '/':
      return t1 / t2 ;
   case '^':
      return powl( t1, t2 ) ;
   default:
      return 0 ;
   }
}

bool isOperator( const char op )
{

}

void infixing2prefix( char op, std::stack<DataOP> &op_stack, std::stack<DataOP> res_stack )
{

}

void infixing2prefix( std::string &&strExpr, std::string &strPrefix )
{
   std::stack<DataOP> op_stack ;
   std::stack<DataOP> res_stack ;
   std::string strElement ;
   // 从右往名扫描
   int length = strExpr.length() ;
   while ( --length >= 0 ) {
      if ( std::isdigit( strExpr[length] ) ) {
         strElement.clear() ;

      }
      strElement += strExpr[length] ;

   }
   

}

void Pubfunc::calculator( boost::shared_array<char> req_ptr, size_t )
{
   std::stringstream iostr( req_ptr.get() ) ;
   boost::property_tree::ptree pt ;
   boost::property_tree::read_json( iostr, pt ) ;
   auto strExpr = pt.get<std::string>( "expr" ) ;
   // 使用前缀表达式求值
   std::string strPrefix ;
   infixing2prefix( std::move(strExpr), strPrefix ) ;
}
