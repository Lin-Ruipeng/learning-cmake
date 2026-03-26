#include <iostream>
#include <vector>

void process_sensor_data(const std::vector<double>& data) {
    double sum = 0;
    // 模拟一段耗时的核心算法逻辑
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i]; // 致命 Bug 藏在这里
    }
    std::cout << "Data sum: " << sum << std::endl;
}

int main() {
    // 模拟传入 5 个传感器数据
    std::vector<double> imu_data = {0.1, 0.5, 0.9, 1.2, 1.5};
    
    std::cout << "[System] Starting sensor processing..." << std::endl;
    process_sensor_data(imu_data);
    std::cout << "[System] Processing complete." << std::endl;
    
    return 0;
}

