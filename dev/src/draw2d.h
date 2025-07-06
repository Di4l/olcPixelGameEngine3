#pragma once

//! START STDHEADER
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
#include "pixel.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_DRAW2D_DECLARED)
namespace olc
{
	class Draw2D
	{
	public:
		Draw2D();

	public:
		// Plot a single pixel
		template<typename T>
		void Draw(const olc::v_2d<T>& pos, const olc::Pixel col = olc::Colour::WHITE)
		{
			/*
				if(target image not in ram)
					copy_gpu_to_cpu(target image)


			
			*/
		}


		// Draws a single pixel wide line
		template<typename T>
		void DrawLine(const olc::v_2d<T>& p1, const olc::v_2d<T>& p2, const olc::Pixel col = olc::Colour::WHITE)
		{
			/*
				if(target image not in ram)
					copy_gpu_to_cpu(target image)



			*/
		}
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION