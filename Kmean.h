#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <iostream>



/* Необходимые операции для типа Т
 T += T
 T / size_t -> return T
 T = T
 T = 0
 */

// Алгоритм k-средних
template <typename T>
class Kmean
{
private:
    static double default_distance(const T& a, const T& b)
    { return std::abs(a - b); }
    
    double(*distance)(const T&, const T&); // Функция которая считает расстояние
    size_t clusterNumber; // Количество кластеров (и соответственно центров)
    std::vector<T> currentCenters; // Текущие центры
    std::vector< std::vector<T> > currentClusters; // Размеры кластеров не равны!
    const std::vector<T>& elementSet;
    
    static const size_t MAX_ITERATION_NUMBER = 30; // Максимальное количество итераций
public:
    // Необходимо убедиться что cluster_number > 0 и initial_data содержат хотя бы один элемент
    Kmean(const std::vector<T>& initial_data, size_t cluster_number, double(*dist)(const T&, const T&) = &Kmean::default_distance) :
    elementSet(initial_data), clusterNumber(cluster_number),
    currentCenters(cluster_number), currentClusters(cluster_number), distance(dist)
    { }
    
    void initialiseCenters()
    {
        std::set<size_t> indexes;
        size_t elementSetSize = elementSet.size();
        // Считаем что количество центров меньше чем количество элементов
        while (indexes.size() < clusterNumber)
            indexes.insert( (rand() % elementSetSize) );
        
        size_t centerIndex = 0;
        for (auto iter = indexes.begin(); iter != indexes.end(); ++iter, ++centerIndex)
            currentCenters[centerIndex] = elementSet[*iter];
    }
    
    T calculateMean(const std::vector<T>& cluster)
    {
        T res = cluster[0];
        
        for (size_t i = 1; i < cluster.size(); ++i)
            res += cluster[i];
        
        return res / (double)cluster.size(); // Может ли кластер быть пустым?
    }
    
    void updateCurrentClusters()
    {
        for (size_t i = 0; i < currentClusters.size(); ++i) // Очищаем кластеры
            currentClusters[i].resize(0);
        // Дорогая операция
        
        for (size_t i = 0; i < elementSet.size(); ++i) // Проходимся по всем точкам из выборки
        {
            double minDistance = distance(elementSet[i], currentCenters[0]); // Выбираем начальное минимальное расстояние
            size_t minCenter = 0;
            
            for (size_t c = 1; c < currentCenters.size(); ++c) // Ищем ближайший центр
            {
                double curDistance = distance(elementSet[i], currentCenters[c]); // Расстояние до текущего центра
                if (curDistance < minDistance)
                {
                    minCenter = c;
                    minDistance = curDistance;
                }
            }
            
            currentClusters[minCenter].push_back(elementSet[i]);
        }
    }
    
    double calculateDifferenceBetweenCenters(const std::vector<T>& a, const std::vector<T>& b)
    {
        double difference = 0.0;
        for (size_t i = 0; i < a.size(); ++i)
            difference += distance(a[i], b[i]);
        
        return difference;
    }
    
    void completeAlgorithm()
    {
        initialiseCenters();
        updateCurrentClusters();
        
        for (size_t n = 0; n < Kmean::MAX_ITERATION_NUMBER; ++n) // Пока
        {
            std::vector<T> newCenters(clusterNumber);
            for (size_t i = 0; i < clusterNumber; ++i)
                newCenters[i] = calculateMean(currentClusters[i]);
            
            if (calculateDifferenceBetweenCenters(currentCenters, newCenters) < 0.3) break; // Если новые центры мало отлиаются от старых
            currentCenters = newCenters;
            updateCurrentClusters();
        }
    }
    
    void printIn(std::ostream& stream)
    {
        for (size_t i = 0; i < clusterNumber; ++i)
        {
            stream << "cluster " << i << " with center (" << currentCenters[i] << ')' << std::endl;
            for (size_t j = 0; j < currentClusters[i].size(); ++j)
                stream << '(' << currentClusters[i][j] << ')' << std::endl;
        }
    }
};

