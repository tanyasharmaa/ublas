//  Copyright (c) 2018 Cem Bassoy
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  The authors gratefully acknowledge the support of
//  Fraunhofer and Google in producing this work
//  which started as a Google Summer of Code project.
//


#include <random>
#include <boost/test/unit_test.hpp>

#include "utility.hpp"
#include <boost/numeric/ublas/tensor/dynamic_tensor.hpp>
#include <boost/numeric/ublas/tensor/tags.hpp>
#include <boost/numeric/ublas/tensor/subtensor.hpp>
#include <boost/numeric/ublas/tensor/span.hpp>



BOOST_AUTO_TEST_SUITE ( subtensor_testsuite ) ;

// double,std::complex<float>



using test_types = zip<int,float,std::complex<float>>::with_t<boost::numeric::ublas::tag::first_order, boost::numeric::ublas::tag::last_order>;



struct fixture_shape
{
  using shape = boost::numeric::ublas::basic_extents<std::size_t>;

	fixture_shape() : extents{
				shape{},    // 0
				shape{1,1}, // 1
				shape{1,2}, // 2
				shape{2,1}, // 3
				shape{2,3}, // 4
				shape{2,3,1}, // 5
				shape{4,1,3}, // 6
				shape{1,2,3}, // 7
				shape{4,2,3}, // 8
				shape{4,2,3,5} // 9
				}
	{}
	std::vector<shape> extents;
};


BOOST_FIXTURE_TEST_CASE_TEMPLATE( subtensor_ctor1_test, value,  test_types, fixture_shape )
{

  namespace ub = boost::numeric::ublas;
  using value_type  = typename value::first_type;
  using layout_type = typename value::second_type;
  using tensor_type = ub::dynamic_tensor<value_type, layout_type>;
  using subtensor_type = ub::subtensor<ub::tag::sliced, tensor_type>;


  auto check = [](auto const& e) {
    auto t = tensor_type{e};
    auto s = subtensor_type(t);
    BOOST_CHECK_EQUAL (  s.size() , t.size() );
    BOOST_CHECK_EQUAL (  s.rank() , t.rank() );
    if(e.empty()) {
      BOOST_CHECK_EQUAL ( s.empty(), t.empty() );
      BOOST_CHECK_EQUAL ( s. data(), t. data() );
    }
    else{
      BOOST_CHECK_EQUAL ( !s.empty(), !t.empty() );
      BOOST_CHECK_EQUAL (  s. data(),  t. data() );
    }
  };

  for(auto const& e : extents)
    check(e);

}



