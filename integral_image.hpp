/*! \file integral_image.hpp 
    \brief Хэдер с объявлениями функций и классом счетчиком
*/ 
#ifndef INTEGRAL_IMAGE_H
#define INTEGRAL_IMAGE_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <mutex>
#include <map>
#include <vector>

using IntegralImage = std::vector<std::vector<double>>;

/**
    Класс счетчика потоков для ограничения максимального количества запускаемых потоков.                 
 */
class ThreadCounter
{
    public:
    /**
     * Метод для установки максимального количества потоков
     * \param[in] _maxCounter Максимальное количество дополнительных потоков
    */
    void setMaximumThreadCounter(int _maxCounter)
    {
        maxCounter = _maxCounter;
    }

    /**
     * Метод проверяет возможность выставления нового потока.
     * Если возможность добавить новый поток есть - увеличиваем счетчик
    */
    bool createNewThread()
    {
        bool res = false;
        std::lock_guard<std::mutex> lock(mtx);
        if (counter < maxCounter)
        {
            ++counter;
            res = true;
        }
        return res;
    }

    /**
     * Метод уменьшает текущее значение счетчика потоков
    */
    void decreaseCounter()
    {
        std::lock_guard<std::mutex> lock(mtx);
        --counter;
    }

    /**
     * Метод для проверки что дополнительные потоки все еще идут
     */ 
    bool isTheadsRunning()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return counter > 0;
    }

    private:
    int maxCounter = 0; ///< Для записи максимального количества потоков
    int counter = 0; ///< Текущий счетчик потоков
    mutable std::mutex mtx;
};

IntegralImage getSingleChannelIntegralImage(cv::Mat &channel);

std::map<int, IntegralImage> getIntegralImage(cv::Mat &&image);

void checkImageAndSaveIntegral(std::string name, bool isNewThread);

void processImage(std::string &name);

#endif // INTEGRAL_IMAGE_H