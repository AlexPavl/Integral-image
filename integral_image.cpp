/*! \file integral_image.cpp 
    \brief Приложение по расчету интегральных изображений
*/ 
#include "integral_image.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <experimental/filesystem>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

ThreadCounter threadCounter;

/**
     * Фукнция расчета одноканального интегрального изображения
     * \param[in] channel Одноканальное изображение
     * \return Одноканальное интегральное изображение
*/
IntegralImage getSingleChannelIntegralImage(cv::Mat &channel)
{
    IntegralImage resImage(channel.rows, std::vector<double>(channel.cols, 0.0));
    for (size_t i = 0; i < channel.rows; ++i)
    {
        channel.row(i).copyTo(resImage[i]);
    }
    for (size_t i = 1; i < resImage.size(); ++i)
    {
        resImage[i][0] += resImage[i - 1][0];
    }
    for (size_t i = 1; i < resImage[0].size(); ++i)
    {
        resImage[0][i] += resImage[0][i - 1];
    }
    for (size_t i = 1; i < resImage.size(); ++i)
    {
        for (size_t j = 1; j < resImage[i].size(); ++j)
        {
            resImage[i][j] += resImage[i - 1][j] + resImage[i][j - 1] - resImage[i - 1][j - 1];
        }
    }
    return resImage;
}

/**
     * Функция расчета многоканального интегрального изображения
     * \param[in] image Возможное многоканальное или одноканальное изображение
     * \return Многоканальное интегральное изображение
*/
std::map<int, IntegralImage> getIntegralImage(cv::Mat &&image)
{
    std::map<int, IntegralImage> result;
    cv::Mat channels[image.channels()];
    cv::split(image, channels);
    for (int i = 0; i < image.channels(); ++i)
    {
        result.emplace(i, getSingleChannelIntegralImage(channels[i]));
    }
    return std::move(result);
}

/**
     * Функция проверяет возможность чтения изображения, если изображение считывается нормально - то расчитываем его
     * интегральное изображение и сохраняем в тектовый документ с постфиксом .integral
     * \param[in] name Имя изображения
     * \param[in] isNewThread Флаг нового потока (нужно чтобы под конец работы потока уменьшить счетчик)
*/
void checkImageAndSaveIntegral(std::string name, bool isNewThread)
{
    try
    {
        cv::Mat image;
        image = cv::imread(name, cv::IMREAD_UNCHANGED);
        if (image.empty())  /// Проверка наличия каналов изображения
        {
            throw std::invalid_argument("Can't read image " + name);
        }
        auto integralImage = getIntegralImage(std::move(image));
        std::ofstream outputFile(name + ".integral");
        outputFile.precision(1);
        for (auto channel : integralImage)
        {
            for (auto row : channel.second)
            {
                for (auto col : row)
                {
                    outputFile << std::fixed << col << " ";
                }
                outputFile << std::endl;
            }
            outputFile << std::endl;
        }
        outputFile.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    if (isNewThread) /// Эта часть должна выполняться независимо от основной части функции
    {
        threadCounter.decreaseCounter();
    }
}

/**
 * Функция проверяет существует ли файл с переданным именем, делается это в основном потоке, так как проверка происходит
 * относительно быстро, чем сразу вызывать дополнительный поток и в нем узнать что файла не существут
 * \param[in] name Имя изображения
 */
void processImage(std::string &name)
{
    try
    {
        if (std::experimental::filesystem::exists(name))
        {
            ///< Если есть возможность создать новый поток - создаем, иначе исполняем функцию в главном потоке
            if (threadCounter.createNewThread())
            {
                std::thread(checkImageAndSaveIntegral, name, true).detach();
            }
            else
            {
                checkImageAndSaveIntegral(name, false);
            }
        }
        else
        {
            throw std::invalid_argument("Image name " + name + " doesn't exist");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

#ifndef TESTING
int main(int argc, char** argv)
{
    std::vector<std::string> imageNames;
    int numberOfThreads = static_cast<int>(std::thread::hardware_concurrency()); /// Если нет -t - устанавливаем максимальное хардкорное значение
    int opt;
    while ((opt = getopt(argc, argv, "t:i:")) != -1)
    {
        switch (opt)
        {
            case 't':
                {
                    std::string threadsNum = optarg;
                    if (std::all_of(threadsNum.begin(), threadsNum.end(), ::isdigit))
                    {
                        int newNumOfThreads = std::stoi(threadsNum);
                        if ((newNumOfThreads != 0) && (newNumOfThreads < numberOfThreads))
                        {
                            numberOfThreads = newNumOfThreads;
                        }
                    }
                    else
                    {
                        std::cout << "Invalid threads number parameter " << optarg << std::endl;
                        return 0;
                    }
                }
                break;
            case 'i':
                imageNames.emplace_back(optarg);
                break;
            default:
                std::cout << "Incorrect option " << opt << std::endl;
                return 0;
                break;
        }
    }
    /**
     * в setMaximumThreadCounter передаем -1 поток, учитывая при этом текущий поток, тем самым счетчик становится 
     * счетчиком не всех потоков а дополнительных
     */
    threadCounter.setMaximumThreadCounter(numberOfThreads - 1);
    for (auto &imageName : imageNames)
    {
        processImage(imageName);
    }
    while (threadCounter.isTheadsRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); /// Для безопасного завершения приложения
    }
    return 0;
}
#endif // not TESTING