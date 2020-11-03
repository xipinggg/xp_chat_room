#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include "TowBuffer.h"
#include "ObjectPool.h"
using namespace std;
class A
{
public:
	A(int i){n = i;}
	A(){}
	void set(int i){n = i;}
private:
	int n;
};
int main()
{
	ObjectPool<A> pool;
	vector<shared_ptr<A>> v;
	int i=5;
	while(i-- > 0)
	{
		cout<<pool.size()<<" "<<pool.totalSize()<<endl;
		v.emplace_back(pool.get(0));
	}
	for(auto i:v)
		pool.recycle(i);
	v.clear();
	cout<<"---\n";
	while(i++ < 5)
	{
		cout<<pool.size()<<" "<<pool.totalSize()<<endl;
		v.emplace_back(pool.get(2));
	}
	
} 

