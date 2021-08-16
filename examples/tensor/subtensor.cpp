#include <boost/numeric/ublas/tensor.hpp>

using namespace boost::numeric::ublas;

void instantiate_subtensor_dynamic() {
  namespace ublas = boost::numeric::ublas;
  using value   = float;
  using layout  = ublas::layout::first_order; // storage format
  using tensor  = boost::numeric::ublas::tensor_dynamic<value,layout>;
  constexpr auto ones = ublas::ones<value,layout>{};

  try {
    tensor t1 = ones(3, 4, 2);
    std::cout << "t1 = " << t1 << std::endl;

    auto st1 = t1(span(1,ublas::max), span(), span(0,1));

    std::cout << "st1 = " << st1 << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Cought exception " << e.what();
    std::cerr << "in the instantiate_tensor_dynamic function of instantiate-tensor." << std::endl;
    throw;
  }
}

void instantiate_tensor_dynamics_with_static_order()
{
}


void instantiate_tensor_static()
{
}



int main() {
  instantiate_subtensor_dynamic();
}
