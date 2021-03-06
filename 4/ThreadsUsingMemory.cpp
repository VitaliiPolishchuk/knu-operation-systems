// OperationSystemLab4Part2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>

std::mutex g_lock;

const int N = 100;

void threadFunctionWithoutLock(int &a)
{
	for (int i = 0; i < N; ++i) {
		int b = a + 1;
		Sleep(1);
		a = b;
	}
}

void threadFunctionWithLock(int &a)
{
	for (int i = 0; i < N; ++i) {
		g_lock.lock();
		int c = a + 1;
		Sleep(1);
		a = c;
		g_lock.unlock();
	}
}

int main()
{
	int a = 0;
	std::thread thr1(threadFunctionWithoutLock, std::ref(a));
	std::thread thr2(threadFunctionWithoutLock, std::ref(a));
	thr1.join();
	thr2.join();
	std::cout << "Without locks" << std::endl;
	std::cout << a << std::endl;




	int b = 0;
	std::thread thr3(threadFunctionWithLock, std::ref(b));
	std::thread thr4(threadFunctionWithLock, std::ref(b));
	thr3.join();
	thr4.join();
	std::cout << "With locks" << std::endl;
	std::cout << b << std::endl;
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
