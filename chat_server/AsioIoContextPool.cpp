#include "AsioIoContextPool.h"

#include <iostream>

AsioIoContextPool::~AsioIoContextPool()
{
	Tidy();
	std::cout << "asio io_context pool deconstructed" << std::endl;
}

AsioIoContextPool::IoService& AsioIoContextPool::GetIoService()
{
	IoService& service = io_services[ next_io_service ];
	next_io_service++;
	if ( next_io_service == io_services.size() )
		next_io_service = 0;
	return service;
}

void AsioIoContextPool::Tidy()
{
	// ����ֱ�ӵ��� work �� reset() ������������ʽ���� io_context �� stop() ����
	// ��Ҫ�ֶ�����ʽ���� io_context �� stop() ���� 
	//��ֻ���Ѿ�ע����д/���¼��� io_context ��Ҫ��Ϊ���ǵļ������񲻻������
	for ( auto& work : works )
	{
		IoService& service = work->get_executor().context();
		if ( !service.stopped() )
			service.stop(); // ʹ io_context �� run() ������������

		work.reset(); // ���
	}

	// �� thread �����е� io_context ���� run() ��ص��������Ѿ��������п���join
	// ��Ҳ��Ϊ�˷�ֹ threads ����ʱ�е��߳�û��ֹͣ����
	for ( std::size_t i = 0; i < threads.size(); i++ )
		threads[ i ].join();
}

AsioIoContextPool::AsioIoContextPool( std::size_t size )
	: io_services( size ), works( size ) /* �˴�����ʼ�� threads �Ĵ�С���������� emplace_back */
	, next_io_service( 0 )
{
	// �� io_context �󶨵���Ӧ�� executor_work_guard
	// executor_work_guard ���Ա�֤ io_context û���¼�ע��ʱ�������� run()
	for ( std::size_t i = 0; i < size; i++ )
	{
		Work work = boost::asio::make_work_guard( io_services[ i ] );
		works[ i ] = std::make_unique<Work>( std::move( work ) );
	}

	// ����ÿ�� io_service��������䵽һ���߳��п�ʼ����
	for ( std::size_t i = 0; i < io_services.size(); i++ )
	{
		threads.emplace_back(
			[this, i] ()
			{
				this->io_services[ i ].run();
			} );
	}
}
