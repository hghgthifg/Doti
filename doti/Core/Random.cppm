export module Core.Random;

import std;

/*!
 * @brief A thread-safe random number generator class with static methods.
 *
 * This class provides static methods to generate various types of random numbers.
 * It uses thread-local storage to ensure thread safety in multi-threaded environments.
 */
export class Random {
public:
    /*!
     * @brief Generates a random integer within the specified range [min, max].
     *
     * @param min The lower bound of the range (inclusive).
     * @param max The upper bound of the range (inclusive).
     * @return A random integer between min and max.
     *
     * @note Uses a uniform integer distribution.
     */
    static int randInt(int min = 0, int max = 100) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random floating-point number within the specified range [min, max).
     *
     * @param min The lower bound of the range (inclusive).
     * @param max The upper bound of the range (exclusive).
     * @return A random float between min (inclusive) and max (exclusive).
     *
     * @note Uses a uniform real distribution.
     */
    static float randFloat(float min = 0.0f, float max = 1.0f) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random double-precision floating-point number within the specified range [min, max).
     *
     * @param min The lower bound of the range (inclusive).
     * @param max The upper bound of the range (exclusive).
     * @return A random double between min (inclusive) and max (exclusive).
     *
     * @note Uses a uniform real distribution.
     */
    static double randDouble(double min = 0.0, double max = 1.0) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random boolean value with the specified probability of being true.
     *
     * @param probability The probability of the generated boolean being true (default is 0.5).
     * @return A random boolean value where true occurs with the given probability.
     *
     * @note Uses a Bernoulli distribution.
     */
    static bool randBool(double probability = 0.5) {
        std::bernoulli_distribution dist(probability);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random value based on a normal (Gaussian) distribution.
     *
     * @param mean The mean (average) of the distribution.
     * @param stddev The standard deviation of the distribution.
     * @return A random double following a normal distribution with the specified mean and standard deviation.
     *
     * @note Uses a normal distribution.
     */
    static double randNormal(double mean = 0.0, double stddev = 1.0) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random value based on an exponential distribution.
     *
     * @param lambda The rate parameter (lambda) of the distribution.
     * @return A random double following an exponential distribution with the specified rate parameter.
     *
     * @note Uses an exponential distribution.
     */
    static double randExponential(double lambda = 1.0) {
        std::exponential_distribution<double> dist(lambda);
        return dist(getEngine());
    }

    /*!
     * @brief Generates a random integer from a list of values based on weighted probabilities.
     *
     * @tparam T The type of the values in the list.
     * @param values A vector of possible integer values to choose from.
     * @param weights A vector of weights corresponding to each value, determining their selection probability.
     * @return A randomly selected integer from the values vector based on the provided weights.
     *
     * @throws std::invalid_argument If the sizes of values and weights do not match or if they are empty.
     *
     * @note Uses a discrete distribution.
     */
    template<typename T>
    static T randWeightedInt(const std::vector<T>& values, const std::vector<double>& weights) {
        if (values.size() != weights.size() || values.empty()) {
            // TODO: Use Core.Logger instead of using throw (Currently Core.Logger is not thread-safe).
            throw std::invalid_argument("Values and weights must have the same non-zero size.");
        }

        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        int                             index = dist(getEngine());
        return values[index];
    }

private:
    /*!
     * @brief Retrieves the thread-local random number engine.
     *
     * @return A reference to the thread-local std::mt19937 engine.
     *
     * @note Each thread gets its own random engine to ensure thread safety.
     */
    static std::mt19937& getEngine() {
        thread_local std::mt19937 engine{std::random_device{}()};
        return engine;
    }
};
