#ifndef _Common_Core_Core_h_
#define _Common_Core_Core_h_

#include <Core/Core.h>

NAMESPACE_UPP

class OnlineAverageWindow1 : public Moveable<OnlineAverageWindow1> {
        Vector<double> win_a;
        double sum_a = 0.0;
        int period = 0, cursor = 0;
        int count = 0;

public:
        OnlineAverageWindow1() {}
        void SetPeriod(int i) {period = i; win_a.SetCount(i,0); cursor = 0;}
        void Add(double a) {
                double& da = win_a[cursor];
                sum_a -= da;
                da = a;
                sum_a += da;
                count++;
                cursor = (cursor + 1) % period;
        }
        double GetMean() const {return sum_a / (double)min(count, period);}
        double GetSum() const {return sum_a;}
        int GetPeriod() const {return period;}
        void Serialize(Stream& s) {s % win_a % sum_a % period % cursor % count;}
        const Vector<double>& GetWindow() const {return win_a;}
};

class OnlineAverageWindow2 : public Moveable<OnlineAverageWindow2> {
        Vector<double> win_a, win_b;
        double sum_a = 0.0, sum_b = 0.0;
        int period = 0, cursor = 0;

public:
        OnlineAverageWindow2() {}
        void SetPeriod(int i) {period = i; win_a.SetCount(i,0); win_b.SetCount(i,0);}
        void Add(double a, double b) {
                double& da = win_a[cursor];
                double& db = win_b[cursor];
                sum_a -= da;
                sum_b -= db;
                da = a;
                db = b;
                sum_a += da;
                sum_b += db;
                cursor = (cursor + 1) % period;
        }
        double GetMeanA() const {return sum_a / period;}
        double GetMeanB() const {return sum_b / period;}
        void Serialize(Stream& s) {s % win_a % win_b % sum_a % sum_b % period % cursor;}
};

struct OnlineAverage2 : public Moveable<OnlineAverage2> {
        double mean_a, mean_b;
        int64 count;
        OnlineAverage2() : mean_a(0), mean_b(0), count(0) {}
        void Add(double a, double b) {
                if (count == 0) {
                        mean_a = a;
                        mean_b = b;
                } else {
                        double delta_a = a - mean_a; mean_a += delta_a / count;
                        double delta_b = b - mean_b; mean_b += delta_b / count;
                }
                count++;
        }
        void Serialize(Stream& s) {s % mean_a % mean_b % count;}
};

struct OnlineAverage1 : public Moveable<OnlineAverage1> {
        double mean;
        int64 count;
        OnlineAverage1() : mean(0), count(0) {}
        void Clear() {mean = 0.0; count = 0;}
        void Add(double a) {
                if (count == 0) {
                        mean = a;
                } else {
                        double delta = a - mean;
                        mean += delta / count;
                }
                count++;
        }
        double GetMean() const {return mean;}
        void Serialize(Stream& s) {s % mean % count;}
};

// StandardNormalCDF and NormalCDF are declared in ExtMath.h and implemented in ExtMath.cpp
// OnlineVariance moved to ExtMath.h


class OnlineStdDevWindow : public Moveable<OnlineStdDevWindow> {
        OnlineAverageWindow1 av;
        Vector<double> win_a;
        double sum_a = 0.0;
        int period = 0, cursor = 0;
public:
        OnlineStdDevWindow() {}
        void SetPeriod(int i) {period = i; win_a.SetCount(i,0); av.SetPeriod(i);}
        void Add(double a) {
                av.Add(a);
                a = a - av.GetMean();
                a = a * a;
                double& da = win_a[cursor];
                sum_a -= da;
                da = a;
                sum_a += da;
                cursor = (cursor + 1) % period;
        }
        double Get() const {return sqrt(sum_a / period);}
        double GetMean() const {return av.GetMean();}
        void Serialize(Stream& s) {s % av % win_a % sum_a % period % cursor;}
};

struct SimpleDistribution {
        VectorMap<double, int> values;
        int total = 0;
        void Add(double d) {values.GetAdd(d, 0)++; total++;}
        void Finish() {SortByKey(values, StdLess<double>());}
        double Limit(double factor) {
                int value_limit = factor * total;
                int sum = 0;
                for(int i = 0; i < values.GetCount(); i++) {
                        int j = values[i];
                        if (sum + j > value_limit) return values.GetKey(i);
                        sum += j;
                }
                return values.TopKey();
        }
};

struct FactoryDeclaration : public Moveable<FactoryDeclaration> {
        static const int max_args = 16;
        int args[max_args];
        int factory = -1;
        int arg_count = 0;
        FactoryDeclaration() {}
        FactoryDeclaration(const FactoryDeclaration& src) {*this = src;}
        FactoryDeclaration& Set(int i) {factory = i; return *this;}
        FactoryDeclaration& AddArg(int i) {ASSERT(arg_count >= 0 && arg_count < max_args); args[arg_count++] = i; return *this;}
        FactoryDeclaration& operator=(const FactoryDeclaration& src) {
                factory = src.factory; arg_count = src.arg_count;
                for(int i = 0; i < max_args; i++) args[i] = src.args[i];
                return *this;
        }
        unsigned GetHashValue() {
                CombineHash ch; ch << factory << 1;
                for(int i = 0; i < arg_count; i++) ch << args[i] << 1;
                return ch;
        }
        void Serialize(Stream& s) {
                if (s.IsLoading()) s.Get(args, sizeof(int) * max_args);
                else if (s.IsStoring()) s.Put(args, sizeof(int) * max_args);
                s % factory % arg_count;
        }
};

END_UPP_NAMESPACE

#endif
