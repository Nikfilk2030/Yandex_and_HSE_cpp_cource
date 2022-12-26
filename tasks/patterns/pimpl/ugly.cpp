#include "ugly.h"

#include "bad.h"

struct SplineImpl {
    SplineImpl(const std::vector<double>& x, const std::vector<double>& y) {
        n_ = x.size();
        x_ = x.data();
        y_ = y.data();
        SplineRes_ = new double[x.size()];
        InterpolateRes_ = new double;
    }

    ~SplineImpl() {
        delete[] SplineRes_;
        delete InterpolateRes_;
    }

    double* GetSplineRes() {
        return SplineRes_;
    }

    const double* GetX() {
        return x_;
    }

    const double* GetY() {
        return y_;
    }

    size_t GetN() {
        return n_;
    }

    double* GetInterpolateRes() {
        return InterpolateRes_;
    }

protected:
    size_t n_;
    const double* x_;
    const double* y_;
    double* SplineRes_;
    double* InterpolateRes_;
};

Spline::Spline(const std::vector<double>& x, const std::vector<double>& y, double a, double b) {
    impl_ = std::make_unique<SplineImpl>(x, y);
    mySplineSnd(x.data(), y.data(), x.size(), a, b, impl_->GetSplineRes());
}

double Spline::Interpolate(double x) {
    mySplintCube(impl_->GetX(), impl_->GetY(), impl_->GetSplineRes(), impl_->GetN(), x,
                 impl_->GetInterpolateRes());
    return *impl_->GetInterpolateRes();
}