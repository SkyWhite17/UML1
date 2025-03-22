#include<iostream>
#include<thread>
#include<chrono>
using namespace std::literals::chrono_literals;

using std::cin;
using std::cout;
using std::endl;


bool finish = false;

void Plus()
{
	while (!finish)
	{
		cout << "+ ";
		std::this_thread::sleep_for(100ms);
	}
}
void Minus()
{
	while (!finish)
	{
		cout << "- ";
		std::this_thread::sleep_for(100ms);

	}
}

void main() {
	setlocale(0, "");
	//Plus();
	//Minus();
	//std::thread thread_name(FunctionName);
	//thread_name.join();

	std::thread plus_thread(Plus);
	std::thread minus_thread(Minus);

	cin.get();
	finish = true;

	if (plus_thread.joinable())plus_thread.join();
	if (minus_thread.joinable())minus_thread.join();
}