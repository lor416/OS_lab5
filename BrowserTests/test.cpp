#include "pch.h"
#include "MathUtils.h"


TEST(PositiveTests, Test_1_1_BasicMath) {
    Task task;
    task.size = 3;
    task.data[0] = 5.0; task.data[1] = 2.0; task.data[2] = 8.0;
    Result res = CalculateDFT(task);
    // При k=0 результат всегда равен сумме
    EXPECT_NEAR(res.real[0], 15.0, 0.0001);
}

TEST(PositiveTests, Test_1_2_ConstantSignal) {
    Task task;
    task.size = 4;
    for (int i = 0; i < 4; i++) 
        task.data[i] = 1.0; 
    Result res = CalculateDFT(task);
    // Сумма должна быть 4.0
    EXPECT_EQ(res.size, 4);
    EXPECT_NEAR(res.real[0], 4.0, 0.0001);
}

TEST(PositiveTests, Test_1_3_ExitType) {
    Task task;
    task.type = TASK_EXIT;
    EXPECT_EQ(task.type, TASK_EXIT);
}

TEST(NegativeTests, Test_2_1_EmptyData) {
    Task emptyTask;
    emptyTask.type = TASK_COMPUTE;
    emptyTask.size = 0;
    Result res = CalculateDFT(emptyTask);
    EXPECT_EQ(res.size, 0);
}