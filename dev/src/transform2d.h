#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "matrix3d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_TRANSFORM2D_DECLARED)
namespace olc
{
	namespace internal
	{
		// Return sign of number as -1 or +1
		template <typename T>
		inline constexpr int sgn(T val)
		{
			return (T(0) < val) - (val < T(0));
		}
	}



	/*
		A complete 2D transformation structure
	*/
	template<class T>
	class t_2d
	{
		static_assert(std::is_floating_point<T>::value, "olc::t_2d<type> must be floating point type");

	public:
		// Constructor
		inline constexpr t_2d() = default;

		// Copy constructor
		inline constexpr t_2d(const t_2d& t) = default;

		// Assignment operator
		inline constexpr t_2d& operator=(const t_2d& t) = default;

		// Transform a vector by this transform
		template<typename Q>
		inline constexpr auto forward(const olc::v_2d<Q>& v) const
		{
			return m_mForward * v;
		}

		template<typename Q>
		inline constexpr auto forwardRound(const olc::v_2d<Q>& v) const
		{
			return (m_mForward * v).round();
		}

		// Transform a vector of v_2d by this transform
		template<typename Q>
		inline constexpr auto forward(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return m_mForward * i; });
			return o;
		}

		template<typename Q>
		inline constexpr auto forwardRound(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return (m_mForward * i).round(); });
			return o;
		}

		// Transform a vector by this transform in place
		template<typename Q>
		inline constexpr auto forwardX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return (m_mForward * i); });
			return v;
		}

		template<typename Q>
		inline constexpr auto forwardRoundX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return (m_mForward * i).round(); });
			return v;
		}

		// Transform a vector by the inverse of this transform
		template<typename Q>
		inline constexpr auto inverse(const olc::v_2d<Q>& v) const
		{
			return m_mInverse * v;
		}

		// Transform a vector by this transform
		template<typename Q>
		inline constexpr auto inverse(const std::vector<olc::v_2d<Q>>& v) const
		{
			std::vector<olc::v_2d<Q>> o(v.size());
			std::transform(v.begin(), v.end(), o.begin(), [this](const olc::v_2d<Q>& i) {return m_mInverse * i; });
			return o;
		}

		// Transform a vector by this transform in place
		template<typename Q>
		inline constexpr auto inverseX(std::vector<olc::v_2d<Q>>&& v) const
		{
			std::transform(v.begin(), v.end(), v.begin(), [this](const olc::v_2d<Q>& i) {return m_mInverse * i; });
			return v;
		}


		// Set scaling component of this transformation
		template<typename Q>
		inline constexpr void scale(const olc::v_2d<Q>& v)
		{
			m_vScale = v;
			m_mScale.scale(m_vScale);
			update();
		}

		// Get scaling component of this transformation
		inline constexpr const auto& scale() const
		{
			return m_vScale;
		}

		// Set translation component of this transformation
		template<typename Q>
		inline constexpr void translate(const olc::v_2d<Q>& v)
		{
			m_vTranslate = v;
			m_mTranslate.translate(m_vTranslate);
			update();
		}

		// Get translation component of this transformation
		inline constexpr const auto& translate() const
		{
			return m_vTranslate;
		}

		// Set translation component of this transformation
		template<typename Q>
		inline constexpr void rotate(const Q& v, const olc::v_2d<T>& p = { 0,0 })
		{
			m_dTheta = T(v);
			m_vRotatePoint = p;
			m_3d<T> matTrans1;
			matTrans1.translate(-m_vRotatePoint);
			m_3d<T> matTrans2;
			matTrans2.translate(m_vRotatePoint);
			m_3d<T> matRotate;
			matRotate.rotate(m_dTheta);
			m_mRotate = matTrans1 * matRotate * matTrans2;
			update();
		}

		// Get translation component of this transformation
		inline constexpr const auto& rotate() const
		{
			return m_dTheta;
		}

		// Set shear component of this transformation
		template<typename Q>
		inline constexpr void shear(const olc::v_2d<Q>& v)
		{
			m_vShear = v;
			m_mShear.shear(m_vShear);
			update();
		}

		// Get shear component of this transformation
		inline constexpr const auto& shear() const
		{
			return m_vShear;
		}

		// Get forward transformation matrix
		inline constexpr const auto& forward_matrix() const
		{
			return m_mForward;
		}

		// Get inverse transformation matrix
		inline constexpr const auto& inverse_matrix() const
		{
			return m_mInverse;
		}

		// Construct transform from an existing transformation matrix
		template<typename Q>
		inline constexpr void from_matrix(const olc::m_3d<Q>& mat)
		{
			// https://math.stackexchange.com/a/13165
			m_vTranslate =
			{
				T(mat(2, 0)),
				T(mat(2, 1))
			};

			m_vScale =
			{
				T(olc::internal::sgn(mat(0, 0)) * std::hypot(mat(0, 0), mat(1, 0))),
				T(olc::internal::sgn(mat(1, 1)) * std::hypot(mat(0, 1), mat(1, 1)))
			};

			m_dTheta = T(std::atan2(mat(0, 1), mat(1, 1)));

			m_mTranslate.translate(m_vTranslate);
			m_mScale.scale(m_vScale);
			m_mRotate.rotate(m_dTheta);
			update();
		}

		template<typename Q>
		inline constexpr auto operator * (const olc::t_2d<Q>& rhs) const
		{
			auto& me = *this;		
			return me.forward_matrix() * rhs.forward_matrix();			
		}

	protected:
		// Constructs resultant matrices when transformation changes
		inline constexpr void update()
		{
			m_mForward = m_mRotate * m_mShear * m_mScale * m_mTranslate;
			m_mInverse = m_mForward.invert();
		}

	protected:
		T            m_dTheta = 0;
		olc::v_2d<T> m_vScale = { 1,1 };
		olc::v_2d<T> m_vTranslate = { 0,0 };
		olc::v_2d<T> m_vRotatePoint = { 0,0 };
		olc::v_2d<T> m_vShear = { 0,0 };
		olc::m_3d<T> m_mForward;
		olc::m_3d<T> m_mInverse;
		olc::m_3d<T> m_mScale;
		olc::m_3d<T> m_mTranslate;
		olc::m_3d<T> m_mRotate;
		olc::m_3d<T> m_mShear;
	};

	typedef t_2d<float> tf2d;
	typedef t_2d<double> td2d;
}
#define PGE_TRANSFORM2D_DECLARED 1
#endif
//! END DECLARATION