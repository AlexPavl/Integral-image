#include "./../integral_image.hpp"
#include <gtest/gtest.h>

TEST(thread_counter, test_default_increase_prohibition)
{
    ThreadCounter tc;
    ASSERT_FALSE(tc.createNewThread());
}

TEST(thread_counter, test_maximum_threads_setting)
{
    int maxThreads = 10;
    ThreadCounter tc;
    tc.setMaximumThreadCounter(maxThreads);
    for (int i = 0; i < maxThreads; ++i)
    {
        ASSERT_TRUE(tc.createNewThread());
    }
}

TEST(thread_counter, test_max_counter_increasing)
{
    int maxThreads = 8;
    ThreadCounter tc;
    tc.setMaximumThreadCounter(maxThreads);
    for (int i = 0; i < maxThreads; ++i)
    {
        ASSERT_TRUE(tc.createNewThread());
    }
    ASSERT_FALSE(tc.createNewThread()); // threads limit is reached
    tc.decreaseCounter();
    ASSERT_TRUE(tc.createNewThread()); // after decreasing one more thread is available
    ASSERT_FALSE(tc.createNewThread());
}

TEST(thread_counter, check_thread_running_flag)
{
    ThreadCounter tc;
    tc.setMaximumThreadCounter(2); // maximum value is not important
    ASSERT_TRUE(tc.createNewThread());
    ASSERT_TRUE(tc.isTheadsRunning());
    tc.decreaseCounter();
    ASSERT_FALSE(tc.isTheadsRunning());
}

TEST(integral_image, check_single_channel_integral_image_calculating)
{
    uint8_t greyArr[4][1] = {
        { 1 },
        { 2 },
        { 3 },
        { 4 }
    };
    cv::Mat img = cv::Mat(4, 1, CV_8U, &greyArr);
    auto result = getSingleChannelIntegralImage(img);
    ASSERT_EQ(10, result.at(3).at(0)); // check the last element which contain summ of all elements
}

TEST(integral_image, check_triple_channels_integral_image_calculating)
{
    uint8_t redArr[1][4] = { {1, 2, 3, 4} };
    uint8_t greenArr[1][4] = { {5, 6, 7, 8} };
    uint8_t blueArr[1][4] = { {9, 10, 11, 12} };
    std::vector<cv::Mat> channels;
    channels.push_back(cv::Mat(1, 4, CV_8U, &redArr));
    channels.push_back(cv::Mat(1, 4, CV_8U, &greenArr));
    channels.push_back(cv::Mat(1, 4, CV_8U, &blueArr));
    cv::Mat combined;
    cv::merge(channels, combined);
    cv::Mat copy;
    combined.copyTo(copy);
    auto result = getIntegralImage(std::move(copy));
    ASSERT_EQ(10, result[0][0][3]);
    ASSERT_EQ(26, result[1][0][3]);
    ASSERT_EQ(42, result[2][0][3]);
}

#ifdef TESTING
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif // TESTING