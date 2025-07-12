#include <vector>
#ifndef MODEL_H
#define MODEL_H
class LogisticRegressionModel{
    std::vector<double> coefficient_list;
    double intercept;
    double buy_amount;

    public:
    void read_intercept_coefficient_file(std::string& text_file);
    LogisticRegressionModel(std::string text_file);
    void print_intercept_coefficient();
    double predict_probability(std::vector<long double>& inference_vector);
};

#endif

