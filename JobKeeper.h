#pragma once
#include "Job.h"
#include <filesystem>
#include <string>
#include "IntegrateJob.h"
#include "OdeParser.h"
#include "OdeSystems.h"


// Типы задач
enum JobType
{
	IntegrateVanDerPol,
	IntegrateLorenzAttractor,
	Kmean,
	Undefine
};

// Функция, которая определяет тип задачи
JobType identify_type(std::ifstream& file)
{
	std::string tag;
	file.seekg(0);
	file >> tag;

	size_t open_bracket = tag.find("<"); // Ищем границы тега
	size_t close_bracket = tag.find(">");

	if (open_bracket != std::string::npos && close_bracket != std::string::npos && open_bracket < close_bracket) // Если нашли все скобки
	{
		size_t length = close_bracket - open_bracket; // Не может быть 0 по условию
		tag = tag.substr(open_bracket + 1, length - 1); // Извлекаем подстроку

		if (tag == "Kmean") return JobType::Kmean; // В зависимости от тега определяем тип работы
		if (tag == "IntegrateVanDerPol") return JobType::IntegrateVanDerPol;
		if (tag == "IntegrateLorenzAttractor") return JobType::IntegrateLorenzAttractor;
	}

	return JobType::Undefine; // Если неизвестный тип задания
}

// Класс который содержит и генерирует задачи
class JobKeeper
{
private:
	std::filesystem::directory_iterator currentEntry; // Текущая директория \ файл
	std::filesystem::path destinationFolderPath; // Путь до папки в которую необходимо записать ответ
	size_t jobNumber; // Номер задачи
public:
	JobKeeper(const std::filesystem::path&, const std::filesystem::path&);
	Job* getJob(); // Передает ответственность за удаление вызвавшему объекту
private:
	std::wstring generateName(size_t);
};

JobKeeper::JobKeeper(const std::filesystem::path& folder_from, const std::filesystem::path& folder_to) :
	currentEntry(std::filesystem::directory_iterator(folder_from)), // Считаем, что переданная директория существует
	destinationFolderPath(folder_to), // Считаем, что директория существует
	jobNumber(0)
{ }

Job* JobKeeper::getJob()
{
	Job* job = nullptr;
	while (1)
	{
		if (currentEntry == std::filesystem::directory_iterator()) // Если прошли всю директорию
			break;
		if (currentEntry->is_regular_file()) // Если текущий итератор указывает на обычный файл
		{
			std::ifstream file(currentEntry->path()); // Открываем его на чтение

			JobType job_type = identify_type(std::ref(file)); // Опеределяем тип задачи в файле

			if (job_type != JobType::Undefine) // Если тип определился
			{
				++jobNumber;
				std::filesystem::path fileOutName = destinationFolderPath;
				fileOutName /= generateName(jobNumber); // Генерируем полное имя файла, в который необходимо записать решение

				switch (job_type) // В зависимости от типа
				{
				case JobType::IntegrateLorenzAttractor: // Создаем задачу Аттрактор Лоренца
					job = new IntegrateJob<LorenzAttractor, OdeParser>(std::move(file), std::ofstream(fileOutName));
					break;
				case JobType::IntegrateVanDerPol: // Создаем задачу Ван Дер Поля
					job = new IntegrateJob<VanDerPol, OdeParser>(std::move(file), std::ofstream(fileOutName));
					break;
				case JobType::Kmean:
					break;
				default:
					break;
				}
			}
			++currentEntry; // Передвигаем итератор
			break;
		}
		++currentEntry;
	}
	return job; // Возвращаем работу
}

std::wstring JobKeeper::generateName(size_t number) // Возвращает двубайтовую версию строки
{
	return std::wstring(L"answer_" + std::to_wstring(number) + L".txt"); // Генерируем имя
}
