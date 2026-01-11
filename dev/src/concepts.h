#pragma once

//! START STDHEADER GLOBAL
#include <concepts>
//! END STDHEADER


//! START DECLARATION
namespace olc
{
	template <typename T>
	concept numeric = std::integral<T> || std::floating_point<T>;
}

//! END DECLARATION