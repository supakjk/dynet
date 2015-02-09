#ifndef CNN_PARAMS_H_
#define CNN_PARAMS_H_

#include <vector>
#include <unordered_map>
#include "cnn/tensor.h"

namespace cnn {

struct ParametersBase {
  virtual ~ParametersBase();
  virtual size_t size() const = 0;
  virtual void accumulate_grad(const Matrix& g) = 0;
  virtual void update(real scale) = 0;
};

// represents parameters (e.g., a weight matrix)
struct Parameters : public ParametersBase {
  explicit Parameters(const Dim& d) : dim(d), values(Random(d)), g(Zero(d)) {}
  explicit Parameters(const Matrix& v) : dim(v.rows(), v.cols()), values(v), g(Zero(dim)) {}
  size_t size() const override;

  real& operator()(int i, int j) { return values(i,j); }
  const real& operator()(int i, int j) const { return values(i,j); }

  void accumulate_grad(const Matrix& g) override;
  void update(real scale) override;

  Dim dim;
  Matrix values;
  Matrix g;
};

// represents an input (i.e., things that aren't optimized)
struct ConstParameters : public ParametersBase {
  explicit ConstParameters(const real& s) : dim(1,1), values(Zero(dim)) { values(0,0) = s; }
  explicit ConstParameters(const Dim& d) : dim(d), values(Zero(d)) {}
  explicit ConstParameters(const Matrix& v) : dim(v.rows(), v.cols()), values(v) {}
  size_t size() const override;
  real& operator()(int i, int j) { return values(i,j); }
  const real& operator()(int i, int j) const { return values(i,j); }

  void accumulate_grad(const Matrix& g) override;
  void update(real scale) override;

  Dim dim;
  Matrix values;
};

// represents a matrix/vector embedding of a discrete set
struct LookupParameters : public ParametersBase {
  LookupParameters(unsigned n, const Dim& d) : dim(d), index(), values(n) {
    for (auto& v : values) v = Random(d);
  }
  size_t size() const override;

  const Matrix& embedding() const { return values[index]; }
  Matrix& operator[](unsigned i) { return values[i]; }
  const Matrix& operator[](unsigned i) const { return values[i]; }

  void accumulate_grad(const Matrix& g) override;
  void update(real scale) override;

  Dim dim;
  unsigned index; // index of item in set to be embedded
  std::vector<Matrix> values;
  std::unordered_map<unsigned, Matrix> g;
};

} // namespace cnn

#endif