BOOST_AUTO_TEST_CASE_TEMPLATE( subtensor_ctor2_test, value,  test_types )
{

  namespace ub = boost::numeric::ublas;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
  using tensor_type = ub::dynamic_tensor<value_type, layout_type>;
  using subtensor_type = ub::subtensor<ub::tag::sliced, tensor_type>;
  using span  = ub::sliced_span;


	{
		auto A    = tensor_type{};
		auto Asub = subtensor_type( A );

		BOOST_CHECK( Asub.span_strides() == A.strides() );
		BOOST_CHECK( Asub.strides() == A.strides() );
		BOOST_CHECK( Asub.extents() == A.extents() );
		BOOST_CHECK( Asub.data() == A.data() );
	}



	{
		auto A    = tensor_type{1,1};
		auto Asub = subtensor_type( A, 0, 0  );

		BOOST_CHECK( Asub.span_strides() == A.strides() );
		BOOST_CHECK( Asub.strides() == A.strides() );
		BOOST_CHECK( Asub.extents() == A.extents() );
		BOOST_CHECK( Asub.data() == A.data() );
	}


	{
		auto A    = tensor_type{1,2};
    auto Asub = subtensor_type( A, 0, span{}  );

		BOOST_CHECK( Asub.span_strides() == A.strides() );
		BOOST_CHECK( Asub.strides() == A.strides() );
		BOOST_CHECK( Asub.extents() == A.extents() );
		BOOST_CHECK( Asub.data() == A.data() );
	}
	{
		auto A    = tensor_type{1,2};
		auto Asub = subtensor_type( A, 0, 1  );

		BOOST_CHECK_EQUAL( Asub.span_strides().at(0), 1 );
		BOOST_CHECK_EQUAL( Asub.span_strides().at(1), 1 );

		BOOST_CHECK_EQUAL( Asub.strides().at(0), 1 );
		BOOST_CHECK_EQUAL( Asub.strides().at(1), 1 );

		BOOST_CHECK_EQUAL( Asub.extents().at(0) , 1 );
		BOOST_CHECK_EQUAL( Asub.extents().at(1) , 1 );

		BOOST_CHECK_EQUAL( Asub.data() , A.data()+
											 Asub.spans().at(0).first()*A.strides().at(0) +
											 Asub.spans().at(1).first()*A.strides().at(1) );
	}


	{
		auto A    = tensor_type{2,3};
		auto Asub = subtensor_type( A, 0, 1  );
		auto B    = tensor_type(Asub.extents());

		BOOST_CHECK_EQUAL( Asub.span_strides().at(0), A.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.span_strides().at(1), A.strides().at(1) );

		BOOST_CHECK_EQUAL( Asub.extents().at(0) , 1 );
		BOOST_CHECK_EQUAL( Asub.extents().at(1) , 1 );

		BOOST_CHECK_EQUAL( Asub.strides().at(0), B.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.strides().at(1), B.strides().at(1) );

		BOOST_CHECK_EQUAL( Asub.data() , A.data()+
											 Asub.spans().at(0).first()*A.strides().at(0) +
											 Asub.spans().at(1).first()*A.strides().at(1) );
	}

	{
		auto A    = tensor_type{4,3};
    auto Asub = subtensor_type( A, span(1,2), span(1,ub::end)  );
		auto B    = tensor_type(Asub.extents());

		BOOST_CHECK_EQUAL( Asub.span_strides().at(0), A.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.span_strides().at(1), A.strides().at(1) );

		BOOST_CHECK_EQUAL( Asub.extents().at(0) , 2 );
		BOOST_CHECK_EQUAL( Asub.extents().at(1) , 2 );

		BOOST_CHECK_EQUAL( Asub.strides().at(0), B.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.strides().at(1), B.strides().at(1) );

		BOOST_CHECK_EQUAL( Asub.data() , A.data()+
											 Asub.spans().at(0).first()*A.strides().at(0) +
											 Asub.spans().at(1).first()*A.strides().at(1) );
	}

	{
		auto A    = tensor_type{4,3,5};
    auto Asub = subtensor_type( A, span(1,2), span(1,ub::end), span(2,4)  );

		auto B    = tensor_type(Asub.extents());

		BOOST_CHECK_EQUAL( Asub.span_strides().at(0), A.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.span_strides().at(1), A.strides().at(1) );
		BOOST_CHECK_EQUAL( Asub.span_strides().at(2), A.strides().at(2) );

		BOOST_CHECK_EQUAL( Asub.extents().at(0) , 2 );
		BOOST_CHECK_EQUAL( Asub.extents().at(1) , 2 );
		BOOST_CHECK_EQUAL( Asub.extents().at(2) , 3 );

		BOOST_CHECK_EQUAL( Asub.strides().at(0), B.strides().at(0) );
		BOOST_CHECK_EQUAL( Asub.strides().at(1), B.strides().at(1) );
		BOOST_CHECK_EQUAL( Asub.strides().at(2), B.strides().at(2) );

		BOOST_CHECK_EQUAL( Asub.data() , A.data()+
											 Asub.spans().at(0).first()*A.strides().at(0) +
											 Asub.spans().at(1).first()*A.strides().at(1)+
											 Asub.spans().at(2).first()*A.strides().at(2));
	}

}



