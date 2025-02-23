#pragma once

#include <memory>
#include <mutex>
#include <iostream>

template< class T >
class Singleton
{
public:
	static std::shared_ptr<T>& GetInstance()
	{
		static std::once_flag flag;
		std::call_once( flag, [] () { instance = std::shared_ptr<T>( new T ); } );
		return instance;
	}

public:
	Singleton( const Singleton& ) = delete;
	Singleton& operator=( const Singleton& ) = delete;

	virtual ~Singleton()
	{
		std::cout << "a singleton is deconstructed" << std::endl;
	}

	void PrintAddress() const
	{
		std::cout << "singleton addr: " << instance.get() << std::endl;
	}

protected:
	Singleton() = default;

protected:
	static std::shared_ptr<T> instance;
};

template< class T >
std::shared_ptr<T> Singleton<T>::instance = nullptr;