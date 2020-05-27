#pragma once
#include "JobKeeper.h"
#include <mutex>
#include <thread>



// Класс который выполняет задания
class JobMaker
{
private:
	const size_t threadNumber; // Количество потоков
	std::mutex jobKeeperAccsess; // Доступ к пулу задач
	std::vector<std::thread> threadPool; // Пул потоков
public:
	JobMaker(size_t number) : threadNumber(number ? number : 2),
		threadPool(threadNumber)
	{ }

	void makeJobs(JobKeeper* job_keeper)
	{
		if (job_keeper)
		{
			for (size_t i = 0; i < threadNumber; ++i)
				threadPool[i] = std::thread(JobMaker::loop_function, job_keeper, std::ref(jobKeeperAccsess)); // Создаем пул потоков

			for (size_t i = 0; i < threadNumber; ++i)
				threadPool[i].join(); // Ждем когда все потоки закончат работу
		}
	}
private:
	static void loop_function(JobKeeper* job_keeper, std::mutex& access) // Функция которая выполняет задачи
	{
		Job* job;

		while (true)
		{
			{
				std::lock_guard<std::mutex> guard(access); // Получаем доступ и блокируем пул задач
				job = job_keeper->getJob(); // Получаем работу
			}

			if (job == nullptr) break; // Если работ нет то выходим
			job->makeJob(); // Выполняем работу
			delete job; // Удаляем работу
		}
	}
};
