#include "errors.hpp"
#include "thread_pool.hpp"
#include <functional>
#include <random>
#include <iostream>

class ProbabilityCalculator
{
	typedef std::function<void( const std::error_code& err)> Action; 
	Action action;
public:
	void operator() ( int probability);
	ProbabilityCalculator( Action _action) : action( _action) {};
};

void ProbabilityCalculator::operator() ( int probability)
{
	std::random_device rd;
	std::mt19937 gen( rd());
	std::uniform_int_distribution<> dist(1,100);

	std::error_code err;
	if ( probability <= dist( gen))
		err = xmpp_error::xmpp_error_t::forbidden;
	else
		err = xmpp_error::xmpp_error_t::ok;

	action( err);
}

std::mutex p_lock;

int main()
{
	ThreadPool pool( 4);

	int probability;

	ProbabilityCalculator error_calc( []( const std::error_code& err){
		std::lock_guard<std::mutex> lock( p_lock);
		std::cout << err.message() << std::endl;
	});

	do {
		std::cin >> probability;
		pool.post( std::bind( error_calc, probability));
	} while( probability > 0);

	error_calc( probability);
}