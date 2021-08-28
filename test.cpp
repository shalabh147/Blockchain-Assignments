#include<iostream>
#include<random>
#include<set>
using namespace std;


int main()
{
		std::random_device rd; 

	std::exponential_distribution<> rng (1/0.5);
	std::mt19937 rnd_gen (rd ());

	for(int i=0;i<5;i++)
	{
		cout<<rng (rnd_gen)<<" ";
	}

	set<int> s;
	s.insert(5);
	for(auto i: s)
	{
		cout<<i<<endl;
	}
	cout<<endl;

}