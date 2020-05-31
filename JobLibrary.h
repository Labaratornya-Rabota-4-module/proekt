#pragma once
#include "Job.h"
#include "IntegrateJob.h"
#include "OdeParser.h"
#include "OdeSystems.h"
#include "KmeanJob.h"

//	АБСТРАКТНЫЙ ОБРАБОТЧИК

class Handler
{
protected:
	Handler* nextHandler;
public:
	virtual Job* handle(const std::string& tag, const std::filesystem::path& fileInName,const std::filesystem::path& fileOutName)
	{
		if (nextHandler)
			return nextHandler->handle(tag, fileInName, fileOutName);
		return nullptr;
	}
	virtual Handler* setNext(Handler* handler)
	{
		nextHandler = handler;
		return handler;
	}
};

// КОНКРЕТНЫЕ ОБРАБОТЧИКИ

class IntegrateLorenzAttractorHandler : public Handler
{
public:
	virtual Job* handle(const std::string& tag, const std::filesystem::path& fileInName, const std::filesystem::path& fileOutName) override
	{
		if (tag == "IntegrateLorenzAttractor")
		{
			return new IntegrateJob<LorenzAttractor, OdeParser>(std::ifstream(fileInName), std::ofstream(fileOutName));
		}
		else
		{
			return Handler::handle(tag, fileInName, fileOutName);
		}
	}
};

class IntegrateVanDerPolHandler : public Handler
{
public:
	virtual Job* handle(const std::string& tag, const std::filesystem::path& fileInName, const std::filesystem::path& fileOutName) override
	{
		if (tag == "IntegrateVanDerPol") // Если тэг подходит под задачу
		{
			return new IntegrateJob<VanDerPol, OdeParser>(std::ifstream(fileInName), std::ofstream(fileOutName));
			// Создаем и возвращаем объект
		}
		else
		{
			return Handler::handle(tag, fileInName, fileOutName);
			// Переходим к следующему обработчику
		}
	}
};

class IntegrateChuaChainHandler : public Handler
{
public:
	virtual Job* handle(const std::string& tag, const std::filesystem::path& fileInName, const std::filesystem::path& fileOutName) override
	{
		if (tag == "IntegrateChuaChain")
		{
			return new IntegrateJob<ChupChain, OdeParser>(std::ifstream(fileInName), std::ofstream(fileOutName));
		}
		else
		{
			return Handler::handle(tag, fileInName, fileOutName);
		}
	}
};

class KmeanHandler : public Handler
{
public:
	virtual Job* handle(const std::string& tag, const std::filesystem::path& fileInName, const std::filesystem::path& fileOutName) override
	{
		if (tag == "Kmean")
		{
			return new KmeanJob<Vector<double>>(std::ifstream(fileInName), std::ofstream(fileOutName));
		}
		else
		{
			return Handler::handle(tag, fileInName, fileOutName);
		}
	}
};

//	БИБЛИОТЕКА, КОТОРАЯ СОЗДАЕТ ЗАДAЧИ

// Класс, у которого можно попросить работу по тэгу
class JobLibrary
{
private:
	IntegrateLorenzAttractorHandler mainHandler;
	IntegrateVanDerPolHandler handler1;
	IntegrateChuaChainHandler handler2;
	KmeanHandler handler3;
public:
	JobLibrary()
	{
		mainHandler.setNext(&handler1)->setNext(&handler2)->setNext(&handler3);
	}

	inline Job* handle(const std::string& tag, const std::filesystem::path& fileInName, const std::filesystem::path& fileOutName)
	{
		return mainHandler.handle(tag, fileInName, fileOutName); // Обрабатываем запрос
	}
};
