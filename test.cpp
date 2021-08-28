#include<iostream>
#include<random>
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

	cout<<endl;

}