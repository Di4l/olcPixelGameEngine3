#pragma once

#if defined(OLC_PGE3_MULTIFILE)
	
#else
	#include <olcPixelGameEngine3.h>
#endif

namespace olc::util3::gsm
{
	template<typename GData, typename GState>
	class Mode
	{
	public:

		Mode() = default;


		Mode(std::shared_ptr<GData> pGlobalData) : m_pGlobalData(pGlobalData)
		{

		}

	public:
		virtual bool OnCreate(olc::PixelGameEngine* pge)
		{
			return false;
		}

		virtual GState OnUpdate(olc::PixelGameEngine* pge, float fElapsedTime)
		{
			return GState{};
		}

		virtual bool OnEnterMode(olc::PixelGameEngine* pge)
		{
			return false;
		}

		virtual bool OnExitMode(olc::PixelGameEngine* pge)
		{
			return false;
		}

	protected:
		std::shared_ptr<GData> m_pGlobalData;
	};

}