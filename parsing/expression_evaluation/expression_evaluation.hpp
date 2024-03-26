#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include <memory>

enum class DataType {
    INT,
    DOUBLE,
};

enum class ExpressionType {
    RAW,
    POWERSERIES
};

#define INT_TYPE int32_t

class DatatypeWrapper {
 public:
	virtual ~DatatypeWrapper() {};
    virtual std::unique_ptr<DatatypeWrapper> as_modlong(const ModLong value) const {
        assert(false);
    }
    virtual std::unique_ptr<DatatypeWrapper> as_double() const {
        assert(false);
    }

    virtual std::unique_ptr<DatatypeWrapper> as_int() const {
        assert(false);
    }

    virtual std::unique_ptr<DatatypeWrapper> as_power_series(const uint32_t size) const {
        assert(false);
    }

    virtual std::ostream & on_print(std::ostream & os) const = 0;
};

inline std::ostream& operator<< (std::ostream& os, const DatatypeWrapper& wrapper)
{
	return wrapper.on_print(os);
}

template<typename T>
class PowerSeriesWrapper: public DatatypeWrapper {
 protected:
    FormalPowerSeries<T> value;
 public:
    PowerSeriesWrapper(FormalPowerSeries<T> value): value(value) {}

    std::unique_ptr<DatatypeWrapper> as_power_series() {
        return std::unique_ptr<PowerSeriesWrapper>(value);
    }

    FormalPowerSeries<T> get_value() const {
    	return value;
    }
    virtual std::ostream & on_print(std::ostream & os) const {
    	os << value;
    	return os;
    }
};


template<typename T>
class ValueWrapper: public DatatypeWrapper {
 protected:
    T value;
 public:
    std::unique_ptr<DatatypeWrapper> as_power_series(const uint32_t size) const {
        return std::make_unique<PowerSeriesWrapper<T>>(FormalPowerSeries<T>::get_atom(value, 0, size));
    }

    T get_value() const {
    	return value;
    }

    virtual std::ostream & on_print(std::ostream & os) const {
    	os << value;
    	return os;
    }
};


class DoubleWrapper : public ValueWrapper<double> {
 public:
    DoubleWrapper(double value) {
        this->value = value;
    }

    std::unique_ptr<DatatypeWrapper> as_double() {
        return std::make_unique<DoubleWrapper>(value);
    }
};

class IntWrapper : public ValueWrapper<INT_TYPE> {
 public:
    IntWrapper(INT_TYPE value) {
        this->value = value;
    }  

    std::unique_ptr<DatatypeWrapper> as_int() {
        return std::make_unique<IntWrapper>(value);
    }

    std::unique_ptr<DatatypeWrapper> as_double() const {
        return std::make_unique<DoubleWrapper>(static_cast<double>(value));
    }
};

class DoublePowerSeriesWrapper : public PowerSeriesWrapper<double> {
 public:
    DoublePowerSeriesWrapper(FormalPowerSeries<double> value): PowerSeriesWrapper<double>(value) {}
};

class IntPowerSeriesWrapper : public PowerSeriesWrapper<INT_TYPE> {
    IntPowerSeriesWrapper(FormalPowerSeries<INT_TYPE> value): PowerSeriesWrapper<INT_TYPE>(value) {}

    std::unique_ptr<DatatypeWrapper> as_double() const {
        auto fps = FormalPowerSeries<double>::get_zero(0.0, value.num_coefficients());
        for (uint32_t i = 0; i < value.num_coefficients(); i++) {
            fps[i] = static_cast<double>(value[i]);
        }
        return std::make_unique<DoublePowerSeriesWrapper>(fps);
    }
};


