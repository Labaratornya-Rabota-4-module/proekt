#pragma once
#include <filesystem>
#include <string>
#include "JobLibrary.h"

// Функция которая возвращает тэг задачи
std::string identify_tag(std::ifstream& file)
{
	std::string tag;
	file.seekg(0);
	file >> tag;

	size_t open_bracket = tag.find("<"); // Ищем границы тега
	size_t close_bracket = tag.find(">");

	if (open_bracket != std::string::npos && close_bracket != std::string::npos && open_bracket < close_bracket) // Если нашли все скобки
	{
		size_t length = close_bracket - open_bracket; // Не может быть 0 по условию
		tag = tag.substr(open_bracket + 1, length - 1); // Извлекаем подстроку (тэг)

		return tag;
	}

	return std::string(); // Если неизвестный тип задания возвращаем пустую строку
}



// Класс который содержит и генерирует задачи
class JobKeeper
{
private:
	std::filesystem::directory_iterator currentEntry; // Текущая директори \ файл
	std::filesystem::path destinationFolderPath; // Путь до папки в которую необходимо записать ответ
	JobLibrary jobLibrary; // Библиотека которая предоставляет задачи
	size_t jobNumber; // Номер задачи
public:
	JobKeeper(const std::filesystem::path&, const std::filesystem::path&);
	Job* getJob(); // Передает ответственность за удаление вызвавшему объекту
private:
	std::wstring generateName(size_t);
};

JobKeeper::JobKeeper(const std::filesystem::path& folder_from, const std::filesystem::path& folder_to) :
	currentEntry(std::filesystem::directory_iterator(folder_from)), // Считаем что переданная директория существует
	destinationFolderPath(folder_to), // Считаем что директория существует
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
			std::string tag;
			{
				std::ifstream file(currentEntry->path()); // Открываем его на чтение
				tag = identify_tag(std::ref(file)); // Опеределяем тип задачи в файле
			} // Далее файл закрывается

			std::filesystem::path fileOutName = destinationFolderPath;
			fileOutName /= generateName(jobNumber); // Генерируем полное имя файла в который необходимо записать решение
				
			job = jobLibrary.handle(tag, currentEntry->path(), fileOutName); // У библиотеки запрашиваем объект по тэгу

			/*std::cout << "JobKeeper Log:" << std::endl;
			std::cout << "tag: " << tag << std::endl;
			std::cout << "fileInName: " << currentEntry->path() << std::endl;
			std::cout << "fileOutName: " << fileOutName << std::endl;
			std::cout << "job: " << job << std::endl;*/

			if (job != nullptr) // Если библиотека вернула работу
			{
				++currentEntry; // Переходим к следующему объекту в директории (папка или файл)
				++jobNumber; // Увеличиваем количество сгенерированных задача
				break;
			}// Иначе переходим к другому файлу/папке
		}
		++currentEntry;
	}

	return job; // Возвращаем работу
}

std::wstring JobKeeper::generateName(size_t number) // Возвращает двубайтовую версию строки
{
	return std::wstring(L"answer_" + std::to_wstring(number) + L".txt"); // Генерируем имя
}
