// Copyright 2023 Nesterov Alexander

#ifndef MODULES_CORE_TASK_HPP_
#define MODULES_CORE_TASK_HPP_

#include <cstdint>
#include <vector>
#include <memory>

namespace ppc {
namespace core {

struct TaskData {
    std::vector<uint8_t *> inputs;
    std::vector<std::uint32_t> inputs_count;
    std::vector<uint8_t *> outputs;
    std::vector<std::uint32_t> outputs_count;
};

// Memory of inputs and outputs need to be initialized before create object of Task class
class Task {
 public:
    explicit Task(std::shared_ptr<TaskData> taskData_);
    // set input and output data
    void set_data(std::shared_ptr<TaskData> taskData_);
    // pre-processing of input data
    virtual bool pre_processing() = 0;
    // validation of data and validation of task attributes before running
    virtual bool validation() = 0;
    // realization of current task
    virtual bool run() = 0;
    // post-processing of output data
    virtual bool post_processing() = 0;
    // this function should have performance measurement (in seconds) of run() function
    double perf();
    // get input and output data
    std::shared_ptr<TaskData> get_data() const;

 protected:
    std::shared_ptr<TaskData> taskData;
};

}  // namespace core
}  // namespace ppc

#endif  // MODULES_CORE_TASK_HPP_