BOOST_FIXTURE_TEST_CASE_TEMPLATE(subtensor_copy_ctor_test, value,  test_types, fixture_shape )
{
  namespace ub = boost::numeric::ublas;
  using value_type  = typename value::first_type;
  using layout_type = typename value::second_type;
  using tensor_type = ub::dynamic_tensor<value_type, layout_type>;
  using subtensor_type = ub::subtensor<ub::tag::sliced, tensor_type>;
  using span  = ub::sliced_span;



	auto check = [](auto const& e)
	{

    auto A    = tensor_type{e};
    value_type i{};
    for(auto & a : A)
      a = i+=value_type{1};

    auto Asub = subtensor_type( A );
    auto Bsub = subtensor_type( A );


    BOOST_CHECK( Asub.span_strides() == A.strides() );
    BOOST_CHECK( Asub.strides()      == A.strides() );
    BOOST_CHECK( Asub.extents()      == A.extents() );
    BOOST_CHECK( Asub.data()         == A.data() );

    BOOST_CHECK( Bsub.span_strides() == A.strides() );
    BOOST_CHECK( Bsub.strides()      == A.strides() );
    BOOST_CHECK( Bsub.extents()      == A.extents() );
    BOOST_CHECK( Bsub.data()         == A.data()    );

    BOOST_CHECK_EQUAL (  Bsub.size() , A.size() );
    BOOST_CHECK_EQUAL (  Bsub.rank() , A.rank() );



		if(e.empty()) {
      BOOST_CHECK       ( Bsub.empty()    );
      BOOST_CHECK_EQUAL ( Bsub.data() , nullptr);
		}
		else{
      BOOST_CHECK       ( !Bsub.empty()    );
      BOOST_CHECK_NE    (  Bsub.data() , nullptr);
		}

    for(auto i = 0ul; i < Asub.size(); ++i)
      BOOST_CHECK_EQUAL( Asub[i], Bsub[i]  );

	};

	for(auto const& e : extents)
		check(e);

}

#if 0

BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_copy_ctor_layout, value,  test_types, fixture_shape )
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;
	using other_layout_type = std::conditional_t<std::is_same<ublas::tag::first_order,layout_type>::value, ublas::tag::last_order, ublas::tag::first_order>;
	using other_tensor_type = ublas::tensor<value_type, other_layout_type>;


	for(auto const& e : extents)
	{
		auto r = tensor_type{e};
		other_tensor_type t = r;
		tensor_type q = t;

		BOOST_CHECK_EQUAL (  t.size() , r.size() );
		BOOST_CHECK_EQUAL (  t.rank() , r.rank() );
		BOOST_CHECK ( t.extents() == r.extents() );

		BOOST_CHECK_EQUAL (  q.size() , r.size() );
		BOOST_CHECK_EQUAL (  q.rank() , r.rank() );
		BOOST_CHECK ( q.strides() == r.strides() );
		BOOST_CHECK ( q.extents() == r.extents() );

		for(auto i = 0ul; i < t.size(); ++i)
			BOOST_CHECK_EQUAL( q[i], r[i]  );
	}
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_copy_move_ctor, value,  test_types, fixture )
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	auto check = [](auto const& e)
	{
		auto r = tensor_type{e};
		auto t = std::move(r);
		BOOST_CHECK_EQUAL (  t.size() , e.product() );
		BOOST_CHECK_EQUAL (  t.rank() , e.size() );

		if(e.empty()) {
			BOOST_CHECK       ( t.empty()    );
			BOOST_CHECK_EQUAL ( t.data() , nullptr);
		}
		else{
			BOOST_CHECK       ( !t.empty()    );
			BOOST_CHECK_NE    (  t.data() , nullptr);
		}

	};

	for(auto const& e : extents)
		check(e);
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_ctor_extents_init, value,  test_types, fixture )
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	std::random_device device{};
	std::minstd_rand0 generator(device());

	using distribution_type = std::conditional_t<std::is_integral_v<value_type>, std::uniform_int_distribution<>, std::uniform_real_distribution<> >;
	auto distribution = distribution_type(1,6);

	for(auto const& e : extents){
		auto r = static_cast<value_type>(distribution(generator));
		auto t = tensor_type{e,r};
		for(auto i = 0ul; i < t.size(); ++i)
			BOOST_CHECK_EQUAL( t[i], r );
	}
}



BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_ctor_extents_array, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;
	using array_type  = typename tensor_type::array_type;

	for(auto const& e : extents) {
		auto a = array_type(e.product());
		auto v = value_type {};

		for(auto& aa : a){
			aa = v;
			v += value_type{1};
		}
		auto t = tensor_type{e, a};
		v = value_type{};

		for(auto i = 0ul; i < t.size(); ++i, v+=value_type{1})
			BOOST_CHECK_EQUAL( t[i], v);
	}
}



BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_read_write_single_index_access, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	for(auto const& e : extents) {
		auto t = tensor_type{e};
		auto v = value_type {};
		for(auto i = 0ul; i < t.size(); ++i, v+=value_type{1}){
			t[i] = v;
			BOOST_CHECK_EQUAL( t[i], v );

			t(i) = v;
			BOOST_CHECK_EQUAL( t(i), v );
		}
	}
}



BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_read_write_multi_index_access_at, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	auto check1 = [](const tensor_type& t)
	{
		auto v = value_type{};
		for(auto k = 0ul; k < t.size(); ++k){
			BOOST_CHECK_EQUAL(t[k], v);
			v+=value_type{1};
		}
	};

	auto check2 = [](const tensor_type& t)
	{
		std::array<unsigned,2> k;
		auto r = std::is_same_v<layout_type,ublas::tag::first_order> ? 1 : 0;
		auto q = std::is_same_v<layout_type,ublas::tag::last_order > ? 1 : 0;
		auto v = value_type{};
		for(k[r] = 0ul; k[r] < t.size(r); ++k[r]){
			for(k[q] = 0ul; k[q] < t.size(q); ++k[q]){
				BOOST_CHECK_EQUAL(t.at(k[0],k[1]), v);
				v+=value_type{1};
			}
		}
	};

	auto check3 = [](const tensor_type& t)
	{
		std::array<unsigned,3> k;
		using op_type = std::conditional_t<std::is_same_v<layout_type,ublas::tag::first_order>, std::minus<>, std::plus<>>;
		auto r = std::is_same_v<layout_type,ublas::tag::first_order> ? 2 : 0;
		auto o = op_type{};
		auto v = value_type{};
		for(k[r] = 0ul; k[r] < t.size(r); ++k[r]){
			for(k[o(r,1)] = 0ul; k[o(r,1)] < t.size(o(r,1)); ++k[o(r,1)]){
				for(k[o(r,2)] = 0ul; k[o(r,2)] < t.size(o(r,2)); ++k[o(r,2)]){
					BOOST_CHECK_EQUAL(t.at(k[0],k[1],k[2]), v);
					v+=value_type{1};
				}
			}
		}
	};

	auto check4 = [](const tensor_type& t)
	{
		std::array<unsigned,4> k;
		using op_type = std::conditional_t<std::is_same_v<layout_type,ublas::tag::first_order>, std::minus<>, std::plus<>>;
		auto r = std::is_same_v<layout_type,ublas::tag::first_order> ? 3 : 0;
		auto o = op_type{};
		auto v = value_type{};
		for(k[r] = 0ul; k[r] < t.size(r); ++k[r]){
			for(k[o(r,1)] = 0ul; k[o(r,1)] < t.size(o(r,1)); ++k[o(r,1)]){
				for(k[o(r,2)] = 0ul; k[o(r,2)] < t.size(o(r,2)); ++k[o(r,2)]){
					for(k[o(r,3)] = 0ul; k[o(r,3)] < t.size(o(r,3)); ++k[o(r,3)]){
						BOOST_CHECK_EQUAL(t.at(k[0],k[1],k[2],k[3]), v);
						v+=value_type{1};
					}
				}
			}
		}
	};

	auto check = [check1,check2,check3,check4](auto const& e) {
		auto t = tensor_type{e};
		auto v = value_type {};
		for(auto i = 0ul; i < t.size(); ++i){
			t[i] = v;
			v+=value_type{1};
		}

			 if(t.rank() == 1) check1(t);
		else if(t.rank() == 2) check2(t);
		else if(t.rank() == 3) check3(t);
		else if(t.rank() == 4) check4(t);

	};

	for(auto const& e : extents)
		check(e);
}




BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_reshape, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;


	for(auto const& efrom : extents){
		for(auto const& eto : extents){

			auto v = value_type {};
			v+=value_type{1};
			auto t = tensor_type{efrom, v};
			for(auto i = 0ul; i < t.size(); ++i)
				BOOST_CHECK_EQUAL( t[i], v );

			t.reshape(eto);
			for(auto i = 0ul; i < std::min(efrom.product(),eto.product()); ++i)
				BOOST_CHECK_EQUAL( t[i], v );

			BOOST_CHECK_EQUAL (  t.size() , eto.product() );
			BOOST_CHECK_EQUAL (  t.rank() , eto.size() );
			BOOST_CHECK ( t.extents() == eto );

			if(efrom != eto){
				for(auto i = efrom.product(); i < t.size(); ++i)
					BOOST_CHECK_EQUAL( t[i], value_type{} );
			}
		}
	}
}




BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_swap, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	for(auto const& e_t : extents){
		for(auto const& e_r : extents) {

			auto v = value_type {} + value_type{1};
			auto w = value_type {} + value_type{2};
			auto t = tensor_type{e_t, v};
			auto r = tensor_type{e_r, w};

			std::swap( r, t );

			for(auto i = 0ul; i < t.size(); ++i)
				BOOST_CHECK_EQUAL( t[i], w );

			BOOST_CHECK_EQUAL (  t.size() , e_r.product() );
			BOOST_CHECK_EQUAL (  t.rank() , e_r.size() );
			BOOST_CHECK ( t.extents() == e_r );

			for(auto i = 0ul; i < r.size(); ++i)
				BOOST_CHECK_EQUAL( r[i], v );

			BOOST_CHECK_EQUAL (  r.size() , e_t.product() );
			BOOST_CHECK_EQUAL (  r.rank() , e_t.size() );
			BOOST_CHECK ( r.extents() == e_t );


		}
	}
}



BOOST_FIXTURE_TEST_CASE_TEMPLATE( test_tensor_standard_iterator, value,  test_types, fixture)
{
	using namespace boost::numeric;
	using value_type  = typename value::first_type;
	using layout_type = typename value::second_type;
	using tensor_type = ublas::tensor<value_type, layout_type>;

	for(auto const& e : extents)
	{
		auto v = value_type {} + value_type{1};
		auto t = tensor_type{e, v};

		BOOST_CHECK_EQUAL( std::distance(t.begin(),  t.end ()), t.size()  );
		BOOST_CHECK_EQUAL( std::distance(t.rbegin(), t.rend()), t.size()  );

		BOOST_CHECK_EQUAL( std::distance(t.cbegin(),  t.cend ()), t.size() );
		BOOST_CHECK_EQUAL( std::distance(t.crbegin(), t.crend()), t.size() );

		if(t.size() > 0) {
			BOOST_CHECK(  t.data() ==  std::addressof( *t.begin () )  ) ;
			BOOST_CHECK(  t.data() ==  std::addressof( *t.cbegin() )  ) ;
		}
	}
}

#endif

BOOST_AUTO_TEST_SUITE_END();
