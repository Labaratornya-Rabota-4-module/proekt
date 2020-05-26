#pragma once
#include <map>
#include <string>
#include <fstream>



// Класс который разбирает начальные данные для ОДУ
class OdeParser
{
private:
public:
    OdeParser();
    std::map<std::string, double> operator() (std::ifstream&); // Перегруженный оператор
};

OdeParser::OdeParser()
{ }

// Функция которая создает массив ключ-значение
std::map<std::string, double> OdeParser::operator() (std::ifstream& file)
{
    std::map<std::string, double> map;
    std::string pName;
    std::string pValue;
    
    while (1)
    {
        std::getline(file, pName); // Берем строку файла
        
        size_t delimiter_position = pName.find('=');
        if (delimiter_position != 0 && delimiter_position != std::string::npos) // Если корректный формат
        {
            pValue = pName.substr(delimiter_position + 1);
            pName = pName.substr(0, delimiter_position);
            
            map[pName] = std::atof(pValue.c_str()); // Если не смогли привести к типу, то записывем ноль
        }
        
        if (file.eof()) break;
    }
    
    return map; // Возвращаем ассоциативный объект
}
