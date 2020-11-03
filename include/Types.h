#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>
//#define NODEBUG
#ifndef NODEBUG
#define debug() \
std::cout<<__FILE__<<" : "\
<<__func__<<"---------"<<__LINE__<<"\n"
#else
#define debug()
#endif//#ifndef NODEBUG

#endif//#ifndef _TYPES_H_